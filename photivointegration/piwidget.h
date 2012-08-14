/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-08-04
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
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

#ifndef KIPI_PIWIDGET_H
#define KIPI_PIWIDGET_H

// Qt includes

#include <qwidget.h>

// KDE includes

#include <kdebug.h>

// forward declaration:
class QTreeWidgetItem;

namespace KIPIPhotivoIntegrationPlugin
{

// forward declaration:
class PIImgList;

// ----------------------------------------------------------------------------

class PIWidget : public QWidget
{
    Q_OBJECT

public:

    PIWidget(QWidget* const parent = 0);
    ~PIWidget();

    PIImgList* imagesList() const;

public Q_SLOTS:

    void slotCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

private:

    // pImpl
    class PIWidgetPriv;
    PIWidgetPriv& d;
};

// ----------------------------------------------------------------------------

} // namespace KIPIPhotivoIntegrationPlugin

#endif // KIPI_PIWIDGET_H

