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

#ifndef __KIPIIMAGECOLLECTIONSELECTOR_H
#define __KIPIIMAGECOLLECTIONSELECTOR_H

// libkipi includes

#include <libkipi/imagecollectionselector.h>

class QListWidget;

class KipiInterface;

class KipiImageCollectionSelector : public KIPI::ImageCollectionSelector
{
    Q_OBJECT

public:

    KipiImageCollectionSelector(KipiInterface* interface, QWidget* parent = 0);
    virtual QList< KIPI::ImageCollection > selectedImageCollections () const;

public Q_SLOTS:

    void on_m_listWidget_itemSelectionChanged();

private:

    KipiInterface*               m_interface;
    QListWidget*                 m_listWidget;
    QList<KIPI::ImageCollection> m_allAlbums;
};

#endif // __KIPIIMAGECOLLECTIONSELECTOR_H
