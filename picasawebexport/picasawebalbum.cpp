/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-10-27
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2010 by Jens Mueller <tschenser at gmx dot de>
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

#include "picasawebalbum.h"
#include "picasawebalbum.moc"

// Qt includes
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>

// KDE includes
#include <KLocale>
#include <KDialog>
#include <KComboBox>
#include <KLineEdit>
#include <KTextEdit>

// local includes
#include <picasawebitem.h>

namespace KIPIPicasawebExportPlugin
{

PicasawebNewAlbum::PicasawebNewAlbum(QWidget* parent)
           : KDialog(parent)
{
    QString header(i18n("Picasaweb New Album"));
    setWindowTitle(header);
    setButtons(Ok|Cancel);
    setDefaultButton(Cancel);
    setModal(false);

    QWidget *mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    mainWidget->setMinimumSize(400, 400);

    QGroupBox* albumBox = new QGroupBox(i18n("Album"), mainWidget);
    albumBox->setWhatsThis(
        i18n("These are basic settings for the new Picasaweb album."));

    m_titleEdt          = new KLineEdit;
    m_titleEdt->setWhatsThis(i18n("Title of the album that will be created (required)."));

    m_dtEdt             = new QDateTimeEdit(QDateTime::currentDateTime());
    m_dtEdt->setDisplayFormat("dd.MM.yyyy HH:mm");
    m_dtEdt->setWhatsThis(i18n("Date and Time of the album that will be created (optional)."));

    m_descEdt           = new KTextEdit;
    m_descEdt->setWhatsThis(i18n("Description of the album that will be created (optional)."));

    m_locEdt            = new KLineEdit;
    m_locEdt->setWhatsThis(i18n("Location of the album that will be created (optional)."));    

    QFormLayout* albumBoxLayout  = new QFormLayout;
    albumBoxLayout->addRow(i18nc("new picasaweb album dialog", "Title:"), m_titleEdt);
    albumBoxLayout->addRow(i18nc("new picasaweb album dialog", "Date & Time:"), m_dtEdt);
    albumBoxLayout->addRow(i18nc("new picasaweb album dialog", "Description:"), m_descEdt);
    albumBoxLayout->addRow(i18nc("new picasaweb album dialog", "Location:"), m_locEdt);
    albumBoxLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    albumBoxLayout->setSpacing(KDialog::spacingHint());
    albumBoxLayout->setMargin(KDialog::spacingHint());
    albumBox->setLayout(albumBoxLayout);

    // ------------------------------------------------------------------------

    QGroupBox* privBox = new QGroupBox(i18n("Access Level"), mainWidget);
    privBox->setWhatsThis(i18n("These are security and privacy settings for the new Picasaweb album."));

    m_publicRBtn        = new QRadioButton(i18nc("picasaweb album privacy", "Public"));
    m_publicRBtn->setChecked(true);
    m_publicRBtn->setWhatsThis(i18n("Public album is listed on your public Picasaweb page."));
    m_unlistedRBtn      = new QRadioButton(i18nc("picasaweb album privacy", "Unlisted / Private"));
    m_unlistedRBtn->setWhatsThis(i18n("Unlisted album is only accessible via URL."));
    m_protectedRBtn     = new QRadioButton(i18nc("picasaweb album privacy", "Sign-In Required to View"));
    m_protectedRBtn->setWhatsThis(i18n("Unlisted album require Sign-In to View"));

    QVBoxLayout* radioLayout = new QVBoxLayout;
    radioLayout->addWidget(m_publicRBtn);
    radioLayout->addWidget(m_unlistedRBtn);
    radioLayout->addWidget(m_protectedRBtn);

    QFormLayout* privBoxLayout = new QFormLayout;
    privBoxLayout->addRow(i18n("Privacy:"), radioLayout);
    privBoxLayout->setSpacing(KDialog::spacingHint());
    privBoxLayout->setMargin(KDialog::spacingHint());
    privBox->setLayout(privBoxLayout);

    // ------------------------------------------------------------------------

    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(albumBox);
    mainLayout->addWidget(privBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);
    mainWidget->setLayout(mainLayout);
}

PicasawebNewAlbum::~PicasawebNewAlbum()
{
}

void PicasawebNewAlbum::getAlbumProperties(PicasaWebAlbum &album)
{
    album.title = m_titleEdt->text();
    album.description = m_descEdt->toPlainText();
    album.location = m_locEdt->text();

    if (m_publicRBtn->isChecked())
        album.access = QString("public");
    else if (m_unlistedRBtn->isChecked())
        album.access = QString("private");
    else
        album.access = QString("protected");

    long long timestamp = m_dtEdt->dateTime().toTime_t();
    album.timestamp = QString("%1").arg(timestamp * 1000);
}

} // namespace KIPISmugPlugin
