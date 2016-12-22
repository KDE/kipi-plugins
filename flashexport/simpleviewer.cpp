/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "simpleviewer.h"

// Qt includes

#include <QByteArray>
#include <QDataStream>
#include <QDir>
#include <QPointer>
#include <QDomText>
#include <QFile>
#include <QPointer>
#include <QTemporaryDir>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/PluginLoader>

// Local includes

#include "kpversion.h"
#include "kpimageinfo.h"
#include "kipiplugins_debug.h"
#include "kputil.h"

namespace KIPIFlashExportPlugin
{

static QDir makeWritableDir(const QString& subpath)
{
    QString path = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)).filePath(subpath);
    QDir().mkpath(path);

    return QDir(path);
}

class SimpleViewer::Private
{
public:

    Private() :
        maxThumbSize(45),
        viewer(QLatin1String("simpleviewer.swf"))
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

    QTemporaryDir*                    tempDir;

    Interface*                        interface;
    QPointer<MetadataProcessor>       meta;

    KPBatchProgressWidget*            progressWdg;

    SimpleViewerSettingsContainer*    settings;
};

SimpleViewer::SimpleViewer(Interface* const interface, QObject* const parent)
    : QObject(parent),
      d(new Private)
{
    d->interface = interface;

    if (d->interface)
    {
        d->meta = d->interface->createMetadataProcessor();
    }

    d->hostName  = QLatin1String("Kipi-plugins");
    d->hostUrl   = QLatin1String("https://projects.kde.org/projects/extragear/graphics/kipi-plugins");
}

SimpleViewer::~SimpleViewer()
{
    delete d->tempDir;
    delete d;
}

void SimpleViewer::appendPluginFiles(int pluginType)
{
    qCDebug(KIPIPLUGINS_LOG) << "Value of plugin type in append files" << pluginType;

    switch (pluginType)
    {
        case 0:
            d->simpleViewerFiles.clear();
            d->simpleViewerFiles.append(QLatin1String("web/svcore/swf/simpleviewer.swf"));
            d->simpleViewerFiles.append(QLatin1String("web/svcore/js/swfobject.js"));
            d->dataLocal = makeWritableDir(QLatin1String("kipiplugin_flashexport/simpleviewer/")).absolutePath() + QLatin1String("/");
            d->dataDir   = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                  QLatin1String("kipiplugin_flashexport/simpleviewer/"), QStandardPaths::LocateDirectory);
            qCDebug(KIPIPLUGINS_LOG) << "Data dir when set is " << d->dataDir;
            break;
        case 1:
            d->simpleViewerFiles.clear();
            d->simpleViewerFiles.append(QLatin1String("autoviewer.swf"));
            d->simpleViewerFiles.append(QLatin1String("swfobject.js"));
            d->dataLocal = makeWritableDir(QLatin1String("kipiplugin_flashexport/autoviewer/")).absolutePath() + QLatin1String("/");
            d->dataDir   = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                  QLatin1String("kipiplugin_flashexport/autoviewer/"), QStandardPaths::LocateDirectory);
            break;
        case 2:
            d->simpleViewerFiles.clear();
            d->simpleViewerFiles.append(QLatin1String("TiltViewer.swf"));
            d->simpleViewerFiles.append(QLatin1String("swfobject.js"));
            d->dataLocal = makeWritableDir(QLatin1String("kipiplugin_flashexport/tiltviewer/")).absolutePath() + QLatin1String("/");
            d->dataDir   = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                  QLatin1String("kipiplugin_flashexport/tiltviewer/"), QStandardPaths::LocateDirectory);
            break;
        case 3:
            d->simpleViewerFiles.clear();
            d->simpleViewerFiles.append(QLatin1String("viewer.swf"));
            d->simpleViewerFiles.append(QLatin1String("swfobject.js"));
            d->dataLocal = makeWritableDir(QLatin1String("kipiplugin_flashexport/postcardviewer/")).absolutePath() + QLatin1String("/");
            d->dataDir   = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                  QLatin1String("kipiplugin_flashexport/postcardviewer/"), QStandardPaths::LocateDirectory);
            break;
        default:
            break;
    }
}

