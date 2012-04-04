/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-04
 * Description : Tool dialog
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kptooldialog.moc"

// Qt includes

#include <QAction>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>

// Local includes

#include "kpversion.h"

namespace KIPIPlugins
{

class KPToolDialog::KPToolDialogPriv
{
public:

    KPToolDialogPriv()
    {
        about = 0;
    }

    KPAboutData* about;
};

KPToolDialog::KPToolDialog(QWidget* const parent)
    : KDialog(parent), d(new KPToolDialogPriv)
{
    setButtons(Help | Ok);
}

KPToolDialog::~KPToolDialog()
{
    delete d->about;
    delete d;
}

void KPToolDialog::setAboutData(KPAboutData* const about)
{
    d->about = about;

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction* handbook   = new QAction(i18n("Handbook"), this);

    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));

    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());
}

void KPToolDialog::slotHelp()
{
    KToolInvocation::invokeHelp(d->about ? d->about->handbookEntry : QString(), "kipi-plugins");
}

} // namespace KIPIPlugins
