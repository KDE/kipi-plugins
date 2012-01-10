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

#include "kipiimagecollectionshared.h"

// Qt includes

#include <QDir>

// local includes:

#include "kipitest-debug.h"

KipiImageCollectionShared::KipiImageCollectionShared(const KUrl& albumPath)
    : KIPI::ImageCollectionShared(),
      m_albumPath(albumPath),
      m_images()
{
    // go through the album and add its images:
    const QString albumPathString = m_albumPath.path();

    // add only the files, because recursion through directories should be
    // handled in KipiInterface::add[Selected]Album
    // TODO: restrict the search to images!
    const QFileInfoList files     = QDir(albumPathString).entryInfoList(QDir::Files);

    for (QFileInfoList::const_iterator it = files.constBegin(); it!=files.constEnd(); ++it)
    {
            m_images.append(KUrl::fromPath(it->absoluteFilePath()));
    }
}

KipiImageCollectionShared::KipiImageCollectionShared(const KUrl::List& images)
    : KIPI::ImageCollectionShared(),
      m_images(images)
{
}

KipiImageCollectionShared::~KipiImageCollectionShared()
{
}

QString KipiImageCollectionShared::name()
{
    return m_albumPath.url();
}

KUrl::List KipiImageCollectionShared::images()
{
    return m_images;
}

void KipiImageCollectionShared::addImages(const KUrl::List& images)
{
    m_images.append(images);
}

void KipiImageCollectionShared::addImage(const KUrl& image)
{
    m_images.append(image);
}

KUrl KipiImageCollectionShared::path()
{
    return m_albumPath;
}

KUrl KipiImageCollectionShared::uploadPath()
{
    return m_albumPath;
}

KUrl KipiImageCollectionShared::uploadRoot()
{
    return m_albumPath;
}

bool KipiImageCollectionShared::isDirectory()
{
    return true;
}
