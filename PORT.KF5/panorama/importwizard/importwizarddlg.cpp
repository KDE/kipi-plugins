/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "importwizarddlg.moc"

// Qt includes

#include <QDesktopWidget>
#include <QApplication>

// KDE includes

#include <kconfig.h>
#include <kmenu.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/interface.h>

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
    : KPWizardDialog(parent), d(new Private)
{
    setModal(false);
    setWindowTitle(i18n("Panorama Creator Wizard"));

    d->mngr              = mngr;
    d->introPage         = new IntroPage(d->mngr, this);
    d->itemsPage         = new ItemsPage(d->mngr, this);
    d->preProcessingPage = new PreProcessingPage(d->mngr, this);
    d->optimizePage      = new OptimizePage(d->mngr, this);
    d->previewPage       = new PreviewPage(d->mngr, this);
    d->lastPage          = new LastPage(d->mngr, this);

    setAboutData(new PanoramaAboutData());

    // ---------------------------------------------------------------

    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Panorama Dialog"));
    
    if(group.exists())
    {
        restoreDialogSize(group);
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

    setValid(d->introPage->page(), d->introPage->binariesFound());
}

ImportWizardDlg::~ImportWizardDlg()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Panorama Dialog"));
    saveDialogSize(group);
    config.sync();
    
    delete d;
}

Manager* ImportWizardDlg::manager() const
{
    return d->mngr;
}

KUrl::List ImportWizardDlg::itemUrls() const
{
    return d->itemsPage->itemUrls();
}

void ImportWizardDlg::next()
{
    if (currentPage() == d->itemsPage->page())
    {
        d->mngr->setItemsList(d->itemsPage->itemUrls());
    }
    else if (currentPage() == d->preProcessingPage->page())
    {
        // Do not give access to Next button during pre-processing.
        setValid(d->preProcessingPage->page(), false);
        d->preProcessingPage->process();
        // Next is handled with signals/slots
        return;
    }
    else if (currentPage() == d->optimizePage->page())
    {
        // Do not give access to Next button during optimization.
        setValid(d->optimizePage->page(), false);
        d->optimizePage->process();
        // Next is handled with signals/slots
        return;
    }
    else if (currentPage() == d->previewPage->page())
    {
        // And start the final stitching process
        setValid(d->previewPage->page(), false);
        d->previewPage->startStitching();

        // Next will be handled using signals/slots
        return;
    }
    else if (currentPage() == d->lastPage->page())
    {
        setValid(d->lastPage->page(), false);
        d->lastPage->copyFiles();
        return;
    }

    KAssistantDialog::next();
}

void ImportWizardDlg::back()
{
    if (currentPage() == d->preProcessingPage->page())
    {
        if (!d->preProcessingPage->cancel())
        {
            setValid(d->preProcessingPage->page(), true);
            return;
        }
        setValid(d->preProcessingPage->page(), true);
    }
    else if (currentPage() == d->optimizePage->page())
    {
        if (!d->optimizePage->cancel())
        {
            setValid(d->optimizePage->page(), true);
            return;
        }
        setValid(d->optimizePage->page(), true);
        d->preProcessingPage->resetPage();
    }
    else if (currentPage() == d->previewPage->page())
    {
        if (d->previewPage->cancel())
        {
            setValid(d->previewPage->page(), true);

            d->optimizePage->resetPage();
        }
        else
        {
            setValid(d->previewPage->page(), true);
            return;
        }
    }
    else if (currentPage() == d->lastPage->page())
    {
        KAssistantDialog::back();
        d->previewPage->resetPage();
        return;
    }

    KAssistantDialog::back();
}

void ImportWizardDlg::accept()
{
    setValid(d->lastPage->page(), false);
    d->lastPage->copyFiles();
}

void ImportWizardDlg::slotIntroPageIsValid(bool binariesFound)
{
    setValid(d->introPage->page(), binariesFound);
}

void ImportWizardDlg::slotItemsPageIsValid(bool valid)
{
    setValid(d->itemsPage->page(), valid);
}

void ImportWizardDlg::slotPreProcessed(bool success)
{
    if (!success)
    {
        // pre-processing failed.
        setValid(d->preProcessingPage->page(), false);
    }
    else
    {
        // pre-processing Done.
        setValid(d->preProcessingPage->page(), true);
        d->optimizePage->resetPage();
        KAssistantDialog::next();
    }
}

void ImportWizardDlg::slotOptimized(bool success)
{
    if (!success)
    {
        // Optimization failed.
        setValid(d->optimizePage->page(), false);
    }
    else
    {
        // Optimization finished.
        setValid(d->optimizePage->page(), true);
        setValid(d->previewPage->page(), true);
        KAssistantDialog::next();

        // Start the Preview generation
        d->previewPage->computePreview();
    }
}

void ImportWizardDlg::slotPreviewStitchingFinished(bool success)
{
    if (!success)
    {
        setValid(d->previewPage->page(), false);
    }
}

void ImportWizardDlg::slotStitchingFinished(bool success)
{
    if (success)
    {
        setValid(d->previewPage->page(), true);
        KAssistantDialog::next();
        d->lastPage->resetTitle();
    }
}

void ImportWizardDlg::slotCopyFinished(bool success)
{
    if (success)
    {   
        KAssistantDialog::accept();
    }
    else
    {
        setValid(d->lastPage->page(), true);
    }
}

void ImportWizardDlg::slotLastPageIsValid(bool isValid)
{
    setValid(d->lastPage->page(), isValid);
}

} // namespace KIPIPanoramaPlugin
