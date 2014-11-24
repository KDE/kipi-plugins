/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011-2013 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "kpversion.h"
#include "kpimageinfo.h"
#include "kpmetadata.h"

namespace KIPIFlashExportPlugin
{

class SimpleViewer::SimpleViewerPriv
{
public:

    SimpleViewerPriv() :
        maxThumbSize(45),
        viewer("simpleviewer.swf")
    {
        totalActions = 0;
        action       = 0;
        canceled     = true;
        tempDir      = 0;
        interface    = 0;
        progressWdg  = 0;
        settings     = 0;
        width        = 0;
        height       = 0;
    }

    bool                              canceled;

    int                               totalActions;
    int                               action;

    //size of every image
    int                               width;
    int                               height;


    const int                         maxThumbSize;
    const QString                     viewer;

    QString                           dataDir;
    QString                           dataLocal;
    QString                           hostName;
    QString                           hostUrl;
    QStringList                       simpleViewerFiles;

    KTempDir*                         tempDir;

    Interface*                        interface;

    KPBatchProgressWidget*            progressWdg;

    SimpleViewerSettingsContainer*    settings;
};

SimpleViewer::SimpleViewer(Interface* const interface, QObject* const parent)
    : QObject(parent), d(new SimpleViewerPriv)
{
    d->interface = interface;
    d->hostName  = KGlobal::mainComponent().aboutData()->programName();
    d->hostUrl   = KGlobal::mainComponent().aboutData()->homepage();

    if (d->hostUrl.isEmpty())
    {
        d->hostName = QString("Kipi");
        d->hostUrl  = QString("http://www.digikam.org");
    }
}

SimpleViewer::~SimpleViewer()
{
    delete d->tempDir;
    delete d;
}

void SimpleViewer::appendPluginFiles(int pluginType)
{
    kDebug() << "Value of plugin type in append files" << pluginType;
    switch(pluginType)
    {
        case 0:
            d->simpleViewerFiles.clear();
            d->simpleViewerFiles.append("web/svcore/swf/simpleviewer.swf");
            d->simpleViewerFiles.append("web/svcore/js/swfobject.js");
                    d->dataLocal = KStandardDirs::locateLocal("data", "kipiplugin_flashexport/simpleviewer/", true);
                    d->dataDir   = KStandardDirs::locate("data", "kipiplugin_flashexport/simpleviewer/");
                    kDebug() << "Data dir when set is " << d->dataDir;
            break;
        case 1:
            d->simpleViewerFiles.clear();
            d->simpleViewerFiles.append("autoviewer.swf");
            d->simpleViewerFiles.append("swfobject.js");
            d->dataLocal = KStandardDirs::locateLocal("data", "kipiplugin_flashexport/autoviewer/", true);
                    d->dataDir   = KStandardDirs::locate("data", "kipiplugin_flashexport/autoviewer/");
            break;
        case 2:
            d->simpleViewerFiles.clear();
            d->simpleViewerFiles.append("TiltViewer.swf");
            d->simpleViewerFiles.append("swfobject.js");
            d->dataLocal = KStandardDirs::locateLocal("data", "kipiplugin_flashexport/tiltviewer/", true);
                        d->dataDir   = KStandardDirs::locate("data", "kipiplugin_flashexport/tiltviewer/");
            break;
        case 3:
            d->simpleViewerFiles.clear();
            d->simpleViewerFiles.append("viewer.swf");
            d->simpleViewerFiles.append("swfobject.js");
            d->dataLocal = KStandardDirs::locateLocal("data", "kipiplugin_flashexport/postcardviewer/", true);
                        d->dataDir   = KStandardDirs::locate("data", "kipiplugin_flashexport/postcardviewer/");
            break;
        default:
            break;
    }
}

void SimpleViewer::initProgressWdg() const
{
    d->progressWdg = new KPBatchProgressWidget(kapp->activeWindow());
    kDebug() << "progress dialog initialized";
}

void SimpleViewer::startExport()
{
    if(d->canceled)
        return;

     kDebug() << "SimpleViewer started...";
    // Estimate the number of actions for the KIPIPlugins progress dialog.
    d->progressWdg->addedAction(i18n("Initialising..."), StartingMessage);
    d->totalActions    = 0;
    d->action          = 0;
    d->progressWdg->reset();

    if(d->settings->imgGetOption == 0)
    {
        for( QList<ImageCollection>::ConstIterator it = d->settings->collections.constBegin() ;
            !d->canceled && (it != d->settings->collections.constEnd()) ; ++it )
        {
            d->totalActions += (*it).images().count();
        }
    }
    else
    {
        d->totalActions+=d->settings->imageDialogList.count();
    }

    // +copying SimpleViewer, +creating index.html
    d->totalActions += 2;

    d->progressWdg->setProgress(0, d->totalActions);

    slotProcess();
}

void SimpleViewer::slotCancel()
{
    d->progressWdg->addedAction(i18n("Export canceled"), ErrorMessage);
    d->canceled = true;
}

void SimpleViewer::slotProcess()
{
    if(d->canceled)
        return;

    if(!d->canceled && !createExportDirectories())
    {
            d->progressWdg->addedAction(i18n("Failed to create export directories"),
                                       ErrorMessage);
        return;
    }

    if(!d->canceled && !exportImages())
    {
        d->progressWdg->addedAction(i18n("Failed to export the images"),
                                   ErrorMessage);
        return;
    }

    if(!d->canceled && !createIndex())
    {
        d->progressWdg->addedAction(i18n("Failed to create index.html"),
                                   ErrorMessage);
        return;
    }

    if(!d->canceled && !copySimpleViewer())
    {
        d->progressWdg->addedAction(i18n("Failed to copy SimpleViewer files"),
                                   ErrorMessage);
        return;
    }

    if(!d->canceled && !upload())
    {
        d->progressWdg->addedAction(i18n("Failed to upload the gallery"),
                                   ErrorMessage);
        return;
    }

    if(d->canceled)
    {

        int ret = KMessageBox::warningYesNo(kapp->activeWindow(),
                                            i18n("Export was canceled.\n"
                                                 "Do you want to delete files in %1 that have already been created?",
                                                 d->settings->exportUrl.path()));
        if(ret == KMessageBox::Yes)
        {
            KIO::NetAccess::del(d->settings->exportUrl, kapp->activeWindow());
        }
    }

    if(!d->canceled)
    {
        d->progressWdg->addedAction(i18nc("Flash export has finished", "Finished..."),
                                    SuccessMessage);
        emit signalProcessingDone();
        if(d->settings->openInKonqueror)
            KToolInvocation::invokeBrowser(d->settings->exportUrl.path());
    }
}

bool SimpleViewer::createExportDirectories() const
{
    delete d->tempDir;
    d->tempDir = new KTempDir(KStandardDirs::locateLocal("tmp", "flashexport"));
    d->tempDir->setAutoRemove(true);

    d->progressWdg->addedAction(i18n("Creating directories..."), StartingMessage);

    KUrl root = d->settings->exportUrl;
    kDebug() << "export url is" << root.url();
    if(!KIO::NetAccess::mkdir(root, kapp->activeWindow()))
    {
        d->progressWdg->addedAction(i18n("Could not create folder '%1'", root.url()),
                                    ErrorMessage);
        return false;
    }

    if(d->settings->plugType == 0)
    {

        KUrl thumbsDir = d->tempDir->name();
        thumbsDir.addPath("/thumbs");
        if(!KIO::NetAccess::mkdir(thumbsDir, kapp->activeWindow()))
        {
            d->progressWdg->addedAction(i18n("Could not create folder '%1'", thumbsDir.url()),
                                   ErrorMessage);
            return false;
        }
    }

    KUrl imagesDir = d->tempDir->name();
    imagesDir.addPath("/images");
    kDebug() << "image folder url is" << imagesDir.url();

    if(!KIO::NetAccess::mkdir(imagesDir, kapp->activeWindow()))
    {
        d->progressWdg->addedAction(i18n("Could not create folder '%1'", imagesDir.url()),
                                    ErrorMessage);
        return false;
    }

    d->progressWdg->addedAction(i18n("Finished Creating directories..."), StartingMessage);
    d->progressWdg->setProgress(++d->action, d->totalActions);

    return true;
}

bool SimpleViewer::cmpUrl(const KUrl& url1, const KUrl& url2)
{
    KPMetadata meta;
    meta.load(url1.path());
    QDateTime clock1 = meta.getImageDateTime();

    meta.load(url2.path());
    QDateTime clock2 = meta.getImageDateTime();

    if (clock1.isValid() || clock2.isValid())
    {
        return clock1 < clock2;
    }
    else
    {
        QString name1 = url1.fileName();
        QString name2 = url2.fileName();

        return (name1 < name2) ;
    }
}

bool SimpleViewer::exportImages()
{
    if(d->canceled)
        return false;

    d->progressWdg->addedAction(i18n("Creating images and thumbnails..."), StartingMessage);

    KUrl xmlFile(d->tempDir->name());
    xmlFile.addPath("/gallery.xml");
    QFile file(xmlFile.path());
    file.open(QIODevice::WriteOnly);

    // header of gallery.xml
    QDomElement  galleryElem;
    QDomElement  photosElem;
    QDomDocument xmlDoc;
    xmlDoc.appendChild(xmlDoc.createProcessingInstruction( QString::fromLatin1("xml"),
                       QString::fromLatin1("version=\"1.0\" encoding=\"UTF-8\"") ) );

    switch(d->settings->plugType)
    {
        case 0:
        {
            galleryElem = xmlDoc.createElement(QString::fromLatin1("simpleviewerGallery"));
            xmlDoc.appendChild( galleryElem );
            galleryElem.setAttribute(QString::fromLatin1("enableRightClickOpen"), d->settings->enableRightClickToOpen());
            galleryElem.setAttribute(QString::fromLatin1("maxImageWidth"),        d->settings->maxImageDimension);
            galleryElem.setAttribute(QString::fromLatin1("maxImageHeight"),       d->settings->maxImageDimension);
            galleryElem.setAttribute(QString::fromLatin1("textColor"),            d->settings->textColor.name().replace('#', "0x"));
            galleryElem.setAttribute(QString::fromLatin1("frameColor"),           d->settings->frameColor.name().replace('#', "0x"));
            galleryElem.setAttribute(QString::fromLatin1("bgColor"),              d->settings->backgroundColor.name().replace('#', "0x"));
            galleryElem.setAttribute(QString::fromLatin1("frameWidth"),           d->settings->frameWidth);
            galleryElem.setAttribute(QString::fromLatin1("stagePadding"),         d->settings->stagePadding);
            galleryElem.setAttribute(QString::fromLatin1("thumbnailColumns"),     d->settings->thumbnailColumns);
            galleryElem.setAttribute(QString::fromLatin1("thumbnailRows"),        d->settings->thumbnailRows);
            galleryElem.setAttribute(QString::fromLatin1("navPosition"),          d->settings->thumbPosition());
            galleryElem.setAttribute(QString::fromLatin1("title"),                d->settings->title);
            galleryElem.setAttribute(QString::fromLatin1("imagePath"),            QString("images/"));
            galleryElem.setAttribute(QString::fromLatin1("thumbPath"),            QString("thumbs/"));
            break;
        }
        case 1:
        {
            galleryElem = xmlDoc.createElement(QString::fromLatin1("gallery"));
            xmlDoc.appendChild( galleryElem );
            galleryElem.setAttribute(QString::fromLatin1("enableRightClickOpen"), d->settings->enableRightClickToOpen());
            galleryElem.setAttribute(QString::fromLatin1("imagePadding"),         d->settings->imagePadding);
            galleryElem.setAttribute(QString::fromLatin1("displayTime"),          d->settings->displayTime);
            galleryElem.setAttribute(QString::fromLatin1("frameWidth"),           d->settings->frameWidth);
            galleryElem.setAttribute(QString::fromLatin1("frameColor"),           d->settings->frameColor.name().replace('#', "0x"));
            galleryElem.setAttribute(QString::fromLatin1("bgColor"),              d->settings->backgroundColor.name().replace('#', "0x"));
            break;
        }
        case 2:
        {
            galleryElem = xmlDoc.createElement(QString::fromLatin1("tiltviewergallery"));
            xmlDoc.appendChild( galleryElem );
            photosElem  = xmlDoc.createElement(QString::fromLatin1("photos"));
            galleryElem.appendChild( photosElem);
            break;
        }
        case 3:
        {
            galleryElem = xmlDoc.createElement(QString::fromLatin1("gallery"));
            xmlDoc.appendChild( galleryElem );
            galleryElem.setAttribute(QString::fromLatin1("enableRightClickOpen"), d->settings->enableRightClickToOpen());
            galleryElem.setAttribute(QString::fromLatin1("cellDimension"),        d->settings->cellDimension);
            galleryElem.setAttribute(QString::fromLatin1("columns"),              d->settings->thumbnailColumns);
            galleryElem.setAttribute(QString::fromLatin1("captionColor"),         d->settings->textColor.name().replace('#', "0x"));
            galleryElem.setAttribute(QString::fromLatin1("zoomInPerc"),           d->settings->zoomInPerc);
            galleryElem.setAttribute(QString::fromLatin1("zoomOutPerc"),          d->settings->zoomOutPerc);
            galleryElem.setAttribute(QString::fromLatin1("frameWidth"),           d->settings->frameWidth);
            galleryElem.setAttribute(QString::fromLatin1("frameColor"),           d->settings->frameColor.name().replace('#', "0x"));
            galleryElem.setAttribute(QString::fromLatin1("bgColor"),              d->settings->backgroundColor.name().replace('#', "0x"));
            break;
        }

        default:
            break;
    }
    if(d->settings->imgGetOption ==0)
    {
        for( QList<ImageCollection>::ConstIterator it = d->settings->collections.constBegin() ;
            !d->canceled && (it != d->settings->collections.constEnd()) ; ++it )
        {
            KUrl::List images = (*it).images();
            processKUrlList(images,xmlDoc,galleryElem,photosElem);
        }
    }
    else
    {
        KUrl::List images = d->settings->imageDialogList;
        processKUrlList(images,xmlDoc,galleryElem,photosElem);
    }

    QByteArray data(xmlDoc.toByteArray());
    QDataStream stream( &file );
    stream.writeRawData(data.data(), data.size());
    file.close();

    return true;
}
void SimpleViewer::processKUrlList(KUrl::List& images, QDomDocument& xmlDoc,
                                   QDomElement& galleryElem, QDomElement& photosElem)
{
    KPMetadata meta;
    QImage     image;
    QImage     thumbnail;
    QString    tmp;
    QString    newName;

    int index           = 1;
    int maxSize         = d->settings->imagesExportSize;
    bool resizeImages   = d->settings->resizeExportImages;
    bool fixOrientation = d->settings->fixOrientation;

    KUrl thumbsDir(d->tempDir->name());
    thumbsDir.addPath("/thumbs");

    KUrl imagesDir(d->tempDir->name());
    imagesDir.addPath("/images");

    qSort(images.begin(), images.end(), cmpUrl);

    for(KUrl::List::ConstIterator it = images.constBegin();
        !d->canceled && (it != images.constEnd()) ; ++it)
    {
        kapp->processEvents();
        KUrl url = *it;
        QFileInfo fi(url.path());

        //video can't be exported, need to add for all video files
        if(fi.suffix().toUpper() == "MOV")
            continue;

        d->progressWdg->addedAction(i18n("Processing %1", url.fileName()), StartingMessage);

        // Check if RAW file.
        if (KPMetadata::isRawFile(url))
            KDcrawIface::KDcraw::loadRawPreview(image, url.path());
        else
            image.load(url.path());

        if(image.isNull())
        {
            d->progressWdg->addedAction(i18n("Could not open image '%1'", url.fileName()),
                                        WarningMessage);
            continue;
        }

        if(d->settings->plugType == 0)
        {
            // Thumbnails are generated only for simpleviewer plugin

            if(!createThumbnail(image, thumbnail))
            {
                d->progressWdg->addedAction(i18n("Could not create thumbnail from '%1'", url.fileName()),
                                        WarningMessage);
                continue;
            }
        }

        if(resizeImages && !resizeImage(image, maxSize, image))
        {
            d->progressWdg->addedAction(i18n("Could not resize image '%1'", url.fileName()),
                                        WarningMessage);
            continue;
        }

        meta.load(url.path());
        bool rotated = false;
        newName      = QString("%1.%2").arg(tmp.sprintf("%03i", index)).arg(QString("jpg"));

        if(d->settings->plugType == 0)
        {
            KUrl thumbnailPath(thumbsDir);
            thumbnailPath.addPath(newName);

            if (resizeImages && fixOrientation)
                meta.rotateExifQImage(thumbnail, meta.getImageOrientation());

            thumbnail.save(thumbnailPath.path(), "JPEG");
        }

        KUrl imagePath(imagesDir);
        imagePath.addPath(newName);

        if (resizeImages && fixOrientation)
            rotated = meta.rotateExifQImage(image, meta.getImageOrientation());

        image.save(imagePath.path(), "JPEG");

        // Backup metadata from original image.
        meta.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
        meta.setImageDimensions(image.size());

        if (rotated)
            meta.setImageOrientation(KPMetadata::ORIENTATION_NORMAL);

        meta.save(imagePath.path());

        d->width  = image.width();
        d->height = image.height();

        if(d->settings->plugType!=2)
            cfgAddImage(xmlDoc, galleryElem, url, newName);
        else
            cfgAddImage(xmlDoc, photosElem, url, newName);

        d->progressWdg->setProgress(++d->action, d->totalActions);
        index++;
    }
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

    KPImageInfo info(url);

    if(d->settings->showComments)
    {
        comment = info.description();
    }
    else
    {
        comment.clear();
    }
    if(d->settings->showKeywords)
    {
        QStringList tagList = info.keywords();
        if(!tagList.join(" ").isEmpty())
            keywords = QString("\nTags: ")+tagList.join(", ");
    }
    else
    {
        keywords.clear();
    }

    switch(d->settings->plugType)
    {
        case 0: //Simpleviewer
        {

            QDomElement img = xmlDoc.createElement(QString::fromLatin1("image"));
            galleryElem.appendChild(img);
            img.setAttribute(QString::fromLatin1("imageURL"),QString("images/")+newName);
            img.setAttribute(QString::fromLatin1("thumbURL"),QString("thumbs/")+newName);
            img.setAttribute(QString::fromLatin1("linkURL"),QString());
            img.setAttribute(QString::fromLatin1("targetURL"),QString());

            QDomElement caption1 = xmlDoc.createElement(QString::fromLatin1("caption"));
            img.appendChild(caption1);

            QDomText captiontxt1 = xmlDoc.createTextNode(comment+keywords);
            caption1.appendChild(captiontxt1);
            break;
        }

        case 1: //Autoviewer
        {
            QDomElement img      = xmlDoc.createElement(QString::fromLatin1("image"));

            galleryElem.appendChild(img);

            QDomElement urlElem  = xmlDoc.createElement(QString::fromLatin1("url"));
            img.appendChild(urlElem);
            QDomText    urlText  = xmlDoc.createTextNode(QString("images/")+ newName);
            urlElem.appendChild(urlText);

            QDomElement caption2 = xmlDoc.createElement(QString::fromLatin1("caption"));
            img.appendChild( caption2 );
            QDomText captiontxt2 = xmlDoc.createTextNode(comment+keywords);
            caption2.appendChild(captiontxt2);

            QDomElement widthElem = xmlDoc.createElement(QString::fromLatin1("width"));
            img.appendChild(widthElem);

            QDomText    widthText = xmlDoc.createTextNode(QString::number(d->width));

            widthElem.appendChild(widthText);

            QDomElement heightElem = xmlDoc.createElement(QString::fromLatin1("height"));
            img.appendChild(heightElem);

            QDomText    heightText = xmlDoc.createTextNode(QString::number(d->height));
            heightElem.appendChild(heightText);
            break;
        }

        case 2: //TiltWiever
        {
            QDomElement img       = xmlDoc.createElement(QString::fromLatin1("photo"));
            galleryElem.appendChild(img);
            img.setAttribute(QString::fromLatin1("imageurl"),QString("images/")+newName);
            img.setAttribute(QString::fromLatin1("linkurl"),QString());

            QDomElement titleElem = xmlDoc.createElement(QString::fromLatin1("title"));
            img.appendChild(titleElem);
            QDomText    titleText = xmlDoc.createTextNode(newName);
            titleElem.appendChild(titleText);

            QDomElement caption1  = xmlDoc.createElement(QString::fromLatin1("description"));
            img.appendChild(caption1);

            QDomText captiontxt1  = xmlDoc.createTextNode(comment+keywords);
            caption1.appendChild(captiontxt1);
            break;
        }

        case 3: //PostcardViewer
        {
            QDomElement img     = xmlDoc.createElement(QString::fromLatin1("image"));
            galleryElem.appendChild(img);

            QDomElement urlElem = xmlDoc.createElement(QString::fromLatin1("url"));
            img.appendChild(urlElem);
            QDomText    urlText = xmlDoc.createTextNode(QString("images/")+ newName);
            urlElem.appendChild(urlText);

            QDomElement caption2 = xmlDoc.createElement(QString::fromLatin1("caption"));
            img.appendChild( caption2 );
            QDomText captiontxt2 = xmlDoc.createTextNode(comment+keywords);
            caption2.appendChild(captiontxt2);
            break;
        }

        default:
            break;
    }
}

bool SimpleViewer::createIndex() const
{
    if(d->canceled)
        return false;

    d->progressWdg->addedAction(i18n("Creating index.html..."), StartingMessage);

    switch(d->settings->plugType)
    {
        case 0:
        {
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

            indexTemplate.replace("{TITLE}",    d->settings->title);
            indexTemplate.replace("{COLOR}",    d->settings->textColor.name());
            indexTemplate.replace("{BGCOLOR}",  d->settings->backgroundColor.name());
            indexTemplate.replace("{HOSTURL}",  d->hostUrl);
            indexTemplate.replace("{HOSTNAME}", d->hostName);

            QFile outfile(d->tempDir->name() + "/index.html");
            outfile.open(QIODevice::WriteOnly);
            QTextStream out(&outfile);
            out << indexTemplate;
            outfile.close();
            break;
        }

        case 1:
        {
            QString indexTemplateName = KStandardDirs::locate("data", "kipiplugin_flashexport/index2.template");
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

            indexTemplate.replace("{TITLE}",    d->settings->title);
            indexTemplate.replace("{COLOR}",    d->settings->textColor.name());
            indexTemplate.replace("{BGCOLOR}",  d->settings->backgroundColor.name());
            //indexTemplate.replace("{HOSTNAME}", d->hostName);
            //indexTemplate.replace("{HOSTURL}",  d->hostUrl);

            QFile outfile(d->tempDir->name() + "/index.html");
            outfile.open(QIODevice::WriteOnly);
            QTextStream out(&outfile);
            out << indexTemplate;
            outfile.close();
            break;
        }

        case 2:
        {
            QString indexTemplateName = KStandardDirs::locate("data", "kipiplugin_flashexport/index3.template");
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

            indexTemplate.replace("{TITLE}",    d->settings->title);
            indexTemplate.replace("{COLOR}",    d->settings->textColor.name());
            if(d->settings->useReloadButton)
                indexTemplate.replace("{RELOADBUTTON}",  QString("true"));
            else
                indexTemplate.replace("{RELOADBUTTON}",  QString("false"));
            if(d->settings->showFlipButton)
                indexTemplate.replace("{FLIPBUTTON}",  QString("true"));
            else
                indexTemplate.replace("{FLIPBUTTON}",  QString("false"));

            indexTemplate.replace("{SIZE}",        QString::number(d->settings->maxImageDimension));
            indexTemplate.replace("{COLUMN}",      QString::number(d->settings->thumbnailColumns));
            indexTemplate.replace("{ROW}",         QString::number(d->settings->thumbnailRows));
            indexTemplate.replace("{FRAMECOLOR}",  d->settings->frameColor.name().replace('#', "0x"));
            indexTemplate.replace("{BACKINCOLOR}", d->settings->bkgndInnerColor.name().replace('#', "0x"));
            indexTemplate.replace("{BACKCOLOR}" ,   d->settings->backColor.name().replace('#', "0x"));
            indexTemplate.replace("{BACKOUTCOLOR}", d->settings->bkgndOuterColor.name().replace('#', "0x"));

            QFile outfile(d->tempDir->name() + "/index.html");
            outfile.open(QIODevice::WriteOnly);
            QTextStream out(&outfile);
            out << indexTemplate;
            outfile.close();
            break;
        }
        case 3:
        {
            QString indexTemplateName = KStandardDirs::locate("data", "kipiplugin_flashexport/index4.template");
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

            indexTemplate.replace("{TITLE}",    d->settings->title);
            indexTemplate.replace("{COLOR}",    d->settings->textColor.name());
            indexTemplate.replace("{BGCOLOR}",  d->settings->backgroundColor.name());

            QFile outfile(d->tempDir->name() + "/index.html");
            outfile.open(QIODevice::WriteOnly);
            QTextStream out(&outfile);
            out << indexTemplate;
            outfile.close();
            break;
        }

        default:
            break;
    }

    d->progressWdg->setProgress(++d->action, d->totalActions);
    d->progressWdg->addedAction(i18n("index.html created..."), SuccessMessage);

    return true;
}

bool SimpleViewer::copySimpleViewer() const
{
    if(d->canceled)
        return false;

    d->progressWdg->addedAction(i18n("Copying flash files..."), StartingMessage);

    // Due to its license, simpleviewer is installed in $KDEHOME
    KUrl::List  files;
    QStringList entries;
    QDir        dir;

    dir.setPath(d->dataDir);
    kDebug() << "Data dir is " << d->dataDir; 
    entries = dir.entryList(QDir::Files);
    for(QStringList::ConstIterator it = entries.constBegin(); it != entries.constEnd(); ++it)
    {
        files.append(KUrl(dir.absolutePath() + '/' + *it));
    }

    // TODO: catch errors
    /*KIO::CopyJob *copyJob =*/ KIO::copy(files, d->settings->exportUrl, KIO::HideProgressInfo);

    d->progressWdg->addedAction(i18n("Flash files copied..."), SuccessMessage);

    return true;
}

bool SimpleViewer::upload() const
{
    if(d->canceled)
        return false;

    d->progressWdg->addedAction(i18n("Uploading gallery..."), StartingMessage);

    if(!KIO::NetAccess::dircopy(KUrl(d->tempDir->name() + "./"), d->settings->exportUrl))
        return false;

    d->progressWdg->addedAction(i18n("Gallery uploaded..."), SuccessMessage);

    return true;
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

    if(entryFile == NULL)
        return false;

    QByteArray array              = entryFile->data();

    QFile file( d->dataLocal + entry->name() );
    if(file.open( QIODevice::WriteOnly ))
    {
        int ret = file.write(array);
        file.close();
        return ret > 0 ? true : false;
    }

    return false;
}

KPBatchProgressWidget* SimpleViewer::progressWidget() const
{
    initProgressWdg();
    return d->progressWdg;
}

void SimpleViewer::setSettings(SimpleViewerSettingsContainer* const setting)
{
    d->settings = setting;
    d->canceled = false;

    kDebug() << "Settings reached SimpleViewer";
}

} // namespace KIPIFlashExportPlugin
