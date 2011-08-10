/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici@gmail.com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "simpleviewer.moc"

// Qt includes

#include <QByteArray>
#include <QDataStream>
#include <QDir>
#include <QDomText>
#include <QFile>
#include <QPointer>

// KDE includes

#include <kaboutdata.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kfilemetainfo.h>
#include <kio/copyjob.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kstandardguiitem.h>
#include <ktempdir.h>
#include <ktoolinvocation.h>

// LibKIPI includes

#include <libkipi/imageinfo.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "batchprogressdialog.h"
#include "firstrundlg.h"
#include "pluginsversion.h"
#include "svedialog.h"

namespace KIPIFlashExportPlugin
{

class SimpleViewer::SimpleViewerPriv
{
public:

    SimpleViewerPriv() : maxThumbSize(45), viewer("simpleviewer.swf")
    {
        configDlg    = 0;
        totalActions = 0;
        action       = 0;
        canceled     = true;
        dataLocal    = KStandardDirs::locateLocal("data", "kipiplugin_flashexport/simpleviewer/", true);
        tempDir      = 0;
        interface    = 0;
        progressDlg  = 0;
    }

    bool                              canceled;

    int                               totalActions;
    int                               action;

    const int                         maxThumbSize;
    const QString                     viewer;

    QString                           dataLocal;
    QString                           hostName;
    QString                           hostUrl;
    QStringList                       simpleViewerFiles;

    KTempDir*                         tempDir;

    QList<KIPI::ImageCollection>      collectionsList;

    KIPI::Interface*                  interface;

    KIPIPlugins::BatchProgressDialog* progressDlg;

