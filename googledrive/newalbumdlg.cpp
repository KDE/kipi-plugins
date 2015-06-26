/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-02-01
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

#include "newalbumdlg.moc"

// Qt includes

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <ktextedit.h>

// local includes

#include <gditem.h>

namespace KIPIGoogleDrivePlugin
{

NewAlbumDlg::NewAlbumDlg(QWidget* const parent, const QString& serviceName)
    : KDialog(parent)
{
    m_serviceName = serviceName;
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
        setWindowTitle(QString("Google Drive New Album"));
    else
        setWindowTitle(QString("Picasaweb New Album"));

    setButtons(Ok|Cancel);
    setDefaultButton(Cancel);
    setModal(false);

    QWidget* const mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    mainWidget->setMinimumSize(400, 400);
    QFormLayout* const albumBoxLayout  = new QFormLayout;

    m_titleEdt          = new KLineEdit;
    m_titleEdt->setWhatsThis(i18n("Title of the album that will be created (required)."));

    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        m_titleEdt->setWhatsThis(i18n("This is the title of the folder that will be created."));
        albumBoxLayout->addRow(i18nc("folder edit","Title:"),m_titleEdt);
        albumBoxLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        albumBoxLayout->setSpacing(KDialog::spacingHint());
        albumBoxLayout->setMargin(KDialog::spacingHint());
        mainWidget->setMinimumSize(300,0);
        mainWidget->setLayout(albumBoxLayout);
        return;
    }
    
    QGroupBox* const albumBox = new QGroupBox(i18n("Album"), mainWidget);
    albumBox->setWhatsThis(
        i18n("These are basic settings for the new Picasaweb album."));

    m_dtEdt             = new QDateTimeEdit(QDateTime::currentDateTime());
    m_dtEdt->setDisplayFormat("dd.MM.yyyy HH:mm");
    m_dtEdt->setWhatsThis(i18n("Date and Time of the album that will be created (optional)."));

    m_descEdt           = new KTextEdit;
    m_descEdt->setWhatsThis(i18n("Description of the album that will be created (optional)."));

    m_locEdt            = new KLineEdit;
    m_locEdt->setWhatsThis(i18n("Location of the album that will be created (optional)."));    

    albumBoxLayout->addRow(i18nc("new picasaweb album dialog", "Title:"), m_titleEdt);
    albumBoxLayout->addRow(i18nc("new picasaweb album dialog", "Date & Time:"), m_dtEdt);
    albumBoxLayout->addRow(i18nc("new picasaweb album dialog", "Description:"), m_descEdt);
    albumBoxLayout->addRow(i18nc("new picasaweb album dialog", "Location:"), m_locEdt);
    albumBoxLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    albumBoxLayout->setSpacing(KDialog::spacingHint());
    albumBoxLayout->setMargin(KDialog::spacingHint());
    albumBox->setLayout(albumBoxLayout);

    // ------------------------------------------------------------------------

    QGroupBox* const privBox = new QGroupBox(i18n("Access Level"), mainWidget);
    privBox->setWhatsThis(i18n("These are security and privacy settings for the new Picasaweb album."));

    m_publicRBtn        = new QRadioButton(i18nc("picasaweb album privacy", "Public"));
    m_publicRBtn->setChecked(true);
    m_publicRBtn->setWhatsThis(i18n("Public album is listed on your public Picasaweb page."));
    m_unlistedRBtn      = new QRadioButton(i18nc("picasaweb album privacy", "Unlisted / Private"));
    m_unlistedRBtn->setWhatsThis(i18n("Unlisted album is only accessible via URL."));
    m_protectedRBtn     = new QRadioButton(i18nc("picasaweb album privacy", "Sign-In Required to View"));
    m_protectedRBtn->setWhatsThis(i18n("Unlisted album require Sign-In to View"));

    QVBoxLayout* const radioLayout = new QVBoxLayout;
    radioLayout->addWidget(m_publicRBtn);
    radioLayout->addWidget(m_unlistedRBtn);
    radioLayout->addWidget(m_protectedRBtn);

    QFormLayout* const privBoxLayout = new QFormLayout;
    privBoxLayout->addRow(i18n("Privacy:"), radioLayout);
    privBoxLayout->setSpacing(KDialog::spacingHint());
    privBoxLayout->setMargin(KDialog::spacingHint());
    privBox->setLayout(privBoxLayout);

    // ------------------------------------------------------------------------

    QVBoxLayout* const mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(albumBox);
    mainLayout->addWidget(privBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);
    mainWidget->setLayout(mainLayout);
}

NewAlbumDlg::~NewAlbumDlg()
{
}

void NewAlbumDlg::getAlbumProperties(GDFolder& album)
{
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        album.title       = m_titleEdt->text();
        return;
    }
    album.title       = m_titleEdt->text();
    album.description = m_descEdt->toPlainText();
    album.location    = m_locEdt->text();

    if (m_publicRBtn->isChecked())
        album.access = QString("public");
    else if (m_unlistedRBtn->isChecked())
        album.access = QString("private");
    else
        album.access = QString("protected");

    long long timestamp = m_dtEdt->dateTime().toTime_t();
    album.timestamp     = QString("%1").arg(timestamp * 1000);
}

} // namespace KIPIGoogleDrivePlugin
