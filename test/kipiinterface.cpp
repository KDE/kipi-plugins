/* ============================================================
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#include "kipiinterface.moc"

// Qt includes

#include <QTextStream>

// KDE includes

#include <klocale.h>

// local includes

#include "kipiimageinfoshared.h"
#include "kipiimagecollectionselector.h"
#include "kipiuploadwidget.h"
#include "kipitest-debug.h"

KipiInterface::KipiInterface(QObject *parent, const char *name)
: KIPI::Interface(parent, name), m_selectedImages(), m_selectedAlbums(), m_albums()
{
}

KipiInterface::~KipiInterface()
{
    //  m_currentSelection->removeRef();
}

KIPI::ImageCollection KipiInterface::currentAlbum()
{
    kipiDebug("KIPI::ImageCollection KipiInterface::currentAlbum()");
    KUrl currentAlbumUrl;
    if (!m_selectedAlbums.isEmpty())
    {
        currentAlbumUrl = m_selectedAlbums.at(0);
    }

    return KIPI::ImageCollection(new KipiImageCollectionShared(currentAlbumUrl));
}

KIPI::ImageCollection KipiInterface::currentSelection()
{
    kipiDebug("KIPI::ImageCollection KipiInterface::currentSelection()");
    return KIPI::ImageCollection(new KipiImageCollectionShared(m_selectedImages));
}

QList<KIPI::ImageCollection> KipiInterface::allAlbums()
{
    QList<KIPI::ImageCollection> listAllAlbums;
    for (KUrl::List::const_iterator it = m_albums.constBegin(); it!=m_albums.constEnd(); ++it)
    {
        listAllAlbums.append(KIPI::ImageCollection(new KipiImageCollectionShared(*it)));
    }

    // make sure albums which have been specified as selectedalbums are also in the allAlbums list:
    for (KUrl::List::const_iterator it = m_selectedAlbums.constBegin(); it!=m_selectedAlbums.constEnd(); ++it)
    {
        if (!m_albums.contains(*it))
        {
            listAllAlbums.append(KIPI::ImageCollection(new KipiImageCollectionShared(*it)));
        }
    }

    return listAllAlbums;
}

KIPI::ImageInfo KipiInterface::info( const KUrl& url)
{
    kipiDebug(QString( "KipiInterface::info: Plugin wants information about image \"%1\"").arg( url.url() ));

    return KIPI::ImageInfo( new KipiImageInfoShared(this, url ) );
}

bool KipiInterface::addImage( const KUrl& url, QString& errmsg )
{
    Q_UNUSED(errmsg);
    kipiDebug(QString( "KipiInterface::addImage: Plugin added an image: \"%1\"").arg( url.url() ));
    return true;
}

void KipiInterface::delImage( const KUrl& url)
{
    kipiDebug(QString( "KipiInterface::delImage: Plugin deleted an image: \"%1\"").arg( url.url() ));
}

void KipiInterface::refreshImages( const KUrl::List& urls)
{
    kipiDebug(QString( "KipiInterface::refreshImages: Plugin asks to refresh %1 images:").arg( urls.size() ));
    for (KUrl::List::ConstIterator it = urls.constBegin(); it!=urls.constEnd(); ++it)
    {
        kipiDebug("  " + (*it).url());
    }
}

int KipiInterface::features() const
{
    kipiDebug("KipiInterface::features");
    return KIPI::ImagesHasTime;
}

KIPI::ImageCollectionSelector* KipiInterface::imageCollectionSelector(QWidget *parent)
{
    kipiDebug("KipiInterface::imageCollectionSelector");
    return new KipiImageCollectionSelector(this, parent);
}

KIPI::UploadWidget* KipiInterface::uploadWidget(QWidget *parent)
{
    kipiDebug("KipiInterface::uploadWidget");
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
        return (QVariant::fromValue(3 /* KExiv2::WRITETOSIDECARONLY4READONLYFILES*/));
    }

    return QVariant();
}
