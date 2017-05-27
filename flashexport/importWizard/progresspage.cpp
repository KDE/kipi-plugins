/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011      by Veaceslav Munteanu <slavuttici at gmail dot com>
 * Copyright (C) 2009-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QIcon>
#include <QComboBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "simpleviewer.h"
#include "simpleviewersettingscontainer.h"
#include "kpbatchprogressdialog.h"
#include "flashmanager.h"

namespace KIPIFlashExportPlugin
{
// link this page to SimpleViewer to gain access to settings container.
ProgressPage::ProgressPage(FlashManager* const mngr, KPWizardDialog* const dlg)
    : KPWizardPage(dlg, i18n("Exporting..."))
{
    SimpleViewer* const simple               = mngr->simpleView();
    KPBatchProgressWidget* const progresswdg = simple->progressWidget();
    progresswdg->show();

    setPageWidget(progresswdg);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("kipi-flash")).pixmap(128));
}

ProgressPage::~ProgressPage()
{
}

}   // namespace KIPIFlashExportPlugin
