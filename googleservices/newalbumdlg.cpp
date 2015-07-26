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

#include "newalbumdlg.h"

// Qt includes

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QIcon>
#include <QApplication>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// local includes

#include <gsitem.h>

namespace KIPIGoogleServicesPlugin
{

NewAlbumDlg::NewAlbumDlg(QWidget* const parent, const QString& serviceName)
    : QDialog(parent)
{
    m_serviceName = serviceName;
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
        setWindowTitle(QString("Google Drive New Album"));
    else
        setWindowTitle(QString("Google Photos/PicasaWeb New Album"));
    
    QDialogButtonBox* const buttonBox   = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(true);    
    setModal(false);
    
    connect(buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));

    connect(buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));

    QWidget* const mainWidget = new QWidget(this);
    mainWidget->setMinimumSize(500, 500);
    QFormLayout* const albumBoxLayout  = new QFormLayout;

    m_titleEdt          = new QLineEdit;
    m_titleEdt->setToolTip(i18n("Title of the album that will be created (required)."));

//     if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
//     {
//         m_titleEdt->setWhatsThis(i18n("This is the title of the folder that will be created."));
//         albumBoxLayout->addRow(i18nc("folder edit","Title:"),m_titleEdt);
//         mainWidget->setMinimumSize(300,0);
//     }
    
    QGroupBox* const albumBox = new QGroupBox(i18n("Album"), mainWidget);
    albumBox->setWhatsThis(
        i18n("These are basic settings for the new Google Photos/Picasaweb album."));

    m_dtEdt             = new QDateTimeEdit(QDateTime::currentDateTime());
    m_dtEdt->setDisplayFormat("dd.MM.yyyy HH:mm");
    m_dtEdt->setWhatsThis(i18n("Date and Time of the album that will be created (optional)."));

    m_descEdt           = new QTextEdit;
    m_descEdt->setToolTip(i18n("Description of the album that will be created (optional)."));

    m_locEdt            = new QLineEdit;
    m_locEdt->setToolTip(i18n("Location of the album that will be created (optional).")); 
    
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        m_titleEdt->setWhatsThis(i18n("This is the title of the folder that will be created."));
        albumBoxLayout->addRow(i18nc("folder edit","Title:"),m_titleEdt);
        mainWidget->setMinimumSize(300,0);
    }
    else
    {
        albumBoxLayout->addRow(i18nc("new google photos/picasaweb album dialog", "Title:"), m_titleEdt);
        albumBoxLayout->addRow(i18nc("new google photos/picasaweb album dialog", "Date & Time:"), m_dtEdt);
        albumBoxLayout->addRow(i18nc("new google photos/picasaweb album dialog", "Description:"), m_descEdt);
        albumBoxLayout->addRow(i18nc("new google photos/picasaweb album dialog", "Location:"), m_locEdt);        
    }
    
    albumBoxLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    albumBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    albumBoxLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    albumBox->setLayout(albumBoxLayout);

    // ------------------------------------------------------------------------

    QGroupBox* const privBox = new QGroupBox(i18n("Access Level"), mainWidget);
    privBox->setWhatsThis(i18n("These are security and privacy settings for the new Google Photos/PicasaWeb album."));

    m_publicRBtn        = new QRadioButton(i18nc("google photos/picasaweb album privacy", "Public"));
    m_publicRBtn->setChecked(true);
    m_publicRBtn->setWhatsThis(i18n("Public album is listed on your public Google Photos/PicasaWeb page."));
    m_unlistedRBtn      = new QRadioButton(i18nc("google photos/picasaweb album privacy", "Unlisted / Private"));
    m_unlistedRBtn->setWhatsThis(i18n("Unlisted album is only accessible via URL."));
    m_protectedRBtn     = new QRadioButton(i18nc("google photos/picasaweb album privacy", "Sign-In Required to View"));
    m_protectedRBtn->setWhatsThis(i18n("Unlisted album require Sign-In to View"));

    QVBoxLayout* const radioLayout = new QVBoxLayout;
    radioLayout->addWidget(m_publicRBtn);
    radioLayout->addWidget(m_unlistedRBtn);
    radioLayout->addWidget(m_protectedRBtn);

    QFormLayout* const privBoxLayout = new QFormLayout;
    privBoxLayout->addRow(i18n("Privacy:"), radioLayout);
    privBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    privBoxLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    privBox->setLayout(privBoxLayout);

    // ------------------------------------------------------------------------

    QVBoxLayout* const mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(albumBox);
    mainLayout->addWidget(privBox);
    mainLayout->addWidget(buttonBox);
    mainLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    mainLayout->setMargin(0);
    mainWidget->setLayout(mainLayout);
    
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        privBox->hide();
    }
}

NewAlbumDlg::~NewAlbumDlg()
{
}

void NewAlbumDlg::getAlbumProperties(GSFolder& album)
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

} // namespace KIPIGoogleServicesPlugin
