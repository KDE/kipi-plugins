/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-07-28
 * Description : Common widgets shared by plugins
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#include "kpsettingswidget.h"

// Qt includes

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QComboBox>

// KDE includes

#include <klocalizedstring.h>

// LibKIPI includes

#include <KIPI/Interface>
#include <KIPI/UploadWidget>
#include <KIPI/ImageCollection>

namespace KIPIPlugins
{

KPSettingsWidget::KPSettingsWidget(QWidget* const parent, KIPI::Interface* const iface, const QString& pluginName):QWidget(parent)
{
    m_pluginName = pluginName;
    setObjectName(m_pluginName+" Widget");

    QHBoxLayout* const mainLayout =new QHBoxLayout(this);

    //----------------------------------------------------------

    m_imgList = new KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(true);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(i18n("This is the list of images to upload to your %1 account.",m_pluginName));

    QWidget* const settingsBox           = new QWidget(this);
    QVBoxLayout* const settingsBoxLayout = new QVBoxLayout(settingsBox);

    m_headerLbl = new QLabel(this);
    m_headerLbl->setWhatsThis(i18n("This is a clickable link to open %1 in a browser.",m_pluginName));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    //------------------------------------------------------------

    QGroupBox* const accountBox   = new QGroupBox(i18n("Account"),settingsBox);
    accountBox->setWhatsThis(i18n("This is the %1 account that is currently logged in.",m_pluginName));    
    QGridLayout* accountBoxLayout = new QGridLayout(accountBox);

    QLabel* const userNameLbl = new QLabel(i18nc("account settings","Name:"),accountBox);
    m_userNameDisplayLbl      = new QLabel(accountBox);
    m_changeUserBtn           = new QPushButton(accountBox);
    m_changeUserBtn->setText(i18n("Change Account"));
    m_changeUserBtn->setIcon(QIcon::fromTheme("system-switch-user").pixmap(16));
    m_changeUserBtn->setToolTip(i18n("Change %1 account for transfer",m_pluginName));
      

    accountBoxLayout->addWidget(userNameLbl,          0,0,1,2);
    accountBoxLayout->addWidget(m_userNameDisplayLbl, 0,2,1,2);
    accountBoxLayout->addWidget(m_changeUserBtn,      1,0,1,4);
    accountBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    accountBoxLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    //-------------------------------------------------------------

    QGroupBox* const albBox            = new QGroupBox(i18n("Album"),settingsBox);
    albBox->setWhatsThis(i18n("This is the %1 folder to/from which selected photos will be uploaded/downloaded.",m_pluginName)); 
    QGridLayout* const albumsBoxLayout = new QGridLayout(albBox);
    QLabel* const albLbl = new QLabel(i18n("Album:"),albBox);
    m_albumsCoB          = new QComboBox(albBox);
    m_albumsCoB->setEditable(false);
        
    m_newAlbumBtn = new QPushButton(accountBox);
    m_newAlbumBtn->setText(i18n("New Album"));
    m_newAlbumBtn->setIcon(QIcon::fromTheme("list-add").pixmap(16));
    m_newAlbumBtn->setToolTip(i18n("Create new %1 folder",m_pluginName));
    
    m_reloadAlbumsBtn = new QPushButton(accountBox);
    m_reloadAlbumsBtn->setText(i18nc("album list","Reload"));
    m_reloadAlbumsBtn->setIcon(QIcon::fromTheme("view-refresh").pixmap(16));
    m_reloadAlbumsBtn->setToolTip(i18n("Reload album list"));    

    albumsBoxLayout->addWidget(albLbl,            0, 0, 1, 1);
    albumsBoxLayout->addWidget(m_albumsCoB,       0, 1, 1, 4);
    albumsBoxLayout->addWidget(m_newAlbumBtn,     1, 3, 1, 1);
    albumsBoxLayout->addWidget(m_reloadAlbumsBtn, 1, 4, 1, 1);
    
    //----------------------------------------------------------
    
    m_sizeBox         = new QGroupBox(i18n("Max Dimension"), settingsBox);
    m_sizeBox->setWhatsThis(i18n("This is the maximum dimension of the images. Images larger than this will be scaled down."));
    m_sizeBoxLayout = new QVBoxLayout(m_sizeBox);
    m_dlDimensionCoB                 = new QComboBox(m_sizeBox);
    m_dlDimensionCoB->addItem(i18n("Original Size"), "d");
    m_dlDimensionCoB->addItem(i18n("1600 px"), "1600");
    m_dlDimensionCoB->addItem(i18n("1440 px"), "1440");
    m_dlDimensionCoB->addItem(i18n("1280 px"), "1280");
    m_dlDimensionCoB->addItem(i18n("1152 px"), "1152");
    m_dlDimensionCoB->addItem(i18n("1024 px"), "1024");
    m_dlDimensionCoB->setCurrentIndex(0);
    m_sizeBoxLayout->addWidget(m_dlDimensionCoB);
    
    // ------------------------------------------------------------------------

    m_uploadBox         = new QGroupBox(i18n("Destination"), settingsBox);
    m_uploadBox->setWhatsThis(i18n("This is the location where %1 images will be downloaded.",m_pluginName));
    m_uploadBoxLayout = new QVBoxLayout(m_uploadBox);
    m_uploadWidget                     = iface->uploadWidget(m_uploadBox);
    m_uploadBoxLayout->addWidget(m_uploadWidget);

    //-----------------------------------------------------------

    m_optionsBox         = new QGroupBox(i18n("Options"),settingsBox);
    m_optionsBox->setWhatsThis(i18n("These are the options that would be applied to photos before upload."));
    m_optionsBoxLayout = new QGridLayout(m_optionsBox);

    m_resizeChB = new QCheckBox(m_optionsBox);
    m_resizeChB->setText(i18n("Resize photos before uploading"));
    m_resizeChB->setChecked(false);

    m_dimensionSpB = new QSpinBox(m_optionsBox);
    m_dimensionSpB->setMinimum(0);
    m_dimensionSpB->setMaximum(5000);
    m_dimensionSpB->setSingleStep(10);
    m_dimensionSpB->setValue(1600);
    m_dimensionSpB->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_dimensionSpB->setEnabled(false);

    QLabel* const dimensionLbl = new QLabel(i18n("Maximum Dimension:"),m_optionsBox);

    m_imageQualitySpB = new QSpinBox(m_optionsBox);
    m_imageQualitySpB->setMinimum(0);
    m_imageQualitySpB->setMaximum(100);
    m_imageQualitySpB->setSingleStep(1);
    m_imageQualitySpB->setValue(90);
    m_imageQualitySpB->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QLabel* const imageQualityLbl = new QLabel(i18n("JPEG Quality:"),m_optionsBox);
    
    m_optionsBoxLayout->addWidget(m_resizeChB,      0, 0, 1, 5);
    m_optionsBoxLayout->addWidget(imageQualityLbl,  1, 1, 1, 1);
    m_optionsBoxLayout->addWidget(m_imageQualitySpB,1, 2, 1, 1);
    m_optionsBoxLayout->addWidget(dimensionLbl,     2, 1, 1, 1);
    m_optionsBoxLayout->addWidget(m_dimensionSpB,   2, 2, 1, 1);
    m_optionsBoxLayout->setRowStretch(3,10);
    m_optionsBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    m_optionsBoxLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    m_progressBar = new KPProgressWidget(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    //------------------------------------------------------

    settingsBoxLayout->addWidget(m_headerLbl);
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->addWidget(m_sizeBox);
    settingsBoxLayout->addWidget(albBox);
    settingsBoxLayout->addWidget(m_uploadBox);
    settingsBoxLayout->addWidget(m_optionsBox);
    settingsBoxLayout->addWidget(m_progressBar);
    settingsBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    settingsBoxLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    //--------------------------------------------------------

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    mainLayout->setMargin(0);

    //-------------------------------------------------------

    connect(m_resizeChB,SIGNAL(clicked()),
            this,SLOT(slotResizeChecked()));
}

KPSettingsWidget::~KPSettingsWidget()
{
}

QString KPSettingsWidget::getDestinationPath() const
{
    return m_uploadWidget->selectedImageCollection().uploadUrl().path();
}

KPImagesList* KPSettingsWidget::imagesList() const
{
    return m_imgList;
}

void KPSettingsWidget::slotResizeChecked()
{
    m_dimensionSpB->setEnabled(m_resizeChB->isChecked());
    m_imageQualitySpB->setEnabled(m_resizeChB->isChecked());
}

KPProgressWidget* KPSettingsWidget::progressBar() const
{
    return m_progressBar;
}

} // namespace KIPIPlugins
