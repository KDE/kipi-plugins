/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-24
 * Description : a dialog to display processed messages in background
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "outputdialog.moc"

// Qt includes

#include <QVBoxLayout>
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

// Local includes

#include "kpaboutdata.h"

namespace KIPIPlugins
{

OutputDialog::OutputDialog(QWidget* parent, const QString& caption,
                           const QString& Messages, const QString& Header)
            : KDialog(parent)
{
    setCaption(caption);
    setModal(true);
    setButtons(Ok | Help | User1);
    setButtonText(User1, i18n("Copy to Clip&board"));
    setDefaultButton(Ok);
    showButton(Help, false);

    //---------------------------------------------

    QWidget* box        = new QWidget(this);
    QVBoxLayout *dvlay  = new QVBoxLayout(box);
    QLabel *labelHeader = new QLabel(Header, box);
    m_debugView         = new KTextBrowser(box);
    m_debugView->append(Messages);

    dvlay->addWidget(labelHeader);
    dvlay->addWidget(m_debugView);
    dvlay->setSpacing(spacingHint());
    dvlay->setMargin(spacingHint());

    setMainWidget(box);

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotCopyToCliboard()));

    resize(600, 400);
}

OutputDialog::~OutputDialog()
{
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
    m_handbookName = handbookName;
    showButton(Help, true);
}

void OutputDialog::slotHelp()
{
    KToolInvocation::invokeHelp(m_handbookName, "kipi-plugins");
}

void OutputDialog::slotCopyToCliboard()
{
    m_debugView->selectAll();
    m_debugView->copy();
    m_debugView->setPlainText(m_debugView->toPlainText());
}

}  // namespace KIPIPlugins
