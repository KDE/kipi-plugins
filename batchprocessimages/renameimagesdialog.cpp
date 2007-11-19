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

// KDE includes.

#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "renameimageswidget.h"
#include "renameimagesdialog.h"
#include "renameimagesdialog.moc"

namespace KIPIBatchProcessImagesPlugin
{

RenameImagesDialog::RenameImagesDialog(const KURL::List& images,
                                       KIPI::Interface* interface,
                                       QWidget* parent)
    : KDialogBase( KDialogBase::Plain, "Rename Images", Help|User1|Close,
                   Close, parent, "RenameImages",
                   false, false, i18n("&Start"))
{
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Batch-rename images"),
                                           NULL,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to batch-rename images"),
                                           "(c) 2003-2007, Gilles Caulier");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                       "caulier dot gilles at gmail dot com");

    QPushButton* helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Batch-Rename Images Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    helpButton->setPopup( helpMenu->menu() );

    // gui

    QWidget* box = plainPage();
    QVBoxLayout* lay = new QVBoxLayout(box);
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
   kapp->invokeHelp("renameimages", "kipi-plugins");
}

}  // namespace KIPIBatchProcessImagesPlugin