void SimpleViewer::initProgressWdg() const
{
    d->progressWdg = new KPBatchProgressWidget(QApplication::activeWindow());
    qCDebug(KIPIPLUGINS_LOG) << "progress dialog initialized";
}

void SimpleViewer::startExport()
{
    if (d->canceled)
        return;

     qCDebug(KIPIPLUGINS_LOG) << "SimpleViewer started...";
    // Estimate the number of actions for the KIPIPlugins progress dialog.
    d->progressWdg->addedAction(i18n("Initialising..."), StartingMessage);
    d->totalActions    = 0;
    d->action          = 0;
    d->progressWdg->reset();

    if (d->settings->imgGetOption == 0)
    {
        for( QList<ImageCollection>::ConstIterator it = d->settings->collections.constBegin() ;
            !d->canceled && (it != d->settings->collections.constEnd()) ; ++it )
        {
            d->totalActions += (*it).images().count();
        }
    }
    else
    {
        d->totalActions += d->settings->imageDialogList.count();
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
    if (d->canceled)
        return;

    if (!d->canceled && !createExportDirectories())
    {
            d->progressWdg->addedAction(i18n("Failed to create export directories"),
                                       ErrorMessage);
        return;
    }

    if (!d->canceled && !exportImages())
    {
        d->progressWdg->addedAction(i18n("Failed to export the images"),
                                   ErrorMessage);
        return;
    }

    if (!d->canceled && !createIndex())
    {
        d->progressWdg->addedAction(i18n("Failed to create index.html"),
                                   ErrorMessage);
        return;
    }

    if (!d->canceled && !copySimpleViewer())
    {
        d->progressWdg->addedAction(i18n("Failed to copy SimpleViewer files"),
                                   ErrorMessage);
        return;
    }

    if (!d->canceled && !upload())
    {
        d->progressWdg->addedAction(i18n("Failed to upload the gallery"),
                                   ErrorMessage);
        return;
    }

    if (d->canceled)
    {

        int ret = QMessageBox::warning(QApplication::activeWindow(),
                                       i18n("Export was canceled"),
                                       i18n("Do you want to delete files in %1 that have already been created?",
                                            d->settings->exportPath),
                                       QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No));
        if (ret == QMessageBox::Yes)
        {
            QDir delDir(d->settings->exportPath);
            delDir.removeRecursively();
        }
    }

    if (!d->canceled)
    {
        d->progressWdg->addedAction(i18nc("Flash export has finished", "Finished..."),
                                    SuccessMessage);
        emit signalProcessingDone();

        if (d->settings->openInBrowser)
            QDesktopServices::openUrl(QUrl::fromLocalFile(d->settings->exportPath + QLatin1String("index.html")));
    }
}

bool SimpleViewer::createExportDirectories() const
{
    delete d->tempDir;

    QString prefix = QDir::tempPath() + QLatin1Char('/') +
                     QLatin1String("kipi-flashexport-tmp-XXXXXX");

    d->tempDir = new QTemporaryDir(prefix);

    d->progressWdg->addedAction(i18n("Creating directories..."), StartingMessage);

    QString root = d->settings->exportPath;
    qCDebug(KIPIPLUGINS_LOG) << "export path is" << root;

    if (!QDir().mkpath(root))
    {
        d->progressWdg->addedAction(i18n("Could not create folder '%1'", root),
                                    ErrorMessage);
        return false;
    }

    if (d->settings->plugType == 0)
    {
        QString thumbsDir = d->tempDir->path() + QLatin1String("/thumbs");
        qCDebug(KIPIPLUGINS_LOG) << "image thumbs path is" << thumbsDir;

        if (!QDir().mkpath(thumbsDir))
        {
            d->progressWdg->addedAction(i18n("Could not create folder '%1'", thumbsDir),
                                        ErrorMessage);
            return false;
        }
    }

    QString imagesDir = d->tempDir->path() + QLatin1String("/images");
    qCDebug(KIPIPLUGINS_LOG) << "image folder path is" << imagesDir;

    if (!QDir().mkpath(imagesDir))
    {
        d->progressWdg->addedAction(i18n("Could not create folder '%1'", imagesDir),
                                    ErrorMessage);
        return false;
    }

    d->progressWdg->addedAction(i18n("Finished Creating directories..."), StartingMessage);
    d->progressWdg->setProgress(++d->action, d->totalActions);

    return true;
}

