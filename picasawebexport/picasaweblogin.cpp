/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#include "picasaweblogin.h"
#include "picasaweblogin.moc"

// Qt includes.

#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

// KDE includes.

#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>

namespace KIPIPicasawebExportPlugin
{

PicasawebLogin::PicasawebLogin(QWidget* parent, const QString& header,
                               const QString& _name, const QString& _passwd)
              : QDialog(parent)
{
    setSizeGripEnabled(false);

    Q3VBoxLayout* vbox = new Q3VBoxLayout( this, 5, 5, "vbox");

    m_headerLabel = new QLabel(this);
    m_headerLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                             QSizePolicy::Fixed));
    m_headerLabel->setText(header);
    vbox->addWidget( m_headerLabel );

    QFrame* hline = new QFrame( this, "hline" );
    hline->setFrameShape( QFrame::HLine );
    hline->setFrameShadow( QFrame::Sunken );
    hline->setFrameShape( QFrame::HLine );
    vbox->addWidget( hline );

    Q3GridLayout* centerLayout = new Q3GridLayout(0, 1, 1, 5, 5);

    m_nameEdit = new KLineEdit( this );
    centerLayout->addWidget(m_nameEdit, 0, 1);

    m_passwdEdit = new KLineEdit( this );
    m_passwdEdit->setEchoMode(KLineEdit::Password);
    centerLayout->addWidget(m_passwdEdit, 1, 1);

    QLabel* nameLabel = new QLabel(this);
    nameLabel->setText(i18n( "Username:" ));
    centerLayout->addWidget(nameLabel, 0, 0);

    QLabel* passwdLabel = new QLabel(this);
    passwdLabel->setText(i18n( "Password:" ));
    centerLayout->addWidget(passwdLabel, 1, 0);

    vbox->addLayout( centerLayout );

    Q3HBoxLayout* btnLayout = new Q3HBoxLayout(0, 0, 5);
    btnLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding,
                                         QSizePolicy::Minimum ) );

    QPushButton *okBtn = new QPushButton( this );
    okBtn->setAutoDefault( true );
    okBtn->setDefault( true );
    okBtn->setText( i18n( "&OK" ) );
    btnLayout->addWidget( okBtn );

    QPushButton *cancelBtn = new QPushButton( this );
    cancelBtn->setText( i18n( "&Cancel" ) );
    btnLayout->addWidget( cancelBtn );

    vbox->addLayout( btnLayout );

    resize( QSize(300, 150).expandedTo(minimumSizeHint()) );
    //PORT to kde4
    //clearWState( WState_Polished );

    m_nameEdit->setText(_name);
    m_passwdEdit->setText(_passwd);

    // signals and slots connections

    connect(okBtn, SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(cancelBtn, SIGNAL(clicked()),
            this, SLOT(reject()));
}

PicasawebLogin::~PicasawebLogin()
{
}

QString PicasawebLogin::name() const
{
    return m_nameEdit->text();
}

QString PicasawebLogin::password() const
{
    return m_passwdEdit->text();
}

QString PicasawebLogin::username() const
{
    return m_nameEdit->text();
}

void PicasawebLogin::setUsername(const QString& username)
{
    kDebug( 51000 )<<" The username passed to me is "<<username <<endl;
    m_nameEdit->setText(username);
    kDebug( 51000 )<<" The username passed to me is "<<username <<endl;
}

void PicasawebLogin::setPassword(const QString& password)
{
    m_passwdEdit->setText(password);
}

} // namespace KIPIPicasawebExportPlugin
