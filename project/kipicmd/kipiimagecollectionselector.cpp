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

#include "kipiimagecollectionselector.moc"

// Qt includes:

#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>

// KDE includes

#include <klocale.h>

// local includes:

#include "kipiinterface.h"
#include "kipitest-debug.h"

KipiImageCollectionSelector::KipiImageCollectionSelector(KipiInterface* interface, QWidget *parent)
: KIPI::ImageCollectionSelector(parent), m_interface(interface), m_listWidget(0)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(i18n("Please select albums:")));
    m_listWidget = new QListWidget(this);
    m_listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    layout->addWidget(m_listWidget);

    setLayout(layout);

    connect(m_listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(on_m_listWidget_itemSelectionChanged()));

    // add all albums to the list widget:
    m_allAlbums = m_interface->allAlbums();
    for (QList<KIPI::ImageCollection>::const_iterator it = m_allAlbums.constBegin(); it!=m_allAlbums.constEnd(); ++it)
    {
        m_listWidget->addItem(it->name());

        // is the album selected?
        const KUrl itemPath = it->path();
        m_listWidget->item(m_listWidget->count()-1)->setSelected(m_interface->m_selectedAlbums.contains(itemPath));
    }
    //TODO: select albums specified on the command line
}

QList< KIPI::ImageCollection > 	KipiImageCollectionSelector::selectedImageCollections () const
{
    // return the selected albums:
    QList< KIPI::ImageCollection > result;
    const QList<QListWidgetItem *> selectedItems = m_listWidget->selectedItems();
    for (QList<QListWidgetItem *>::const_iterator it = selectedItems.constBegin(); it!=selectedItems.constEnd(); ++it)
    {
        const int row = m_listWidget->row(*it);
        result.append(m_allAlbums.at(row));
    }
    //  result.append(m_interface->currentAlbum());
    kipiDebug(QString("selectedImageCollections: %1 items selected").arg(result.size()));
    return result;
}

void KipiImageCollectionSelector::on_m_listWidget_itemSelectionChanged()
{
    emit(selectionChanged());
}
