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

#ifndef __KIPIIMAGECOLLECTIONSHARED_H
#define __KIPIIMAGECOLLECTIONSHARED_H

// libkipi includes

#include <libkipi/imagecollectionshared.h>

class KipiImageCollectionShared : public KIPI::ImageCollectionShared
{
public:

    // re-implemented inherited functions:
    KipiImageCollectionShared(const KUrl& albumPath);
    KipiImageCollectionShared(const KUrl::List& images);
    virtual ~KipiImageCollectionShared();

    virtual QString name();
    virtual KUrl::List images();
    virtual KUrl path();
    virtual KUrl uploadPath();
    virtual KUrl uploadRoot();
    virtual bool isDirectory();


    // functions used internally:
    void addImages(const KUrl::List& images);
    void addImage(const KUrl& image);

private:

    KUrl       m_albumPath;
    KUrl::List m_images;
};

#endif // __KIPIIMAGECOLLECTIONSHARED_H
