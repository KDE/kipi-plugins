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

#include "fbalbum.h"

// Qt includes

#include <QFormLayout>
#include <QComboBox>
#include <QApplication>
#include <QStyle>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>

// KDE includes

#include <klocalizedstring.h>

// local includes

#include "fbitem.h"

namespace KIPIFacebookPlugin
{

FbNewAlbum::FbNewAlbum(QWidget* const parent, const QString& pluginName)
    : KPNewAlbumDialog(parent, pluginName)
{
    hideDateTime();

    m_privacyCoB        = new QComboBox;
    m_privacyCoB->setEditable(false);
    m_privacyCoB->setWhatsThis(i18n("Privacy setting of the album that will be created (required)."));
    m_privacyCoB->addItem(QIcon::fromTheme(QString::fromLatin1("secure-card")),
                          i18n("Only Me"),                 FB_ME);
    m_privacyCoB->addItem(QIcon::fromTheme(QString::fromLatin1("user-identity")),
                          i18n("Only Friends"),            FB_FRIENDS);
    m_privacyCoB->addItem(QIcon::fromTheme(QString::fromLatin1("system-users")),
                          i18n("Friends of Friends"),      FB_FRIENDS_OF_FRIENDS);
    m_privacyCoB->addItem(QIcon::fromTheme(QString::fromLatin1("network-workgroup")),
                          i18n("My Networks and Friends"), FB_NETWORKS);
    m_privacyCoB->addItem(QIcon::fromTheme(QString::fromLatin1("folder-html")),
                          i18n("Everyone"),                FB_EVERYONE);
    m_privacyCoB->setCurrentIndex(1);

    addToMainLayout(m_privacyCoB);
}

FbNewAlbum::~FbNewAlbum()
{
}

void FbNewAlbum::getAlbumProperties(FbAlbum& album)
{
    album.title       = getTitleEdit()->text();
    album.location    = getLocEdit()->text();
    album.description = getDescEdit()->toPlainText();
    album.privacy     = static_cast<KIPIFacebookPlugin::FbPrivacy>(m_privacyCoB->itemData(m_privacyCoB->currentIndex()).toInt());
}

} // namespace KIPIFacebookPlugin
