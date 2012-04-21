/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010      by Jens Mueller <tschenser at gmx dot de>
 * Copyright (C) 2010-2012 by Caulier Gilles <caulier dot gilles at gmail dot com>
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

#include "picasawebwidget.moc"

// Qt includes

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kpushbutton.h>

// Libkipi includes

#include <libkipi/imagecollection.h>

// Local includes

#include "picasawebimglist.h"
#include "kpprogresswidget.h"

namespace KIPIPicasawebExportPlugin
{

PicasawebWidget::PicasawebWidget(QWidget* const parent, Interface* const iface, bool import)
    : QWidget(parent)
{
    setObjectName("PicasawebWidget");

    QHBoxLayout* mainLayout        = new QHBoxLayout(this);
    m_imgList                      = new PicasawebImagesList(this);
    QWidget* settingsBox           = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);
    m_headerLbl                    = new QLabel(settingsBox);
    m_headerLbl->setWhatsThis(
        i18n("This is a clickable link to open the Picasaweb home page in a web browser."));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    // ------------------------------------------------------------------------

    QGroupBox* accountBox         = new QGroupBox(i18n("Account"), settingsBox);
    accountBox->setWhatsThis(i18n("This is the Picasaweb account that will be used to authenticate."));
    QGridLayout* accountBoxLayout = new QGridLayout(accountBox);

    m_anonymousRBtn     = new QRadioButton(i18nc("Picasaweb account login", "Anonymous"), accountBox);
    m_anonymousRBtn->setWhatsThis(i18n("Login as anonymous to Picasaweb web service."));

    m_accountRBtn       = new QRadioButton(i18n("Picasaweb Account"), accountBox);
    m_accountRBtn->setWhatsThis(i18n("Login to Picasaweb web service using username and password."));

    m_userNameLbl       = new QLabel(i18nc("Picasaweb account settings", "Name:"), accountBox);
    m_userName          = new QLabel(accountBox);
    m_changeUserBtn     = new KPushButton(KGuiItem(i18n("Change Account"), "system-switch-user",
                              i18n("Change Picasaweb Account used to authenticate")), accountBox);

    accountBoxLayout->addWidget(m_anonymousRBtn, 0, 0, 1, 2);
    accountBoxLayout->addWidget(m_accountRBtn,   1, 0, 1, 2);
    accountBoxLayout->addWidget(m_userNameLbl,   2, 0, 1, 1);
    accountBoxLayout->addWidget(m_userName,      2, 1, 1, 1);
    accountBoxLayout->addWidget(m_changeUserBtn, 3, 1, 1, 1);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    QGroupBox* albumsBox         = new QGroupBox(i18n("Album"), settingsBox);
    albumsBox->setWhatsThis(i18n("This is the Picasaweb album that will be used for transfer."));
    QGridLayout* albumsBoxLayout = new QGridLayout(albumsBox);

    m_albumsCoB         = new KComboBox(albumsBox);
    m_albumsCoB->setEditable(false);

    m_newAlbumBtn       = new KPushButton(KGuiItem(i18n("New Album"), "list-add",
                              i18n("Create new Picasaweb album")), accountBox);
    m_reloadAlbumsBtn   = new KPushButton(KGuiItem(i18nc("reload album list", "Reload"), "view-refresh",
                              i18n("Reload album list")), accountBox);

    albumsBoxLayout->addWidget(m_albumsCoB,       0, 0, 1, 5);
    albumsBoxLayout->addWidget(m_newAlbumBtn,     1, 3, 1, 1);
    albumsBoxLayout->addWidget(m_reloadAlbumsBtn, 1, 4, 1, 1);

    // ------------------------------------------------------------------------

    QGroupBox* sizeBox         = new QGroupBox(i18n("Max Dimension"), settingsBox);
    sizeBox->setWhatsThis(i18n("This is the maximum dimension of the images. Images larger than this will be scaled down."));
    QVBoxLayout* sizeBoxLayout = new QVBoxLayout(sizeBox);
    m_dlDimensionCoB           = new KComboBox(sizeBox);
    m_dlDimensionCoB->addItem("Original Size", "d");
    m_dlDimensionCoB->addItem("1600 px", "1600");
    m_dlDimensionCoB->addItem("1440 px", "1440");
    m_dlDimensionCoB->addItem("1280 px", "1280");
    m_dlDimensionCoB->addItem("1152 px", "1152");
    m_dlDimensionCoB->addItem("1024 px", "1024");
    m_dlDimensionCoB->setCurrentIndex(0);
    sizeBoxLayout->addWidget(m_dlDimensionCoB);

    // ------------------------------------------------------------------------

