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

#ifndef __KIPIIMAGECOLLECTIONSELECTOR_H
#define __KIPIIMAGECOLLECTIONSELECTOR_H

// libkipi includes

#include <libkipi/imagecollectionselector.h>

class KipiInterface;
class QListWidget;

class KipiImageCollectionSelector : public KIPI::ImageCollectionSelector
{
Q_OBJECT

private:
    KipiInterface* m_interface;
    QListWidget* m_listWidget;
    QList<KIPI::ImageCollection> m_allAlbums;

public:
    KipiImageCollectionSelector(KipiInterface* interface, QWidget *parent = 0);
    virtual QList< KIPI::ImageCollection > 	selectedImageCollections () const;

public slots:
    void on_m_listWidget_itemSelectionChanged();
};


#endif // __KIPIIMAGECOLLECTIONSELECTOR_H

