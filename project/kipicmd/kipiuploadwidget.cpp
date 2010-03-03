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

#include "kipiuploadwidget.moc"

// Qt includes:

#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>

// KDE includes

#include <klocale.h>

// local includes

#include "kipiinterface.h"
#include "kipitest-debug.h"

KipiUploadWidget::KipiUploadWidget(KipiInterface* interface, QWidget* parent)
    : KIPI::UploadWidget(parent), m_interface(interface), m_listWidget(0)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(i18n("Please select a target album:")));
    m_listWidget = new QListWidget(this);
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
}

KipiUploadWidget::~KipiUploadWidget()
{
}

KIPI::ImageCollection KipiUploadWidget::selectedImageCollection() const
{
    // return the selected albums (should be only one):
    const QList<QListWidgetItem *> selectedItems = m_listWidget->selectedItems();
    if (selectedItems.isEmpty())
    {
        // this should not happen!!! the calling application will probably crash now...
        kDebug() << "Nothing selected... impossible!";
        return KIPI::ImageCollection(0);
    }
    const int row = m_listWidget->row( selectedItems.at(0) );
    return m_allAlbums.at(row);
}

void KipiUploadWidget::on_m_listWidget_itemSelectionChanged()
{
    emit(selectionChanged());
}
