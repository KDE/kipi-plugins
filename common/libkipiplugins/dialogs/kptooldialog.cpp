/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-04
 * Description : Tool dialog
 *
 * Copyright (C) 2012-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kptooldialog.h"

// KDE includes

#include <kdialog.h>
#include <kaction.h>
#include <khelpmenu.h>
#include <kicon.h>
#include <klocale.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Libkipi includes

#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "kpaboutdata.h"

namespace KIPIPlugins
{

class KPDialogBase::Private
{

public:

    Private()
    {
        about  = 0;
        dialog = 0;
        iface  = 0;

        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }

    }

    Interface*   iface;
    KPAboutData* about;
    KDialog*     dialog;
};

KPDialogBase::KPDialogBase(KDialog* const dlg)
    : d(new Private)
{
    d->dialog = dlg;
}

KPDialogBase::~KPDialogBase()
{
    delete d->about;
    delete d;
}

Interface* KPDialogBase::iface() const
{
    return d->iface;
}

void KPDialogBase::setAboutData(KPAboutData* const data, KPushButton* help)
{
    if (!data || !d->dialog) return;

    if (!help) help = d->dialog->button(KDialog::Help);
    if (!help) return;

    d->about = data;
    d->about->setHelpButton(help);
}

// -----------------------------------------------------------------------------------

KPToolDialog::KPToolDialog(QWidget* const parent)
    : KDialog(parent), KPDialogBase(this)
{
    setButtons(Help | Ok);
}

KPToolDialog::~KPToolDialog()
{
}

// -----------------------------------------------------------------------------------

KPWizardDialog::KPWizardDialog(QWidget* const parent)
    : KAssistantDialog(parent), KPDialogBase(this)
{
}

KPWizardDialog::~KPWizardDialog()
{
}

// -----------------------------------------------------------------------------------

KPPageDialog::KPPageDialog(QWidget* const parent)
    : KPageDialog(parent), KPDialogBase(this)
{
    setButtons(Help | Ok);
}

KPPageDialog::~KPPageDialog()
{
}

} // namespace KIPIPlugins
