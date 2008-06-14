// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 Aurelien Gateau <aurelien dot gateau at free.fr>

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
// Self
#include "generator.moc"

// Qt
#include <QDir>
#include <QFile>
#include <QImageReader>
#include <QPainter>
#include <QRegExp>
#include <QStringList>

// KDE
#include <kaboutdata.h>
#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <kurl.h>

// KIPI
#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>

// libkipiplugins
#include <batchprogressdialog.h>

// libxslt
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/xslt.h>
#include <libexslt/exslt.h>

// Local
#include "abstractthemeparameter.h"
#include "galleryinfo.h"
#include "theme.h"
#include "xmlutils.h"

namespace KIPIHTMLExport {


typedef QMap<QByteArray,QByteArray> XsltParameterMap;


/**
 * Produce a web-friendly file name 
 */
QString webifyFileName(QString fileName) {
	fileName=fileName.toLower();
	
	// Remove potentially troublesome chars
	fileName=fileName.replace(QRegExp("[^-0-9a-z]+"), "_");

	return fileName;
}


/**
 * This helper class is used to make sure we use unique filenames
 */
class UniqueNameHelper {
public:
	QString makeNameUnique(QString name) {
		QString nameBase = name;
		int count=2;
		while (mList.indexOf(name)!=-1) {
			name = nameBase + QString::number(count);
			++count;
		};

		mList.append(name);
		return name;
	}

private:
	QStringList mList;
};


/**
 * Prepare an XSLT param, managing quote mess.
 * abc   => 'abc'
 * a"bc  => 'a"bc'
 * a'bc  => "a'bc"
 * a"'bc => concat('a"', "'", 'bc')
 */
QByteArray makeXsltParam(const QString& txt) {
	QString param;
	static const char apos='\'';
	static const char quote='"';
	
	if (txt.indexOf(apos)==-1) {
		// First or second case: no apos
		param= apos + txt + apos;
		
	} else if (txt.indexOf(quote)==-1) {
		// Third case: only apos, no quote
		param= quote + txt + quote;
		
	} else {
		// Forth case: both apos and quote :-(
		QStringList lst = txt.split(apos, QString::KeepEmptyParts);

		QStringList::Iterator it=lst.begin(), end=lst.end();
		param= "concat(";
		param+= apos + *it + apos;
		++it;
		for (;it!=end; ++it) {
			param+= ", \"'\", ";
			param+= apos + *it + apos;
		}
		param+= ")";
	}
	//kdDebug() << "param: " << txt << " => " << param << endl;
	return param.toUtf8();
}


/**
 * Genearate a square thumbnail from @fullImage of @size x @size pixels
 */
QImage generateSquareThumbnail(const QImage& fullImage, int size) {
	QImage image = fullImage.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

	if (image.width() != size || image.height() != size) {
		int sx=0, sy=0;
		if (image.width() > size) {
			sx=(image.width() - size) / 2;
		} else {
			sy=(image.height() - size) / 2;
		}
		image = image.copy(sx, sy, size, size);
	}

	return image;
}


struct Generator::Private {
	KIPI::Interface* mInterface;
	GalleryInfo* mInfo;
	KIPIPlugins::BatchProgressDialog* mProgressDialog;
	Theme::Ptr mTheme;
	
	// State info
	bool mWarnings;
	QString mXMLFileName;
	UniqueNameHelper mUniqueNameHelper;

	bool init() {
		mTheme=Theme::findByInternalName(mInfo->theme());
		if (!mTheme) {
			logError( i18n("Could not find theme in '%1'", mInfo->theme()) );
			return false;
		}
		return true;
	}

	bool copyTheme() {
		mProgressDialog->addedAction(i18n("Copying theme"), KIPIPlugins::ProgressMessage);
		
		KUrl srcURL=KUrl(mTheme->directory());

		KUrl destURL=mInfo->destUrl();
		destURL.addPath(srcURL.fileName());
		
		if (QFile::exists(destURL.path())) {
			KIO::NetAccess::del(destURL, mProgressDialog);
		}
		bool ok=KIO::NetAccess::dircopy(srcURL, destURL, mProgressDialog);
		if (!ok) {
			logError(i18n("Could not copy theme"));
			return false;
		}
		return true;
	}

	bool writeDataToFile(const QByteArray& data, const QString& destPath) {
		QFile destFile(destPath);
		if (!destFile.open(QIODevice::WriteOnly)) {
			logWarning(i18n("Could not open file '%1' for writing", destPath));
			return false;
		}
		if (destFile.write(data) != data.size()) {
			logWarning(i18n("Could not save image to file '%1'", destPath));
			return false;
		}
		return true;
	}

	/**
	 * Helper class for generateImageAndXMLForURL
	 */
	void appendImageElementToXML(XMLWriter& xmlWriter, const QString& elementName, const QString& fileName, const QImage& image) {
		XMLAttributeList attrList;
		attrList.append("fileName", fileName);
		attrList.append("width", image.width());
		attrList.append("height", image.height());
		XMLElement elem(xmlWriter, elementName, &attrList);
	}


