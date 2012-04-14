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

// KDE includes

#include <kaction.h>
#include <khelpmenu.h>
#include <kicon.h>
#include <klocale.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Local includes

#include "kptooldialog.h"
#include "kpdialogprivate.h"

namespace KIPIPlugins
{

KPToolDialog::KPToolDialog(QWidget* const parent)
    : KDialog(parent), d(new KPDialogPrivate(this))
{
    setButtons(Help | Ok);
}

KPToolDialog::~KPToolDialog()
{
}

void KPToolDialog::setAboutData(KPAboutData* const about)
{
    d->setAboutData(about);
}

// -----------------------------------------------------------------------------------

KPWizardDialog::KPWizardDialog(QWidget* const parent)
    : KAssistantDialog(parent), d(new KPDialogPrivate(this))
{
}

KPWizardDialog::~KPWizardDialog()
{
}

void KPWizardDialog::setAboutData(KPAboutData* const about)
{
    d->setAboutData(about);
}

// -----------------------------------------------------------------------------------

KPPageDialog::KPPageDialog(QWidget* const parent)
    : KPageDialog(parent), d(new KPDialogPrivate(this))
{
    setButtons(Help | Ok);
}

KPPageDialog::~KPPageDialog()
{
}

void KPPageDialog::setAboutData(KPAboutData* const about)
{
    d->setAboutData(about);
}

} // namespace KIPIPlugins
