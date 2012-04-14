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

#include "imageshackwidget.moc"

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

// KDE includes

#include <kpushbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kdialog.h>
#include <kdebug.h>

// LibKIPI includes

#include <libkipi/uploadwidget.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpimageslist.h"
#include "imageshack.h"
#include "kpprogresswidget.h"

namespace KIPIImageshackExportPlugin
{

ImageshackWidget::ImageshackWidget(QWidget* const parent, Imageshack* const imageshack)
    : QWidget(parent)
{
    setObjectName("ImageshackWidget");

    m_imageshack = imageshack;

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // --------------------------------------------

    m_imgList = new KIPIPlugins::KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KIPIPlugins::KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(false);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->setWhatsThis("Images to upload to Imageshack web service");

    QWidget* settingsBox           = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    m_headerLbl = new QLabel(settingsBox);
    m_headerLbl->setWhatsThis(i18n("This is a clickable link to open the Imageshack home page in a web browser"));
    m_headerLbl->setText(QString("<b><h2><a href='http://imageshack.us'>ImageShack</a></h2></b>"));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    // --------------------------------------------

    QGroupBox* accountBox = new QGroupBox(i18n("Account"), settingsBox);
    accountBox->setWhatsThis(i18n("Imageshack account info"));
    QGridLayout *accountBoxLayout = new QGridLayout(accountBox);

    QLabel* accountName  = new QLabel(i18nc("Account info", "Name:"),  accountBox);
    QLabel* accountEmail = new QLabel(i18nc("Account info", "Email:"), accountBox);

    m_accountNameLbl  = new QLabel(accountBox);
    m_accountEmailLbl = new QLabel(accountBox);
    m_chgRegCodeBtn   = new KPushButton(KGuiItem(i18n("Change account"),
                                                 "system-switch-user",
                                                 i18n("Change the registration code for the Imageshack account used to upload images")),
                                        accountBox);

    accountBoxLayout->addWidget(accountName,          0, 0, 1, 1);
    accountBoxLayout->addWidget(m_accountNameLbl,     0, 1, 1, 2);
    accountBoxLayout->addWidget(accountEmail,         1, 0, 1, 1);
    accountBoxLayout->addWidget(m_accountEmailLbl,    1, 1, 1, 2);
    accountBoxLayout->addWidget(m_chgRegCodeBtn,      2, 0, 1, 1);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    // ----------------------------------------------

    QGroupBox* galleriesBox = new QGroupBox(settingsBox);

    // TODO implement the download

    galleriesBox->setTitle(i18n("Destination"));
    galleriesBox->setWhatsThis(i18n("This is the ImageShack gallery to which selected items will be uploaded"));

    QGridLayout* galleriesBoxLayout = new QGridLayout(galleriesBox);

    QLabel* galLbl = new QLabel(i18n("Gallery:"), galleriesBox);
    m_galleriesCob = new KComboBox(galleriesBox);
    m_galleriesCob->setEditable(false);

    m_newGalleryBtn = new KPushButton(KGuiItem(i18n("Create new gallery"), "list-add",
                                               i18n("Create new ImageShack gallery")), galleriesBox);

    m_reloadGalleriesBtn = new KPushButton(KGuiItem(i18nc("ImageShack galleries list", "Reload"),
                                                    "view-refresh", i18n("Reload ImageShack galleries list")), galleriesBox);

    galleriesBoxLayout->addWidget(galLbl,               0, 0, 1, 1);
    galleriesBoxLayout->addWidget(m_galleriesCob,       0, 1, 1, 4);
    galleriesBoxLayout->addWidget(m_newGalleryBtn,      1, 3, 1, 1);
    galleriesBoxLayout->addWidget(m_reloadGalleriesBtn, 1, 4, 1, 1);

    // ----------------------------------------------

    QGroupBox* optionsBox = new QGroupBox(i18n("Options"), settingsBox);
    optionsBox->setWhatsThis("These are the options that will be applied to photos");
    QGridLayout* optionsBoxLayout = new QGridLayout(optionsBox);

    m_privateImagesChb = new QCheckBox(optionsBox);
    m_privateImagesChb->setText(i18n("Make private"));
    m_privateImagesChb->setChecked(false);

    QGroupBox* resizeOptions = new QGroupBox(i18n("Resize options"), optionsBox);
    QGridLayout* resizeOptionsLayout = new QGridLayout(resizeOptions);

    m_noResizeRdb = new QRadioButton(optionsBox);
    m_noResizeRdb->setText(i18n("Do not resize photos"));

    m_predefSizeRdb = new QRadioButton(optionsBox);
    m_predefSizeRdb->setText(i18n("Predefined templates"));
    connect(m_predefSizeRdb, SIGNAL(toggled(bool)),
            this, SLOT(slotEnablePredefComboBox(bool)));

    m_customSizeRdb = new QRadioButton(optionsBox);
    m_customSizeRdb->setText("Custom resize dimensions");

    m_widthSpb = new QSpinBox(optionsBox);
    m_widthSpb->setMinimum(1);
    m_widthSpb->setMaximum(6000);
    m_widthSpb->setSingleStep(10);
    m_widthSpb->setValue(1000);
    m_widthSpb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_widthSpb->setEnabled(false);
    QLabel*  widthLbl = new QLabel(i18n("Width:"), optionsBox);

    m_heightSpb = new QSpinBox(optionsBox);
    m_heightSpb->setMinimum(1);
    m_heightSpb->setMaximum(6000);
    m_heightSpb->setSingleStep(10);
    m_heightSpb->setValue(1000);
    m_heightSpb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_heightSpb->setEnabled(false);

    connect(m_customSizeRdb, SIGNAL(toggled(bool)),
            this, SLOT(slotEnableCustomSize(bool)));

    QLabel* heightLbl = new QLabel(i18n("Height"), optionsBox);

    m_resizeOptsCob = new KComboBox(optionsBox);
    updateResizeOpts();
    m_resizeOptsCob->setEditable(false);

    m_noResizeRdb->setChecked(true);
    m_resizeOptsCob->setEnabled(false);

    resizeOptionsLayout->addWidget(m_noResizeRdb,   0, 0, 1, 5);
    resizeOptionsLayout->addWidget(m_predefSizeRdb, 1, 0, 1, 5);
    resizeOptionsLayout->addWidget(m_resizeOptsCob, 2, 1, 1, 4);
    resizeOptionsLayout->addWidget(m_customSizeRdb, 3, 0, 1, 5);
    resizeOptionsLayout->addWidget(widthLbl,        4, 1, 1, 2);
    resizeOptionsLayout->addWidget(m_widthSpb,      4, 2, 1, 2);
    resizeOptionsLayout->addWidget(heightLbl,       5, 1, 1, 2);
    resizeOptionsLayout->addWidget(m_heightSpb,     5, 2, 1, 2);
    resizeOptionsLayout->setSpacing(KDialog::spacingHint());
    resizeOptionsLayout->setMargin(KDialog::spacingHint());

    m_tagsFld = new QLineEdit(settingsBox);
    QLabel* tagsLbl = new QLabel(i18n("Tags (optional):"), settingsBox);

    m_remBarChb = new QCheckBox(i18n("Remove information bar on thumbnails"));
    m_remBarChb->setChecked(false);

    optionsBoxLayout->addWidget(m_privateImagesChb, 0, 0, 1, 2);
    optionsBoxLayout->addWidget(resizeOptions,      1, 0, 1, 2);
    optionsBoxLayout->addWidget(tagsLbl,            2, 0, 1, 1);
    optionsBoxLayout->addWidget(m_tagsFld,          2, 1, 1, 1);
    optionsBoxLayout->addWidget(m_remBarChb,        3, 0, 1, 2);
    optionsBoxLayout->setRowStretch(4, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    m_progressBar = new KIPIPlugins::KPProgressWidget(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred ,QSizePolicy::Fixed);
    m_progressBar->setVisible(false);

    // --------------------------------------------

    settingsBoxLayout->addWidget(m_headerLbl);
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->addWidget(galleriesBox);
    settingsBoxLayout->addWidget(optionsBox);
    settingsBoxLayout->addWidget(m_progressBar);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    // --------------------------------------------

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    updateLabels();
}

ImageshackWidget::~ImageshackWidget()
{
}

KIPIPlugins::KPImagesList* ImageshackWidget::imagesList() const
{
    return m_imgList;
}

KIPIPlugins::KPProgressWidget* ImageshackWidget::progressBar() const
{
    return m_progressBar;
}

void ImageshackWidget::updateLabels()
{
    if (m_imageshack->loggedIn())
    {
        kDebug() << m_imageshack->username() << " -- " << m_imageshack->email();
        m_accountNameLbl->setText(m_imageshack->username());
        m_accountEmailLbl->setText(m_imageshack->email());
    }
    else
    {
        m_accountNameLbl->clear();
        m_accountEmailLbl->clear();
    }
}

void ImageshackWidget::updateResizeOpts()
{
    QStringList titleList;
    QStringList valueList;
    valueList << "100x75"<< "150x122"<< "320x240"<< "640x480"<< "800x600";
    valueList << "1024x768"<< "1280x1024"<< "1600x1200"<< "resample";
    titleList << "100x75 (avatar)";
    titleList << "150x122 (thumbnail)";
    titleList << "320x240 (for websites and email)";
    titleList << "640x480 (for message boards)";
    titleList << "800x600 (15-inch monitor)";
    titleList << "1024x768 (17-inch monitor)";
    titleList << "1280x1024 (19-inch monitor)";
    titleList << "1600x1200 (21-inch monitor)";
    titleList << "optimize without resizing";

    for (int i = 0; i < titleList.size(); i++)
    {
        m_resizeOptsCob->addItem(titleList.at(i), valueList.at(i));
    }
}

void ImageshackWidget::slotEnablePredefComboBox(bool checked)
{
    m_resizeOptsCob->setEnabled(checked);
}

void ImageshackWidget::slotEnableCustomSize(bool checked)
{
    m_widthSpb->setEnabled(checked);
    m_heightSpb->setEnabled(checked);
}

}  // namespace KIPIImageshackExportPlugin
