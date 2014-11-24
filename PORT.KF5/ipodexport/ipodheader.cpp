/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz at kde dot org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "ipodheader.moc"

// Qt includes

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>

// KDE includes

#include <klocale.h>
#include <kpushbutton.h>

// Locale includes

#include "uploaddialog.h"

namespace KIPIIpodExportPlugin
{

IpodHeader::IpodHeader(QWidget* const parent, Qt::WFlags f)
    : QFrame(parent, f)
{
    m_viewType = NoIpod;
    setAutoFillBackground( true );

    QVBoxLayout* const layout = new QVBoxLayout( this );
    layout->setContentsMargins( 10, 10, 10, 10 );
    layout->setSpacing( 5 );

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    m_messageLabel = new QLabel( QString(), this );
    m_messageLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    QHBoxLayout* const buttonLayout = new QHBoxLayout();
    m_button                        = new KPushButton( this );
    m_button->hide();

    buttonLayout->addStretch( 1 );
    buttonLayout->addWidget( m_button );
    buttonLayout->addStretch( 1 );

    layout->addWidget( m_messageLabel );
    layout->addLayout( buttonLayout );
}

void IpodHeader::setViewType(ViewType view)
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

    QPalette p = palette();
    p.setColor( QPalette::Window, QColor(147, 18, 18) );
    p.setColor( QPalette::WindowText, Qt::white );
    setPalette( p );

    m_button->setText( i18n( "Refresh" ) );
    m_button->show();

    m_button->disconnect();

    connect(m_button, SIGNAL(clicked()),
            this, SIGNAL(refreshDevices()));
}

void IpodHeader::setIncompatibleIpod()
{
    const QString modelType = UploadDialog::instance()->ipodModel();

    m_messageLabel->setText( i18n("<p align=\"center\"><b>Your iPod (%1) "
                                  "does not seem to support artwork.</b></p>", modelType ) );

    QPalette p = palette();
    p.setColor( QPalette::Window, QColor(225, 150, 0) );
    p.setColor( QPalette::WindowText, Qt::white );
    setPalette( p );

    m_button->setText( i18n( "Set iPod Model" ) );
    m_button->hide(); // FIXME its not implemented!

    m_button->disconnect();

    connect(m_button, SIGNAL(clicked()),
            this, SIGNAL(updateSysInfo()));
}

void IpodHeader::setValidIpod()
{
    const QString modelType  = UploadDialog::instance()->ipodModel();
    const QString mountPoint = UploadDialog::instance()->mountPoint();

    if( !mountPoint.isEmpty() )
    {
        m_messageLabel->setText( i18n("<p align=\"center\"><b>iPod %1 detected at: %2</b></p>",
                                 modelType, mountPoint) );
    }
    else
    {
        m_messageLabel->setText( i18n("<p align=\"center\"><b>iPod %1 detected</b></p>",
                                 modelType) );
    }

    QPalette p = palette();
    p.setColor( QPalette::Window, QColor(0, 98, 0) );
    p.setColor( QPalette::WindowText, Qt::white );
    setPalette( p );

    m_button->hide();
}

} // namespace KIPIIpodExportPlugin
