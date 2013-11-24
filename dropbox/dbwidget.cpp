/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Dropbox web service
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

#include "dbwidget.moc"

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
#include <QPixmap>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <kcombobox.h>
#include <kpushbutton.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpimageslist.h"
#include "kpprogresswidget.h"

namespace KIPIDropboxPlugin
{

DropboxWidget::DropboxWidget(QWidget* const parent):QWidget(parent)
{
    setObjectName("Dropbox Widget");

    QHBoxLayout* const mainLayout =new QHBoxLayout(this);

    //----------------------------------------------------------

    m_imgList = new KIPIPlugins::KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KIPIPlugins::KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(true);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(i18n("This is the list of images to upload to your Dropbox account."));

    QWidget* const settingsBox           = new QWidget(this);
    QVBoxLayout* const settingsBoxLayout = new QVBoxLayout(settingsBox);

    m_headerLbl = new QLabel(this);
    m_headerLbl->setWhatsThis(i18n("This is a clickable link to open Dropbox in a browser."));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    //------------------------------------------------------------

    QGroupBox* const accountBox   = new QGroupBox(i18n("Account"),settingsBox);
    accountBox->setWhatsThis(i18n("This is the Dropbox account that is currently logged in."));
    QGridLayout* accountBoxLayout = new QGridLayout(accountBox);

    QLabel* const userNameLbl = new QLabel(i18nc("account settings","Name:"),accountBox);
    m_userNameDisplayLbl      = new QLabel(accountBox);

    m_changeUserBtn = new KPushButton(KGuiItem(i18n("Change Account"),"switch-system-user",i18n("Change Dropbox "
                                                                                                "account for transfer")),accountBox);

    accountBoxLayout->addWidget(userNameLbl,          0,0,1,2);
    accountBoxLayout->addWidget(m_userNameDisplayLbl, 0,2,1,2);
    accountBoxLayout->addWidget(m_changeUserBtn,      1,0,1,4);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    //-------------------------------------------------------------

    QGroupBox* const albBox            = new QGroupBox(i18n("Destination"),settingsBox);
    albBox->setWhatsThis(i18n("This is the Dropbox folder to which selected photos will be uploaded."));
    QGridLayout* const albumsBoxLayout = new QGridLayout(albBox);

    QLabel* const albLbl = new QLabel(i18n("Folder:"),albBox);
    m_albumsCoB          = new KComboBox(albBox);
    m_albumsCoB->setEditable(false);

    m_newAlbumBtn     = new KPushButton(KGuiItem(i18n("New Folder"),"list-add",
                                                 i18n("Create new Dropbox folder")),accountBox);
    m_reloadAlbumsBtn = new KPushButton(KGuiItem(i18nc("album list","Reload"),"view-refresh",
                                                 i18n("Reload album list")),accountBox);

    albumsBoxLayout->addWidget(albLbl,            0, 0, 1, 1);
    albumsBoxLayout->addWidget(m_albumsCoB,       0, 1, 1, 4);
    albumsBoxLayout->addWidget(m_newAlbumBtn,     1, 3, 1, 1);
    albumsBoxLayout->addWidget(m_reloadAlbumsBtn, 1, 4, 1, 1);

    //-----------------------------------------------------------

    QGroupBox* const optionsBox         = new QGroupBox(i18n("Options"),settingsBox);
    optionsBox->setWhatsThis(i18n("These are the options that would be applied to photos before upload."));
    QGridLayout *const optionsBoxLayout = new QGridLayout(optionsBox);

    m_resizeChB = new QCheckBox(optionsBox);
    m_resizeChB->setText(i18n("Resize photos before uploading"));
    m_resizeChB->setChecked(false);

    m_dimensionSpB = new QSpinBox(optionsBox);
    m_dimensionSpB->setMinimum(0);
    m_dimensionSpB->setMaximum(5000);
    m_dimensionSpB->setSingleStep(10);
    m_dimensionSpB->setValue(1600);
    m_dimensionSpB->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_dimensionSpB->setEnabled(false);

    QLabel* const dimensionLbl = new QLabel(i18n("Maximum Dimension:"),optionsBox);

    m_imageQualitySpB = new QSpinBox(optionsBox);
    m_imageQualitySpB->setMinimum(0);
    m_imageQualitySpB->setMaximum(100);
    m_imageQualitySpB->setSingleStep(1);
    m_imageQualitySpB->setValue(90);
    m_imageQualitySpB->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QLabel* const imageQualityLbl = new QLabel(i18n("JPEG Quality:"),optionsBox);

    optionsBoxLayout->addWidget(m_resizeChB,      0, 0, 1, 5);
    optionsBoxLayout->addWidget(imageQualityLbl,  1, 1, 1, 1);
    optionsBoxLayout->addWidget(m_imageQualitySpB,1, 2, 1, 1);
    optionsBoxLayout->addWidget(dimensionLbl,     2, 1, 1, 1);
    optionsBoxLayout->addWidget(m_dimensionSpB,   2, 2, 1, 1);
    optionsBoxLayout->setRowStretch(3,10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    m_progressBar = new KIPIPlugins::KPProgressWidget(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    //------------------------------------------------------

    settingsBoxLayout->addWidget(m_headerLbl);
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->addWidget(albBox);
    settingsBoxLayout->addWidget(optionsBox);
    settingsBoxLayout->addWidget(m_progressBar);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    //--------------------------------------------------------

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    updateLabels();

    //-------------------------------------------------------

   connect(m_resizeChB,SIGNAL(clicked()),
           this,SLOT(slotResizeChecked()));
}

DropboxWidget::~DropboxWidget()
{
}

void DropboxWidget::updateLabels(const QString& name, const QString& url)
{
    QString web("https://www.dropbox.com/");

    if (!url.isEmpty())
        web = url;

    m_headerLbl->setText(QString("<b><h2><a href='%1'>"
                                 "<font color=\"#9ACD32\">Dropbox</font>"
                                 "</a></h2></b>").arg(web));

    if (name.isEmpty())
    {
        m_userNameDisplayLbl->clear();
    }
    else
    {
        m_userNameDisplayLbl->setText( QString("<b>%1</b>").arg(name) );
    }
}

KIPIPlugins::KPImagesList* DropboxWidget::imagesList() const
{
    return m_imgList;
}

void DropboxWidget::slotResizeChecked()
{
    m_dimensionSpB->setEnabled(m_resizeChB->isChecked());
    m_imageQualitySpB->setEnabled(m_resizeChB->isChecked());
}

KIPIPlugins::KPProgressWidget* DropboxWidget::progressBar() const
{
    return m_progressBar;
}

} // namespace KIPIDropboxPlugin
