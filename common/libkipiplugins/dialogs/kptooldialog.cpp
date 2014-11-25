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

// Qt includes

#include <QIcon>
#include <QAction>
#include <QDialog>

// KDE includes

#include <kdialog.h>
#include <khelpmenu.h>
#include <klocale.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Libkipi includes

#include <interface.h>
#include <pluginloader.h>

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

    QDialog*     dialog;

    Interface*   iface;
    KPAboutData* about;
};

KPDialogBase::KPDialogBase(QDialog* const dlg)
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

void KPDialogBase::setAboutData(KPAboutData* const data, QPushButton* help)
{
    if (!data || !d->dialog) return;

    if (!help)
    {
        KDialog* const kdlg = dynamic_cast<KDialog*>(d->dialog);

        if (kdlg)
        {
            help = kdlg->button(KDialog::Help);
        }
        else
        {
            KPageDialog* const kpdlg = dynamic_cast<KPageDialog*>(d->dialog);

            if (kpdlg)
            {
                help = kpdlg->button(QDialogButtonBox::Help);
            }
        }
    }

    if (!help) return;

    d->about = data;
    d->about->setHelpButton(help);
}

// -----------------------------------------------------------------------------------

KPToolDialog::KPToolDialog(QWidget* const parent)
    : KDialog(parent), KPDialogBase(this)
{
    setButtons(KDialog::Help | KDialog::Ok);
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
    setStandardButtons(QDialogButtonBox::Help | QDialogButtonBox::Ok);
}

KPPageDialog::~KPPageDialog()
{
}

} // namespace KIPIPlugins
