/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-01
 * Description : a kipi plugin to import/export images to/from
                 SmugMug web service
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

#include "smugwidget.moc"

// Qt includes

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
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

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpimageslist.h"
#include "kpprogresswidget.h"

namespace KIPISmugPlugin
{

SmugWidget::SmugWidget(QWidget* const parent, KIPI::Interface* const iface, bool import)
    : QWidget(parent)
{
    setObjectName("SmugWidget");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // -------------------------------------------------------------------

    m_imgList = new KIPIPlugins::KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KIPIPlugins::KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(true);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(
        i18n("This is the list of images to upload to your SmugMug account."));

    QWidget* settingsBox           = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    m_headerLbl = new QLabel(settingsBox);
    m_headerLbl->setWhatsThis(
        i18n("This is a clickable link to open the SmugMug home page in a web browser."));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    // ------------------------------------------------------------------------

    QGroupBox* accountBox         = new QGroupBox(i18n("Account"), settingsBox);
    accountBox->setWhatsThis(
        i18n("This is the SmugMug account that will be used to authenticate."));
    QGridLayout* accountBoxLayout = new QGridLayout(accountBox);

    m_anonymousRBtn     = new QRadioButton(i18nc("smug account login", "Anonymous"), accountBox);
    m_anonymousRBtn->setWhatsThis(
        i18n("Login as anonymous to SmugMug web service."));

    m_accountRBtn       = new QRadioButton(i18n("SmugMug Account"), accountBox);
    m_accountRBtn->setWhatsThis(
        i18n("Login to SmugMug web service using email and password."));

    m_userNameLbl       = new QLabel(i18nc("smug account settings", "Name:"), accountBox);
    m_userName          = new QLabel(accountBox);
    m_emailLbl          = new QLabel(i18nc("smug account settings", "Email:"), accountBox);
    m_email             = new QLabel(accountBox);
    m_changeUserBtn     = new KPushButton(KGuiItem(i18n("Change Account"), "system-switch-user",
                                          i18n("Change SmugMug Account used to authenticate")),
                                           accountBox);

    accountBoxLayout->addWidget(m_anonymousRBtn,        0, 0, 1, 2);
    accountBoxLayout->addWidget(m_accountRBtn,          1, 0, 1, 2);
    accountBoxLayout->addWidget(m_userNameLbl,          2, 0, 1, 1);
    accountBoxLayout->addWidget(m_userName,             2, 1, 1, 1);
    accountBoxLayout->addWidget(m_emailLbl,             3, 0, 1, 1);
    accountBoxLayout->addWidget(m_email,                3, 1, 1, 1);
    accountBoxLayout->addWidget(m_changeUserBtn,        4, 1, 1, 1);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    QGroupBox* albumsBox         = new QGroupBox(i18n("Album"), settingsBox);
    albumsBox->setWhatsThis(i18n("This is the SmugMug album that will be used for transfer."));
    QGridLayout* albumsBoxLayout = new QGridLayout(albumsBox);

    m_albumsCoB         = new KComboBox(albumsBox);
    m_albumsCoB->setEditable(false);
    m_nickNameLbl       = new QLabel(i18n("Nickname:"), albumsBox);
    m_nickNameEdt       = new KLineEdit(albumsBox);
    m_nickNameEdt->setWhatsThis(
        i18n("Nickname of SmugMug user to list albums."));
    m_sitePasswordLbl   = new QLabel(i18n("Site Password:"), albumsBox);
    m_sitePasswordEdt   = new KLineEdit(albumsBox);
    m_sitePasswordEdt->setWhatsThis(
        i18n("Site-wide password for specified SmugMug nick/user."));
    m_albumPasswordLbl  = new QLabel(i18n("Album Password:"), albumsBox);
    m_albumPasswordEdt  = new KLineEdit(albumsBox);
    m_albumPasswordEdt->setWhatsThis(
        i18n("Password for SmugMug album."));

    m_newAlbumBtn       = new KPushButton(KGuiItem(i18n("New Album"), "list-add",
                                          i18n("Create new SmugMug album")), accountBox);
    m_reloadAlbumsBtn   = new KPushButton(KGuiItem(i18nc("reload album list", "Reload"), "view-refresh",
                                          i18n("Reload album list")), accountBox);

    albumsBoxLayout->addWidget(m_albumsCoB,         0, 0, 1, 5);
    albumsBoxLayout->addWidget(m_nickNameLbl,       1, 0, 1, 1);
    albumsBoxLayout->addWidget(m_nickNameEdt,       1, 1, 1, 3);
    albumsBoxLayout->addWidget(m_newAlbumBtn,       1, 3, 1, 1);
    albumsBoxLayout->addWidget(m_reloadAlbumsBtn,   1, 4, 1, 1);
    albumsBoxLayout->addWidget(m_sitePasswordLbl,   2, 0, 1, 1);
    albumsBoxLayout->addWidget(m_sitePasswordEdt,   2, 1, 1, 4);
    albumsBoxLayout->addWidget(m_albumPasswordLbl,  3, 0, 1, 1);
    albumsBoxLayout->addWidget(m_albumPasswordEdt,  3, 1, 1, 4);

    // ------------------------------------------------------------------------

    QGroupBox* uploadBox         = new QGroupBox(i18n("Destination"), settingsBox);
    uploadBox->setWhatsThis(i18n("This is the location where SmugMug images will be downloaded."));
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadBox);
    m_uploadWidget               = iface->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(m_uploadWidget);

