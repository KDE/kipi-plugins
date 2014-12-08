/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#include "login.moc"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

// KDE includes

#include <kdialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kseparator.h>

namespace KIPIFlickrExportPlugin
{

FlickrLogin::FlickrLogin(QWidget* const parent, const QString& header,
                         const QString& _name, const QString& _passwd)
    : KDialog(parent)
{
    setWindowTitle(header);
    setButtons(Help | Ok | Cancel);
    setDefaultButton(Ok);
    setModal(false);

    QWidget* widget = new QWidget(this);
    setMainWidget(widget);

    QVBoxLayout* const vbox = new QVBoxLayout(widget);
    m_headerLabel     = new QLabel(widget);
    m_headerLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
    m_headerLabel->setText(header);

    KSeparator* const hline = new KSeparator(Qt::Horizontal, widget);

    QGridLayout* const centerLayout = new QGridLayout();

    m_nameEdit   = new KLineEdit(widget);
    m_passwdEdit = new KLineEdit(widget);
    m_passwdEdit->setEchoMode(KLineEdit::Password);

    QLabel* const nameLabel = new QLabel(widget);
    nameLabel->setText(i18nc("flickr login", "Username:"));

    QLabel* const passwdLabel = new QLabel(widget);
    passwdLabel->setText(i18n("Password:"));

    centerLayout->addWidget(m_nameEdit,   0, 1);
    centerLayout->addWidget(m_passwdEdit, 1, 1);
    centerLayout->addWidget(nameLabel,    0, 0);
    centerLayout->addWidget(passwdLabel,  1, 0);
    centerLayout->setMargin(KDialog::spacingHint());
    centerLayout->setSpacing(KDialog::spacingHint());

    vbox->addWidget(m_headerLabel);
    vbox->addWidget(hline);
    vbox->addLayout(centerLayout);
    vbox->setMargin(0);
    vbox->setSpacing(KDialog::spacingHint());

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    m_nameEdit->setText(_name);
    m_passwdEdit->setText(_passwd);
}

FlickrLogin::~FlickrLogin()
{
}

QString FlickrLogin::name() const
{
    return m_nameEdit->text();
}

QString FlickrLogin::password() const
{
    return m_passwdEdit->text();
}

} // namespace KIPIFlickrExportPlugin
