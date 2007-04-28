/* ============================================================
 * File  : firstrundlg.cpp
 * Author: Joern Ahrens <joern.ahrens@kdemail.net>
 * Date  : 2006-01-06
 * Description :
 *
 * Copyright 20066 by Joern Ahrens <joern.ahrens@kdemail.net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// QT include files

#include <qframe.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>

// KDE include files

#include <klocale.h>
#include <kapplication.h>
#include <kurllabel.h>
#include <kurlrequester.h>

// KIPI include files

#include <libkipi/version.h>
#include <libkipi/imagecollectionselector.h>

// Local include files

#include "firstrundlg.h"

namespace KIPISimpleViewerExportPlugin
{

FirstRunDlg::FirstRunDlg( QWidget *parent )
    : KDialogBase( parent, "svefirstrun", true, "BLA",
                   KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true )
{
    setCaption(i18n("Simple Viewer Export"));
    
    enableButtonOK(false);
    
    QFrame *page = new QFrame(this);
    setMainWidget(page);
    
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
    
    QLabel *info = new QLabel( page );
    info->setText( i18n( "SimpleViewer is free to use, but uses a license which comes into conflict with\n"
                         "several distributions. Due to the license it is not possible to ship it with this plugin.\n\n"
                         "You can now download SimpleViewer from its homepage and point the plugin\n"
                         "to the downloaded archive. The archive will be stored with the plugin configuration,\n"
                         "so it is available for further use.\n\n"));
    topLayout->addWidget( info );
    
    info = new QLabel(page);
    info->setText(i18n( "1.) Download SimpleViewer Version 1.7.X (1.8.X is not compatible yet):\n"));
    topLayout->addWidget(info);
    
    KURLLabel *link = new KURLLabel(page);
    link->setText("http://www.airtightinteractive.com/simpleviewer/");
    link->setURL("http://www.airtightinteractive.com/simpleviewer/");
    topLayout->addWidget(link);
    connect(link, SIGNAL(leftClickedURL(const QString &)),
            SLOT(slotDownload(const QString &)));
    
    info = new QLabel(page);
    info->setText(i18n("\n2.) Point the plugin to the downloaded archive\n"));
    topLayout->addWidget( info );
    
    m_urlRequester = new KURLRequester(page);
    topLayout->addWidget(m_urlRequester);
    connect(m_urlRequester, SIGNAL(urlSelected(const QString&)),
            SLOT(slotURLSelected(const QString&)));
    
    topLayout->addStretch(10);
}

FirstRunDlg::~FirstRunDlg()
{
}

void FirstRunDlg::slotDownload(const QString &url)
{
    KApplication::kApplication()->invokeBrowser(url);
}

void FirstRunDlg::slotURLSelected(const QString &url)
{
    enableButtonOK(true);
    m_url = url;
}

QString FirstRunDlg::getURL()
{
    return m_url;
}

}

#include "firstrundlg.moc"