    // ------------------------------------------------------------------------

    QGroupBox* optionsBox         = new QGroupBox(i18n("Options"), settingsBox);
    optionsBox->setWhatsThis(i18n("These are options that will be applied to images before upload."));
    QGridLayout* optionsBoxLayout = new QGridLayout(optionsBox);

    m_resizeChB                   = new QCheckBox(optionsBox);
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

    m_progressBar = new KIPIPlugins::KPProgressWidget(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    // ------------------------------------------------------------------------

    settingsBoxLayout->addWidget(m_headerLbl);
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->addWidget(albumsBox);
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
        m_imgList->hide();
        m_newAlbumBtn->hide();
        optionsBox->hide();
    }
    else
    {
        m_anonymousRBtn->hide();
        m_accountRBtn->hide();

        m_nickNameLbl->hide();
        m_nickNameEdt->hide();
        m_sitePasswordLbl->hide();
        m_sitePasswordEdt->hide();
        m_albumPasswordLbl->hide();
        m_albumPasswordEdt->hide();

        uploadBox->hide();
    }
}

SmugWidget::~SmugWidget()
{
}

KIPIPlugins::KPImagesList* SmugWidget::imagesList() const
{
    return m_imgList;
}

KIPIPlugins::KPProgressWidget* SmugWidget::progressBar() const
{
    return m_progressBar;
}

bool SmugWidget::isAnonymous() const
{
    return m_anonymousRBtn->isChecked();
}

void SmugWidget::setAnonymous(bool checked)
{
    m_anonymousRBtn->setChecked(checked);
    m_accountRBtn->setChecked(!checked);
}

QString SmugWidget::getNickName() const
{
    return m_nickNameEdt->text();
}

QString SmugWidget::getSitePassword() const
{
    return m_sitePasswordEdt->text();
}

QString SmugWidget::getAlbumPassword() const
{
    return m_albumPasswordEdt->text();
}

QString SmugWidget::getDestinationPath() const
{
    return m_uploadWidget->selectedImageCollection().uploadPath().path();
}

void SmugWidget::setNickName(const QString& nick)
{
    m_nickNameEdt->setText(nick);
    m_headerLbl->setText(QString("<b><h2><a href='http://%1.smugmug.com.'>"
                                   "<font color=\"#9ACD32\">SmugMug</font>"
                                   "</a></h2></b>").arg(nick));
}

void SmugWidget::updateLabels(const QString& email, const QString& name, const QString& nick)
{
    m_email->setText(email);
    m_userName->setText(QString("<b>%1</b>").arg(name));
    QString web("www");
    if (!nick.isEmpty())
        web = nick;
    m_headerLbl->setText(QString("<b><h2><a href='http://%1.smugmug.com.'>"
                                   "<font color=\"#9ACD32\">SmugMug</font>"
                                   "</a></h2></b>").arg(web));
}

void SmugWidget::slotAnonymousToggled(bool checked)
{
    m_emailLbl->setEnabled(!checked);
    m_email->setEnabled(!checked);
    m_userNameLbl->setEnabled(!checked);
    m_userName->setEnabled(!checked);
    m_changeUserBtn->setEnabled(!checked);

    emit signalUserChangeRequest(checked);
}

void SmugWidget::slotChangeUserClicked()
{
    emit signalUserChangeRequest(false);
}

void SmugWidget::slotResizeChecked()
{
    m_dimensionSpB->setEnabled(m_resizeChB->isChecked());
    m_imageQualitySpB->setEnabled(m_resizeChB->isChecked());
}

} // namespace KIPISmugPlugin
