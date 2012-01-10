/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2012-01-05
 * Description : a widget to find missing binaries.
 *
 * Copyright (C) 2012-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "binarysearch.moc"

// Qt includes

#include <QLabel>
#include <QGridLayout>

// KDE includes

#include <kvbox.h>
#include <klocale.h>

// Local includes

#include "binaryiface.h"

namespace KIPIPlugins
{

struct BinarySearch::BinarySearchPriv
{
    BinarySearchPriv()
        : binariesFound(false)
    {
    }

    bool                binariesFound;

    QGridLayout*        layout;
    QList<QWidget*>     binaryWidgets;
    QList<BinaryIface*> binaryIfaces;
    QLabel*             downloadLabel;

};

BinarySearch::BinarySearch(QWidget* parent)
            : QGroupBox(parent), d(new BinarySearchPriv)
{
    d->layout = new QGridLayout;
    setLayout(d->layout);

    d->downloadLabel = new QLabel(parentWidget());
}

BinarySearch::~BinarySearch()
{
    delete d;
}

void BinarySearch::addBinary(BinaryIface& binary)
{
    delete d->downloadLabel;

    QWidget* w = binary.binaryFileStatusWidget(parentWidget(), d->layout, d->binaryWidgets.size());
    d->binaryWidgets.append(w);
    d->binaryIfaces.append(&binary);
    connect(&binary, SIGNAL(signalBinaryValid(bool)),
            this, SLOT(slotAreBinariesFound(bool)));
    connect(&binary, SIGNAL(signalSearchDirectoryAdded(QString)),
            this, SIGNAL(signalAddDirectory(QString)));
    connect(&binary, SIGNAL(signalSearchDirectoryAdded(QString)),
            this, SLOT(signalAddDirectory(QString)));

    d->downloadLabel = new QLabel(i18n(
        "<font color=\"red\">Warning:</font> The necessary binaries have not been found on "
        "your system. If you have these binaries installed, please click the 'Find' button to locate them on your "
        "system, otherwise please download them to proceed."), parentWidget());
    d->downloadLabel->hide();
}

void BinarySearch::addDirectory(const QString& dir)
{
    emit(signalAddDirectory(dir));
}

bool BinarySearch::allBinariesFound()
{
    foreach(BinaryIface* biniface, d->binaryIfaces)
    {
        if (!biniface->isValid())
        {
            return false;
        }
    }
    return true;
}

void BinarySearch::slotAreBinariesFound(bool)
{
    d->binariesFound = allBinariesFound();
    if (d->binariesFound)
    {
        d->downloadLabel->hide();
    }
    else
    {
        d->downloadLabel->show();
    }
    emit signalBinariesFound(d->binariesFound);
}

} // namespace KIPIPlugins
