/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-01-06
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// QT include files

#include <qframe.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qpushbutton.h>

// KDE include files

#include <klocale.h>
#include <kapplication.h>
#include <kurllabel.h>
#include <kurlrequester.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kapplication.h>

// KIPI include files

#include <libkipi/version.h>
#include <libkipi/imagecollectionselector.h>

// Local include files

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "firstrundlg.h"
#include "firstrundlg.moc"

namespace KIPISimpleViewerExportPlugin
{

FirstRunDlg::FirstRunDlg(QWidget *parent)
           : KDialogBase(parent, 0, true, i18n("Flash Export"),
                         Help|Ok|Cancel, Ok, true)
{
    enableButtonOK(false);

    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Flash Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to export images to Flash using Simple Viewer component"),
                                           "(c) 2005-2006, Joern Ahrens\n"
                                           "(c) 2008, Gilles Caulier");

    m_about->addAuthor("Joern Ahrens", 
                       I18N_NOOP("Author and maintainer"),
                       "joern dot ahrens at kdemail dot net");

    m_about->addAuthor("Gilles Caulier", 
                       I18N_NOOP("Developer and maintainer"),
                       "caulier dot gilles at gmail dot com");

    m_about->addCredit("Felix Turner",
                       "Author of the Simple Viewer Flash component",
                       0,
                       "http://www.airtightinteractive.com/simpleviewer");

    m_about->addCredit("Mikkel B. Stegmann",
                       "Basis for the index.html template",
                       0,
                       "http://www.stegmann.dk/mikkel/porta");

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    QFrame *page = new QFrame(this);
    setMainWidget(page);

    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );

    QLabel *info = new QLabel( page );
    info->setText( i18n( "<p>SimpleViewer is a Flash component which is free to use, "
                         "but uses a license which comes into conflict with several distributions. "
                         "Due to the license it is not possible to ship it with this plugin.</p>"
                         "<p>You can now download SimpleViewer from its homepage and point this tool "
                         "to the downloaded archive. The archive will be stored with the plugin configuration, "
                         "so it is available for further use.</p>"));
    topLayout->addWidget(info);

    info = new QLabel(page);
    info->setText(i18n( "<p>1.) Download SimpleViewer Version 1.8.x</p>"));
    topLayout->addWidget(info);

    KURLLabel *link = new KURLLabel(page);
    link->setText("http://www.airtightinteractive.com/simpleviewer");
    link->setURL("http://www.airtightinteractive.com/simpleviewer");
    topLayout->addWidget(link);
    connect(link, SIGNAL(leftClickedURL(const QString &)),
            this, SLOT(slotDownload(const QString &)));

    info = new QLabel(page);
    info->setText(i18n("<p>2.) Point this tool to the downloaded archive</p>"));
    topLayout->addWidget( info );

    m_urlRequester = new KURLRequester(page);
    topLayout->addWidget(m_urlRequester);
    connect(m_urlRequester, SIGNAL(urlSelected(const QString&)),
            this, SLOT(slotURLSelected(const QString&)));

    topLayout->addStretch(10);
}

FirstRunDlg::~FirstRunDlg()
{
    delete m_about;
}

void FirstRunDlg::slotHelp()
{
    KApplication::kApplication()->invokeHelp("simpleviewerexport", "kipi-plugins");
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

} // namespace KIPISimpleViewerExportPlugin
