/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#include "newalbumdialog.h"

// Qt includes

#include <QCheckBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

// KDE includes

#include <klocalizedstring.h>

namespace KIPIRajcePlugin
{

NewAlbumDialog::NewAlbumDialog(QWidget* const parent)
    : KPNewAlbumDialog(parent,
      QString::fromLatin1("Rajce.net"))
{
    getLocEdit()->hide();
    getDateTimeEdit()->hide();

    QGroupBox* const privBox = new QGroupBox(i18n("Visibility"), getMainWidget());
    privBox->setWhatsThis(i18n("Set the visibility of the album"));

    QLabel* const lbl   = new QLabel(i18n("Public"), privBox);

    m_albumVisible = new QCheckBox;
    m_albumVisible->setChecked(true);

    QGridLayout* const layout = new QGridLayout(privBox);

    layout->addWidget(lbl,             0, 0, 1, 1);
    layout->addWidget(m_albumVisible,  0, 1, 1, 1);

    addToMainLayout(privBox);
}

QString NewAlbumDialog::albumDescription() const
{
    return getDescEdit()->toPlainText();
}

QString NewAlbumDialog::albumName() const
{
    return getTitleEdit()->text();
}

bool NewAlbumDialog::albumVisible() const
{
    return m_albumVisible->isChecked();
}

} // namespace KIPIRajcePlugin
