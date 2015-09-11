/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
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

#include "imageshackwidget.h"

// Qt includes

#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QStringList>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QApplication>
#include <QPushButton>
#include <QtCore/QMimeDatabase>
#include <QtCore/QMimeType>

// KDE includes

#include <klocalizedstring.h>
#include <KGuiItem>

// Libkipi includes

#include <KIPI/UploadWidget>
#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_debug.h"
#include "kpimageslist.h"
#include "imageshack.h"
#include "kpprogresswidget.h"

namespace KIPIImageshackPlugin
{

ImageshackWidget::ImageshackWidget(QWidget* const parent, Imageshack* const imageshack, KIPI::Interface* const iface, const QString& pluginName)
    : KPSettingsWidget(parent, iface, pluginName),
      m_imageshack(imageshack)
{
//     setObjectName(QStringLiteral("ImageshackWidget"));
// 
//     QHBoxLayout* const mainLayout = new QHBoxLayout(this);
// 
//     // --------------------------------------------
// 
//     m_imgList = new KIPIPlugins::KPImagesList(this);
//     m_imgList->setControlButtonsPlacement(KIPIPlugins::KPImagesList::ControlButtonsBelow);
//     m_imgList->setAllowRAW(false);
//     m_imgList->loadImagesFromCurrentSelection();
//     removeVideosFromList();
//     m_imgList->setWhatsThis(i18n("Images to upload to Imageshack web service"));
// 
//     QWidget* const settingsBox           = new QWidget(this);
//     QVBoxLayout* const settingsBoxLayout = new QVBoxLayout(settingsBox);
// 
//     m_headerLbl = new QLabel(settingsBox);
//     m_headerLbl->setWhatsThis(i18n("This is a clickable link to open the Imageshack home page in a web browser"));
//     m_headerLbl->setText(QStringLiteral("<b><h2><a href='http://imageshack.us'>%1</a></h2></b>").arg(i18n("ImageShack")));
//     m_headerLbl->setOpenExternalLinks(true);
//     m_headerLbl->setFocusPolicy(Qt::NoFocus);
// 
//     // --------------------------------------------
// 
//     QGroupBox* const accountBox = new QGroupBox(i18n("Account"), settingsBox);
//     accountBox->setWhatsThis(i18n("Imageshack account info"));
//     QGridLayout* const accountBoxLayout = new QGridLayout(accountBox);
// 
//     QLabel* const accountName  = new QLabel(i18nc("Account info", "Name:"),  accountBox);
//     QLabel* const accountEmail = new QLabel(i18nc("Account info", "Email:"), accountBox);
// 
//     m_accountNameLbl  = new QLabel(accountBox);
//     m_accountEmailLbl = new QLabel(accountBox);
//     m_chgRegCodeBtn   = new QPushButton(accountBox);
//     KGuiItem::assign(m_chgRegCodeBtn,
//                      KGuiItem(i18n("Change account"), QStringLiteral("system-switch-user"),
//                               i18n("Change the registration code for the Imageshack account used to upload images")));
// 
//     accountBoxLayout->addWidget(accountName,          0, 0, 1, 1);
//     accountBoxLayout->addWidget(m_accountNameLbl,     0, 1, 1, 2);
//     accountBoxLayout->addWidget(accountEmail,         1, 0, 1, 1);
//     accountBoxLayout->addWidget(m_accountEmailLbl,    1, 1, 1, 2);
//     accountBoxLayout->addWidget(m_chgRegCodeBtn,      2, 0, 1, 1);
//     accountBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
//     accountBoxLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
// 
//     // ----------------------------------------------
// 
//     m_galleriesBox = new QGroupBox(settingsBox);
// 
//     // TODO implement the download
// 
//     m_galleriesBox->setTitle(i18n("Destination"));
//     m_galleriesBox->setWhatsThis(i18n("This is the ImageShack gallery to which selected items will be uploaded"));
// 
//     QGridLayout* const galleriesBoxLayout = new QGridLayout(m_galleriesBox);
// 
//     m_useGalleriesChb = new QCheckBox(m_galleriesBox);
//     m_useGalleriesChb->setText(i18n("Upload to galleries"));
// //    useGalleries->setEnabled(false);
// 
//     m_galleriesWidget           = new QWidget(m_galleriesBox);
//     QGridLayout* const gwLayout = new QGridLayout(m_galleriesWidget);
// 
//     QLabel* const galLbl = new QLabel(i18n("Gallery:"), m_galleriesWidget);
//     m_galleriesCob       = new QComboBox(m_galleriesWidget);
//     m_galleriesCob->addItem(i18nc("@item:inlistbox", "Create new gallery"),
//                             QStringLiteral("--new-gallery--"));
//     m_galleriesCob->setEditable(false);
// 
//     QLabel* const gallNameLbl = new QLabel(m_galleriesWidget);
//     gallNameLbl->setText(i18n("Name:"));
// 
//     m_newGalleryName          = new QLineEdit(m_galleriesWidget);
//     m_reloadGalleriesBtn      = new QPushButton(m_galleriesWidget);
//     KGuiItem::assign(m_reloadGalleriesBtn,
//                      KGuiItem(i18nc("ImageShack galleries list", "Reload"),
//                               QStringLiteral("view-refresh"),
//                               i18n("Reload ImageShack galleries list")));
// 
//     galleriesBoxLayout->addWidget(m_useGalleriesChb,    0, 0, 1, 5);
//     galleriesBoxLayout->addWidget(m_galleriesWidget,    1, 0, 5, 5);
// 
//     gwLayout->addWidget(galLbl,               0, 0, 1, 1);
//     gwLayout->addWidget(m_galleriesCob,       0, 1, 1, 4);
//     gwLayout->addWidget(gallNameLbl,          1, 0, 1, 1);
//     gwLayout->addWidget(m_newGalleryName,     1, 1, 1, 3);
//     gwLayout->addWidget(m_reloadGalleriesBtn, 1, 4, 1, 1);
// 
// 
//     connect(m_useGalleriesChb, SIGNAL(toggled(bool)),
//             m_galleriesWidget, SLOT(setEnabled(bool)));
// 
//     connect(m_galleriesCob, SIGNAL(activated(int)),
//             this, SLOT(slotEnableNewGalleryLE(int)));
// 
//     connect(m_reloadGalleriesBtn, SIGNAL(clicked()),
//             this, SLOT(slotReloadGalleries()));
// 
//     m_galleriesWidget->setEnabled(m_useGalleriesChb->isChecked());
// 
//     // ----------------------------------------------
// 
//     QGroupBox* const optionsBox         = new QGroupBox(i18n("Options"), settingsBox);
//     optionsBox->setWhatsThis(i18n("These are the options that will be applied to photos"));
//     QGridLayout* const optionsBoxLayout = new QGridLayout(optionsBox);
// 
//     m_privateImagesChb = new QCheckBox(optionsBox);
//     m_privateImagesChb->setText(i18n("Make private"));
//     m_privateImagesChb->setChecked(false);
// 
//     QGroupBox* const resizeOptions = new QGroupBox(i18n("Resize options"), optionsBox);
//     QGridLayout* const resizeOptionsLayout = new QGridLayout(resizeOptions);
// 
//     m_noResizeRdb = new QRadioButton(optionsBox);
//     m_noResizeRdb->setText(i18n("Do not resize photos"));
// 
//     m_predefSizeRdb = new QRadioButton(optionsBox);
//     m_predefSizeRdb->setText(i18n("Predefined templates"));
// 
//     connect(m_predefSizeRdb, SIGNAL(toggled(bool)),
//             this, SLOT(slotEnablePredefComboBox(bool)));
// 
//     m_customSizeRdb = new QRadioButton(optionsBox);
//     m_customSizeRdb->setText(i18n("Custom resize dimensions"));
// 
//     m_widthSpb = new QSpinBox(optionsBox);
//     m_widthSpb->setMinimum(1);
//     m_widthSpb->setMaximum(6000);
//     m_widthSpb->setSingleStep(10);
//     m_widthSpb->setValue(1000);
//     m_widthSpb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//     m_widthSpb->setEnabled(false);
//     QLabel* const widthLbl = new QLabel(i18n("Width:"), optionsBox);
// 
//     m_heightSpb = new QSpinBox(optionsBox);
//     m_heightSpb->setMinimum(1);
//     m_heightSpb->setMaximum(6000);
//     m_heightSpb->setSingleStep(10);
//     m_heightSpb->setValue(1000);
//     m_heightSpb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//     m_heightSpb->setEnabled(false);
// 
//     connect(m_customSizeRdb, SIGNAL(toggled(bool)),
//             this, SLOT(slotEnableCustomSize(bool)));
// 
//     QLabel* const heightLbl = new QLabel(i18n("Height:"), optionsBox);
// 
//     m_resizeOptsCob = new QComboBox(optionsBox);
//     updateResizeOpts();
//     m_resizeOptsCob->setEditable(false);
// 
//     m_noResizeRdb->setChecked(true);
//     m_resizeOptsCob->setEnabled(false);
// 
//     resizeOptionsLayout->addWidget(m_noResizeRdb,   0, 0, 1, 5);
//     resizeOptionsLayout->addWidget(m_predefSizeRdb, 1, 0, 1, 5);
//     resizeOptionsLayout->addWidget(m_resizeOptsCob, 2, 1, 1, 4);
//     resizeOptionsLayout->addWidget(m_customSizeRdb, 3, 0, 1, 5);
//     resizeOptionsLayout->addWidget(widthLbl,        4, 1, 1, 2);
//     resizeOptionsLayout->addWidget(m_widthSpb,      4, 2, 1, 2);
//     resizeOptionsLayout->addWidget(heightLbl,       5, 1, 1, 2);
//     resizeOptionsLayout->addWidget(m_heightSpb,     5, 2, 1, 2);
//     resizeOptionsLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
//     resizeOptionsLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
// 
//     m_tagsFld             = new QLineEdit(settingsBox);
//     QLabel* const tagsLbl = new QLabel(i18n("Tags (optional):"), settingsBox);
// 
//     m_remBarChb           = new QCheckBox(i18n("Remove information bar on thumbnails"));
//     m_remBarChb->setChecked(false);
// 
//     optionsBoxLayout->addWidget(m_privateImagesChb, 0, 0, 1, 2);
//     optionsBoxLayout->addWidget(resizeOptions,      1, 0, 1, 2);
//     optionsBoxLayout->addWidget(tagsLbl,            2, 0, 1, 1);
//     optionsBoxLayout->addWidget(m_tagsFld,          2, 1, 1, 1);
//     optionsBoxLayout->addWidget(m_remBarChb,        3, 0, 1, 2);
//     optionsBoxLayout->setRowStretch(4, 10);
//     optionsBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
//     optionsBoxLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
// 
//     m_progressBar         = new KIPIPlugins::KPProgressWidget(settingsBox);
//     m_progressBar->setSizePolicy(QSizePolicy::Preferred ,QSizePolicy::Fixed);
//     m_progressBar->setVisible(false);
// 
//     // --------------------------------------------
// 
//     settingsBoxLayout->addWidget(m_headerLbl);
//     settingsBoxLayout->addWidget(accountBox);
//     settingsBoxLayout->addWidget(m_galleriesBox);
//     settingsBoxLayout->addWidget(optionsBox);
//     settingsBoxLayout->addWidget(m_progressBar);
//     settingsBoxLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
//     settingsBoxLayout->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
// 
//     // --------------------------------------------
// 
//     mainLayout->addWidget(m_imgList);
//     mainLayout->addWidget(settingsBox);
//     mainLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
//     mainLayout->setMargin(0);
    
    m_imgList = imagesList();
    m_headerLbl = getHeaderLbl();
    m_accountNameLbl = getUserNameLabel();
    m_chgRegCodeBtn = getChangeUserBtn();
    m_reloadGalleriesBtn = getReloadBtn();
    m_galleriesCob       = getAlbumsCoB();
    m_progressBar        = progressBar();
    
    

//     QLabel*                        m_accountEmailLbl;
//     QLineEdit*                     m_newGalleryName;
//     QRadioButton*                  m_noResizeRdb;
//     QRadioButton*                  m_predefSizeRdb;
//     QRadioButton*                  m_customSizeRdb;
//     QCheckBox*                     m_useGalleriesChb;
//     QComboBox*                     m_resizeOptsCob;
//     QSpinBox*                      m_widthSpb;
//     QSpinBox*                      m_heightSpb;  
    
    connect(m_reloadGalleriesBtn, SIGNAL(clicked()),
            this, SLOT(slotReloadGalleries()));    
    
    QGroupBox* tagsBox = new QGroupBox(QStringLiteral(""), getSettingsBox());
    QGridLayout* tagsLayout = new QGridLayout(tagsBox);
    
    m_privateImagesChb = new QCheckBox(tagsBox);
    m_privateImagesChb->setText(i18n("Make private"));
    m_privateImagesChb->setChecked(false);
    
    m_tagsFld             = new QLineEdit(tagsBox);
    QLabel* const tagsLbl = new QLabel(i18n("Tags (optional):"), tagsBox);

    m_remBarChb           = new QCheckBox(i18n("Remove information bar on thumbnails"));
    m_remBarChb->setChecked(false);
    
    tagsLayout->addWidget(m_privateImagesChb, 0, 0);
    tagsLayout->addWidget(tagsLbl           , 1, 0);
    tagsLayout->addWidget(m_tagsFld         , 1, 1);
    
    addWidgetToSettingsBox(tagsBox);
    
    getUploadBox()->hide();
    getSizeBox()->hide();    

    updateLabels();
}

ImageshackWidget::~ImageshackWidget()
{
}

// void ImageshackWidget::removeVideosFromList()
// {
//     QList<QUrl> urls = m_imgList->imageUrls();
// 
//     for (int i = 0; i < urls.size(); ++i)
//     {
//         QMimeDatabase db;
//         QMimeType ptr = db.mimeTypeForUrl(urls[i]);
//         QString mime = ptr.name();
// 
//         if (mime.startsWith(QLatin1String("video/")))
//             m_imgList->removeItemByUrl(urls[i]);
//     }
// }

// KIPIPlugins::KPImagesList* ImageshackWidget::imagesList() const
// {
//     return m_imgList;
// }
// 
// KIPIPlugins::KPProgressWidget* ImageshackWidget::progressBar() const
// {
//     return m_progressBar;
// }

void ImageshackWidget::updateLabels(const QString& name, const QString& url)
{
    if (m_imageshack->loggedIn())
    {
        m_accountNameLbl->setText(m_imageshack->username());
        //m_accountEmailLbl->setText(m_imageshack->email());
    }
    else
    {
        m_accountNameLbl->clear();
        //m_accountEmailLbl->clear();
    }
}

// void ImageshackWidget::updateResizeOpts()
// {
//     QStringList titleList;
//     QStringList valueList;
//     valueList << QStringLiteral("100x75");
//     valueList << QStringLiteral("150x122");
//     valueList << QStringLiteral("320x240");
//     valueList << QStringLiteral("640x480");
//     valueList << QStringLiteral("800x600");
//     valueList << QStringLiteral("1024x768");
//     valueList << QStringLiteral("1280x1024");
//     valueList << QStringLiteral("1600x1200");
//     valueList << QStringLiteral("resample");
// 
//     titleList << i18n("100x75 (avatar)");
//     titleList << i18n("150x122 (thumbnail)");
//     titleList << i18n("320x240 (for websites and email)");
//     titleList << i18n("640x480 (for message boards)");
//     titleList << i18n("800x600 (15-inch monitor)");
//     titleList << i18n("1024x768 (17-inch monitor)");
//     titleList << i18n("1280x1024 (19-inch monitor)");
//     titleList << i18n("1600x1200 (21-inch monitor)");
//     titleList << i18n("optimize without resizing");
// 
//     for (int i = 0; i < titleList.size(); i++)
//     {
//         m_resizeOptsCob->addItem(titleList.at(i), valueList.at(i));
//     }
// }

// void ImageshackWidget::slotEnablePredefComboBox(bool checked)
// {
//     m_resizeOptsCob->setEnabled(checked);
// }

// void ImageshackWidget::getGalleriesDone(int errCode)
// {
//     m_galleriesBox->setEnabled(errCode == 0);
// }

void ImageshackWidget::slotGetGalleries(const QStringList &gTexts, const QStringList &gNames)
{
    m_galleriesCob->clear();

    m_galleriesCob->addItem(i18nc("@item:inlistbox", "Add to root folder"),
                            QStringLiteral("--add-to-root--"));    
    
    m_galleriesCob->addItem(i18nc("@item:inlistbox", "Create new gallery"),
                            QStringLiteral("--new-gallery--"));

    // TODO check if the lists have the same size
    for (int i = 0; i < gTexts.size(); ++i)
        m_galleriesCob->addItem(gTexts[i], gNames[i]);

//     slotEnableNewGalleryLE(m_galleriesCob->currentIndex());
}

// void ImageshackWidget::slotEnableNewGalleryLE(int index)
// {
//     m_newGalleryName->setEnabled(!index);
// }

void ImageshackWidget::slotReloadGalleries()
{
    emit signalReloadGalleries();
}

}  // namespace KIPIImageshackPlugin
