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

class KPToolDialog::KPToolDialogPriv
{
public:

    KPToolDialogPriv(KDialog* const dlg)
    {
        about  = 0;
        dialog = dlg;
    }

    ~KPToolDialogPriv()
    {
        delete about;
    }

    void setupHelpButton(KPAboutData* const data);
    void callHelpHandbook();

public:

    KPAboutData* about;
    KDialog*     dialog;
};

void KPToolDialog::KPToolDialogPriv::setupHelpButton(KPAboutData* const data)
{
    if (!data) return;

    about = data;

    QObject::disconnect(dialog, SIGNAL(helpClicked()),
                        dialog, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(dialog, about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    KAction* handbook   = new KAction(KIcon("help-contents"), i18n("Handbook"), dialog);

    QObject::connect(handbook, SIGNAL(triggered(bool)),
                     dialog, SLOT(slotHelp()));

    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    dialog->button(KDialog::Help)->setMenu(helpMenu->menu());
}

void KPToolDialog::KPToolDialogPriv::callHelpHandbook()
{
    KToolInvocation::invokeHelp(about ? about->handbookEntry : QString(), "kipi-plugins");
}

// -----------------------------------------------------------------------------------

KPToolDialog::KPToolDialog(QWidget* const parent)
    : KDialog(parent), d(new KPToolDialogPriv(this))
{
    setButtons(Help | Ok);
}

KPToolDialog::~KPToolDialog()
{
    delete d;
}

void KPToolDialog::setAboutData(KPAboutData* const about)
{
    d->setupHelpButton(about);
}

void KPToolDialog::slotHelp()
{
    d->callHelpHandbook();
}

// -----------------------------------------------------------------------------------

KPWizardDialog::KPWizardDialog(QWidget* const parent)
    : KAssistantDialog(parent), d(new KPToolDialog::KPToolDialogPriv(this))
{
}

KPWizardDialog::~KPWizardDialog()
{
    delete d;
}

void KPWizardDialog::setAboutData(KPAboutData* const about)
{
    d->setupHelpButton(about);
}

void KPWizardDialog::slotHelp()
{
    d->callHelpHandbook();
}

// -----------------------------------------------------------------------------------

KPPageDialog::KPPageDialog(QWidget* const parent)
    : KPageDialog(parent), d(new KPToolDialog::KPToolDialogPriv(this))
{
    setButtons(Help | Ok);
}

KPPageDialog::~KPPageDialog()
{
    delete d;
}

void KPPageDialog::setAboutData(KPAboutData* const about)
{
    d->setupHelpButton(about);
}

void KPPageDialog::slotHelp()
{
    d->callHelpHandbook();
}

} // namespace KIPIPlugins
