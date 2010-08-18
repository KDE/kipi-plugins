/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-24
 * Description : a dialog to display processed messages in background
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "outputdialog.moc"

// Qt includes

#include <QLabel>

// KDE includes

#include <kapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <ktextbrowser.h>
#include <khelpmenu.h>
#include <kpushbutton.h>
#include <kvbox.h>

// Local includes

#include "kpaboutdata.h"

namespace KIPIPlugins
{

class OutputDialog::OutputDialogPriv
{
public:

    OutputDialogPriv()
    {
        debugView = 0;
    }

    QString       handbookName;
    KTextBrowser* debugView;
};

OutputDialog::OutputDialog(QWidget* parent, const QString& caption,
                           const QString& Messages, const QString& Header)
            : KDialog(parent), d(new OutputDialogPriv)
{
    setCaption(caption);
    setModal(true);
    setButtons(Ok | Help | User1);
    setButtonText(User1, i18n("Copy to Clip&board"));
    setDefaultButton(Ok);
    showButton(Help, false);

    //---------------------------------------------

    KVBox* vbox     = new KVBox(this);
    QLabel* lHeader = new QLabel(vbox);
    d->debugView    = new KTextBrowser(vbox);
    d->debugView->append(Messages);
    lHeader->setText(Header);
    vbox->setSpacing(spacingHint());
    vbox->setMargin(spacingHint());

    setMainWidget(vbox);

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotCopyToCliboard()));

    resize(600, 400);
}

OutputDialog::~OutputDialog()
{
    delete d;
}

void OutputDialog::setAboutData(KPAboutData* about, const QString& handbookName)
{
    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction* handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());
    d->handbookName = handbookName;
    showButton(Help, true);
}

void OutputDialog::slotHelp()
{
    KToolInvocation::invokeHelp(d->handbookName, "kipi-plugins");
}

void OutputDialog::slotCopyToCliboard()
{
    d->debugView->selectAll();
    d->debugView->copy();
    d->debugView->setPlainText(d->debugView->toPlainText());
}

}  // namespace KIPIPlugins
