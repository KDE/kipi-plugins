// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 Aurelien Gateau

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// Self
#include "generator.moc"

// Qt
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>

// KDE
#include <kaboutdata.h>
#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <kurl.h>

// KIPI
#include <libkipi/batchprogressdialog.h>
#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>

// libxslt
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/xslt.h>
#include <libexslt/exslt.h>

// Local
#include "galleryinfo.h"
#include "xmlutils.h"

namespace KIPIHTMLExport {


/**
 * Produce a web-friendly file name 
 */
QString webifyFileName(QString fileName) {
	fileName=fileName.lower();
	
	// Remove potentially troublesome chars
	fileName=fileName.replace(QRegExp("[^-0-9a-z]+"), "_");

	return fileName;
}


/**
 * Make sure a file name is unique in list
 */
QString makeFileNameUnique(const QStringList& list, QString fileName) {
	// Make sure the file name is unique
	QString fileNameBase=fileName;
	int count=2;
	while (list.findIndex(fileName)!=-1) {
		fileName=fileNameBase + QString::number(count);
		++count;
	};

	return fileName;
}


/**
 * Prepare an XSLT param, quoting it at start and at end
 */
QCString makeXsltParam(const QString& txt) {
	// FIXME check for quotes
	QString param="'" + txt + "'";
	return param.utf8();
}


struct Generator::Private {
	KIPI::Interface* mInterface;
	GalleryInfo* mInfo;
	KIPI::BatchProgressDialog* mProgressDialog;
	KURL mThemeURL;
	
	// State info
	bool mWarnings;
	QString mXMLFileName;

	bool init() {
		// mThemeURL
		QString themeDir="kipiplugin_htmlexport/themes/" + mInfo->mTheme;
		QString themeBaseDir=KGlobal::instance()->dirs()->findResourceDir("data", themeDir + "/style.css");
		if (themeBaseDir.isEmpty()) {
			logError(i18n("Could not find theme"));
			return false;
		}

		mThemeURL.setPath(themeBaseDir);
		mThemeURL.addPath(themeDir);
	
		return true;
	}

	bool copyTheme() {
		mProgressDialog->addedAction(i18n("Copying theme"), KIPI::ProgressMessage);
		
		KURL destURL=mInfo->mDestURL;
		destURL.addPath(mInfo->mTheme);
		
		if (QFile::exists(destURL.path())) {
			KIO::NetAccess::del(destURL, mProgressDialog);
		}
		bool ok=KIO::NetAccess::dircopy(mThemeURL, destURL, mProgressDialog);
		if (!ok) {
			logError(i18n("Could not copy theme"));
			return false;
		}
		return true;
	}

	bool generateImagesAndXML() {
		QString baseDestDir=mInfo->mDestURL.path();
		if (!createDir(baseDestDir)) return false;
		
		mXMLFileName=baseDestDir + "/gallery.xml";
		XMLWriter xmlWriter;
		if (!xmlWriter.open(mXMLFileName)) {
			logError(i18n("Couldn't create gallery.xml"));
			return false;
		}

		XMLElement collectionsX(xmlWriter, "collections");

		// Loop on collections
		QValueList<KIPI::ImageCollection>::Iterator collectionIt=mInfo->mCollectionList.begin();
		QValueList<KIPI::ImageCollection>::Iterator collectionEnd=mInfo->mCollectionList.end();
		for (; collectionIt!=collectionEnd; ++collectionIt) {
			KIPI::ImageCollection collection=*collectionIt;
			logInfo( i18n("Generating files for \"%1\"").arg(collection.name()) );

			QString collectionFileName = webifyFileName(collection.name());
			QString destDir = baseDestDir + "/" + collectionFileName;
			if (!createDir(destDir)) return false;
			
			XMLElement collectionX(xmlWriter, "collection");
			xmlWriter.writeElement("name", collection.name());
			xmlWriter.writeElement("fileName", collectionFileName);

			QStringList fileNameList;
			
			// Loop on images
			KURL::List imageList=collection.images();
			KURL::List::Iterator it=imageList.begin();
			KURL::List::Iterator end=imageList.end();

			int pos=1;
			int count=imageList.count();
			for (; it!=end; ++it, ++pos) {
				mProgressDialog->setProgress(pos, count);
				qApp->processEvents();
				
				KIPI::ImageInfo info=mInterface->info(*it);
				XMLElement imageX(xmlWriter, "image");
				xmlWriter.writeElement("title", info.title());
				xmlWriter.writeElement("description", info.description());
			
				// Load image
				QImage image;
				QString path=(*it).path();
				if (!image.load(path) ) {
					logWarning(i18n("Could not load image '%1'").arg(path));
					continue;
				}
				
				// Prepare filenames
				QString baseFileName=webifyFileName(info.title());
				baseFileName=makeFileNameUnique(fileNameList, baseFileName);
				fileNameList.append(baseFileName);

				// Process full image
				{
					if (mInfo->mFullResize) {
						image=image.smoothScale(mInfo->mFullSize, mInfo->mFullSize, QImage::ScaleMin);
					}
					
					QString fileName=baseFileName + "." + mInfo->mFullFormat.lower();
					QString destPath=destDir + "/" + fileName;
					if (!image.save(destPath, mInfo->mFullFormat.ascii())) {
						logWarning(i18n("Could not save image '%1' to '%2'").arg(path).arg(destPath));
						continue;
					}
					XMLAttributeList attrList;
					attrList.append("fileName", fileName);
					attrList.append("width", image.width());
					attrList.append("height", image.height());
					XMLElement elem(xmlWriter, "full", &attrList);
				}
				
				// Process thumbnail
				{
					QImage thumbnail=image.smoothScale(mInfo->mThumbnailSize, mInfo->mThumbnailSize, QImage::ScaleMin);
					QString fileName="thumb_" + baseFileName + "." + mInfo->mThumbnailFormat.lower();

					QString destPath=destDir + "/" + fileName;
					if (!thumbnail.save(destPath, mInfo->mThumbnailFormat.ascii())) {
						logWarning(i18n("Could not save thumbnail for image '%1' to '%2'").arg(path).arg(destPath));
						continue;
					}
					
					XMLAttributeList attrList;
					attrList.append("fileName", fileName);
					attrList.append("width", thumbnail.width());
					attrList.append("height", thumbnail.height());
					XMLElement elem(xmlWriter, "thumbnail", &attrList);
				}
			}
		}
		return true;
	}


