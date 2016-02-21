/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "importwizarddlg.h"

// Qt includes

#include <QDesktopWidget>
#include <QApplication>

// KDE includes

#include <kconfig.h>
#include <klocalizedstring.h>
#include <kwindowconfig.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes.

#include "manager.h"
#include "aboutdata.h"
#include "intropage.h"
#include "itemspage.h"
#include "preprocessingpage.h"
#include "optimizepage.h"
#include "previewpage.h"
#include "lastpage.h"

namespace KIPIPanoramaPlugin
{

struct ImportWizardDlg::Private
{
    Private()
      : mngr(0),
        introPage(0),
        itemsPage(0),
        preProcessingPage(0),
        optimizePage(0),
        previewPage(0),
        lastPage(0)
    {
    }

    Manager*           mngr;
    IntroPage*         introPage;
    ItemsPage*         itemsPage;
    PreProcessingPage* preProcessingPage;
    OptimizePage*      optimizePage;
    PreviewPage*       previewPage;
    LastPage*          lastPage;
};

ImportWizardDlg::ImportWizardDlg(Manager* const mngr, QWidget* const parent)
    : KPWizardDialog(parent),
      d(new Private)
{
    setModal(false);
    setWindowTitle(i18nc("@title:window", "Panorama Creator Wizard"));

    d->mngr              = mngr;
    d->introPage         = new IntroPage(d->mngr, this);
    d->itemsPage         = new ItemsPage(d->mngr, this);
    d->preProcessingPage = new PreProcessingPage(d->mngr, this);
    d->optimizePage      = new OptimizePage(d->mngr, this);
    d->previewPage       = new PreviewPage(d->mngr, this);
    d->lastPage          = new LastPage(d->mngr, this);

    setAboutData(new PanoramaAboutData());

    // ---------------------------------------------------------------

    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("Panorama Dialog");

    if (group.exists())
    {
        KWindowConfig::restoreWindowSize(windowHandle(), group);
    }
    else
    {
        QDesktopWidget* const desktop = QApplication::desktop();
        int screen                    = desktop->screenNumber();
        QRect srect                   = desktop->availableGeometry(screen);
        resize(800 <= srect.width()  ? 800 : srect.width(),
               750 <= srect.height() ? 750 : srect.height());
    }

    // ---------------------------------------------------------------

    connect(d->preProcessingPage, SIGNAL(signalPreProcessed(void)),
            this, SLOT(next(void)));

    connect(d->optimizePage, SIGNAL(signalOptimized(void)),
            this, SLOT(next(void)));

    connect(d->previewPage, SIGNAL(signalStitchingFinished(void)),
            this, SLOT(next(void)));

    connect(d->lastPage, SIGNAL(signalCopyFinished(void)),
            this, SLOT(accept(void)));
}

ImportWizardDlg::~ImportWizardDlg()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("Panorama Dialog");
    KWindowConfig::saveWindowSize(windowHandle(), group);
    config.sync();

    delete d;
}

} // namespace KIPIPanoramaPlugin