bool SimpleViewer::cmpUrl(const QUrl& url1, const QUrl& url2)
{
    QPointer<MetadataProcessor> meta = 0;
    PluginLoader* const pl           = PluginLoader::instance();

    if (pl)
    {
        Interface* const iface = pl->interface();

        if (iface)
            meta = iface->createMetadataProcessor();
    }

    if (!meta)
        return cmpUrlByName(url1, url2);

    if (!meta->load(url1))
        return cmpUrlByName(url1, url2);

    QDateTime clock1 = meta->getImageDateTime();

    if (!meta->load(url2))
        return cmpUrlByName(url1, url2);

    QDateTime clock2 = meta->getImageDateTime();

    if (clock1.isValid() || clock2.isValid())
    {
        return (clock1 < clock2);
    }

    return cmpUrlByName(url1, url2);
}

bool SimpleViewer::cmpUrlByName(const QUrl& url1, const QUrl& url2)
{
    QString name1 = url1.fileName();
    QString name2 = url2.fileName();

    return (name1 < name2);
}

bool SimpleViewer::exportImages()
{
    if (d->canceled)
        return false;

    d->progressWdg->addedAction(i18n("Creating images and thumbnails..."), StartingMessage);

    QFile file(d->tempDir->path() + QLatin1String("/gallery.xml"));
    file.open(QIODevice::WriteOnly);

    // header of gallery.xml
    QDomElement  galleryElem;
    QDomElement  photosElem;
    QDomDocument xmlDoc;
    xmlDoc.appendChild(xmlDoc.createProcessingInstruction( QLatin1String("xml"),
                       QLatin1String("version=\"1.0\" encoding=\"UTF-8\"") ) );

    switch (d->settings->plugType)
    {
        case 0:
        {
            galleryElem = xmlDoc.createElement(QLatin1String("simpleviewerGallery"));
            xmlDoc.appendChild( galleryElem );
            galleryElem.setAttribute(QLatin1String("enableRightClickOpen"),
                                     d->settings->enableRightClickToOpen());
            galleryElem.setAttribute(QLatin1String("maxImageWidth"),
                                     d->settings->maxImageDimension);
            galleryElem.setAttribute(QLatin1String("maxImageHeight"),
                                     d->settings->maxImageDimension);
            galleryElem.setAttribute(QLatin1String("textColor"),
                                     d->settings->textColor.name().replace(QLatin1Char('#'), QLatin1String("0x")));
            galleryElem.setAttribute(QLatin1String("frameColor"),
                                     d->settings->frameColor.name().replace(QLatin1Char('#'), QLatin1String("0x")));
            galleryElem.setAttribute(QLatin1String("bgColor"),
                                     d->settings->backgroundColor.name().replace(QLatin1Char('#'), QLatin1String("0x")));
            galleryElem.setAttribute(QLatin1String("frameWidth"),
                                     d->settings->frameWidth);
            galleryElem.setAttribute(QLatin1String("stagePadding"),
                                     d->settings->stagePadding);
            galleryElem.setAttribute(QLatin1String("thumbnailColumns"),
                                     d->settings->thumbnailColumns);
            galleryElem.setAttribute(QLatin1String("thumbnailRows"),
                                     d->settings->thumbnailRows);
            galleryElem.setAttribute(QLatin1String("navPosition"),
                                     d->settings->thumbPosition());
            galleryElem.setAttribute(QLatin1String("title"),                d->settings->title);
            galleryElem.setAttribute(QLatin1String("imagePath"),            QLatin1String("images/"));
            galleryElem.setAttribute(QLatin1String("thumbPath"),            QLatin1String("thumbs/"));
            break;
        }
        case 1:
        {
            galleryElem = xmlDoc.createElement(QLatin1String("gallery"));
            xmlDoc.appendChild( galleryElem );
            galleryElem.setAttribute(QLatin1String("enableRightClickOpen"),
                                     d->settings->enableRightClickToOpen());
            galleryElem.setAttribute(QLatin1String("imagePadding"),
                                     d->settings->imagePadding);
            galleryElem.setAttribute(QLatin1String("displayTime"),
                                     d->settings->displayTime);
            galleryElem.setAttribute(QLatin1String("frameWidth"),
                                     d->settings->frameWidth);
            galleryElem.setAttribute(QLatin1String("frameColor"),
                                     d->settings->frameColor.name().replace(QLatin1Char('#'), QLatin1String("0x")));
            galleryElem.setAttribute(QLatin1String("bgColor"),
                                     d->settings->backgroundColor.name().replace(QLatin1Char('#'), QLatin1String("0x")));
            break;
        }
        case 2:
        {
            galleryElem = xmlDoc.createElement(QLatin1String("tiltviewergallery"));
            xmlDoc.appendChild( galleryElem );
            photosElem  = xmlDoc.createElement(QLatin1String("photos"));
            galleryElem.appendChild( photosElem);
            break;
        }
        case 3:
        {
            galleryElem = xmlDoc.createElement(QLatin1String("gallery"));
            xmlDoc.appendChild( galleryElem );
            galleryElem.setAttribute(QLatin1String("enableRightClickOpen"),
                                     d->settings->enableRightClickToOpen());
            galleryElem.setAttribute(QLatin1String("cellDimension"),
                                     d->settings->cellDimension);
            galleryElem.setAttribute(QLatin1String("columns"),
                                     d->settings->thumbnailColumns);
            galleryElem.setAttribute(QLatin1String("captionColor"),
                                     d->settings->textColor.name().replace(QLatin1Char('#'), QLatin1String("0x")));
            galleryElem.setAttribute(QLatin1String("zoomInPerc"),
                                     d->settings->zoomInPerc);
            galleryElem.setAttribute(QLatin1String("zoomOutPerc"),
                                     d->settings->zoomOutPerc);
            galleryElem.setAttribute(QLatin1String("frameWidth"),
                                     d->settings->frameWidth);
            galleryElem.setAttribute(QLatin1String("frameColor"),
                                     d->settings->frameColor.name().replace(QLatin1Char('#'), QLatin1String("0x")));
            galleryElem.setAttribute(QLatin1String("bgColor"),
                                     d->settings->backgroundColor.name().replace(QLatin1Char('#'), QLatin1String("0x")));
            break;
        }

        default:
            break;
    }

    if (d->settings->imgGetOption == 0)
    {
        for( QList<ImageCollection>::ConstIterator it = d->settings->collections.constBegin() ;
            !d->canceled && (it != d->settings->collections.constEnd()) ; ++it )
        {
            QList<QUrl> images = (*it).images();
            processQUrlList(images, xmlDoc, galleryElem, photosElem);
        }
    }
    else
    {
        QList<QUrl> images = d->settings->imageDialogList;
        processQUrlList(images, xmlDoc, galleryElem, photosElem);
    }

    QByteArray data(xmlDoc.toByteArray());
    QDataStream stream( &file );
    stream.writeRawData(data.data(), data.size());
    file.close();

    return true;
}
void SimpleViewer::processQUrlList(QList<QUrl>& images, QDomDocument& xmlDoc,
                                   QDomElement& galleryElem, QDomElement& photosElem)
{
    QImage     image;
    QImage     thumbnail;
    QString    tmp;
    QString    newName;

    int index           = 1;
    int maxSize         = d->settings->imagesExportSize;
    bool resizeImages   = d->settings->resizeExportImages;
    bool fixOrientation = d->settings->fixOrientation;

    QUrl thumbsDir = QUrl::fromLocalFile(d->tempDir->path());
    thumbsDir.setPath(thumbsDir.path() + QLatin1String("/thumbs/"));

    QUrl imagesDir = QUrl::fromLocalFile(d->tempDir->path());
    imagesDir.setPath(imagesDir.path() + QLatin1String("/images/"));

    qSort(images.begin(), images.end(), cmpUrl);

    for (QList<QUrl>::ConstIterator it = images.constBegin();
         !d->canceled && (it != images.constEnd()) ; ++it)
    {
        QApplication::processEvents();
        QUrl url = *it;
        QFileInfo fi(url.toLocalFile());

        //video can't be exported, need to add for all video files
        if (fi.suffix().toUpper() == QLatin1String("MOV"))
            continue;

        d->progressWdg->addedAction(i18n("Processing %1", url.fileName()), StartingMessage);

        // Clear image.
        image = QImage();

        if (d->interface)
        {
            image = d->interface->preview(url);
        }

        if (image.isNull())
        {
            image.load(url.toLocalFile());
        }

        if (image.isNull())
        {
            d->progressWdg->addedAction(i18n("Could not open image '%1'", url.fileName()),
                                        WarningMessage);
            continue;
        }

        if (d->settings->plugType == 0)
        {
            // Thumbnails are generated only for simpleviewer plugin

            if (!createThumbnail(image, thumbnail))
            {
                d->progressWdg->addedAction(i18n("Could not create thumbnail from '%1'", url.fileName()),
                                            WarningMessage);
                continue;
            }
        }

        if (resizeImages && !resizeImage(image, maxSize, image))
        {
            d->progressWdg->addedAction(i18n("Could not resize image '%1'", url.fileName()),
                                        WarningMessage);
            continue;
        }

        if (d->meta && d->meta->load(url))
        {
            bool rotated = false;
            newName      = QString::fromUtf8("%1.%2").arg(tmp.sprintf("%03i", index)).arg(QLatin1String("jpg"));

            if (d->settings->plugType == 0)
            {
                QUrl thumbnailPath(thumbsDir);
                thumbnailPath.setPath(thumbnailPath.path() + newName);

                if (resizeImages && fixOrientation)
                    d->meta->rotateExifQImage(thumbnail, d->meta->getImageOrientation());

                thumbnail.save(thumbnailPath.toLocalFile(), "JPEG");
            }

            QUrl imagePath(imagesDir);
            imagePath.setPath(imagePath.path() + newName);

            if (resizeImages && fixOrientation)
                rotated = d->meta->rotateExifQImage(image, d->meta->getImageOrientation());

            image.save(imagePath.toLocalFile(), "JPEG");

            // Backup metadata from original image.
            d->meta->setImageProgramId(QLatin1String("Kipi-plugins"), kipipluginsVersion());
            d->meta->setImageDimensions(image.size());

            if (rotated)
                d->meta->setImageOrientation(MetadataProcessor::NORMAL);

            d->meta->save(imagePath, true);
        }

        d->width  = image.width();
        d->height = image.height();

        if (d->settings->plugType!=2)
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

    if (w > d->maxThumbSize || h > d->maxThumbSize)
    {
        if (w > h)
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

    if (w > maxSize || h > maxSize)
    {
        if (w > h)
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
                               const QUrl& url, const QString& newName) const
{
    if (d->canceled)
        return;

    QString comment;
    QString keywords;

    KPImageInfo info(url);

    if (d->settings->showComments)
    {
        comment = info.description();
    }
    else
    {
        comment.clear();
    }
    if (d->settings->showKeywords)
    {
        QStringList tagList = info.keywords();

        if (!tagList.join(QLatin1String(" ")).isEmpty())
            keywords = QLatin1String("\nTags: ") + tagList.join(QLatin1String(", "));
    }
    else
    {
        keywords.clear();
    }

    switch (d->settings->plugType)
    {
        case 0: //Simpleviewer
        {

            QDomElement img      = xmlDoc.createElement(QLatin1String("image"));
            galleryElem.appendChild(img);
            img.setAttribute(QLatin1String("imageURL"),  QLatin1String("images/") + newName);
            img.setAttribute(QLatin1String("thumbURL"),  QLatin1String("thumbs/") + newName);
            img.setAttribute(QLatin1String("linkURL"),   QString());
            img.setAttribute(QLatin1String("targetURL"), QString());

            QDomElement caption1 = xmlDoc.createElement(QLatin1String("caption"));
            img.appendChild(caption1);

            QDomText captiontxt1 = xmlDoc.createTextNode(comment+keywords);
            caption1.appendChild(captiontxt1);
            break;
        }

        case 1: //Autoviewer
        {
            QDomElement img        = xmlDoc.createElement(QLatin1String("image"));

            galleryElem.appendChild(img);

            QDomElement urlElem    = xmlDoc.createElement(QLatin1String("url"));
            img.appendChild(urlElem);
            QDomText    urlText    = xmlDoc.createTextNode(QLatin1String("images/") + newName);
            urlElem.appendChild(urlText);

            QDomElement caption2   = xmlDoc.createElement(QLatin1String("caption"));
            img.appendChild( caption2 );
            QDomText captiontxt2   = xmlDoc.createTextNode(comment+keywords);
            caption2.appendChild(captiontxt2);

            QDomElement widthElem  = xmlDoc.createElement(QLatin1String("width"));
            img.appendChild(widthElem);

            QDomText    widthText  = xmlDoc.createTextNode(QString::number(d->width));

            widthElem.appendChild(widthText);

            QDomElement heightElem = xmlDoc.createElement(QLatin1String("height"));
            img.appendChild(heightElem);

            QDomText    heightText = xmlDoc.createTextNode(QString::number(d->height));
            heightElem.appendChild(heightText);
            break;
        }

        case 2: //TiltWiever
        {
            QDomElement img       = xmlDoc.createElement(QLatin1String("photo"));
            galleryElem.appendChild(img);
            img.setAttribute(QLatin1String("imageurl"), QLatin1String("images/") + newName);
            img.setAttribute(QLatin1String("linkurl"), QString());

            QDomElement titleElem = xmlDoc.createElement(QLatin1String("title"));
            img.appendChild(titleElem);
            QDomText    titleText = xmlDoc.createTextNode(newName);
            titleElem.appendChild(titleText);

            QDomElement caption1  = xmlDoc.createElement(QLatin1String("description"));
            img.appendChild(caption1);

            QDomText captiontxt1  = xmlDoc.createTextNode(comment+keywords);
            caption1.appendChild(captiontxt1);
            break;
        }

        case 3: //PostcardViewer
        {
            QDomElement img      = xmlDoc.createElement(QLatin1String("image"));
            galleryElem.appendChild(img);

            QDomElement urlElem  = xmlDoc.createElement(QLatin1String("url"));
            img.appendChild(urlElem);
            QDomText    urlText  = xmlDoc.createTextNode(QLatin1String("images/") + newName);
            urlElem.appendChild(urlText);

            QDomElement caption2 = xmlDoc.createElement(QLatin1String("caption"));
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
    if (d->canceled)
        return false;

    d->progressWdg->addedAction(i18n("Creating index.html..."), StartingMessage);

    switch(d->settings->plugType)
    {
        case 0:
        {
            QString indexTemplateName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                               QLatin1String("kipiplugin_flashexport/index.template"));
            if (indexTemplateName.isEmpty())
            {
                //TODO: errormsg
                qCDebug(KIPIPLUGINS_LOG) << "No indexTemplateName" ;
                return false;
            }

            QFile infile(indexTemplateName);
            infile.open(QIODevice::ReadOnly);
            QTextStream in(&infile);
            QString indexTemplate = in.readAll();
            infile.close();

            indexTemplate.replace(QLatin1String("{TITLE}"),    d->settings->title);
            indexTemplate.replace(QLatin1String("{COLOR}"),    d->settings->textColor.name());
            indexTemplate.replace(QLatin1String("{BGCOLOR}"),  d->settings->backgroundColor.name());
            indexTemplate.replace(QLatin1String("{HOSTURL}"),  d->hostUrl);
            indexTemplate.replace(QLatin1String("{HOSTNAME}"), d->hostName);

            QFile outfile(d->tempDir->path() + QLatin1String("/index.html"));
            outfile.open(QIODevice::WriteOnly);
            QTextStream out(&outfile);
            out << indexTemplate;
            outfile.close();
            break;
        }

        case 1:
        {
            QString indexTemplateName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                               QLatin1String("kipiplugin_flashexport/index2.template"));
            if (indexTemplateName.isEmpty())
            {
                //TODO: errormsg
                qCDebug(KIPIPLUGINS_LOG) << "No indexTemplateName" ;
                return false;
            }

            QFile infile(indexTemplateName);
            infile.open(QIODevice::ReadOnly);
            QTextStream in(&infile);
            QString indexTemplate = in.readAll();
            infile.close();

            indexTemplate.replace(QLatin1String("{TITLE}"),    d->settings->title);
            indexTemplate.replace(QLatin1String("{COLOR}"),    d->settings->textColor.name());
            indexTemplate.replace(QLatin1String("{BGCOLOR}"),  d->settings->backgroundColor.name());
            //indexTemplate.replace("{HOSTNAME}", d->hostName);
            //indexTemplate.replace("{HOSTURL}",  d->hostUrl);

            QFile outfile(d->tempDir->path() + QLatin1String("/index.html"));
            outfile.open(QIODevice::WriteOnly);
            QTextStream out(&outfile);
            out << indexTemplate;
            outfile.close();
            break;
        }

        case 2:
        {
            QString indexTemplateName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                               QLatin1String("kipiplugin_flashexport/index3.template"));
            if (indexTemplateName.isEmpty())
            {
                //TODO: errormsg
                qCDebug(KIPIPLUGINS_LOG) << "No indexTemplateName" ;
                return false;
            }

            QFile infile(indexTemplateName);
            infile.open(QIODevice::ReadOnly);
            QTextStream in(&infile);
            QString indexTemplate = in.readAll();
            infile.close();

            indexTemplate.replace(QLatin1String("{TITLE}"),    d->settings->title);
            indexTemplate.replace(QLatin1String("{COLOR}"),    d->settings->textColor.name());

            if (d->settings->useReloadButton)
                indexTemplate.replace(QLatin1String("{RELOADBUTTON}"), QLatin1String("true"));
            else
                indexTemplate.replace(QLatin1String("{RELOADBUTTON}"), QLatin1String("false"));

            if (d->settings->showFlipButton)
                indexTemplate.replace(QLatin1String("{FLIPBUTTON}"), QLatin1String("true"));
            else
                indexTemplate.replace(QLatin1String("{FLIPBUTTON}"), QLatin1String("false"));

            indexTemplate.replace(QLatin1String("{SIZE}"),
                                  QString::number(d->settings->maxImageDimension));
            indexTemplate.replace(QLatin1String("{COLUMN}"),
                                  QString::number(d->settings->thumbnailColumns));
            indexTemplate.replace(QLatin1String("{ROW}"),
                                  QString::number(d->settings->thumbnailRows));
            indexTemplate.replace(QLatin1String("{FRAMECOLOR}"),
                                  d->settings->frameColor.name().replace(
                                      QLatin1Char('#'), QLatin1String("0x")));
            indexTemplate.replace(QLatin1String("{BACKINCOLOR}"),
                                  d->settings->bkgndInnerColor.name().replace(
                                      QLatin1Char('#'), QLatin1String("0x")));
            indexTemplate.replace(QLatin1String("{BACKCOLOR}"),
                                  d->settings->backColor.name().replace(
                                      QLatin1Char('#'), QLatin1String("0x")));
            indexTemplate.replace(QLatin1String("{BACKOUTCOLOR}"),
                                  d->settings->bkgndOuterColor.name().replace(
                                      QLatin1Char('#'), QLatin1String("0x")));

            QFile outfile(d->tempDir->path() + QLatin1String("/index.html"));
            outfile.open(QIODevice::WriteOnly);
            QTextStream out(&outfile);
            out << indexTemplate;
            outfile.close();
            break;
        }
        case 3:
        {
            QString indexTemplateName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                               QLatin1String("kipiplugin_flashexport/index4.template"));
            if (indexTemplateName.isEmpty())
            {
                //TODO: errormsg
                qCDebug(KIPIPLUGINS_LOG) << "No indexTemplateName" ;
                return false;
            }

            QFile infile(indexTemplateName);
            infile.open(QIODevice::ReadOnly);
            QTextStream in(&infile);
            QString indexTemplate = in.readAll();
            infile.close();

            indexTemplate.replace(QLatin1String("{TITLE}"),    d->settings->title);
            indexTemplate.replace(QLatin1String("{COLOR}"),    d->settings->textColor.name());
            indexTemplate.replace(QLatin1String("{BGCOLOR}"),  d->settings->backgroundColor.name());

            QFile outfile(d->tempDir->path() + QLatin1String("/index.html"));
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
    if (d->canceled)
        return false;

    d->progressWdg->addedAction(i18n("Copying flash files..."), StartingMessage);

    // Due to its license, simpleviewer is installed in $KDEHOME

    QDir dir;
    dir.setPath(d->dataDir);
    qCDebug(KIPIPLUGINS_LOG) << "Data dir is " << d->dataDir;

    QStringList files;
    QStringList  entries = dir.entryList(QDir::Files);
    qCDebug(KIPIPLUGINS_LOG) << "Files to copy " << entries;

    for (QStringList::ConstIterator it = entries.constBegin(); it != entries.constEnd(); ++it)
    {
        files.append(dir.absolutePath() + QLatin1Char('/') + *it);
    }

    if (!copyFiles(files, d->tempDir->path()))
        return false;

    d->progressWdg->addedAction(i18n("Flash files copied..."), SuccessMessage);

    return true;
}

bool SimpleViewer::upload() const
{
    if (d->canceled)
        return false;

    d->progressWdg->addedAction(i18n("Copying gallery..."), StartingMessage);

    if (!copyFolderRecursively(d->tempDir->path(), d->settings->exportPath))
        return false;

    d->progressWdg->addedAction(i18n("Gallery copied..."), SuccessMessage);

    return true;
}

bool SimpleViewer::copyFolderRecursively(const QString& srcPath, const QString& dstPath) const
{
    QDir srcDir(srcPath);
    QString newCopyPath = dstPath + QLatin1Char('/') + srcDir.dirName();

    if (!srcDir.mkpath(newCopyPath))
    {
        return false;
    }

    foreach (const QFileInfo& fileInfo, srcDir.entryInfoList(QDir::Files))
    {
        QString copyPath = newCopyPath + QLatin1Char('/') + fileInfo.fileName();

        if (!QFile::copy(fileInfo.filePath(), copyPath))
            return false;
    }

    foreach (const QFileInfo& fileInfo, srcDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        copyFolderRecursively(fileInfo.filePath(), newCopyPath);
    }

    return true;
}

bool SimpleViewer::copyFiles(const QStringList& srcPaths, const QString& dstPath) const
{
    foreach (const QString& path, srcPaths)
    {
        QFileInfo fileInfo(path);
        QString copyPath = dstPath + QLatin1Char('/') + fileInfo.fileName();

        if (!QFile::copy(fileInfo.filePath(), copyPath))
            return false;
    }

    return true;
}

bool SimpleViewer::unzip(const QString& url) const
{
    KZip zip(url);

    if (!openArchive(zip))
    {
        qCDebug(KIPIPLUGINS_LOG) << "Cannot open zip archive" ;
        return false;
    }

    return extractArchive(zip);
}

bool SimpleViewer::openArchive(KZip& zip) const
{
    if (!zip.open(QIODevice::ReadOnly))
    {
        qCDebug(KIPIPLUGINS_LOG) << "Open archive failed\n";
        return false;
    }

    return true;
}

bool SimpleViewer::extractArchive(KZip& zip) const
{
    // read root directory content
    QStringList names = zip.directory()->entries();

    if (names.count() != 1)
    {
        qCDebug(KIPIPLUGINS_LOG) << "Wrong SimpleViewer Version or corrupted archive" ;
        qCDebug(KIPIPLUGINS_LOG) << "Content of the archive root folder" << names ;
        return false;
    }

    // open root directory
    const KArchiveEntry* const root = zip.directory()->entry(names[0]);

    if (!root || !root->isDirectory())
    {
        qCDebug(KIPIPLUGINS_LOG) << "could not open " << names[0] << " of zipname" ;
        return false;
    }

    const KArchiveDirectory* const dir = dynamic_cast<const KArchiveDirectory*>(root);

    // extract the needed files from SimpleViewer archive
    for (QStringList::ConstIterator it = d->simpleViewerFiles.constBegin();
         it != d->simpleViewerFiles.constEnd(); ++it )
    {
        const KArchiveEntry* const entry = dir->entry(*it);

        if (!extractFile(entry))
        {
            //TODO error msg
            qCDebug(KIPIPLUGINS_LOG) << "could not open " << *it << " of zipname" ;
            return false;
        }
    }

    return true;
}

bool SimpleViewer::extractFile(const KArchiveEntry* entry) const
{
    if (!entry || !entry->isFile())
        return false;

    const KArchiveFile* const entryFile = dynamic_cast<const KArchiveFile*>(entry);

    if (entryFile == NULL)
        return false;

    QByteArray array = entryFile->data();

    QFile file( d->dataLocal + entry->name() );

    if (file.open( QIODevice::WriteOnly ))
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

    qCDebug(KIPIPLUGINS_LOG) << "Settings reached SimpleViewer";
}

} // namespace KIPIFlashExportPlugin
