/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-11-21
 * @brief  kipi host test application
 *
 * @author Copyright (C) 2009-2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 * @author Copyright (C) 2011-2012 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "kipiinterface.moc"

// Qt includes

#include <QTextStream>

// KDE includes

#include <klocale.h>
#include <kdeversion.h>
#include <kdebug.h>
#include <kfileitem.h>
#include <kio/previewjob.h>

// LibKIPI includes

#include <libkipi/version.h>
#include <libkipi/imagecollection.h>

// local includes

#include "kpmetadata.h"
#include "kprawthumbthread.h"
#include "kipiimageinfoshared.h"
#include "kipiimagecollectionselector.h"
#include "kipiuploadwidget.h"
#include "kipiimagecollectionshared.h"
#include "kipitest-debug.h"

KipiInterface::KipiInterface(QObject* const parent, const char* name)
    : Interface(parent, name),
      m_selectedImages(),
      m_selectedAlbums(),
      m_albums()
{
    m_loadRawThumb = new KPRawThumbThread(this);

    connect(m_loadRawThumb, SIGNAL(signalRawThumb(KUrl,QImage)),
            this, SLOT(slotRawThumb(KUrl,QImage)));
}

KipiInterface::~KipiInterface()
{
    //  m_currentSelection->removeRef();
}

ImageCollection KipiInterface::currentAlbum()
{
    kipiDebug("Called by plugins");

    KUrl currentAlbumUrl;
    if (!m_selectedAlbums.isEmpty())
    {
        currentAlbumUrl = m_selectedAlbums.at(0);
    }

    return ImageCollection(new KipiImageCollectionShared(currentAlbumUrl));
}

ImageCollection KipiInterface::currentSelection()
{
    kipiDebug("Called by plugins");
    return ImageCollection(new KipiImageCollectionShared(m_selectedImages));
}

QList<ImageCollection> KipiInterface::allAlbums()
{
    QList<ImageCollection> listAllAlbums;
    for (KUrl::List::const_iterator it = m_albums.constBegin(); it!=m_albums.constEnd(); ++it)
    {
        listAllAlbums.append(ImageCollection(new KipiImageCollectionShared(*it)));
    }

    // make sure albums which have been specified as selectedalbums are also in the allAlbums list:
    for (KUrl::List::const_iterator it = m_selectedAlbums.constBegin(); it!=m_selectedAlbums.constEnd(); ++it)
    {
        if (!m_albums.contains(*it))
        {
            listAllAlbums.append(ImageCollection(new KipiImageCollectionShared(*it)));
        }
    }

    return listAllAlbums;
}

ImageInfo KipiInterface::info(const KUrl& url)
{
    kipiDebug(QString( "Plugin wants information about image \"%1\"").arg( url.url() ));

    return ImageInfo( new KipiImageInfoShared(this, url ) );
}

bool KipiInterface::addImage(const KUrl& url, QString& errmsg)
{
    Q_UNUSED(errmsg);
    kipiDebug(QString( "Plugin added an image: \"%1\"").arg( url.url() ));
    return true;
}

void KipiInterface::delImage(const KUrl& url)
{
    kipiDebug(QString( "Plugin deleted an image: \"%1\"").arg( url.url() ));
}

void KipiInterface::refreshImages(const KUrl::List& urls)
{
    kipiDebug(QString( "Plugin asks to refresh %1 images:").arg( urls.size() ));
    for (KUrl::List::ConstIterator it = urls.constBegin(); it!=urls.constEnd(); ++it)
    {
        kipiDebug("  " + (*it).url());
    }
}

int KipiInterface::features() const
{
    kipiDebug("Called by plugins");
    return ImagesHasTime;
}

ImageCollectionSelector* KipiInterface::imageCollectionSelector(QWidget* parent)
{
    kipiDebug("Called by plugins");
    return new KipiImageCollectionSelector(this, parent);
}

UploadWidget* KipiInterface::uploadWidget(QWidget* parent)
{
    kipiDebug("Called by plugins");
    return new KipiUploadWidget(this, parent);
}

void KipiInterface::addSelectedImages(const KUrl::List& images)
{
    m_selectedImages.append(images);
}

void KipiInterface::addSelectedImage(const KUrl& image)
{
    m_selectedImages.append(image);
}

void KipiInterface::addAlbums(const KUrl::List& albums)
{
    for (KUrl::List::const_iterator it = albums.constBegin(); it!=albums.constEnd(); ++it)
    {
        addAlbum(*it);
    }
}

void KipiInterface::addAlbum(const KUrl& album)
{
    m_albums.append(album);

    // TODO: recurse through sub-directories?
}

void KipiInterface::addSelectedAlbums(const KUrl::List& albums)
{
    for (KUrl::List::const_iterator it = albums.constBegin(); it!=albums.constEnd(); ++it)
    {
        addSelectedAlbum(*it);
    }
}

void KipiInterface::addSelectedAlbum(const KUrl& album)
{
    m_selectedAlbums.append(album);

    // TODO: recurse through sub-directories?
}

QVariant KipiInterface::hostSetting(const QString& settingName)
{
    if (settingName == QString("UseXMPSidecar4Reading"))
    {
        return (QVariant::fromValue(true));
    }
    else if (settingName == QString("MetadataWritingMode"))
    {
        return (QVariant::fromValue((int)KPMetadata::WRITETOSIDECARONLY4READONLYFILES));
    }

    return QVariant();
}

void KipiInterface::thumbnails(const KUrl::List& list, int)
{
    foreach(const KUrl& url, list)
        m_loadRawThumb->getRawThumb(url);
}

void KipiInterface::slotRawThumb(const KUrl& url, const QImage& img)
{
    if (img.isNull())
    {
#if KDE_IS_VERSION(4,7,0)
        KFileItemList items;
        items.append(KFileItem(KFileItem::Unknown, KFileItem::Unknown, url, true));
        KIO::PreviewJob* job = KIO::filePreview(items, QSize(256, 256));
#else
        KIO::PreviewJob *job = KIO::filePreview(KUrl::List() << url, 256);
#endif

        connect(job, SIGNAL(gotPreview(KFileItem,QPixmap)),
                this, SLOT(slotGotKDEPreview(KFileItem,QPixmap)));

        connect(job, SIGNAL(failed(KFileItem)),
                this, SLOT(slotFailedKDEPreview(KFileItem)));
    }
    else
    {
        emit gotThumbnail(url, QPixmap::fromImage(img));
    }
}

void KipiInterface::slotGotKDEPreview(const KFileItem& item, const QPixmap& pix)
{
    emit gotThumbnail(item.url(), pix);
}

void KipiInterface::slotFailedKDEPreview(const KFileItem& item)
{
    emit gotThumbnail(item.url(), QPixmap());
}
