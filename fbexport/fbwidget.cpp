/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

// KDE includes.
#include <KLocale>
#include <KDialog>
#include <KComboBox>
#include <KPushButton>

// LibKIPI includes.
#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// Local includes.
#include "imageslist.h"

namespace KIPIFbPlugin
{

FbWidget::FbWidget(QWidget* parent, KIPI::Interface *iface, bool import)
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
        i18n("This is a clickable link to open Facebook home page in web browser."));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    // ------------------------------------------------------------------------

    QGroupBox* accountBox         = new QGroupBox(i18n("Account"), settingsBox);
    accountBox->setWhatsThis(
        i18n("This is the Facebook account that will be used for upload."));
    QGridLayout* accountBoxLayout = new QGridLayout(accountBox);

    QLabel *userNameLbl     = new QLabel(i18n("Name:"), accountBox);
    m_userNameDisplayLbl    = new QLabel(accountBox);
    QLabel *permissionLbl   = new QLabel(i18n("Permission:"), accountBox);
    permissionLbl->setWhatsThis(
        i18n("Permission of KIPI Plugin application to upload photos directly. If not, user will need to manually approve uploaded photos in Facebook."));
    m_permissionLbl         = new QLabel(accountBox);
    m_changeUserBtn         = new KPushButton(
        KGuiItem(i18n("Change Account"), "system-switch-user",
                 i18n("Logout and change Facebook Account used for upload")), 
        accountBox);
    m_changePermBtn         = new KPushButton(
        KGuiItem(i18n("Change Permission"), "security-high",
                 i18n("Change permission for KIPI application for photo upload")), 
        accountBox);

    accountBoxLayout->addWidget(userNameLbl,            0, 0, 1, 2);
    accountBoxLayout->addWidget(m_userNameDisplayLbl,   0, 2, 1, 2);
    accountBoxLayout->addWidget(permissionLbl,          1, 0, 1, 2);
    accountBoxLayout->addWidget(m_permissionLbl,        1, 2, 1, 2);
    accountBoxLayout->addWidget(m_changeUserBtn,        2, 0, 1, 2);
    accountBoxLayout->addWidget(m_changePermBtn,        2, 2, 1, 2);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    QGroupBox* albumsBox    = new QGroupBox(i18n("Album"), settingsBox);
    albumsBox->setWhatsThis(
        i18n("This is the Facebook album where selected photos will be uploaded."));
    QGridLayout* albumsBoxLayout  = new QGridLayout(albumsBox);

    m_albumsCoB         = new KComboBox(albumsBox);
    m_albumsCoB->setEditable(false);
    m_newAlbumBtn       = new KPushButton(
            KGuiItem(i18n("New Album"), "list-add",
                     i18n("Create new Facebook album")), accountBox);
    m_reloadAlbumsBtn   = new KPushButton(
            KGuiItem(i18n("Reload"), "view-refresh", 
                     i18n("Reload album list")), accountBox);

    albumsBoxLayout->addWidget(m_albumsCoB,         0, 0, 1, 5);
    albumsBoxLayout->addWidget(m_newAlbumBtn,       1, 3, 1, 1);
    albumsBoxLayout->addWidget(m_reloadAlbumsBtn,   1, 4, 1, 1);

    // ------------------------------------------------------------------------

    QGroupBox* uploadBox    = new QGroupBox(i18n("Destination"), settingsBox);
    uploadBox->setWhatsThis(
        i18n("This is the location where Facebook images will be downloaded to."));
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadBox);
    m_uploadWidget = iface->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(m_uploadWidget);

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
    settingsBoxLayout->addWidget(uploadBox);
    settingsBoxLayout->addWidget(optionsBox);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    updateLabels();  // use empty labels until login

    // ------------------------------------------------------------------------

    connect(m_resizeChB, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));

    // ------------------------------------------------------------------------

    if (import) 
    {
        m_imgList->hide();

        permissionLbl->hide();
        m_permissionLbl->hide();
        m_changePermBtn->hide();
        m_newAlbumBtn->hide();

        optionsBox->hide();
    }
    else
    {
        uploadBox->hide();
    }
}

FbWidget::~FbWidget()
{
}

QString FbWidget::getDestinationPath()
{
    return m_uploadWidget->selectedImageCollection().uploadPath().path();
}

void FbWidget::updateLabels(const QString& name, const QString& url, bool uplPerm)
{
    QString web("http://www.facebook.com");
    if (!url.isEmpty())
        web = url;
    m_headerLbl->setText(QString("<b><h2><a href='%1'>"
                                   "<font color=\"#3B5998\">facebook</font>"
                                   "</a></h2></b>").arg(web));
    if (name.isEmpty())
    {
        m_userNameDisplayLbl->clear();
        m_permissionLbl->clear();
    }
    else 
    {
        m_userNameDisplayLbl->setText(QString("<b>%1</b>").arg(name));
        if (uplPerm)
        {
            m_permissionLbl->setText(i18n("Direct upload"));
            m_permissionLbl->setWhatsThis(
                i18n("Uploaded photos will not need manual approval by user."));
        }
        else
        {
            m_permissionLbl->setText(i18n("Manual upload approval"));
            m_permissionLbl->setWhatsThis(
                i18n("Uploaded photos will wait in pending state until manually approved by user."));
        }
    }
}

void FbWidget::slotResizeChecked()
{
    m_dimensionSpB->setEnabled(m_resizeChB->isChecked());
    m_imageQualitySpB->setEnabled(m_resizeChB->isChecked());
}

} // namespace KIPIFbPlugin
