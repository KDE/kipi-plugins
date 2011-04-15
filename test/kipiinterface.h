/* ============================================================
 *
 * Copyright (C) 2009,2010 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef __KIPIINTERFACE_H
#define __KIPIINTERFACE_H

// LibKIPI includes

#include <libkipi/version.h>
#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>
#include <libkipi/imageinfo.h>
#include <libkipi/imageinfoshared.h>
#include <libkipi/imagecollectionshared.h>

// local includes

#include "kipiimagecollectionshared.h"

class KipiInterface : public KIPI::Interface
{
    Q_OBJECT

private:
    KUrl::List m_selectedImages;
    KUrl::List m_selectedAlbums;
    KUrl::List m_albums;

public:
    KipiInterface(QObject* parent, const char *name=0);
    ~KipiInterface();

    KIPI::ImageCollection currentAlbum();
    KIPI::ImageCollection currentSelection();
    QList<KIPI::ImageCollection> allAlbums();
    KIPI::ImageInfo info( const KUrl& );

    bool addImage( const KUrl& url, QString& errmsg );
    void delImage( const KUrl& url );
    void refreshImages( const KUrl::List& urls );

    int features() const;
    QVariant hostSetting(const QString& settingName);

    KIPI::ImageCollectionSelector* imageCollectionSelector( QWidget *parent );
    KIPI::UploadWidget* uploadWidget( QWidget *parent);

    void addSelectedImages(const KUrl::List& images);
    void addSelectedImage(const KUrl& image);

    void addSelectedAlbums(const KUrl::List& albums);
    void addSelectedAlbum(const KUrl& album);

    void addAlbums(const KUrl::List& albums);
    void addAlbum(const KUrl& album);

    friend class KipiUploadWidget;
    friend class KipiImageCollectionSelector;
};

#endif // __KIPIINTERFACE_H
