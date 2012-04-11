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

// KDE includes

#include <kaction.h>
#include <khelpmenu.h>
#include <kicon.h>
#include <klocale.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Local includes

#include "kpaboutdata.h"

namespace KIPIPlugins
{

static void setupHelpButton(KDialog* const dlg, KPAboutData* const about)
{
    QObject::disconnect(dlg, SIGNAL(helpClicked()),
                        dlg, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(dlg, about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    KAction* handbook   = new KAction(KIcon("help-contents"), i18n("Handbook"), dlg);

    QObject::connect(handbook, SIGNAL(triggered(bool)),
                     dlg, SLOT(slotHelp()));

    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    dlg->button(KDialog::Help)->setMenu(helpMenu->menu());
}

static void callHelpHandbook(KPAboutData* const about)
{
    KToolInvocation::invokeHelp(about ? about->handbookEntry : QString(), "kipi-plugins");
}

// -----------------------------------------------------------------------------------

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
    setupHelpButton(this, about);
}

void KPToolDialog::slotHelp()
{
    callHelpHandbook(d->about);
}

// -----------------------------------------------------------------------------------

class KPWizardDialog::KPWizardDialogPriv
{
public:

    KPWizardDialogPriv()
    {
        about = 0;
    }

    KPAboutData* about;
};

KPWizardDialog::KPWizardDialog(QWidget* const parent)
    : KAssistantDialog(parent), d(new KPWizardDialogPriv)
{
}

KPWizardDialog::~KPWizardDialog()
{
    delete d->about;
    delete d;
}

void KPWizardDialog::setAboutData(KPAboutData* const about)
{
    d->about = about;
    setupHelpButton(this, about);
}

void KPWizardDialog::slotHelp()
{
    callHelpHandbook(d->about);
}

// -----------------------------------------------------------------------------------

class KPPageDialog::KPPageDialogPriv
{
public:

    KPPageDialogPriv()
    {
        about = 0;
    }

    KPAboutData* about;
};

KPPageDialog::KPPageDialog(QWidget* const parent)
    : KPageDialog(parent), d(new KPPageDialogPriv)
{
    setButtons(Help | Ok);
}

KPPageDialog::~KPPageDialog()
{
    delete d->about;
    delete d;
}

void KPPageDialog::setAboutData(KPAboutData* const about)
{
    d->about = about;
    setupHelpButton(this, about);
}

void KPPageDialog::slotHelp()
{
    callHelpHandbook(d->about);
}

} // namespace KIPIPlugins
