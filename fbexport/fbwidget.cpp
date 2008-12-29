/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to export images to Facebook web service
 *
 * Copyright (C) 2008 by Luka Renko <lure at kubuntu dot org>
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

#include "fbwidget.h"
#include "fbwidget.moc"

// Qt includes.
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

// KDE includes.
#include <klocale.h>
#include <kdialog.h>

// LibKIPI includes.
#include <libkipi/interface.h>

// Local includes.
#include "imageslist.h"

namespace KIPIFbExportPlugin
{

FbWidget::FbWidget(QWidget* parent, KIPI::Interface *iface)
            : QWidget(parent)
{
    setObjectName("FbWidget");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // -------------------------------------------------------------------

    m_imgList  = new KIPIPlugins::ImagesList(iface, this, 
                                  KIPIPlugins::ImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(true);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(
        i18n("This is the list of images to upload on your Facebook account."));

    QWidget* settingsBox           = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    m_headerLbl = new QLabel(settingsBox);
    m_headerLbl->setWhatsThis(
        i18n("This is clickable link to open Facebook home page in web browser."));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    // ------------------------------------------------------------------------

    QGroupBox* accountBox         = new QGroupBox(i18n("Account"), settingsBox);
    accountBox->setWhatsThis(
        i18n("This is the Facebook account that will be used for upload."));
    QGridLayout* accountBoxLayout = new QGridLayout(accountBox);

    QLabel *userNameLbl     = new QLabel(i18n("Name: "), accountBox);
    m_userNameDisplayLbl    = new QLabel(accountBox);
    m_changeUserBtn         = new QPushButton(accountBox);
    m_changeUserBtn->setText(i18n("Change Account"));
    m_changeUserBtn->setIcon(SmallIcon("system-switch-user"));

    accountBoxLayout->addWidget(userNameLbl,            0, 0, 1, 1);
    accountBoxLayout->addWidget(m_userNameDisplayLbl,   0, 1, 1, 1);
    accountBoxLayout->addWidget(m_changeUserBtn,        2, 1, 1, 1);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    QGroupBox* albumsBox    = new QGroupBox(i18n("Album"), settingsBox);
    albumsBox->setWhatsThis(
        i18n("This is the Facebook album where selected photos will be uploaded."));
    QGridLayout* albumsBoxLayout  = new QGridLayout(albumsBox);

    m_albumsCoB         = new QComboBox(albumsBox);
    m_albumsCoB->setEditable(false);
    m_newAlbumBtn       = new QPushButton(accountBox);
    m_newAlbumBtn->setText(i18n("New Album"));
    m_newAlbumBtn->setIcon(SmallIcon("list-add"));
    m_reloadAlbumsBtn   = new QPushButton(accountBox);
    m_reloadAlbumsBtn->setText(i18n("Reload"));
    m_reloadAlbumsBtn->setIcon(SmallIcon("view-refresh"));

    albumsBoxLayout->addWidget(m_albumsCoB,         0, 0, 1, 5);
    albumsBoxLayout->addWidget(m_newAlbumBtn,       1, 3, 1, 1);
    albumsBoxLayout->addWidget(m_reloadAlbumsBtn,   1, 4, 1, 1);

    // ------------------------------------------------------------------------

    QGroupBox* optionsBox         = new QGroupBox(i18n("Options"), settingsBox);
    optionsBox->setWhatsThis(
        i18n("These are options that will be applied to photos before upload."));
    QGridLayout* optionsBoxLayout = new QGridLayout(optionsBox);

    m_resizeChB     = new QCheckBox(optionsBox);
    m_resizeChB->setText(i18n("Resize photos before uploading"));
    m_resizeChB->setChecked(false);

    m_dimensionSpB  = new QSpinBox(optionsBox);
    m_dimensionSpB->setMinimum(0);
    m_dimensionSpB->setMaximum(5000);
    m_dimensionSpB->setSingleStep(10);
    m_dimensionSpB->setValue(600);
    m_dimensionSpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_dimensionSpB->setEnabled(false);
    QLabel* dimensionLbl = new QLabel(i18n("Maximum dimension:"), optionsBox);

    m_imageQualitySpB = new QSpinBox(optionsBox);
    m_imageQualitySpB->setMinimum(0);
    m_imageQualitySpB->setMaximum(100);
    m_imageQualitySpB->setSingleStep(1);
    m_imageQualitySpB->setValue(85);
    m_imageQualitySpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel* imageQualityLbl = new QLabel(i18n("JPEG quality:"), optionsBox);

    optionsBoxLayout->addWidget(m_resizeChB,        0, 0, 1, 5);
    optionsBoxLayout->addWidget(imageQualityLbl,    1, 1, 1, 1);
    optionsBoxLayout->addWidget(m_imageQualitySpB,  1, 2, 1, 1);
    optionsBoxLayout->addWidget(dimensionLbl,       2, 1, 1, 1);
    optionsBoxLayout->addWidget(m_dimensionSpB,     2, 2, 1, 1);
    optionsBoxLayout->setRowStretch(3, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    settingsBoxLayout->addWidget(m_headerLbl);
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->addWidget(albumsBox);
    settingsBoxLayout->addWidget(optionsBox);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    updateLabels("", "");  // use empty labels until login

    // ------------------------------------------------------------------------

    connect(m_resizeChB, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));
}

FbWidget::~FbWidget()
{
}

void FbWidget::updateLabels(const QString& name, const QString& url)
{
    m_userNameDisplayLbl->setText(QString("<b>%1</b>").arg(name));
    QString web("http://www.facebook.com");
    if (!url.isEmpty())
        web = url;
    m_headerLbl->setText(QString("<b><h2><a href='%1'>"
                                   "<font color=\"#3B5998\">Facebook</font>"
                                   "</a></h2></b>").arg(web));
}

void FbWidget::slotResizeChecked()
{
    m_dimensionSpB->setEnabled(m_resizeChB->isChecked());
    m_imageQualitySpB->setEnabled(m_resizeChB->isChecked());
}

} // namespace KIPIFbExportPlugin