    QGroupBox* uploadBox    = new QGroupBox(i18n("Destination"), settingsBox);
    uploadBox->setWhatsThis(i18n("This is the location where Picasaweb images will be downloaded."));
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadBox);
    m_uploadWidget = iface->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(m_uploadWidget);

    // ------------------------------------------------------------------------

    QGroupBox* optionsBox         = new QGroupBox(i18n("Options"), settingsBox);
    optionsBox->setWhatsThis(i18n("These are options that will be applied to images before upload."));
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

    QSpacerItem* spacer = new QSpacerItem(1, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QLabel* tagsLbl     = new QLabel(i18n("Tag path behavior:"), optionsBox);

    QRadioButton* leafTagsBtn     = new QRadioButton(i18n("Leaf tags only"), optionsBox);
    leafTagsBtn->setWhatsThis(i18n("Export only the leaf tags of tag hierarchies"));
    QRadioButton* splitTagsBtn    = new QRadioButton(i18n("Split tags"), optionsBox);
    splitTagsBtn->setWhatsThis(i18n("Export the leaf tag and all ancestors as single tags."));
    QRadioButton* combinedTagsBtn = new QRadioButton(i18n("Combined String"), optionsBox);
    combinedTagsBtn->setWhatsThis(i18n("Build a combined tag string."));

    m_tagsBGrp = new QButtonGroup(optionsBox);
    m_tagsBGrp->addButton(leafTagsBtn, PwTagLeaf);
    m_tagsBGrp->addButton(splitTagsBtn, PwTagSplit);
    m_tagsBGrp->addButton(combinedTagsBtn, PwTagCombined);

    optionsBoxLayout->addWidget(m_resizeChB,       0, 0, 1, 5);
    optionsBoxLayout->addWidget(imageQualityLbl,   1, 1, 1, 1);
    optionsBoxLayout->addWidget(m_imageQualitySpB, 1, 2, 1, 1);
    optionsBoxLayout->addWidget(dimensionLbl,      2, 1, 1, 1);
    optionsBoxLayout->addWidget(m_dimensionSpB,    2, 2, 1, 1);
    optionsBoxLayout->addItem(spacer,              3, 0, 1, 5);
    optionsBoxLayout->addWidget(tagsLbl,           4, 0, 1, 5);
    optionsBoxLayout->addWidget(leafTagsBtn,       5, 1, 1, 4);
    optionsBoxLayout->addWidget(splitTagsBtn,      6, 1, 1, 4);
    optionsBoxLayout->addWidget(combinedTagsBtn,   7, 1, 1, 4);
    optionsBoxLayout->setRowStretch(8, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    m_progressBar = new KPProgressWidget(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    // ------------------------------------------------------------------------

    settingsBoxLayout->addWidget(m_headerLbl);
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->addWidget(albumsBox);
    settingsBoxLayout->addWidget(sizeBox);
    settingsBoxLayout->addWidget(uploadBox);
    settingsBoxLayout->addWidget(optionsBox);
    settingsBoxLayout->addWidget(m_progressBar);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    updateLabels();  // use empty labels until login

    // ------------------------------------------------------------------------

    connect(m_changeUserBtn, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));

    connect(m_resizeChB, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));

    connect(m_anonymousRBtn, SIGNAL(toggled(bool)),
            this, SLOT(slotAnonymousToggled(bool)) );

    // ------------------------------------------------------------------------

    if (import)
    {
        m_anonymousRBtn->hide();
        m_accountRBtn->hide();

        m_imgList->hide();
        m_newAlbumBtn->hide();
        optionsBox->hide();
    }
    else
    {
        m_anonymousRBtn->hide();
        m_accountRBtn->hide();

        uploadBox->hide();
        sizeBox->hide();
    }
}

PicasawebWidget::~PicasawebWidget()
{
}

KPImagesList* PicasawebWidget::imagesList() const
{
    return m_imgList;
}

KPProgressWidget* PicasawebWidget::progressBar() const
{
    return m_progressBar;
}

QString PicasawebWidget::getDestinationPath()
{
    return m_uploadWidget->selectedImageCollection().uploadPath().path();
}

void PicasawebWidget::updateLabels(const QString& name)
{
    m_userName->setText(QString("<b>%1</b>").arg(name));
    m_headerLbl->setText(QString("<b><h2><a href='http://picasaweb.google.com/%1'>"
                                 "<font color=\"#9ACD32\">Picasaweb</font>"
                                 "</a></h2></b>").arg(name));
}

void PicasawebWidget::slotChangeUserClicked()
{
    emit signalUserChangeRequest(false);
}

void PicasawebWidget::slotResizeChecked()
{
    m_dimensionSpB->setEnabled(m_resizeChB->isChecked());
    m_imageQualitySpB->setEnabled(m_resizeChB->isChecked());
}

} // namespace KIPIPicasawebExportPlugin
