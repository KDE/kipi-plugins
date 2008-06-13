/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QLayout>

// KDE includes.

#include <ktabwidget.h>
#include <klineedit.h>
#include <kdialog.h>
#include <klocale.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kseparator.h>
#include <kiconloader.h>
#include <kapplication.h>

// Libkipi includes.

#include <libkipi/interface.h>

// Local includes.

#include "imageslist.h"
#include "flickrwidget.h"
#include "flickrwidget.moc"

namespace KIPIFlickrExportPlugin
{

FlickrWidget::FlickrWidget(QWidget* parent, KIPI::Interface *iface)
            : QWidget(parent)
{
    setObjectName("FlickrWidget");

    QVBoxLayout* flickrWidgetLayout = new QVBoxLayout(this);

    m_photoView         = 0; //new KHTMLPart(splitter);
    KSeparator *line    = new KSeparator(Qt::Horizontal, this);
    m_tab               = new KTabWidget(this);
    QLabel *headerLabel = new QLabel(this);
    headerLabel->setOpenExternalLinks(true);
    headerLabel->setFocusPolicy(Qt::NoFocus);
    headerLabel->setText(i18n("<qt><b><h2><a href='http://www.flickr.com'>"
                              "<font color=\"#0065DE\">flick</font>"
                              "<font color=\"#FF0084\">r</font></a>"
                              " Export"
                              "</h2></b></qt>"));

    // -------------------------------------------------------------------

    m_imglst                       = new ImagesList(m_tab, iface);
    QWidget* settingsBox           = new QWidget(m_tab);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    //m_newAlbumBtn = new QPushButton(settingsBox, "m_newAlbumBtn");
    //m_newAlbumBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    //m_newAlbumBtn->setText(i18n("&New Album"));

    QGridLayout* tagsLayout  = new QGridLayout();
    QLabel* tagsLabel        = new QLabel(i18n("Added Tags: "), settingsBox);
    m_tagsLineEdit           = new KLineEdit(settingsBox);
    m_exportHostTagsCheckBox = new QCheckBox(settingsBox);
    m_exportHostTagsCheckBox->setText(i18n("Use Host Application Tags"));
    m_stripSpaceTagsCheckBox = new QCheckBox(settingsBox);
    m_stripSpaceTagsCheckBox->setText(i18n("Strip Space From Host Application Tags"));
    m_tagsLineEdit->setToolTip(i18n("Enter here new tags separated by space."));

    tagsLayout->addWidget(tagsLabel,                0, 0);
    tagsLayout->addWidget(m_tagsLineEdit,           0, 1);
    tagsLayout->addWidget(m_exportHostTagsCheckBox, 1, 1);
    tagsLayout->addWidget(m_stripSpaceTagsCheckBox, 2, 1);

    // ------------------------------------------------------------------------

    QGroupBox* optionsBox         = new QGroupBox(i18n("Override Default Options"), settingsBox);
    QGridLayout* optionsBoxLayout = new QGridLayout(optionsBox);

    m_publicCheckBox = new QCheckBox(optionsBox);
    m_publicCheckBox->setText(i18nc("As in accessible for people", "Public (anyone can see them)"));

    m_familyCheckBox = new QCheckBox(optionsBox);
    m_familyCheckBox->setText(i18n("Visible to Family"));

    m_friendsCheckBox = new QCheckBox(optionsBox);
    m_friendsCheckBox->setText(i18n("Visible to Friends"));

    m_resizeCheckBox = new QCheckBox(optionsBox);
    m_resizeCheckBox->setText(i18n("Resize photos before uploading"));
    m_resizeCheckBox->setChecked(false);

    m_dimensionSpinBox = new QSpinBox(optionsBox);
    m_dimensionSpinBox->setMinimum(0);
    m_dimensionSpinBox->setMaximum(5000);
    m_dimensionSpinBox->setSingleStep(10);
    m_dimensionSpinBox->setValue(600);
    m_dimensionSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_dimensionSpinBox->setEnabled(false);

    QLabel* resizeLabel   = new QLabel(i18n("Maximum dimension (pixels):"), optionsBox);

    m_imageQualitySpinBox = new QSpinBox(optionsBox);
    m_imageQualitySpinBox->setMinimum(0);
    m_imageQualitySpinBox->setMaximum(100);
    m_imageQualitySpinBox->setSingleStep(1);
    m_imageQualitySpinBox->setValue(85);
    m_imageQualitySpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // NOTE: The term Compression factor may be to technical to write in the label
    QLabel* imageQualityLabel = new QLabel(i18n("JPEG Image Quality (higher is better):"), optionsBox);

    optionsBoxLayout->addWidget(m_publicCheckBox,      0, 0, 1, 4);
    optionsBoxLayout->addWidget(m_familyCheckBox,      1, 0, 1, 4);
    optionsBoxLayout->addWidget(m_friendsCheckBox,     2, 0, 1, 4);
    optionsBoxLayout->addWidget(imageQualityLabel,     3, 0, 1, 3);
    optionsBoxLayout->addWidget(m_imageQualitySpinBox, 3, 3, 1, 1);
    optionsBoxLayout->addWidget(m_resizeCheckBox,      4, 0, 1, 4);
    optionsBoxLayout->addWidget(resizeLabel,           5, 1, 1, 2);
    optionsBoxLayout->addWidget(m_dimensionSpinBox,    5, 3, 1, 1);
    optionsBoxLayout->setColumnMinimumWidth(0, KDialog::spacingHint());
    optionsBoxLayout->setColumnStretch(1, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    QGroupBox* accountBox         = new QGroupBox(i18n("Account"), settingsBox);
    QGridLayout* accountBoxLayout = new QGridLayout(accountBox);

    QLabel *userNameLabel  = new QLabel(i18n("User Name: "), accountBox);
    m_userNameDisplayLabel = new QLabel(accountBox);
    m_changeUserButton     = new QPushButton(accountBox);
    m_changeUserButton->setText(i18n("Use a different account"));
    m_changeUserButton->setIcon(SmallIcon("switchuser"));

    accountBoxLayout->addWidget(userNameLabel,          0, 0, 1, 1);
    accountBoxLayout->addWidget(m_userNameDisplayLabel, 0, 1, 1, 1);
    accountBoxLayout->addWidget(m_changeUserButton,     0, 3, 1, 1);
    accountBoxLayout->setColumnStretch(2, 10);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    settingsBoxLayout->addLayout(tagsLayout);
    settingsBoxLayout->addWidget(optionsBox);
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->addStretch(10);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    flickrWidgetLayout->addWidget(headerLabel);
    flickrWidgetLayout->addWidget(line);
    flickrWidgetLayout->addWidget(m_tab, 5);
    flickrWidgetLayout->setSpacing(KDialog::spacingHint());
    flickrWidgetLayout->setMargin(0);

    m_tab->insertTab(FILELIST, m_imglst,    i18n("Files List"));
    m_tab->insertTab(UPLOAD,   settingsBox, i18n("Upload Options"));

    // ------------------------------------------------------------------------

    connect(m_resizeCheckBox, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));

    connect(m_exportHostTagsCheckBox, SIGNAL(clicked()),
            this, SLOT(slotExportHostTagsChecked()));
}

FlickrWidget::~FlickrWidget()
{
}

void FlickrWidget::slotResizeChecked()
{
    m_dimensionSpinBox->setEnabled(m_resizeCheckBox->isChecked());
}

void FlickrWidget::slotExportHostTagsChecked()
{
    m_stripSpaceTagsCheckBox->setEnabled(m_exportHostTagsCheckBox->isChecked());
}

} // namespace KIPIFlickrExportPlugin