	bool generateHTML() {
		logInfo(i18n("Generating HTML files"));

		QString xsltFileName=mThemeURL.path() + "/template.xslt";
		CWrapper<xsltStylesheetPtr, xsltFreeStylesheet> xslt= xsltParseStylesheetFile( (const xmlChar*) xsltFileName.ascii() );

		if (!xslt) {
			logError(i18n("Could not load XSLT file '%1'").arg(xsltFileName));
			return false;
		}
		
		CWrapper<xmlDocPtr, xmlFreeDoc> xmlGallery=xmlParseFile( mXMLFileName.ascii() );
		if (!xmlGallery) {
			logError(i18n("Could not load XML file '%1'").arg(mXMLFileName));
			return false;
		}
		
		// Prepare i18n params
		typedef QMap<QCString,QCString> I18nMap;
		I18nMap map;
		map["i18nPrevious"]=makeXsltParam(i18n("Previous"));
		map["i18nNext"]=makeXsltParam(i18n("Next"));

		const char** params=new const char*[map.size()*2+1];
		
		I18nMap::Iterator it=map.begin(), end=map.end();
		const char** ptr=params;
		for (;it!=end; ++it) {
			*ptr=it.key().data();
			++ptr;
			*ptr=it.data().data();
			++ptr;
		}
		*ptr=0;

		// Move to the destination dir, so that external documents get correctly
		// produced
		QString oldCD=QDir::currentDirPath();
		QDir::setCurrent(mInfo->mDestURL.path());
		
		CWrapper<xmlDocPtr, xmlFreeDoc> xmlOutput= xsltApplyStylesheet(xslt, xmlGallery, params);
		
		QDir::setCurrent(oldCD);
		//delete []params;
		
		if (!xmlOutput) {
			logError(i18n("Error processing XML file"));
			return false;
		}

		QString destFileName=mInfo->mDestURL.path() + "/index.html";
		FILE* file=fopen(destFileName.ascii(), "w");
		if (!file) {
			logError(i18n("Could not open '%1' for writting").arg(destFileName));
			return false;
		}
		xsltSaveResultToFile(file, xmlOutput, xslt);
		fclose(file);

		return true;
	}
	

	bool createDir(QDir dir) {
		if (dir.exists()) return true;
		
		QDir parent=dir;
		parent.cdUp();
		bool ok=createDir(parent);
		if (!ok) {
			logError(i18n("Could not create '%1").arg(parent.path()));
			return false;
		}
		return parent.mkdir(dir.dirName());
	}


	void logInfo(const QString& msg) {
		mProgressDialog->addedAction(msg, KIPI::ProgressMessage);
	}
	
	void logError(const QString& msg) {
		mProgressDialog->addedAction(msg, KIPI::ErrorMessage);
	}

	void logWarning(const QString& msg) {
		mProgressDialog->addedAction(msg, KIPI::WarningMessage);
		mWarnings=true;
	}
};

Generator::Generator(KIPI::Interface* interface, GalleryInfo* info, KIPI::BatchProgressDialog* progressDialog)
: QObject() {
	d=new Private;
	d->mInterface=interface;
	d->mInfo=info;
	d->mProgressDialog=progressDialog;
	d->mWarnings=false;
}


Generator::~Generator() {
	delete d;
}


bool Generator::run() {
	if (!d->init()) return false;
	
	QString destDir=d->mInfo->mDestURL.path();
	if (!d->createDir(destDir)) return false;

	if (!d->copyTheme()) return false;

	if (!d->generateImagesAndXML()) return false;
	
	exsltRegisterAll();
	bool result=d->generateHTML();
	xsltCleanupGlobals();
    xmlCleanupParser();
	return result;
}

bool Generator::warnings() const {
	return d->mWarnings;
}

} // namespace
