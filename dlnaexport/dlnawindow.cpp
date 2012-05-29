/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#include "dlnawindow.moc"

// Qt includes

#include <QAction>

// KDE includes

#include <klocalizedstring.h>
#include <kmenu.h>
#include <kpushbutton.h>

// Local includes

#include "kpaboutdata.h"
#include "dlnawidget.h"

namespace KIPIDLNAExportPlugin
{

DLNAWindow::DLNAWindow(const QString& tmpFolder)
    : KPToolDialog(0)
{
    m_widget = new DLNAWidget(iface(), tmpFolder, this);

    setMainWidget(m_widget);
    setWindowIcon(KIcon("dlna"));
    setButtons(Help|Close);
    setDefaultButton(Close);
    setModal(false);

    setWindowTitle(i18n("Export via DLNA"));
    setButtonGuiItem(User1, KGuiItem(i18n("Start Upload"), "network-workgroup",
                                     i18n("Start upload via DLNA")));
    m_widget->setMinimumSize(700, 500);

    //--------------------------------------------------------------------

    KPAboutData* about = new KPAboutData(ki18n("DLNA Export"),
                             0,
                             KAboutData::License_GPL,
                             ki18n("A Kipi plugin to export image collections via "
                             "DLNA."),
                             ki18n( "(c) 2012, Smit Mehta" ));

    about->addAuthor(ki18n( "Smit Mehta" ), ki18n("Author and maintainer"),
                     "smit dot meh at gmail dot com");

    about->setHandbookEntry("dlnaexport");
    setAboutData(about);
}

DLNAWindow::~DLNAWindow()
{
}

} //namespace KIPIDLNAExportPlugin
