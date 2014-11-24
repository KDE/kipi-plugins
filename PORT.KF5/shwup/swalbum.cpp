/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009      by Timothée Groleau <kde at timotheegroleau dot com>
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

#include "swalbum.moc"

// Qt includes

#include <QFormLayout>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kcombobox.h>

// local includes

#include "switem.h"

namespace KIPIShwupPlugin
{

SwNewAlbum::SwNewAlbum(QWidget* const parent)
    : KDialog(parent)
{
    QString header(i18n("Shwup New Album"));
    setWindowTitle(header);
    setButtons(Ok|Cancel);
    setDefaultButton(Cancel);
    setModal(false);

    QWidget* const mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    mainWidget->setMinimumSize(300, 0);

    // ------------------------------------------------------------------------

    m_titleEdt = new KLineEdit;
    m_titleEdt->setWhatsThis(i18n("Title of the album that will be created (required)."));

    QFormLayout* const albumBoxLayout = new QFormLayout;
    albumBoxLayout->addRow(i18nc("album edit", "Title:"), m_titleEdt);
    albumBoxLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    albumBoxLayout->setSpacing(KDialog::spacingHint());
    albumBoxLayout->setMargin(KDialog::spacingHint());
    mainWidget->setLayout(albumBoxLayout);
}

SwNewAlbum::~SwNewAlbum()
{
}

void SwNewAlbum::getAlbumProperties(SwAlbum& album)
{
    album.title = m_titleEdt->text();
}

} // namespace KIPIShwupPlugin
