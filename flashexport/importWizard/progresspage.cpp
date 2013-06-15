/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#include "progresspage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>
#include <kcombobox.h>
#include <kiconloader.h>

//Local includes

#include "simpleviewer.h"
#include "simpleviewersettingscontainer.h"
#include "kpbatchprogressdialog.h"
#include "flashmanager.h"

namespace KIPIFlashExportPlugin
{
// link this page to SimpleViewer to gain access to settings container.
ProgressPage::ProgressPage(FlashManager* const mngr, KAssistantDialog* const dlg)
    : KPWizardPage(dlg, i18n("Exporting..."))
{
    SimpleViewer* simple               = mngr->simpleView();
    KPBatchProgressWidget* progresswdg = simple->progressWidget();
    progresswdg->show();

    setPageWidget(progresswdg);
    setLeftBottomPix(DesktopIcon("kipi-flash", 128));
}

ProgressPage::~ProgressPage()
{
}

}   // namespace KIPIFlashExportPlugin
