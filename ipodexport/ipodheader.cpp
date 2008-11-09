/***************************************************************************
 * copyright            : (C) 2006 Seb Ruiz <me@sebruiz.net>               *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ipodheader.h"
#include "ipodexportdialog.h"

#include "klocale.h"
#include "kpushbutton.h"

#include "qlabel.h"
#include "qlayout.h"
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <Q3Frame>

using namespace IpodExport;

IpodHeader::IpodHeader( QWidget *parent, const char *name, WFlags f )
    : Q3Frame( parent, name, f )
{
    Q3VBoxLayout *layout = new Q3VBoxLayout( this, 10/*margin*/, 5/*spacing*/ );

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

    m_messageLabel = new QLabel( QString::null, this );
    m_messageLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

    Q3HBoxLayout *buttonLayout = new Q3HBoxLayout;
    m_button = new KPushButton( this );
    m_button->hide();

    buttonLayout->addStretch( 1 );
    buttonLayout->addWidget( m_button );
    buttonLayout->addStretch( 1 );

    layout->addWidget( m_messageLabel );
    layout->addLayout( buttonLayout );
}

void IpodHeader::setViewType( ViewType view )
{
    m_viewType = view;

    switch( view )
    {
        case NoIpod:
            setNoIpod();
            break;

        case IncompatibleIpod:
            setIncompatibleIpod();
            break;

        case ValidIpod:
            setValidIpod();
            break;

        default:
            break;
    }
}

void IpodHeader::setNoIpod()
{
    m_messageLabel->setText( i18n("<p align=\"center\"><b>No iPod was detected</b></p>" ) );

    setPaletteBackgroundColor( QColor(147,18,18) );
    m_messageLabel->setPaletteBackgroundColor( QColor(147,18,18) );
    m_messageLabel->setPaletteForegroundColor( Qt::white );

    m_button->setText( i18n( "Refresh" ) );
    m_button->show();

    m_button->disconnect();
    connect( m_button, SIGNAL( clicked() ), SIGNAL( refreshDevices() ) );
}

void IpodHeader::setIncompatibleIpod()
{
    const QString modelType = UploadDialog::instance()->ipodModel();

    m_messageLabel->setText( i18n("<p align=\"center\"><b>Your iPod (%1) does not seem to support artwork.</b></p>" ).arg( modelType ) );

    setPaletteBackgroundColor( QColor(225,150,0) );
    m_messageLabel->setPaletteBackgroundColor( QColor(225,150,0)  );
    m_messageLabel->setPaletteForegroundColor( Qt::white );

    m_button->setText( i18n( "Set iPod Model" ) );
//     m_button->show();
    m_button->hide(); // FIXME its not implemented!

    m_button->disconnect();
    connect( m_button, SIGNAL( clicked() ), SIGNAL( updateSysInfo() ) );
}


void IpodHeader::setValidIpod()
{
    const QString modelType  = UploadDialog::instance()->ipodModel();
    const QString mountPoint = UploadDialog::instance()->mountPoint();

    if( !mountPoint.isEmpty() )
    {
        m_messageLabel->setText( i18n("<p align=\"center\"><b>iPod %1 detected at: %2</b></p>" )
                                 .arg( modelType, mountPoint ) );
    }
    else
    {
        m_messageLabel->setText( i18n("<p align=\"center\"><b>iPod %1 detected</b></p>" )
                                .arg( modelType ) );
    }
    setPaletteBackgroundColor( QColor(0,98,0) );
    m_messageLabel->setPaletteBackgroundColor( QColor(0,98,0) );
    m_messageLabel->setPaletteForegroundColor( Qt::white );

    m_button->hide();
}

