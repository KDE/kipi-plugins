/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005 by Owen Hirst <n8rider@sbcglobal.net>
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

// Qt includes.

#include <qlayout.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

// KDE includes.

#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <ktoolinvocation.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "renameimageswidget.h"
#include "renameimagesdialog.h"
#include "renameimagesdialog.moc"

namespace KIPIBatchProcessImagesPlugin
{

RenameImagesDialog::RenameImagesDialog(const KUrl::List& images,
                                       KIPI::Interface* interface,
                                       QWidget* parent)
    : KDialog(parent)
{
    setCaption(i18n("Rename Images"));
    setModal(true);
    setButtons(Help | User1 | Close);
    setButtonGuiItem(User1, i18n("&Start"));
    setDefaultButton(Close);
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Batch-rename images"),
                                           0,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to batch-rename images"),
                                           "(c) 2003-2007, Gilles Caulier");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                       "caulier dot gilles at gmail dot com");

    QPushButton* helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    helpButton->setPopup( helpMenu->menu() );

    // gui

    QWidget* box = plainPage();
    Q3VBoxLayout* lay = new Q3VBoxLayout(box);
    m_widget = new RenameImagesWidget(box, interface, images);
    lay->addWidget(m_widget);

    connect(this, SIGNAL(user1Clicked()),
            m_widget, SLOT(slotStart()));
    
    adjustSize();
}

RenameImagesDialog::~RenameImagesDialog()
{
    delete m_about; 
}

void RenameImagesDialog::slotHelp(void)
{
   KToolInvocation::invokeHelp("renameimages", "kipi-plugins");
}

}  // namespace KIPIBatchProcessImagesPlugin
