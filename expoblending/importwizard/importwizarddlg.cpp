/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kmenu.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Locale incudes.

#include "manager.h"
#include "actionthread.h"
#include "aboutdata.h"
#include "intropage.h"
#include "itemspage.h"
#include "rawpage.h"
#include "alignpage.h"
#include "lastpage.h"

namespace KIPIExpoBlendingPlugin
{

class ImportWizardDlgPriv
{
public:

    ImportWizardDlgPriv()
    {
        mngr      = 0;
        introPage = 0;
        itemsPage = 0;
        rawPage   = 0;
        alignPage = 0;
        lastPage  = 0;
    }

    Manager*   mngr;

    IntroPage* introPage;
    ItemsPage* itemsPage;
    RawPage*   rawPage;
    AlignPage* alignPage;
    LastPage*  lastPage;
};

ImportWizardDlg::ImportWizardDlg(Manager* mngr, QWidget* parent)
               : KAssistantDialog(parent), d(new ImportWizardDlgPriv)
{
    d->mngr      = mngr;
    d->introPage = new IntroPage(this);
    d->itemsPage = new ItemsPage(d->mngr, this);
    //d->rawPage   = new RawPage(this);
    d->alignPage = new AlignPage(d->mngr, this);
    d->lastPage  = new LastPage(d->mngr, this);

    // ---------------------------------------------------------------
    // About data and help button.

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->mngr->about(), false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ---------------------------------------------------------------

    resize(600, 500);

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotFinishPressed()));

    connect(d->itemsPage, SIGNAL(signalItemsPageIsValid(bool)),
            this, SLOT(slotItemsPageIsValid(bool)));

    connect(d->alignPage, SIGNAL(signalAligned(const ItemUrlsMap&)),
            this, SLOT(slotAligned(const ItemUrlsMap&)));
}

ImportWizardDlg::~ImportWizardDlg()
{
    delete d;
}

void ImportWizardDlg::slotHelp()
{
    KToolInvocation::invokeHelp("expoblending", "kipi-plugins");
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
/*
    else if (currentPage() == d->rawPage->page())
    {
        d->mngr->setRawDecodingSettings(d->rawPage->rawDecodingSettings());
    }
*/
    else if (currentPage() == d->alignPage->page())
    {
        // Do not give acces to Next button during alignment process.
        setValid(d->alignPage->page(), false);
        d->alignPage->processAlignement();
        // Next is handled with signals/slots
        return;
    }
    KAssistantDialog::next();
}

void ImportWizardDlg::back()
{
    if (currentPage() == d->alignPage->page())
    {
        d->alignPage->cancelAlignement();
        KAssistantDialog::back();
        setValid(d->alignPage->page(), true);
    }
}

void ImportWizardDlg::slotAligned(const ItemUrlsMap& alignedUrlsMap)
{
    if (alignedUrlsMap.isEmpty())
    {
        // Alignement failed.
        setValid(d->alignPage->page(), false);
    }
    else
    {
        // Alignement Done.
        d->mngr->setAlignedMap(alignedUrlsMap);
        KAssistantDialog::next();
    }
}

void ImportWizardDlg::slotItemsPageIsValid(bool valid)
{
    setValid(d->itemsPage->page(), valid);
}

void ImportWizardDlg::slotFinishPressed()
{
}

} // namespace KIPIExpoBlendingPlugin
