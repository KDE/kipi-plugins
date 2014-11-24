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
    : QWidget(parent), m_imageshack(imageshack)
{
    setObjectName("ImageshackWidget");


    QHBoxLayout* const mainLayout = new QHBoxLayout(this);

    // --------------------------------------------

    m_imgList = new KIPIPlugins::KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KIPIPlugins::KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(false);
    m_imgList->loadImagesFromCurrentSelection();
    removeVideosFromList();
    m_imgList->setWhatsThis(i18n("Images to upload to Imageshack web service"));

    QWidget* const settingsBox           = new QWidget(this);
    QVBoxLayout* const settingsBoxLayout = new QVBoxLayout(settingsBox);

    m_headerLbl = new QLabel(settingsBox);
    m_headerLbl->setWhatsThis(i18n("This is a clickable link to open the Imageshack home page in a web browser"));
    m_headerLbl->setText(QString("<b><h2><a href='http://imageshack.us'>%1</a></h2></b>").arg(i18n("ImageShack")));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    // --------------------------------------------

    QGroupBox* const accountBox = new QGroupBox(i18n("Account"), settingsBox);
    accountBox->setWhatsThis(i18n("Imageshack account info"));
    QGridLayout* const accountBoxLayout = new QGridLayout(accountBox);

    QLabel* const accountName  = new QLabel(i18nc("Account info", "Name:"),  accountBox);
    QLabel* const accountEmail = new QLabel(i18nc("Account info", "Email:"), accountBox);

    m_accountNameLbl  = new QLabel(accountBox);
    m_accountEmailLbl = new QLabel(accountBox);
    m_chgRegCodeBtn   = new KPushButton(KGuiItem(i18n("Change account"), "system-switch-user",
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

    m_galleriesBox = new QGroupBox(settingsBox);

    // TODO implement the download

    m_galleriesBox->setTitle(i18n("Destination"));
    m_galleriesBox->setWhatsThis(i18n("This is the ImageShack gallery to which selected items will be uploaded"));

    QGridLayout* const galleriesBoxLayout = new QGridLayout(m_galleriesBox);

    m_useGalleriesChb = new QCheckBox(m_galleriesBox);
    m_useGalleriesChb->setText(i18n("Upload to galleries"));
//    useGalleries->setEnabled(false);

    m_galleriesWidget           = new QWidget(m_galleriesBox);
    QGridLayout* const gwLayout = new QGridLayout(m_galleriesWidget);

    QLabel* const galLbl = new QLabel(i18n("Gallery:"), m_galleriesWidget);
    m_galleriesCob       = new KComboBox(m_galleriesWidget);
    m_galleriesCob->addItem(i18n("Create new gallery"), "--new-gallery--");
    m_galleriesCob->setEditable(false);

    QLabel* const gallNameLbl = new QLabel(m_galleriesWidget);
    gallNameLbl->setText(i18n("Name:"));

    m_newGalleryName          = new QLineEdit(m_galleriesWidget);
    m_reloadGalleriesBtn      = new KPushButton(KGuiItem(i18nc("ImageShack galleries list", "Reload"),
                                                         "view-refresh", i18n("Reload ImageShack galleries list")), m_galleriesWidget);

    galleriesBoxLayout->addWidget(m_useGalleriesChb,    0, 0, 1, 5);
    galleriesBoxLayout->addWidget(m_galleriesWidget,    1, 0, 5, 5);

    gwLayout->addWidget(galLbl,               0, 0, 1, 1);
    gwLayout->addWidget(m_galleriesCob,       0, 1, 1, 4);
    gwLayout->addWidget(gallNameLbl,          1, 0, 1, 1);
    gwLayout->addWidget(m_newGalleryName,     1, 1, 1, 3);
    gwLayout->addWidget(m_reloadGalleriesBtn, 1, 4, 1, 1);


    connect(m_useGalleriesChb, SIGNAL(toggled(bool)),
            m_galleriesWidget, SLOT(setEnabled(bool)));

    connect(m_galleriesCob, SIGNAL(activated(int)),
            this, SLOT(slotEnableNewGalleryLE(int)));

    connect(m_reloadGalleriesBtn, SIGNAL(clicked()),
            this, SLOT(slotReloadGalleries()));

    m_galleriesWidget->setEnabled(m_useGalleriesChb->isChecked());

    // ----------------------------------------------

    QGroupBox* const optionsBox         = new QGroupBox(i18n("Options"), settingsBox);
    optionsBox->setWhatsThis(i18n("These are the options that will be applied to photos"));
    QGridLayout* const optionsBoxLayout = new QGridLayout(optionsBox);

    m_privateImagesChb = new QCheckBox(optionsBox);
    m_privateImagesChb->setText(i18n("Make private"));
    m_privateImagesChb->setChecked(false);

    QGroupBox* const resizeOptions = new QGroupBox(i18n("Resize options"), optionsBox);
    QGridLayout* const resizeOptionsLayout = new QGridLayout(resizeOptions);

    m_noResizeRdb = new QRadioButton(optionsBox);
    m_noResizeRdb->setText(i18n("Do not resize photos"));

    m_predefSizeRdb = new QRadioButton(optionsBox);
    m_predefSizeRdb->setText(i18n("Predefined templates"));

    connect(m_predefSizeRdb, SIGNAL(toggled(bool)),
            this, SLOT(slotEnablePredefComboBox(bool)));

    m_customSizeRdb = new QRadioButton(optionsBox);
    m_customSizeRdb->setText(i18n("Custom resize dimensions"));

    m_widthSpb = new QSpinBox(optionsBox);
    m_widthSpb->setMinimum(1);
    m_widthSpb->setMaximum(6000);
    m_widthSpb->setSingleStep(10);
    m_widthSpb->setValue(1000);
    m_widthSpb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_widthSpb->setEnabled(false);
    QLabel* const widthLbl = new QLabel(i18n("Width:"), optionsBox);

    m_heightSpb = new QSpinBox(optionsBox);
    m_heightSpb->setMinimum(1);
    m_heightSpb->setMaximum(6000);
    m_heightSpb->setSingleStep(10);
    m_heightSpb->setValue(1000);
    m_heightSpb->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_heightSpb->setEnabled(false);

    connect(m_customSizeRdb, SIGNAL(toggled(bool)),
            this, SLOT(slotEnableCustomSize(bool)));

    QLabel* const heightLbl = new QLabel(i18n("Height:"), optionsBox);

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

    m_tagsFld             = new QLineEdit(settingsBox);
    QLabel* const tagsLbl = new QLabel(i18n("Tags (optional):"), settingsBox);

    m_remBarChb           = new QCheckBox(i18n("Remove information bar on thumbnails"));
    m_remBarChb->setChecked(false);

    optionsBoxLayout->addWidget(m_privateImagesChb, 0, 0, 1, 2);
    optionsBoxLayout->addWidget(resizeOptions,      1, 0, 1, 2);
    optionsBoxLayout->addWidget(tagsLbl,            2, 0, 1, 1);
    optionsBoxLayout->addWidget(m_tagsFld,          2, 1, 1, 1);
    optionsBoxLayout->addWidget(m_remBarChb,        3, 0, 1, 2);
    optionsBoxLayout->setRowStretch(4, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    m_progressBar         = new KIPIPlugins::KPProgressWidget(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred ,QSizePolicy::Fixed);
    m_progressBar->setVisible(false);

    // --------------------------------------------

    settingsBoxLayout->addWidget(m_headerLbl);
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->addWidget(m_galleriesBox);
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

void ImageshackWidget::removeVideosFromList()
{
    KUrl::List urls = m_imgList->imageUrls();

    for (int i = 0; i < urls.size(); ++i)
    {
        KMimeType::Ptr mimePtr = KMimeType::findByUrl(urls[i]);

        if (mimePtr->name().startsWith(QLatin1String("video/")))
            m_imgList->removeItemByUrl(urls[i]);
    }
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
    titleList << i18n("100x75 (avatar)");
    titleList << i18n("150x122 (thumbnail)");
    titleList << i18n("320x240 (for websites and email)");
    titleList << i18n("640x480 (for message boards)");
    titleList << i18n("800x600 (15-inch monitor)");
    titleList << i18n("1024x768 (17-inch monitor)");
    titleList << i18n("1280x1024 (19-inch monitor)");
    titleList << i18n("1600x1200 (21-inch monitor)");
    titleList << i18n("optimize without resizing");

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

void ImageshackWidget::getGalleriesDone(int errCode)
{
    m_galleriesBox->setEnabled(errCode == 0);
}

void ImageshackWidget::slotGetGalleries(const QStringList &gTexts, const QStringList &gNames)
{
    m_galleriesCob->clear();

    m_galleriesCob->addItem(i18n("Create new gallery."), "--new-gallery--");

    // TODO check if the lists have the same size
    for (int i = 0; i < gTexts.size(); ++i)
        m_galleriesCob->addItem(gTexts[i], gNames[i]);

    slotEnableNewGalleryLE(m_galleriesCob->currentIndex());
}

void ImageshackWidget::slotEnableNewGalleryLE(int index)
{
    m_newGalleryName->setEnabled(!index);
}

void ImageshackWidget::slotReloadGalleries()
{
    emit signalReloadGalleries();
}

}  // namespace KIPIImageshackExportPlugin
