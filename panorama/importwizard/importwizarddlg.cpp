/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
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
#include "actionthread.h"
#include "aboutdata.h"
#include "cpfindbinary.h"
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

    KConfig config(QStringLiteral("kipirc"));
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

    connect(d->introPage, SIGNAL(signalIntroPageIsValid(bool)),
            this, SLOT(slotIntroPageIsValid(bool)));

    connect(d->itemsPage, SIGNAL(signalItemsPageIsValid(bool)),
            this, SLOT(slotItemsPageIsValid(bool)));

    connect(d->preProcessingPage, SIGNAL(signalPreProcessed(bool)),
            this, SLOT(slotPreProcessed(bool)));

    connect(d->optimizePage, SIGNAL(signalOptimized(bool)),
            this, SLOT(slotOptimized(bool)));

    connect(d->previewPage, SIGNAL(signalPreviewStitchingFinished(bool)),
            this, SLOT(slotPreviewStitchingFinished(bool)));

    connect(d->previewPage, SIGNAL(signalStitchingFinished(bool)),
            this, SLOT(slotStitchingFinished(bool)));

    connect(d->lastPage, SIGNAL(signalCopyFinished(bool)),
            this, SLOT(slotCopyFinished(bool)));

    connect(d->lastPage, SIGNAL(signalIsValid(bool)),
            this, SLOT(slotLastPageIsValid(bool)));

    d->introPage->setComplete(d->introPage->binariesFound());
}

ImportWizardDlg::~ImportWizardDlg()
{
    KConfig config(QStringLiteral("kipirc"));
    KConfigGroup group = config.group("Panorama Dialog");
    KWindowConfig::saveWindowSize(windowHandle(), group);
    config.sync();

    delete d;
}

Manager* ImportWizardDlg::manager() const
{
    return d->mngr;
}

QList<QUrl> ImportWizardDlg::itemUrls() const
{
    return d->itemsPage->itemUrls();
}

void ImportWizardDlg::next()
{
    if (currentPage() == d->itemsPage)
    {
        d->mngr->setItemsList(d->itemsPage->itemUrls());
    }
    else if (currentPage() == d->preProcessingPage)
    {
        // Do not give access to Next button during pre-processing.
        d->preProcessingPage->setComplete(false);
        d->preProcessingPage->process();
        // Next is handled with signals/slots
        return;
    }
    else if (currentPage() == d->optimizePage)
    {
        // Do not give access to Next button during optimization.
        d->optimizePage->setComplete(false);
        d->optimizePage->process();
        // Next is handled with signals/slots
        return;
    }
    else if (currentPage() == d->previewPage)
    {
        // And start the final stitching process
        d->previewPage->setComplete(false);
        d->previewPage->startStitching();

        // Next will be handled using signals/slots
        return;
    }
    else if (currentPage() == d->lastPage)
    {
        d->lastPage->setComplete(false);
        d->lastPage->copyFiles();
        return;
    }

    KPWizardDialog::next();
}

void ImportWizardDlg::back()
{
    if (currentPage() == d->preProcessingPage)
    {
        if (!d->preProcessingPage->cancel())
        {
            d->preProcessingPage->setComplete(true);
            return;
        }

        d->preProcessingPage->setComplete(true);
    }
    else if (currentPage() == d->optimizePage)
    {
        if (!d->optimizePage->cancel())
        {
            d->optimizePage->setComplete(true);
            return;
        }

        d->optimizePage->setComplete(true);
        d->preProcessingPage->resetPage();
    }
    else if (currentPage() == d->previewPage)
    {
        if (d->previewPage->cancel())
        {
            d->previewPage->setComplete(true);

            d->optimizePage->resetPage();
        }
        else
        {
            d->previewPage->setComplete(true);
            return;
        }
    }
    else if (currentPage() == d->lastPage)
    {
        KPWizardDialog::back();
        d->previewPage->resetPage();
        return;
    }

    KPWizardDialog::back();
}

void ImportWizardDlg::accept()
{
    d->lastPage->setComplete(false);
    d->lastPage->copyFiles();
}

void ImportWizardDlg::slotIntroPageIsValid(bool binariesFound)
{
    d->introPage->setComplete(binariesFound);
}

void ImportWizardDlg::slotItemsPageIsValid(bool valid)
{
    d->itemsPage->setComplete(valid);
}

void ImportWizardDlg::slotPreProcessed(bool success)
{
    if (!success)
    {
        // pre-processing failed.
        d->preProcessingPage->setComplete(false);
    }
    else
    {
        // pre-processing Done.
        d->preProcessingPage->setComplete(true);
        d->optimizePage->resetPage();
        KPWizardDialog::next();
    }
}

void ImportWizardDlg::slotOptimized(bool success)
{
    if (!success)
    {
        // Optimization failed.
        d->optimizePage->setComplete(false);
    }
    else
    {
        // Optimization finished.
        d->optimizePage->setComplete(true);
        d->previewPage->setComplete(true);
        KPWizardDialog::next();

        // Start the Preview generation
        d->previewPage->computePreview();
    }
}

void ImportWizardDlg::slotPreviewStitchingFinished(bool success)
{
    if (!success)
    {
        d->previewPage->setComplete(false);
    }
}

void ImportWizardDlg::slotStitchingFinished(bool success)
{
    if (success)
    {
        d->previewPage->setComplete(true);
        KPWizardDialog::next();
        d->lastPage->resetTitle();
    }
}

void ImportWizardDlg::slotCopyFinished(bool success)
{
    if (success)
    {
        KPWizardDialog::accept();
    }
    else
    {
        d->lastPage->setComplete(true);
    }
}

void ImportWizardDlg::slotLastPageIsValid(bool isValid)
{
    d->lastPage->setComplete(isValid);
}

} // namespace KIPIPanoramaPlugin