    SVEDialog*                        configDlg;
};

SimpleViewer::SimpleViewer(KIPI::Interface* interface, QObject* parent)
    : QObject(parent), d(new SimpleViewerPriv)
{
    d->interface = interface;

    d->simpleViewerFiles.append("web/svcore/swf/simpleviewer.swf");
    d->simpleViewerFiles.append("web/svcore/js/swfobject.js");
    d->hostName = KGlobal::mainComponent().aboutData()->programName();
    d->hostUrl  = KGlobal::mainComponent().aboutData()->homepage();

    if (d->hostUrl.isEmpty())
    {
        d->hostName = QString("Kipi");
        d->hostUrl  = QString("http://www.kipi-plugins.org");
    }
}

SimpleViewer::~SimpleViewer()
{
    delete d->tempDir;
    delete d;
}

void SimpleViewer::startExport()
{
    if(d->canceled)
        return;

    d->progressDlg = new KIPIPlugins::BatchProgressDialog(kapp->activeWindow(), i18n("Flash Export"));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();
    kapp->processEvents();

    // Estimate the number of actions for the KIPIPlugins progress dialog.
    d->progressDlg->addedAction(i18n("Initialising..."), KIPIPlugins::StartingMessage);
    d->collectionsList = d->configDlg->settings().collections;
    d->totalActions    = 0;
    for( QList<KIPI::ImageCollection>::ConstIterator it = d->collectionsList.constBegin() ;
         !d->canceled && (it != d->collectionsList.constEnd()) ; ++it )
    {
        d->totalActions += (*it).images().count();
    }

    // +copying SimpleViewer, +creating index.html
    d->totalActions += 2;

    d->progressDlg->setProgress(0, d->totalActions);

    slotProcess();

    d->progressDlg->setButtonGuiItem(KDialog::Cancel, KStandardGuiItem::close());
}

void SimpleViewer::slotCancel()
{
    d->progressDlg->addedAction(i18n("Export canceled"), KIPIPlugins::ErrorMessage);
    d->canceled = true;
}

void SimpleViewer::slotProcess()
{
    if(d->canceled)
        return;

    if(!d->canceled && !createExportDirectories())
    {
            d->progressDlg->addedAction(i18n("Failed to create export directories"),
                                       KIPIPlugins::ErrorMessage);
        return;
    }

    if(!d->canceled && !exportImages())
    {
        d->progressDlg->addedAction(i18n("Failed to export the images"),
                                   KIPIPlugins::ErrorMessage);
        return;
    }

    if(!d->canceled && !createIndex())
    {
        d->progressDlg->addedAction(i18n("Failed to create index.html"),
                                   KIPIPlugins::ErrorMessage);
        return;
    }

    if(!d->canceled && !copySimpleViewer())
    {
        d->progressDlg->addedAction(i18n("Failed to copy SimpleViewer files"),
                                   KIPIPlugins::ErrorMessage);
        return;
    }

    if(!d->canceled && !upload())
    {
        d->progressDlg->addedAction(i18n("Failed to upload the gallery"),
                                   KIPIPlugins::ErrorMessage);
        return;
    }

    if(d->canceled)
    {
        int ret = KMessageBox::warningYesNo(kapp->activeWindow(),
                                            i18n("Export was canceled.\n"
                                                 "Do you want to delete files in %1 that have already been created?",
                                                 d->configDlg->settings().exportUrl.path()));
        if(ret == KMessageBox::Yes)
        {
            KIO::NetAccess::del(d->configDlg->settings().exportUrl, kapp->activeWindow());
        }
    }

    if(!d->canceled)
        d->progressDlg->addedAction(i18nc("Flash export has finished", "Finished..."),
                                    KIPIPlugins::SuccessMessage);

    if(d->configDlg->settings().openInKonqueror)
        KToolInvocation::invokeBrowser(d->configDlg->settings().exportUrl.path());
}

bool SimpleViewer::createExportDirectories() const
{
    delete d->tempDir;
    d->tempDir = new KTempDir(KStandardDirs::locateLocal("tmp", "flashexport"));
    d->tempDir->setAutoRemove(true);

    d->progressDlg->addedAction(i18n("Creating directories..."), KIPIPlugins::StartingMessage);

    KUrl root = d->configDlg->settings().exportUrl;
    if(!KIO::NetAccess::mkdir(root, kapp->activeWindow()))
    {
        d->progressDlg->addedAction(i18n("Could not create folder '%1'", root.url()),
                                    KIPIPlugins::ErrorMessage);
        return(false);
    }

    KUrl thumbsDir = d->tempDir->name();
    thumbsDir.addPath("/thumbs");
    if(!KIO::NetAccess::mkdir(thumbsDir, kapp->activeWindow()))
    {
        d->progressDlg->addedAction(i18n("Could not create folder '%1'", thumbsDir.url()),
                                   KIPIPlugins::ErrorMessage);
        return(false);
    }

    KUrl imagesDir = d->tempDir->name();
    imagesDir.addPath("/images");
    if(!KIO::NetAccess::mkdir(imagesDir, kapp->activeWindow()))
    {
        d->progressDlg->addedAction(i18n("Could not create folder '%1'", imagesDir.url()),
                                    KIPIPlugins::ErrorMessage);
        return(false);
    }

    d->progressDlg->setProgress(++d->action, d->totalActions);

    return true;
}

bool SimpleViewer::cmpUrl(const KUrl &url1, const KUrl &url2)
{

    KExiv2Iface::KExiv2 meta1;
    meta1.load(url1.path());
    QDateTime clock1              = meta1.getImageDateTime();
    
    KExiv2Iface::KExiv2 meta2;
    meta2.load(url2.path());
    QDateTime clock2             = meta2.getImageDateTime();

    if(clock1.isValid() || clock2.isValid())
    	return clock1 < clock2;
    else
    {
    	QString name1 = url1.fileName();
    	QString name2 = url2.fileName();
    	
    	return name1 < name2;
    }
    	
}

bool SimpleViewer::exportImages() const
{
    if(d->canceled)
        return false;

    d->progressDlg->addedAction(i18n("Creating images and thumbnails..."), KIPIPlugins::StartingMessage);

    KUrl thumbsDir(d->tempDir->name());
    thumbsDir.addPath("/thumbs");

    KUrl imagesDir(d->tempDir->name());
    imagesDir.addPath("/images");

    KUrl xmlFile(d->tempDir->name());
    xmlFile.addPath("/gallery.xml");
    QFile file(xmlFile.path());
    file.open(QIODevice::WriteOnly);

    // header of gallery.xml
    QDomDocument xmlDoc;
    xmlDoc.appendChild(xmlDoc.createProcessingInstruction( QString::fromLatin1("xml"),
                       QString::fromLatin1("version=\"1.0\" encoding=\"UTF-8\"") ) );
    QDomElement galleryElem = xmlDoc.createElement(QString::fromLatin1("simpleviewerGallery"));
    xmlDoc.appendChild( galleryElem );
    galleryElem.setAttribute(QString::fromLatin1("enableRightClickOpen"), d->configDlg->settings().enableRightClickToOpen());
    galleryElem.setAttribute(QString::fromLatin1("maxImageWidth"),        d->configDlg->settings().maxImageDimension);
    galleryElem.setAttribute(QString::fromLatin1("maxImageHeight"),       d->configDlg->settings().maxImageDimension);
    galleryElem.setAttribute(QString::fromLatin1("textColor"),            d->configDlg->settings().textColor.name().replace('#', "0x"));
    galleryElem.setAttribute(QString::fromLatin1("frameColor"),           d->configDlg->settings().frameColor.name().replace('#', "0x"));
    galleryElem.setAttribute(QString::fromLatin1("bgColor"),              d->configDlg->settings().backgroundColor.name().replace('#', "0x"));
    galleryElem.setAttribute(QString::fromLatin1("frameWidth"),           d->configDlg->settings().frameWidth);
    galleryElem.setAttribute(QString::fromLatin1("stagePadding"),         d->configDlg->settings().stagePadding);
    galleryElem.setAttribute(QString::fromLatin1("thumbnailColumns"),     d->configDlg->settings().thumbnailColumns);
    galleryElem.setAttribute(QString::fromLatin1("thumbnailRows"),        d->configDlg->settings().thumbnailRows);
    galleryElem.setAttribute(QString::fromLatin1("navPosition"),          d->configDlg->settings().thumbPosition());
    galleryElem.setAttribute(QString::fromLatin1("navDirection"),         d->configDlg->settings().navDir());
    galleryElem.setAttribute(QString::fromLatin1("title"),                d->configDlg->settings().title);
    galleryElem.setAttribute(QString::fromLatin1("imagePath"),            QString("images/"));
    galleryElem.setAttribute(QString::fromLatin1("thumbPath"),            QString("thumbs/"));

    KExiv2Iface::KExiv2 meta;
    QImage              image;
    QImage              thumbnail;
    QString             tmp;
    QString             newName;

    int index           = 1;
    int maxSize         = d->configDlg->settings().imagesExportSize;
    bool resizeImages   = d->configDlg->settings().resizeExportImages;
    bool fixOrientation = d->configDlg->settings().fixOrientation;

    for( QList<KIPI::ImageCollection>::ConstIterator it = d->collectionsList.constBegin() ;
         !d->canceled && (it != d->collectionsList.constEnd()) ; ++it )
    {
         KUrl::List images = (*it).images();
        
         qSort(images.begin(),images.end(),cmpUrl);
        
        for(KUrl::List::ConstIterator it = images.constBegin();
            !d->canceled && (it != images.constEnd()) ; ++it)
        {
            kapp->processEvents();
            KUrl url = *it;
            QFileInfo fi(url.path());

            d->progressDlg->addedAction(i18n("Processing %1", url.fileName()), KIPIPlugins::StartingMessage);

            // Check if RAW file.
            QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
            if (rawFilesExt.toUpper().contains( fi.suffix().toUpper()))
                KDcrawIface::KDcraw::loadDcrawPreview(image, url.path());
            else
                image.load(url.path());

            if(image.isNull())
            {
                d->progressDlg->addedAction(i18n("Could not open image '%1'", url.fileName()),
                                           KIPIPlugins::WarningMessage);
                continue;
            }

            if(!createThumbnail(image, thumbnail))
            {
                d->progressDlg->addedAction(i18n("Could not create thumbnail from '%1'", url.fileName()),
                                           KIPIPlugins::WarningMessage);
                continue;
            }

            if(resizeImages && !resizeImage(image, maxSize, image))
            {
                d->progressDlg->addedAction(i18n("Could not resize image '%1'", url.fileName()),
                                           KIPIPlugins::WarningMessage);
                continue;
            }

            meta.load(url.path());
            bool rotated = false;
            newName      = QString("%1.%2").arg(tmp.sprintf("%03i", index)).arg(QString("jpg"));

            KUrl thumbnailPath(thumbsDir);
            thumbnailPath.addPath(newName);
            if (resizeImages && fixOrientation)
                meta.rotateExifQImage(thumbnail, meta.getImageOrientation());
            thumbnail.save(thumbnailPath.path(), "JPEG");

            KUrl imagePath(imagesDir);
            imagePath.addPath(newName);
            if (resizeImages && fixOrientation)
                rotated = meta.rotateExifQImage(image, meta.getImageOrientation());
            image.save(imagePath.path(), "JPEG");

            // Backup metadata from original image.
            meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
            meta.setImageDimensions(image.size());
            if (rotated) meta.setImageOrientation(KExiv2Iface::KExiv2::ORIENTATION_NORMAL);
            meta.save(imagePath.path());

            cfgAddImage(xmlDoc, galleryElem, url, newName);
            d->progressDlg->setProgress(++d->action, d->totalActions);
            index++;
        }
    }

    QByteArray data(xmlDoc.toByteArray());
    QDataStream stream( &file );
    stream.writeRawData(data.data(), data.size());
    file.close();

    return true;
}

bool SimpleViewer::createThumbnail(const QImage& image, QImage& thumbnail) const
{
    int w = image.width();
    int h = image.height();

    int maxSize = 0;

    if(w > d->maxThumbSize || h > d->maxThumbSize)
    {
        if(w > h)
        {
            maxSize = (int)(double)(w * d->maxThumbSize) / h;
        }
        else
        {
            maxSize = (int)(double)(h * d->maxThumbSize) / w;
        }
    }

    maxSize = (maxSize < d->maxThumbSize) ? d->maxThumbSize : maxSize;

    return resizeImage(image, maxSize, thumbnail);
}

bool SimpleViewer::resizeImage(const QImage& image, int maxSize, QImage& resizedImage) const
{
    int w = image.width();
    int h = image.height();

    if(w > maxSize || h > maxSize)
    {
        if(w > h)
        {
            h = (int)(double)(h * maxSize) / w;
            h = (h == 0) ? 1 : h;
            w = maxSize;
        }
        else
        {
            w = (int)(double)(w * maxSize) / h;
            w = (w == 0) ? 1 : w;
            h = maxSize;
        }
        resizedImage = image.scaled(w, h, Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation);
    }

    return true;
}

void SimpleViewer::cfgAddImage(QDomDocument& xmlDoc, QDomElement& galleryElem,
                               const KUrl& url, const QString& newName) const
{
    if(d->canceled)
        return;

    QString comment;
    QString keywords;

    if(d->configDlg->settings().showComments)
    {
        KIPI::ImageInfo info = d->interface->info(url);
        comment              = info.description();
    }
    else
    {
        comment.clear();
    }
    if(d->configDlg->settings().showKeywords && d->interface->hasFeature(KIPI::HostSupportsTags))
    {
        KIPI::ImageInfo info =d->interface->info(url);
        QMap<QString, QVariant> attribs = info.attributes();
        QStringList tagList = attribs["tags"].toStringList();
        if(tagList.join(" ")!="")
            keywords = QString("\nTags: ")+tagList.join(", ");
    }
    else
    {
        keywords.clear();
    }
    QDomElement img = xmlDoc.createElement(QString::fromLatin1("image"));
    galleryElem.appendChild(img);
    img.setAttribute(QString::fromLatin1("imageURL"),QString("images/")+newName);  
    img.setAttribute(QString::fromLatin1("thumbURL"),QString("thumbs/")+newName); 
    img.setAttribute(QString::fromLatin1("linkURL"),QString());
    img.setAttribute(QString::fromLatin1("targetURL"),QString());

    QDomElement caption = xmlDoc.createElement(QString::fromLatin1("caption"));
    img.appendChild(caption);
    QDomText captiontxt = xmlDoc.createTextNode(comment+keywords);
    caption.appendChild(captiontxt);
}

bool SimpleViewer::createIndex() const
{
    if(d->canceled)
        return false;

    d->progressDlg->addedAction(i18n("Creating index.html..."), KIPIPlugins::StartingMessage);

    QString indexTemplateName = KStandardDirs::locate("data", "kipiplugin_flashexport/index.template");
    if(indexTemplateName.isEmpty())
    {
        //TODO: errormsg
        kDebug() << "No indexTemplateName" ;
        return false;
    }

    QFile infile(indexTemplateName);
    infile.open(QIODevice::ReadOnly);
    QTextStream in(&infile);
    QString indexTemplate = in.readAll();
    infile.close();

    indexTemplate.replace("{TITLE}",    d->configDlg->settings().title);
    indexTemplate.replace("{COLOR}",    d->configDlg->settings().textColor.name());
    indexTemplate.replace("{BGCOLOR}",  d->configDlg->settings().backgroundColor.name());
    indexTemplate.replace("{HOSTURL}",  d->hostUrl);
    indexTemplate.replace("{HOSTNAME}", d->hostName);

    QFile outfile(d->tempDir->name() + "/index.html");
    outfile.open(QIODevice::WriteOnly);
    QTextStream out(&outfile);
    out << indexTemplate;
    outfile.close();

    d->progressDlg->setProgress(++d->action, d->totalActions);
    d->progressDlg->addedAction(i18n("index.html created..."), KIPIPlugins::SuccessMessage);

    return true;
}

bool SimpleViewer::copySimpleViewer() const
{
    if(d->canceled)
        return false;

    d->progressDlg->addedAction(i18n("Copying flash files..."), KIPIPlugins::StartingMessage);

    QString dataDir;

    // Due to its license, simpleviewer is installed in $KDEHOME
    dataDir = KStandardDirs::locate("data", "kipiplugin_flashexport/simpleviewer/");
    if(dataDir.isEmpty())
        installSimpleViewer();
    if(dataDir.isEmpty())
        return false;

    KUrl::List  files;
    QStringList entries;
    QDir        dir;

    dir.setPath(dataDir);
    entries = dir.entryList(QDir::Files);
    for(QStringList::ConstIterator it = entries.constBegin(); it != entries.constEnd(); ++it)
    {
        files.append(KUrl(dir.absolutePath() + '/' + *it));
    }

    // files distributed with the plugin are installed in $KDEDIRS
    dataDir = KStandardDirs::locate("data", "kipiplugin_flashexport/simpleviewer_html/");
    dir.setPath(dataDir);
    entries = dir.entryList(QDir::Files);
    for(QStringList::ConstIterator it = entries.constBegin(); it != entries.constEnd(); ++it)
    {
        files.append(QString(dir.absolutePath() + QLatin1Char('/') + *it));
    }
    // TODO: catch errors
    /*KIO::CopyJob *copyJob = */KIO::copy(files, d->configDlg->settings().exportUrl, KIO::HideProgressInfo);

    d->progressDlg->addedAction(i18n("Flash files copied..."), KIPIPlugins::SuccessMessage);

    return true;
}

bool SimpleViewer::upload() const
{
    if(d->canceled)
        return false;

    d->progressDlg->addedAction(i18n("Uploading gallery..."), KIPIPlugins::StartingMessage);

    if(!KIO::NetAccess::dircopy(KUrl(d->tempDir->name() + "./"), d->configDlg->settings().exportUrl))
        return false;

    d->progressDlg->addedAction(i18n("Gallery uploaded..."), KIPIPlugins::SuccessMessage);

    return true;
}

bool SimpleViewer::configure() const
{
    d->canceled = false;

    if(!d->configDlg)
        d->configDlg = new SVEDialog(d->interface, kapp->activeWindow());

    bool configured = false;
    while(!configured)
    {
        if(d->configDlg->exec() == QDialog::Rejected)
            return false;

        configured = true;

        if(KIO::NetAccess::exists(d->configDlg->settings().exportUrl, KIO::NetAccess::DestinationSide, kapp->activeWindow()))
        {
            int ret = KMessageBox::warningYesNoCancel(kapp->activeWindow(),
                                                      i18n("Target folder %1 already exists.\n"
                                                           "Do you want to overwrite it?  All data in this folder will be lost.",
                                                           d->configDlg->settings().exportUrl.path()));

            switch(ret)
            {
                case KMessageBox::Yes:
                    if(!KIO::NetAccess::del(d->configDlg->settings().exportUrl, kapp->activeWindow()))
                    {
                        KMessageBox::error(kapp->activeWindow(), i18n("Could not delete %1.\n"
                                           "Please choose another export folder.",
                                           d->configDlg->settings().exportUrl.path()));
                        configured = false;
                    }
                    break;

                case KMessageBox::No:
                    configured = false;
                    break;

                case KMessageBox::Cancel:
                    return false;
                    break;
            };
        }
    }

    return true;
}

bool SimpleViewer::checkSimpleViewer() const
{
    return ! KStandardDirs::locate("data", "kipiplugin_flashexport/simpleviewer/" + d->viewer).isEmpty();
}

bool SimpleViewer::installSimpleViewer() const
{
    QPointer<FirstRunDlg> firstRunDlg = new FirstRunDlg(kapp->activeWindow());
    if(firstRunDlg->exec() == QDialog::Accepted)
    {
        KUrl url = firstRunDlg->getUrl();

        if(unzip(url.path()))
        {
            delete firstRunDlg;
            return true;
        }
        else
        {
            kDebug() << "Archive extraction failed\n";
        }
    }
    else
    {
        // User press Cancel. No messageBox
        delete firstRunDlg;
        return false;
    }

    delete firstRunDlg;
    KMessageBox::error(kapp->activeWindow(), i18n("SimpleViewer installation failed"));

    return false;
}

bool SimpleViewer::unzip(const QString& url) const
{
    KZip zip(url);

    if(!openArchive(zip))
    {
        return false;
    }

    return extractArchive(zip);
}

bool SimpleViewer::openArchive(KZip& zip) const
{
    if(!zip.open(QIODevice::ReadOnly))
    {
        kDebug() << "Open archive failed\n";
        return false;
    }
    return true;
}

bool SimpleViewer::extractArchive(KZip& zip) const
{
    // read root directory content
    QStringList names = zip.directory()->entries();
    if(names.count() != 1)
    {
        kDebug() << "Wrong SimpleViewer Version or corrupted archive" ;
        kDebug() << "Content of the archive root folder" << names ;
        return false;
    }

    // open root directory
    const KArchiveEntry* root = zip.directory()->entry(names[0]);
    if(!root || !root->isDirectory())
    {
        kDebug() << "could not open " << names[0] << " of zipname" ;
        return false;
    }

    const KArchiveDirectory* dir = dynamic_cast<const KArchiveDirectory*>(root);

    // extract the needed files from SimpleViewer archive
    for(QStringList::ConstIterator it = d->simpleViewerFiles.constBegin();
        it != d->simpleViewerFiles.constEnd(); ++it )
    {
        const KArchiveEntry* entry = dir->entry(*it);
        if(!extractFile(entry))
        {
            //TODO error msg
            kDebug() << "could not open " << *it << " of zipname" ;
            return false;
        }
    }

    return true;
}

bool SimpleViewer::extractFile(const KArchiveEntry* entry) const
{
    if( !entry || !entry->isFile() )
        return false;

    const KArchiveFile* entryFile = dynamic_cast<const KArchiveFile*>(entry);
    QByteArray array = entryFile->data();

    QFile file( d->dataLocal + entry->name() );
    if(file.open( QIODevice::WriteOnly ))
    {
        int ret = file.write(array);
        file.close();
        return ret > 0 ? true : false;
    }

    return false;
}

/// Static method.
void SimpleViewer::run(KIPI::Interface* interface, QObject* parent)
{
    SimpleViewer plugin(interface, parent);

    if(!plugin.checkSimpleViewer())
    {
        if(!plugin.installSimpleViewer())
        {
            return;
        }
    }

    if(plugin.configure())
        plugin.startExport();
}

} // namespace KIPIFlashExportPlugin
