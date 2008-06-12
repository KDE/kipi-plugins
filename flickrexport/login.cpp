/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

// Qt includes.

#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <q3frame.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

// KDE includes.

#include <klocale.h>

// Local includes.

#include "login.h"
#include "login.moc"

namespace KIPIFlickrExportPlugin
{

FlickrLogin::FlickrLogin(QWidget* parent, const QString& header,
                         const QString& _name, const QString& _passwd)
           : QDialog(parent)
{
    setSizeGripEnabled(false);

    Q3VBoxLayout* vbox = new Q3VBoxLayout(this, 5, 5, "vbox");
    m_headerLabel     = new QLabel(this);
    m_headerLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
    m_headerLabel->setText(header);

    Q3Frame* hline = new Q3Frame(this, "hline");
    hline->setFrameShape(Q3Frame::HLine);
    hline->setFrameShadow(Q3Frame::Sunken);

    Q3GridLayout* centerLayout = new Q3GridLayout(0, 1, 1, 5, 5);

    m_nameEdit   = new QLineEdit(this);
    m_passwdEdit = new QLineEdit(this);
    m_passwdEdit->setEchoMode(QLineEdit::Password);

    QLabel* nameLabel = new QLabel(this);
    nameLabel->setText(i18n("Username:"));

    QLabel* passwdLabel = new QLabel(this);
    passwdLabel->setText(i18n("Password:"));

    centerLayout->addWidget(m_nameEdit,   0, 1);
    centerLayout->addWidget(m_passwdEdit, 1, 1);
    centerLayout->addWidget(nameLabel,    0, 0);
    centerLayout->addWidget(passwdLabel,  1, 0);

    Q3HBoxLayout* btnLayout = new Q3HBoxLayout(0, 0, 5);
    btnLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QPushButton *okBtn = new QPushButton(this);
    okBtn->setAutoDefault(true);
    okBtn->setDefault(true);
    okBtn->setText(i18n("&OK"));

    QPushButton *cancelBtn = new QPushButton(this);
    cancelBtn->setText(i18n("&Cancel"));

    btnLayout->addWidget( okBtn );
    btnLayout->addWidget( cancelBtn );

    vbox->addWidget(m_headerLabel);
    vbox->addWidget(hline);
    vbox->addLayout(centerLayout);
    vbox->addLayout(btnLayout);

    resize( QSize(300, 150).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );

    m_nameEdit->setText(_name);
    m_passwdEdit->setText(_passwd);

    // ---------------------------------------------------------------

    connect(okBtn, SIGNAL( clicked() ),
            this, SLOT( accept() ));

    connect(cancelBtn, SIGNAL( clicked() ),
            this, SLOT( reject() ));
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
