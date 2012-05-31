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

#ifndef __KIPIINTERFACE_H
#define __KIPIINTERFACE_H

// LibKIPI includes

#include <libkipi/interface.h>

class KFileItem;

namespace KIPI
{
    class ImageCollection;
    class ImageInfo;
}

namespace KIPIPlugins
{
    class KPRawThumbThread;
}

using namespace KIPI;
using namespace KIPIPlugins;

class KipiInterface : public Interface
{
    Q_OBJECT

private:

    KUrl::List m_selectedImages;
    KUrl::List m_selectedAlbums;
    KUrl::List m_albums;

public:

    KipiInterface(QObject* const parent, const char* name=0);
    ~KipiInterface();

    ImageCollection        currentAlbum();
    ImageCollection        currentSelection();
    QList<ImageCollection> allAlbums();
    ImageInfo              info(const KUrl&);

    bool addImage(const KUrl& url, QString& errmsg);
    void delImage(const KUrl& url);
    void refreshImages(const KUrl::List& urls);

    int      features() const;
    QVariant hostSetting(const QString& settingName);

    ImageCollectionSelector* imageCollectionSelector(QWidget* parent);
    UploadWidget*            uploadWidget(QWidget* parent);

    void addSelectedImages(const KUrl::List& images);
    void addSelectedImage(const KUrl& image);

    void addSelectedAlbums(const KUrl::List& albums);
    void addSelectedAlbum(const KUrl& album);

    void addAlbums(const KUrl::List& albums);
    void addAlbum(const KUrl& album);

    void thumbnails(const KUrl::List& list, int size);

private Q_SLOTS:

    void slotRawThumb(const KUrl&, const QImage&);
    void slotGotKDEPreview(const KFileItem&, const QPixmap&);
    void slotFailedKDEPreview(const KFileItem&);

private:

    KPRawThumbThread* m_loadRawThumb;

    friend class KipiUploadWidget;
    friend class KipiImageCollectionSelector;
};

#endif // __KIPIINTERFACE_H
