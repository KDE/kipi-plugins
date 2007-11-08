/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : images list settings page.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGES_PAGE_H
#define IMAGES_PAGE_H

// Qt includes.

#include <QWidget>

// KDE includes.

#include <kurl.h>

namespace KIPI
{
    class Interface;
}

namespace KIPISendimagesPlugin
{

class ImagesPagePriv;

class ImagesPage : public QWidget
{
    Q_OBJECT
    
public:

    ImagesPage(QWidget* parent, KIPI::Interface *iface);
    ~ImagesPage();

    void addImages(const KUrl::List& list);

private slots:

    void slotAddItems();
    void slotRemoveItems();

private:

    ImagesPagePriv* d;
};

}  // namespace KIPISendimagesPlugin

#endif // IMAGES_PAGE_H 
