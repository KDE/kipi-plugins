/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
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

#include "fbalbum.moc"

// Qt includes

#include <QFormLayout>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kcombobox.h>

// local includes

#include "fbitem.h"

namespace KIPIFacebookPlugin
{

FbNewAlbum::FbNewAlbum(QWidget* const parent)
    : KDialog(parent)
{
    QString header(i18n("Facebook New Album"));
    setWindowTitle(header);
    setButtons(Ok|Cancel);
    setDefaultButton(Cancel);
    setModal(false);

    QWidget* const mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    mainWidget->setMinimumSize(400, 300);

    // ------------------------------------------------------------------------
    m_titleEdt          = new KLineEdit;
    m_titleEdt->setWhatsThis(i18n("Title of the album that will be created (required)."));

    m_locEdt            = new KLineEdit;
    m_locEdt->setWhatsThis(i18n("Location of the album that will be created (optional)."));

    m_descEdt           = new KTextEdit;
    m_descEdt->setWhatsThis(i18n("Description of the album that will be created (optional)."));

    m_privacyCoB        = new KComboBox;
    m_privacyCoB->setEditable(false);
    m_privacyCoB->setWhatsThis(i18n("Privacy setting of the album that will be created (required)."));
    m_privacyCoB->addItem(KIcon("secure-card"),           i18n("Only Me"),                 FB_ME);
    m_privacyCoB->addItem(KIcon("user-identity"),         i18n("Only Friends"),            FB_FRIENDS);
    m_privacyCoB->addItem(KIcon("system-users"),          i18n("Friends of Friends"),      FB_FRIENDS_OF_FRIENDS);
    m_privacyCoB->addItem(KIcon("network-workgroup"),     i18n("My Networks and Friends"), FB_NETWORKS);
    m_privacyCoB->addItem(KIcon("applications-internet"), i18n("Everyone"),                FB_EVERYONE);
    m_privacyCoB->setCurrentIndex(1);

    QFormLayout* const albumBoxLayout = new QFormLayout;
    albumBoxLayout->addRow(i18nc("new facebook album", "Title:"),       m_titleEdt);
    albumBoxLayout->addRow(i18nc("new facebook album", "Location:"),    m_locEdt);
    albumBoxLayout->addRow(i18nc("new facebook album", "Description:"), m_descEdt);
    albumBoxLayout->addRow(i18nc("new facebook album", "Privacy:"),     m_privacyCoB);
    albumBoxLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    albumBoxLayout->setSpacing(KDialog::spacingHint());
    albumBoxLayout->setMargin(KDialog::spacingHint());
    mainWidget->setLayout(albumBoxLayout);
}

FbNewAlbum::~FbNewAlbum()
{
}

void FbNewAlbum::getAlbumProperties(FbAlbum& album)
{
    album.title       = m_titleEdt->text();
    album.location    = m_locEdt->text();
    album.description = m_descEdt->toPlainText();
    album.privacy     = static_cast<KIPIFacebookPlugin::FbPrivacy>(m_privacyCoB->itemData(m_privacyCoB->currentIndex()).toInt());
}

} // namespace KIPIFacebookPlugin
