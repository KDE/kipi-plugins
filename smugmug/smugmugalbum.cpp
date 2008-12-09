/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-07
 * Description : a kipi plugin to export images to SmugMug web service
 *
 * Copyright (C) 2008 by Luka Renko <lure at kubuntu dot org>
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

#include "smugmugalbum.h"
#include "smugmugalbum.moc"

// Qt includes.
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QComboBox>
#include <QRadioButton>

// KDE includes.
#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>

// local includes
#include <smugmugitem.h>

namespace KIPISmugMugPlugin
{

SmugMugNewAlbum::SmugMugNewAlbum(QWidget* parent)
           : KDialog(parent)
{
    QString header(i18n("SmugMug New Album"));
    setWindowTitle(header);
    setButtons(Ok|Cancel);
    setDefaultButton(Cancel);
    setModal(false);

    QWidget *mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    mainWidget->setMinimumSize(400, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);

    // ------------------------------------------------------------------------
    QGroupBox* albumBox = new QGroupBox(i18n("Album"), mainWidget);
    albumBox->setWhatsThis(
        i18n("These are basic settings for new SmugMug album."));
    QGridLayout* albumBoxLayout = new QGridLayout(albumBox);

    QLabel *titleLbl    = new QLabel(i18n("Title:"), albumBox);
    m_titleEdt          = new KLineEdit(albumBox);
    titleLbl->setWhatsThis(
        i18n("Title of the album that will be created (required)."));
    m_titleEdt->setWhatsThis(
        i18n("Title of the album that will be created (required)."));

    QLabel *categLbl    = new QLabel(i18n("Category:"), albumBox);
    m_categCoB          = new QComboBox(albumBox);
    m_categCoB->setEditable(false);
    categLbl->setWhatsThis(
        i18n("Category of the album that will be created (required)."));
    m_categCoB->setWhatsThis(
        i18n("Category of the album that will be created (required)."));

    QLabel *subCategLbl = new QLabel(i18n("Subcategory:"), albumBox);
    m_subCategCoB       = new QComboBox(albumBox);
    m_subCategCoB->setEditable(false);
    subCategLbl->setWhatsThis(
        i18n("Subcategory of the album that will be created (optional)."));
    m_subCategCoB->setWhatsThis(
        i18n("Subcategory of the album that will be created (optional)."));

    QLabel *descLbl     = new QLabel(i18n("Description:"), albumBox);
    m_descEdt           = new KTextEdit(albumBox);
    descLbl->setWhatsThis(
        i18n("Description of the album that will be created (optional)."));
    m_descEdt->setWhatsThis(
        i18n("Description of the album that will be created (optional)."));

    albumBoxLayout->addWidget(titleLbl,         0, 0, 1, 1);
    albumBoxLayout->addWidget(m_titleEdt,       0, 1, 1, 1);
    albumBoxLayout->addWidget(categLbl,         1, 0, 1, 1);
    albumBoxLayout->addWidget(m_categCoB,       1, 1, 1, 1);
    albumBoxLayout->addWidget(subCategLbl,      2, 0, 1, 1);
    albumBoxLayout->addWidget(m_subCategCoB,    2, 1, 1, 1);
    albumBoxLayout->addWidget(descLbl,          3, 0, 1, 1);
    albumBoxLayout->addWidget(m_descEdt,        3, 1, 1, 2);
    albumBoxLayout->setSpacing(KDialog::spacingHint());
    albumBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------
    QGroupBox* privBox = new QGroupBox(i18n("Security && Privacy"), mainWidget);
    privBox->setWhatsThis(
        i18n("These are security and privacy settings for new SmugMug album."));
    QGridLayout* privBoxLayout = new QGridLayout(privBox);

    QLabel *privacyLbl  = new QLabel(i18n("Privacy:"), privBox);
    privacyLbl->setWhatsThis(
        i18n("Privacy of the new SmugMug album (required)."));
    m_publicRBtn        = new QRadioButton(i18n("Public"), privBox);
    m_publicRBtn->setChecked(true);
    m_publicRBtn->setWhatsThis(
        i18n("Public album is listed on your public SmugMug page."));
    m_unlistedRBtn      = new QRadioButton(i18n("Unlisted"), privBox);
    m_unlistedRBtn->setWhatsThis(
        i18n("Unlisted album is only accessible via URL."));

    QLabel *passwdLbl   = new QLabel(i18n("Password:"), privBox);
    m_passwdEdt         = new KLineEdit(privBox);
    passwdLbl->setWhatsThis(
        i18n("Require password to access the album (optional)."));
    m_passwdEdt->setWhatsThis(
        i18n("Require password to access the album (optional)."));

    QLabel *hintLbl     = new QLabel(i18n("Password Hint:"), privBox);
    m_hintEdt           = new KLineEdit(privBox);
    hintLbl->setWhatsThis(
        i18n("Password hint to present to users on the password prompt (optional)."));
    m_hintEdt->setWhatsThis(
        i18n("Password hint to present to users on the password prompt (optional)."));

    privBoxLayout->addWidget(privacyLbl,        0, 0, 1, 1);
    privBoxLayout->addWidget(m_publicRBtn,      0, 1, 1, 1);
    privBoxLayout->addWidget(m_unlistedRBtn,    0, 2, 1, 1);
    privBoxLayout->addWidget(passwdLbl,         1, 0, 1, 1);
    privBoxLayout->addWidget(m_passwdEdt,       1, 1, 1, 2);
    privBoxLayout->addWidget(hintLbl,           2, 0, 1, 1);
    privBoxLayout->addWidget(m_hintEdt,         2, 1, 1, 2);
    privBoxLayout->setSpacing(KDialog::spacingHint());
    privBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------
    mainLayout->addWidget(albumBox);
    mainLayout->addWidget(privBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    // ------------------------------------------------------------------------
}

SmugMugNewAlbum::~SmugMugNewAlbum()
{
}

void SmugMugNewAlbum::getAlbumProperties(SMAlbum &album)
{
    album.title = m_titleEdt->text();

    album.category = m_categCoB->currentText();
    album.categoryID = m_categCoB->itemData(m_categCoB->currentIndex()).toInt();

    album.subCategory = m_subCategCoB->currentText();
    album.subCategoryID = m_subCategCoB->itemData(
                                         m_subCategCoB->currentIndex()).toInt();

    album.description = m_descEdt->toPlainText();

    album.isPublic = m_publicRBtn->isChecked();
    album.password = m_passwdEdt->text();
    album.passwordHint = m_hintEdt->text();
}

} // namespace KIPISmugMugPlugin
