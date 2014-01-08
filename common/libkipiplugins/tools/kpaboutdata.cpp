/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins shared library.
 *
 * Copyright (C) 2006-2010 by Angelo Naselli <anaselli at linux dot it>
 * Copyright (C) 2010-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpaboutdata.moc"

// KDE includes

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>
#include <kdebug.h>
#include <kaction.h>
#include <khelpmenu.h>
#include <kicon.h>
#include <klocale.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Local includes

#include "kpversion.h"

namespace KIPIPlugins
{

KPAboutData::KPAboutData(const KLocalizedString& pluginName,
                         const QByteArray& /*pluginVersion*/,
                         enum  LicenseKey licenseType,
                         const KLocalizedString& pluginDescription,
                         const KLocalizedString& copyrightStatement)
    : QObject(),
      KAboutData(QByteArray("kipiplugins"),  // Name without minus separator for KDE bug report.
                 QByteArray(),
                 pluginName,
                 kipipluginsVersion().toAscii(),
                 KLocalizedString(),
                 licenseType,
                 copyrightStatement,
                 pluginDescription,
                 QByteArray("http://www.digikam.org"))
{
    if (KGlobal::hasMainComponent())
    {
        // setProgramLogo is defined from kde 3.4.0 on
        QString directory = KStandardDirs::locate("data", "kipi/data/kipi-plugins_logo.png");

        // set the kipiplugins logo inside the about dialog
        setProgramLogo(QImage(directory));
    }

    // set the plugin description into long text description
    setOtherText(pluginDescription);

    // put the plugin name and version with kipi-plugins and kipi-plugins version
    KLocalizedString shortDesc = additionalInformation();

    if (KGlobal::hasMainComponent())
    {
        kDebug(AREA_CODE_LOADING) << shortDesc.toString().constData() ;
    }

    // and set the string into the short description
    setShortDescription(shortDesc);
}

KPAboutData::KPAboutData(const KPAboutData& other)
    : QObject((QObject*)(&other)), KAboutData(other)
{
     setHandbookEntry(other.m_handbookEntry);
}

KPAboutData::~KPAboutData()
{
}

void KPAboutData::setHandbookEntry(const QString& entry)
{
    m_handbookEntry = entry;
}

void KPAboutData::setHelpButton(KPushButton* const help)
{
    if (!help) return;

    KHelpMenu* const helpMenu = new KHelpMenu(help, this, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    KAction* const handbook   = new KAction(KIcon("help-contents"), i18n("Handbook"), helpMenu);

    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));

    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    help->setMenu(helpMenu->menu());
}

void KPAboutData::slotHelp()
{
    KToolInvocation::invokeHelp(m_handbookEntry, "kipi-plugins");
}

}   // namespace KIPIPlugins
