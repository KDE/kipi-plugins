/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#include "rajcewindow.moc"

// Qt includes

#include <QAction>

// KDE includes

#include <klocalizedstring.h>
#include <ktoolinvocation.h>
#include <khelpmenu.h>
#include <kmenu.h>
#include <kpushbutton.h>

// Local includes

#include "kpaboutdata.h"
#include "rajcewidget.h"

namespace KIPIRajceExportPlugin
{

RajceWindow::RajceWindow(KIPI::Interface* interface, const QString& tmpFolder,
                         QWidget* /*parent*/, Qt::WFlags /*flags*/)
    : KDialog(0), m_interface(interface)
{
    m_widget = new RajceWidget(interface, tmpFolder, this);
    m_widget->readSettings();

    setMainWidget(m_widget);
    setWindowIcon(KIcon("rajce"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);

    setWindowTitle(i18n("Export to Rajce.net"));
    setButtonGuiItem(User1, KGuiItem(i18n("Start Upload"), "network-workgroup",
                                     i18n("Start upload to Rajce.net")));
    m_widget->setMinimumSize(700, 500);

    connect(this, SIGNAL(user1Clicked()),
            m_widget, SLOT(startUpload()));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(m_widget, SIGNAL(loginStatusChanged(bool)),
            this, SLOT(slotSetUploadButtonEnabled(bool)));

    //--------------------------------------------------------------------

    m_about = new KIPIPlugins::KPAboutData(ki18n("Rajce.net Export"),
                               0,
                               KAboutData::License_GPL,
                               ki18n("A Kipi plugin to export image collections to "
                               "Rajce.net."),
                               ki18n( "(c) 2011, Lukas Krejci" ));

    m_about->addAuthor(ki18n( "Lukas Krejci" ), ki18n("Author and maintainer"),
                       "metlosh at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(showHelp()) );

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction* handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(showHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    button(User1)->setEnabled(false);
}

RajceWindow::~RajceWindow()
{
    delete m_about;
}

void RajceWindow::reactivate()
{
    m_widget->reactivate();
    show();
}

void RajceWindow::showHelp()
{
    KToolInvocation::invokeHelp("rajceexport", "kipi-plugins");
}

void RajceWindow::slotSetUploadButtonEnabled(bool enabled)
{
    button(User1)->setEnabled(enabled);
}

void RajceWindow::slotClose()
{
    m_widget->cancelUpload();
    m_widget->writeSettings();
}

} //namespace KIPIRajceExportPlugin
