/* ============================================================
 * File  : gallerylogin.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-04
 * Description :
 *
 * Copyright 2004 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <qlabel.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <klocale.h>

#include "gallerylogin.h"

namespace KIPIGalleryExportPlugin
{

GalleryLogin::GalleryLogin(QWidget* parent, const QString& header,
                           const QString& _url,
                           const QString& _name,
                           const QString& _passwd)
    : QDialog(parent)
{
    setSizeGripEnabled( false );

    QVBoxLayout* vbox = new QVBoxLayout( this, 5, 5, "vbox");

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

    QGridLayout* centerLayout = new QGridLayout(0, 1, 1, 5, 5);

    m_urlEdit = new QLineEdit( this );
    centerLayout->addWidget(m_urlEdit, 0, 1);

    m_nameEdit = new QLineEdit( this );
    centerLayout->addWidget(m_nameEdit, 1, 1);

    m_passwdEdit = new QLineEdit( this );
    m_passwdEdit->setEchoMode(QLineEdit::Password);
    centerLayout->addWidget(m_passwdEdit, 2, 1);

    QLabel* urlLabel = new QLabel(this);
    urlLabel->setText(i18n( "URL:" ));
    centerLayout->addWidget(urlLabel, 0, 0);

    QLabel* nameLabel = new QLabel(this);
    nameLabel->setText(i18n( "Username:" ));
    centerLayout->addWidget(nameLabel, 1, 0);

    QLabel* passwdLabel = new QLabel(this);
    passwdLabel->setText(i18n( "Password:" ));
    centerLayout->addWidget(passwdLabel, 2, 0);

    vbox->addLayout( centerLayout );

    QHBoxLayout* btnLayout = new QHBoxLayout(0, 0, 5);
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
    clearWState( WState_Polished );

    m_urlEdit->setText(_url);
    m_nameEdit->setText(_name);
    m_passwdEdit->setText(_passwd);

    // signals and slots connections
    connect( okBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

GalleryLogin::~GalleryLogin()
{

}

QString GalleryLogin::url() const
{
    return m_urlEdit->text();
}

QString GalleryLogin::name() const
{
    return m_nameEdit->text();
}

QString GalleryLogin::password() const
{
    return m_passwdEdit->text();
}

}
