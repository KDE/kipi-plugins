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
 * Copyright (C) 2009 by Luka Renko <lure at kubuntu dot org>
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

#include "flickrwidget.h"
#include "flickrwidget.moc"

// Qt includes

#include <QGroupBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>

// KDE includes

#include <kcombobox.h>
#include <ktabwidget.h>
#include <klineedit.h>
#include <kdialog.h>
#include <klocale.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kseparator.h>
#include <kiconloader.h>
#include <kapplication.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "imageslist.h"
#include "flickrlist.h"

namespace KIPIFlickrExportPlugin
{

FlickrWidget::FlickrWidget(QWidget* parent, KIPI::Interface *iface, const QString& serviceName)
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
    if (serviceName == "23")
        headerLabel->setText(i18n("<b><h2><a href='http://www.23hq.com'>"
                                  "<font color=\"#7CD164\">23</font></a>"
                                  " Export"
                                  "</h2></b>"));
    else
        headerLabel->setText(i18n("<b><h2><a href='http://www.flickr.com'>"
                                  "<font color=\"#0065DE\">flick</font>"
                                  "<font color=\"#FF0084\">r</font></a>"
                                  " Export"
                                  "</h2></b>"));

    // -------------------------------------------------------------------

    m_imglst = new KIPIFlickrExportPlugin::FlickrList(iface, m_tab,
                                                      (serviceName == "23"));

    // For figuring out the width of the permission columns.
    QHeaderView *hdr = m_imglst->listView()->header();
    QFontMetrics hdrFont = QFontMetrics(hdr->font());
    int permColWidth = hdrFont.width(i18n("Public"));

    m_imglst->setAllowRAW(true);
    m_imglst->loadImagesFromCurrentSelection();
    m_imglst->listView()->setWhatsThis(i18n("This is the list of images to upload to your Flickr account."));
    m_imglst->listView()->setColumn(static_cast<KIPIPlugins::ImagesListView::ColumnType>(FlickrList::PUBLIC), i18n("Public"), true);
    if (serviceName != "23")
    {
        int tmpWidth;
        if ((tmpWidth = hdrFont.width(i18n("Family"))) > permColWidth)
        {
            permColWidth = tmpWidth;
        }
        if ((tmpWidth = hdrFont.width(i18n("Friends"))) > permColWidth)
        {
            permColWidth = tmpWidth;
        }

        m_imglst->listView()->setColumn(static_cast<KIPIPlugins::ImagesListView::ColumnType>(FlickrList::FAMILY),
                                        i18n("Family"), true);
        m_imglst->listView()->setColumn(static_cast<KIPIPlugins::ImagesListView::ColumnType>(FlickrList::FRIENDS),
                                        i18n("Friends"), true);
        hdr->setResizeMode(FlickrList::FAMILY,  QHeaderView::Interactive);
        hdr->setResizeMode(FlickrList::FRIENDS, QHeaderView::Interactive);
        hdr->resizeSection(FlickrList::FAMILY,  permColWidth);
        hdr->resizeSection(FlickrList::FRIENDS, permColWidth);
    }
    hdr->setResizeMode(FlickrList::PUBLIC, QHeaderView::Interactive);
    hdr->resizeSection(FlickrList::PUBLIC, permColWidth);

    QWidget* settingsBox           = new QWidget(m_tab);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    QGridLayout* albumsSettingsLayout = new QGridLayout();
    QLabel* albumLabel = new QLabel(i18n("PhotoSet:"), settingsBox);
    m_newAlbumBtn = new QPushButton(settingsBox);
    m_newAlbumBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_newAlbumBtn->setText(i18n("&New PhotoSet"));
    m_albumsListComboBox = new KComboBox(settingsBox);
    albumsSettingsLayout->addWidget(albumLabel, 0, 0, 1, 1);
    albumsSettingsLayout->addWidget(m_albumsListComboBox, 0, 1, 1, 1);
    albumsSettingsLayout->addWidget(m_newAlbumBtn, 0, 2, 1, 1);
    albumsSettingsLayout->setColumnStretch(1, 10);
	
    QGridLayout* tagsLayout  = new QGridLayout();
    QLabel* tagsLabel        = new QLabel(i18n("Added Tags: "), settingsBox);
    m_tagsLineEdit           = new KLineEdit(settingsBox);
    m_exportHostTagsCheckBox = new QCheckBox(settingsBox);
    m_exportHostTagsCheckBox->setText(i18n("Use Host Application Tags"));
    m_stripSpaceTagsCheckBox = new QCheckBox(settingsBox);
    m_stripSpaceTagsCheckBox->setText(i18n("Strip Space From Host Application Tags"));
    m_tagsLineEdit->setToolTip(i18n("Enter new tags here, separated by spaces."));

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

    QLabel *userNameLabel  = new QLabel(i18n("Username: "), accountBox);
    m_userNameDisplayLabel = new QLabel(accountBox);
    m_changeUserButton     = new QPushButton(accountBox);
    m_changeUserButton->setText(i18n("Use a different account"));
    m_changeUserButton->setIcon(SmallIcon("system-switch-user"));

