/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "PLEConfigDialog.h"

#include "PLEConfigSkeleton.h"
#include "PLEConfigViewWidget.h"

#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::PLEConfigDialogPrivate
{
    PLEConfigViewWidget * confVWdg;

    friend class PLEConfigDialog;
};

PLEConfigDialog::PLEConfigDialog(QWidget * parent) :
    KConfigDialog(parent, "settings", PLEConfigSkeleton::self()),
    d(new PLEConfigDialogPrivate)
{
    d->confVWdg = new PLEConfigViewWidget( 0, i18n("View") );
    this->addPage( d->confVWdg, i18n("View") )->setIcon(KIcon(QIcon(":/view.png")));
}

PLEConfigDialog::~PLEConfigDialog()
{
   delete d;
}

void PLEConfigDialog::updateSettings()
{
    d->confVWdg->updateSettings();
}

void PLEConfigDialog::updateWidgets()
{
    d->confVWdg->updateWidgets();
}
