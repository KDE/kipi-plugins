/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#include "gdalbum.moc"

// Qt includes

#include <QFormLayout>

// KDE includes

#include <klocale.h>
#include <klineedit.h>
#include <kdialog.h>

// Local includes

#include "gditem.h"

namespace KIPIGoogleDrivePlugin
{

GDNewAlbum::GDNewAlbum(QWidget* const parent):KDialog(parent)
{
    QString header(i18n("Google Drive New Album"));
    setWindowTitle(header);
    setButtons(Ok|Cancel);
    setDefaultButton(Cancel);
    setModal(false);

    QWidget* const mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    mainWidget->setMinimumSize(300,0);

    //--------------------------------------------------

    m_titleEdt = new KLineEdit;
    m_titleEdt->setWhatsThis("This is title of folder that will be created");

    QFormLayout* const albumBoxLayout = new QFormLayout;
    albumBoxLayout->addRow(i18nc("folder edit","Title:"),m_titleEdt);
    albumBoxLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    albumBoxLayout->setSpacing(KDialog::spacingHint());
    albumBoxLayout->setMargin(KDialog::spacingHint());
    mainWidget->setLayout(albumBoxLayout);
}

GDNewAlbum::~GDNewAlbum()
{
}

void GDNewAlbum::getAlbumTitle(GDFolder& album)
{
    album.title = m_titleEdt->text();
}

} // namespace KIPIGoogleDrivePlugin