    accountBoxLayout->addWidget(userNameLabel,          0, 0, 1, 1);
    accountBoxLayout->addWidget(m_userNameDisplayLabel, 0, 1, 1, 1);
    accountBoxLayout->addWidget(m_changeUserButton,     0, 3, 1, 1);
    accountBoxLayout->setColumnStretch(2, 10);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    settingsBoxLayout->addLayout(albumsSettingsLayout);
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

    m_tab->insertTab(FILELIST, m_imglst,    i18n("File List"));
    m_tab->insertTab(UPLOAD,   settingsBox, i18n("Upload Options"));

    // ------------------------------------------------------------------------

    connect(m_resizeCheckBox, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));

    connect(m_exportHostTagsCheckBox, SIGNAL(clicked()),
            this, SLOT(slotExportHostTagsChecked()));

    connect(m_imglst, SIGNAL(signalPermissionChanged(FlickrList::FieldType, Qt::CheckState)),
            this, SLOT(slotPermissionChanged(FlickrList::FieldType, Qt::CheckState)));

    connect(m_publicCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(slotMainPublicToggled(int)));

    if (serviceName != "23")
    {
        // 23hq.com does not support Family/Friends concept
        connect(m_familyCheckBox, SIGNAL(stateChanged(int)),
                this, SLOT(slotMainFamilyToggled(int)));
        connect(m_friendsCheckBox, SIGNAL(stateChanged(int)),
                this, SLOT(slotMainFriendsToggled(int)));
    }
    else
    {
        // 23hq.com does not support Family/Friends concept, so hide it
        m_familyCheckBox->hide();
        m_friendsCheckBox->hide();
    }
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

void FlickrWidget::slotPermissionChanged(FlickrList::FieldType checkbox,
                                         Qt::CheckState state)
{
    /* Slot for handling the signal from the FlickrList that the general
     * permissions have changed, considering the clicks in the checkboxes next
     * to each image. In response, the main permission checkboxes should be set
     * to the proper state.
     * checkbox_num determins which checkbox should be changed: 0 for public,
     * 1 for family, 2 for friends.
     * state follows the checked state rules from Qt: 0 means false, 1 means
     * intermediate, 2 means true. */

    // Select the proper checkbox.
    QCheckBox *currBox;
    if      (checkbox == FlickrList::PUBLIC) currBox = m_publicCheckBox;
    else if (checkbox == FlickrList::FAMILY) currBox = m_familyCheckBox;
    else                                     currBox = m_friendsCheckBox;

    // If the checkbox should be set in the intermediate state, the tristate
    // property of the checkbox should be manually set to true, otherwise, it
    // has to be set to false so that the user cannot select it.
    currBox->setCheckState(state);
    if ((state == Qt::Checked) || (state == Qt::Unchecked))
        currBox->setTristate(false);
    else
        currBox->setTristate(true);
}

void FlickrWidget::slotMainPublicToggled(int state)
    {mainPermissionToggled(FlickrList::PUBLIC, static_cast<Qt::CheckState>(state));}
void FlickrWidget::slotMainFamilyToggled(int state)
    {mainPermissionToggled(FlickrList::FAMILY, static_cast<Qt::CheckState>(state));}
void FlickrWidget::slotMainFriendsToggled(int state)
    {mainPermissionToggled(FlickrList::FRIENDS, static_cast<Qt::CheckState>(state));}

void FlickrWidget::mainPermissionToggled(FlickrList::FieldType checkbox,
                                         Qt::CheckState state)
{
    /* Callback for when one of the main permission checkboxes is toggled.
     * checkbox specifies which of the checkboxes is toggled. */

    if (state != Qt::PartiallyChecked) {
        // Set the states for the image list.
        if      (checkbox == FlickrList::PUBLIC)  m_imglst->setPublic(state);
        else if (checkbox == FlickrList::FAMILY)  m_imglst->setFamily(state);
        else if (checkbox == FlickrList::FRIENDS) m_imglst->setFriends(state);

        // Dis- or enable the family and friends checkboxes if the public
        // checkbox is clicked.
        if (checkbox == 0)
        {
            if (state == Qt::Checked)
            {
                m_familyCheckBox->setEnabled(false);
                m_friendsCheckBox->setEnabled(false);
            }
            else if (state == Qt::Unchecked)
            {
                m_familyCheckBox->setEnabled(true);
                m_friendsCheckBox->setEnabled(true);
            }
        }

        // Set the main checkbox tristate state to false, so that the user
        // cannot select the intermediate state.
        if (checkbox == FlickrList::PUBLIC)       m_publicCheckBox->setTristate(false);
        else if (checkbox == FlickrList::FAMILY)  m_familyCheckBox->setTristate(false);
        else if (checkbox == FlickrList::FRIENDS) m_friendsCheckBox->setTristate(false);
    }
}

} // namespace KIPIFlickrExportPlugin
