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

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>

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

#include "comboboxdelegate.h"
#include "comboboxintermediate.h"
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
    else if (serviceName == "Zooomr")
        headerLabel->setText(i18n("<b><h2><a href='http://www.zooomr.com'>"
                                  "<font color=\"#7CD164\">zooomr</font></a>"
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

        m_imglst->listView()->setColumn(static_cast<KIPIPlugins::ImagesListView::ColumnType>(FlickrList::SAFETYLEVEL),
                                        i18n("Safety level"), true);
        m_imglst->listView()->setColumn(static_cast<KIPIPlugins::ImagesListView::ColumnType>(FlickrList::CONTENTTYPE),
                                        i18n("Content type"), true);
        QMap<int, QString> safetyLevelItems;
        QMap<int, QString> contentTypeItems;
        safetyLevelItems[FlickrList::SAFE]       = i18n("Safe");
        safetyLevelItems[FlickrList::MODERATE]   = i18n("Moderate");
        safetyLevelItems[FlickrList::RESTRICTED] = i18n("Restricted");
        contentTypeItems[FlickrList::PHOTO]      = i18n("Photo");
        contentTypeItems[FlickrList::SCREENSHOT] = i18n("Screenshot");
        contentTypeItems[FlickrList::OTHER]      = i18n("Other");
        ComboBoxDelegate *safetyLevelDelegate = new ComboBoxDelegate(m_imglst, safetyLevelItems);
        ComboBoxDelegate *contentTypeDelegate = new ComboBoxDelegate(m_imglst, contentTypeItems);
        m_imglst->listView()->setItemDelegateForColumn(static_cast<KIPIPlugins::ImagesListView::ColumnType>(FlickrList::SAFETYLEVEL),
                                                       safetyLevelDelegate);
        m_imglst->listView()->setItemDelegateForColumn(static_cast<KIPIPlugins::ImagesListView::ColumnType>(FlickrList::CONTENTTYPE),
                                                       contentTypeDelegate);
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

    QGroupBox* optionsBox           = new QGroupBox(i18n("Override Default Options"), settingsBox);
    QGridLayout *optionsBoxLayout   = new QGridLayout;
    optionsBox->setLayout(optionsBoxLayout);

    // Wrap the options box in a scroll area, so that it can be resized without
    // squeezing the contents.
    QScrollArea* optionsScrollArea = new QScrollArea;
    optionsScrollArea->setFrameShadow(QFrame::Plain);
    optionsBoxLayout->addWidget(optionsScrollArea, 0, 0);
    QWidget* optionsWidget = new QWidget;
    optionsScrollArea->setWidget(optionsWidget);
    QGridLayout* optionsLayout = new QGridLayout(optionsWidget);

    m_publicCheckBox = new QCheckBox(optionsWidget);
    m_publicCheckBox->setText(i18nc("As in accessible for people", "Public (anyone can see them)"));

    m_familyCheckBox = new QCheckBox(optionsWidget);
    m_familyCheckBox->setText(i18n("Visible to Family"));

    m_friendsCheckBox = new QCheckBox(optionsWidget);
    m_friendsCheckBox->setText(i18n("Visible to Friends"));

    m_resizeCheckBox = new QCheckBox(optionsWidget);
    m_resizeCheckBox->setText(i18n("Resize photos before uploading"));
    m_resizeCheckBox->setChecked(false);

    m_dimensionSpinBox = new QSpinBox(optionsWidget);
    m_dimensionSpinBox->setMinimum(0);
    m_dimensionSpinBox->setMaximum(5000);
    m_dimensionSpinBox->setSingleStep(10);
    m_dimensionSpinBox->setValue(600);
    m_dimensionSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_dimensionSpinBox->setEnabled(false);

    QLabel* resizeLabel   = new QLabel(i18n("Maximum dimension (pixels):"), optionsWidget);

    m_imageQualitySpinBox = new QSpinBox(optionsWidget);
    m_imageQualitySpinBox->setMinimum(0);
    m_imageQualitySpinBox->setMaximum(100);
    m_imageQualitySpinBox->setSingleStep(1);
    m_imageQualitySpinBox->setValue(85);
    m_imageQualitySpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // NOTE: The term Compression factor may be to technical to write in the label
    QLabel* imageQualityLabel = new QLabel(i18n("JPEG Image Quality (higher is better):"), optionsWidget);

    // -- Extended options ----------------------------------------------------

    m_extendedButton = new QPushButton(i18n("Extended options"));
    m_extendedButton->setCheckable(true);
    // Initialize this button to checked, because extended options are shown.
    // FlickrWindow::readSettings can change this, but if checked is false it
    // cannot uncheck and subsequently hide the extended options (the toggled
    // signal won't be emitted).
    m_extendedButton->setChecked(true);
    m_extendedButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    m_extendedSettingsBox = new QGroupBox("", settingsBox);
    m_extendedSettingsBox->setFlat(true);
    QGridLayout *extendedSettingsLayout = new QGridLayout(m_extendedSettingsBox);

    QLabel *imageSafetyLabel = new QLabel(i18n("Safety level:"));
    m_safetyLevelComboBox = new ComboBoxIntermediate();
    m_safetyLevelComboBox->addItem(i18n("Safe"),       QVariant(FlickrList::SAFE));
    m_safetyLevelComboBox->addItem(i18n("Moderate"),   QVariant(FlickrList::MODERATE));
    m_safetyLevelComboBox->addItem(i18n("Restricted"), QVariant(FlickrList::RESTRICTED));

    QLabel *imageTypeLabel = new QLabel(i18n("Content type:"));
    m_contentTypeComboBox = new ComboBoxIntermediate();
    m_contentTypeComboBox->addItem(i18n("Photo"),      QVariant(FlickrList::PHOTO));
    m_contentTypeComboBox->addItem(i18n("Screenshot"), QVariant(FlickrList::SCREENSHOT));
    m_contentTypeComboBox->addItem(i18n("Other"),      QVariant(FlickrList::OTHER));

    extendedSettingsLayout->addWidget(imageSafetyLabel, 1, 0, Qt::AlignLeft);
    extendedSettingsLayout->addWidget(m_safetyLevelComboBox, 1, 1, Qt::AlignLeft);
    extendedSettingsLayout->addWidget(imageTypeLabel, 0, 0, Qt::AlignLeft);
    extendedSettingsLayout->addWidget(m_contentTypeComboBox, 0, 1, Qt::AlignLeft);
    extendedSettingsLayout->setColumnStretch(0, 0);
    extendedSettingsLayout->setColumnStretch(1, 1);

    optionsLayout->addWidget(m_publicCheckBox,      0, 0, 1, 4);
    optionsLayout->addWidget(m_familyCheckBox,      1, 0, 1, 4);
    optionsLayout->addWidget(m_friendsCheckBox,     2, 0, 1, 4);
    optionsLayout->addWidget(imageQualityLabel,     3, 0, 1, 3);
    optionsLayout->addWidget(m_imageQualitySpinBox, 3, 3, 1, 1);
    optionsLayout->addWidget(m_resizeCheckBox,      4, 0, 1, 4);
    optionsLayout->addWidget(resizeLabel,           5, 1, 1, 2);
    optionsLayout->addWidget(m_dimensionSpinBox,    5, 3, 1, 1);
    optionsLayout->addWidget(m_extendedButton,      6, 0, 1, 1);
    optionsLayout->addWidget(m_extendedSettingsBox, 7, 0, 4, 4);
    optionsLayout->setColumnMinimumWidth(0, KDialog::spacingHint());
    optionsLayout->setColumnStretch(1, 10);
    optionsLayout->setSpacing(KDialog::spacingHint());
    optionsLayout->setMargin(KDialog::spacingHint());

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

    optionsWidget->adjustSize();

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

    // Zooomr doesn't support explicit Photosets.
    if (serviceName == "Zooomr")
    {
        albumLabel->hide();
        m_albumsListComboBox->hide();
        m_newAlbumBtn->setEnabled(false);
        m_newAlbumBtn->hide();
    }

    // 23HQ doesn't support the Family and Friends concept.
    if (serviceName != "23")
    {
        connect(m_familyCheckBox, SIGNAL(stateChanged(int)),
                this, SLOT(slotMainFamilyToggled(int)));
        connect(m_friendsCheckBox, SIGNAL(stateChanged(int)),
                this, SLOT(slotMainFriendsToggled(int)));
    }
    else
    {
        m_familyCheckBox->hide();
        m_friendsCheckBox->hide();
    }

    // 23HQ and Zooomr don't support the Safety Level and Content Type concept.
    if ((serviceName != "23") && (serviceName != "Zooomr"))
    {
        connect(m_safetyLevelComboBox, SIGNAL(currentIndexChanged(int)),
                this, SLOT(slotMainSafetyLevelChanged(int)));
        connect(m_contentTypeComboBox, SIGNAL(currentIndexChanged(int)),
                this, SLOT(slotMainContentTypeChanged(int)));
        connect(m_extendedButton, SIGNAL(toggled(bool)),
                this, SLOT(slotExtendedSettingsToggled(bool)));
        connect(m_imglst, SIGNAL(signalSafetyLevelChanged(FlickrList::SafetyLevel)),
                this, SLOT(slotSafetyLevelChanged(FlickrList::SafetyLevel)));
        connect(m_imglst, SIGNAL(signalContentTypeChanged(FlickrList::ContentType)),
                this, SLOT(slotContentTypeChanged(FlickrList::ContentType)));
    }
    else
    {
        m_extendedSettingsBox->hide();
        m_extendedButton->hide();
    }
    optionsWidget->adjustSize();
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
     * The checkbox variable determines which of the checkboxes should be
     * changed. */

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

void FlickrWidget::slotSafetyLevelChanged(FlickrList::SafetyLevel safetyLevel)
{
    /* Called when the general safety level of the FlickrList has changed,
     * considering the individual comboboxes next to the photos. Used to set the
     * main safety level combobox appropriately. */
    if (safetyLevel == FlickrList::MIXEDLEVELS)
    {
        m_safetyLevelComboBox->setIntermediate(true);
    }
    else
    {
        int index = m_safetyLevelComboBox->findData(QVariant(static_cast<int>(safetyLevel)));
        m_safetyLevelComboBox->setCurrentIndex(index);
    }
}

void FlickrWidget::slotContentTypeChanged(FlickrList::ContentType contentType)
{
    /* Called when the general content type of the FlickrList has changed,
     * considering the individual comboboxes next to the photos. Used to set the
     * main content type combobox appropriately. */
    if (contentType == FlickrList::MIXEDTYPES)
    {
        m_contentTypeComboBox->setIntermediate(true);
    }
    else
    {
        int index = m_contentTypeComboBox->findData(QVariant(static_cast<int>(contentType)));
        m_contentTypeComboBox->setCurrentIndex(index);
    }
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

    if (state != Qt::PartiallyChecked)
    {
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

void FlickrWidget::slotMainSafetyLevelChanged(int index)
{
    int currValue = qVariantValue<int>(m_safetyLevelComboBox->itemData(index));
    m_imglst->setSafetyLevels(static_cast<FlickrList::SafetyLevel>(currValue));
}

void FlickrWidget::slotMainContentTypeChanged(int index)
{
    int currValue = qVariantValue<int>(m_contentTypeComboBox->itemData(index));
    m_imglst->setContentTypes(static_cast<FlickrList::ContentType>(currValue));
}

void FlickrWidget::slotExtendedSettingsToggled(bool status)
{
    /* Show or hide the extended settings when the extended settings button
     * is toggled. */
    m_extendedSettingsBox->setVisible(status);
    m_imglst->listView()->setColumnHidden(FlickrList::SAFETYLEVEL, !status);
    m_imglst->listView()->setColumnHidden(FlickrList::CONTENTTYPE, !status);
}

} // namespace KIPIFlickrExportPlugin