	/**
	 * Generate images (full and thumbnail) for imageURL
	 * Fills xmlWriter with info about this image
	 */
	void generateImageAndXMLForURL(XMLWriter& xmlWriter, const QString& destDir, const KUrl& imageURL) {
		KIPI::ImageInfo info=mInterface->info(imageURL);
	
		// Load image
		QString path=imageURL.path();
		QFile imageFile(path);
		if (!imageFile.open(QIODevice::ReadOnly)) {
			logWarning(i18n("Could not read image '%1'", path));
			return;
		}
		
		QString imageFormat = QImageReader::imageFormat(&imageFile);
		if (imageFormat.isEmpty()) {
			logWarning(i18n("Format of image '%1' is unknown", path));
			return;
		}
		imageFile.close();
		imageFile.open(QIODevice::ReadOnly);

		QByteArray imageData = imageFile.readAll();
		QImage originalImage;
		if (!originalImage.loadFromData(imageData) ) {
			logWarning(i18n("Error loading image '%1'", path));
			return;
		}

		// Process images
		QImage fullImage = originalImage;
		if (!mInfo->useOriginalImageAsFullImage()) {
			if (mInfo->fullResize()) {
				int size = mInfo->fullSize();
				fullImage = fullImage.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}
			if (info.angle() != 0) {
				QMatrix matrix;
				matrix.rotate(info.angle());
				fullImage = fullImage.transformed(matrix);
			}
		}

		QImage thumbnail = generateSquareThumbnail(fullImage, mInfo->thumbnailSize());

		// Save images
		QString baseFileName = webifyFileName(info.title());
		baseFileName = mUniqueNameHelper.makeNameUnique(baseFileName);

		// Save full
		QString fullFileName;
		if (mInfo->useOriginalImageAsFullImage()) {
			fullFileName = baseFileName + "." + imageFormat.toLower();
			if (!writeDataToFile(imageData, destDir + "/" + fullFileName)) {
				return;
			}

		} else {
			fullFileName = baseFileName + "." + mInfo->fullFormatString().toLower();
			QString destPath = destDir + "/" + fullFileName;
			if (!fullImage.save(destPath, mInfo->fullFormatString().toAscii(), mInfo->fullQuality())) {
				logWarning(i18n("Could not save image '%1' to '%2'", path, destPath));
				return;
			}
		}

		// Save original
		QString originalFileName;
		if (mInfo->copyOriginalImage()) {
			originalFileName = "original_" + fullFileName;
			if (!writeDataToFile(imageData, destDir + "/" + originalFileName)) {
				return;
			}
		}

		// Save thumbnail
		QString thumbnailFileName = "thumb_" + baseFileName + "." + mInfo->thumbnailFormatString().toLower();
		QString destPath = destDir + "/" + thumbnailFileName;
		if (!thumbnail.save(destPath, mInfo->thumbnailFormatString().toAscii(), mInfo->thumbnailQuality())) {
			logWarning(i18n("Could not save thumbnail for image '%1' to '%2'", path, destPath));
			return;
		}
		
		// Write XML
		XMLElement imageX(xmlWriter, "image");
		xmlWriter.writeElement("title", info.title());
		xmlWriter.writeElement("description", info.description());

		appendImageElementToXML(xmlWriter, "full", fullFileName, fullImage);
		appendImageElementToXML(xmlWriter, "thumbnail", thumbnailFileName, thumbnail);
		if (mInfo->copyOriginalImage()) {
			appendImageElementToXML(xmlWriter, "original", originalFileName, originalImage);
		}
	}


	bool generateImagesAndXML() {
		QString baseDestDir=mInfo->destUrl().path();
		if (!createDir(baseDestDir)) return false;
		
		mXMLFileName=baseDestDir + "/gallery.xml";
		XMLWriter xmlWriter;
		if (!xmlWriter.open(mXMLFileName)) {
			logError(i18n("Could not create gallery.xml"));
			return false;
		}

		XMLElement collectionsX(xmlWriter, "collections");

		// Loop on collections
		QList<KIPI::ImageCollection>::Iterator collectionIt=mInfo->mCollectionList.begin();
		QList<KIPI::ImageCollection>::Iterator collectionEnd=mInfo->mCollectionList.end();
		for (; collectionIt!=collectionEnd; ++collectionIt) {
			KIPI::ImageCollection collection=*collectionIt;
			logInfo( i18n("Generating files for \"%1\"", collection.name()) );

			QString collectionFileName = webifyFileName(collection.name());
			QString destDir = baseDestDir + "/" + collectionFileName;
			if (!createDir(destDir)) return false;
			
			XMLElement collectionX(xmlWriter, "collection");
			xmlWriter.writeElement("name", collection.name());
			xmlWriter.writeElement("fileName", collectionFileName);

			// Loop on image in collection
			KUrl::List imageList = collection.images();
			KUrl::List::Iterator it = imageList.begin();
			KUrl::List::Iterator end = imageList.end();

			int pos = 1;
			int count = imageList.count();
			for (; it!=end; ++it, ++pos) {
				mProgressDialog->setProgress(pos, count);
				qApp->processEvents();
				generateImageAndXMLForURL(xmlWriter, destDir, *it);
			}

		}
		return true;
	}


