/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-07
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

#include "smugalbum.moc"

// Qt includes

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <ktextedit.h>

// local includes

#include "smugitem.h"

namespace KIPISmugPlugin
{

SmugNewAlbum::SmugNewAlbum(QWidget* parent)
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

    // ------------------------------------------------------------------------

    QGroupBox* albumBox = new QGroupBox(i18n("Album"), mainWidget);
    albumBox->setWhatsThis(
        i18n("These are basic settings for the new SmugMug album."));

    m_titleEdt          = new KLineEdit;
    m_titleEdt->setWhatsThis(
        i18n("Title of the album that will be created (required)."));

    m_categCoB          = new KComboBox;
    m_categCoB->setEditable(false);
    m_categCoB->setWhatsThis(
        i18n("Category of the album that will be created (required)."));

    m_subCategCoB       = new KComboBox;
    m_subCategCoB->setEditable(false);
    m_subCategCoB->setWhatsThis(
        i18n("Subcategory of the album that will be created (optional)."));

    m_descEdt           = new KTextEdit;
    m_descEdt->setWhatsThis(
        i18n("Description of the album that will be created (optional)."));

    m_templateCoB      = new KComboBox;
    m_templateCoB->setEditable(false);
    m_templateCoB->setWhatsThis(
        i18n("Album template for the new album (optional)."));

    QFormLayout* albumBoxLayout = new QFormLayout;
    albumBoxLayout->addRow(i18nc("new smug album dialog", "Title:"), m_titleEdt);
    albumBoxLayout->addRow(i18nc("new smug album dialog", "Category:"), m_categCoB);
    albumBoxLayout->addRow(i18nc("new smug album dialog", "Subcategory:"), m_subCategCoB);
    albumBoxLayout->addRow(i18nc("new smug album dialog", "Description:"), m_descEdt);
    albumBoxLayout->addRow(i18nc("new smug album dialog", "Template:"), m_templateCoB);
    albumBoxLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    albumBoxLayout->setSpacing(KDialog::spacingHint());
    albumBoxLayout->setMargin(KDialog::spacingHint());
    albumBox->setLayout(albumBoxLayout);

    // ------------------------------------------------------------------------

    m_privBox = new QGroupBox(i18n("Security && Privacy"), mainWidget);
    m_privBox->setWhatsThis(
        i18n("These are security and privacy settings for the new SmugMug album."));

    m_publicRBtn        = new QRadioButton(i18nc("smug album privacy", "Public"));
    m_publicRBtn->setChecked(true);
    m_publicRBtn->setWhatsThis(
        i18n("Public album is listed on your public SmugMug page."));
    m_unlistedRBtn      = new QRadioButton(i18nc("smug album privacy", "Unlisted"));
    m_unlistedRBtn->setWhatsThis(
        i18n("Unlisted album is only accessible via URL."));

    QHBoxLayout* radioLayout = new QHBoxLayout;
    radioLayout->addWidget(m_publicRBtn);
    radioLayout->addWidget(m_unlistedRBtn);

    m_passwdEdt         = new KLineEdit;
    m_passwdEdt->setWhatsThis(
        i18n("Require password to access the album (optional)."));

    m_hintEdt           = new KLineEdit;
    m_hintEdt->setWhatsThis(
        i18n("Password hint to present to users in the password prompt (optional)."));

    QFormLayout* privBoxLayout = new QFormLayout;
    privBoxLayout->addRow(i18n("Privacy:"), radioLayout);
    privBoxLayout->addRow(i18n("Password:"), m_passwdEdt);
    privBoxLayout->addRow(i18n("Password Hint:"), m_hintEdt);
    privBoxLayout->setSpacing(KDialog::spacingHint());
    privBoxLayout->setMargin(KDialog::spacingHint());
    m_privBox->setLayout(privBoxLayout);

    // ------------------------------------------------------------------------
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(albumBox);
    mainLayout->addWidget(m_privBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);
    mainWidget->setLayout(mainLayout);

    // ------------------------------------------------------------------------
}

SmugNewAlbum::~SmugNewAlbum()
{
}

void SmugNewAlbum::getAlbumProperties(SmugAlbum &album)
{
    album.title      = m_titleEdt->text();

    album.category   = m_categCoB->currentText();
    album.categoryID = m_categCoB->itemData(m_categCoB->currentIndex()).toLongLong();

    album.subCategory   = m_subCategCoB->currentText();
    album.subCategoryID = m_subCategCoB->itemData(m_subCategCoB->currentIndex()).toLongLong();

    album.description   = m_descEdt->toPlainText();

    album.tmpl   = m_templateCoB->currentText();
    album.tmplID = m_templateCoB->itemData(m_templateCoB->currentIndex()).toLongLong();

    album.isPublic     = m_publicRBtn->isChecked();
    album.password     = m_passwdEdt->text();
    album.passwordHint = m_hintEdt->text();
}

} // namespace KIPISmugPlugin