	/**
	 * Add to map all the i18n parameters.
	 */
	void addI18nParameters(XsltParameterMap& map) {
		map["i18nPrevious"] = makeXsltParam(i18n("Previous"));
		map["i18nNext"] = makeXsltParam(i18n("Next"));
		map["i18nCollectionList"] = makeXsltParam(i18n("Collection List"));
		map["i18nOriginalImage"] = makeXsltParam(i18n("Original Image"));
		map["i18nUp"] = makeXsltParam(i18n("Go Up"));
	}


	/**
	 * Add to map all the theme parameters, as specified by the user.
	 */
	void addThemeParameters(XsltParameterMap& map) {
		Theme::ParameterList parameterList = mTheme->parameterList();
		QString themeInternalName = mTheme->internalName();
		Theme::ParameterList::ConstIterator
			it = parameterList.begin(),
			end = parameterList.end();
		for (; it!=end; ++it) {
			AbstractThemeParameter* themeParameter = *it;
			QByteArray internalName = themeParameter->internalName();
			QString value = mInfo->getThemeParameterValue(
				themeInternalName,
				internalName,
				themeParameter->defaultValue());

			map[internalName] = makeXsltParam(value);
		}
	}


	bool generateHTML() {
		logInfo(i18n("Generating HTML files"));

		QString xsltFileName=mTheme->directory() + "/template.xsl";
		CWrapper<xsltStylesheetPtr, xsltFreeStylesheet> xslt= xsltParseStylesheetFile( (const xmlChar*) xsltFileName.toLocal8Bit().data() );

		if (!xslt) {
			logError(i18n("Could not load XSL file '%1'", xsltFileName));
			return false;
		}
		
		CWrapper<xmlDocPtr, xmlFreeDoc> xmlGallery=xmlParseFile( mXMLFileName.toLocal8Bit().data() );
		if (!xmlGallery) {
			logError(i18n("Could not load XML file '%1'", mXMLFileName));
			return false;
		}
		
		// Prepare parameters
		XsltParameterMap map;
		addI18nParameters(map);
		addThemeParameters(map);

		const char** params=new const char*[map.size()*2+1];
		
		XsltParameterMap::Iterator it=map.begin(), end=map.end();
		const char** ptr=params;
		for (;it!=end; ++it) {
			*ptr=it.key().data();
			++ptr;
			*ptr=it.value().data();
			++ptr;
		}
		*ptr=0;

		// Move to the destination dir, so that external documents get correctly
		// produced
		QString oldCD=QDir::currentPath();
		QDir::setCurrent(mInfo->destUrl().path());
		
		CWrapper<xmlDocPtr, xmlFreeDoc> xmlOutput= xsltApplyStylesheet(xslt, xmlGallery, params);
		
		QDir::setCurrent(oldCD);
		//delete []params;
		
		if (!xmlOutput) {
			logError(i18n("Error processing XML file"));
			return false;
		}

		QString destFileName=mInfo->destUrl().path() + "/index.html";
		FILE* file=fopen(destFileName.toLocal8Bit().data(), "w");
		if (!file) {
			logError(i18n("Could not open '%1' for writing", destFileName));
			return false;
		}
		xsltSaveResultToFile(file, xmlOutput, xslt);
		fclose(file);

		return true;
	}
	

	bool createDir(const QString& dirName) {
		QStringList parts = dirName.split('/', QString::SkipEmptyParts);
		QDir dir = QDir::root();
		Q_FOREACH(const QString& part, parts) {
			if (!dir.exists(part)) {
				if (!dir.mkdir(part)) {
					logError(i18n("Could not create folder '%1' in '%2'", part, dir.absolutePath()));
					return false;
				}
			}
			dir.cd(part);
		}
		return true;
	}


	void logInfo(const QString& msg) {
		mProgressDialog->addedAction(msg, KIPIPlugins::ProgressMessage);
	}
	
	void logError(const QString& msg) {
		mProgressDialog->addedAction(msg, KIPIPlugins::ErrorMessage);
	}

	void logWarning(const QString& msg) {
		mProgressDialog->addedAction(msg, KIPIPlugins::WarningMessage);
		mWarnings=true;
	}
};

Generator::Generator(KIPI::Interface* interface, GalleryInfo* info, KIPIPlugins::BatchProgressDialog* progressDialog)
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

	QString destDir=d->mInfo->destUrl().path();
	kDebug() << destDir;
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
