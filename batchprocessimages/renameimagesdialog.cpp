/* ============================================================
 *
 * Copyright (C) 2003-2005 Gilles CAULIER <caulier dot gilles at free.fr>
 * Copyright (C) 2005 by Owen Hirst <n8rider@sbcglobal.net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
 *  
 * ============================================================ */

#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

#include <qlayout.h>
#include <qpushbutton.h>

// Includes files for KIPI

#include <libkipi/version.h>

#include "renameimagesdialog.h"
#include "renameimageswidget.h"

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
    
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Batch-rename images"), 
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin to batch-rename images"),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2005, Gilles Caulier", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi");
    
    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at free.fr");
                        
    QPushButton* helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
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
    
}

void RenameImagesDialog::slotHelp(void)
{
   kapp->invokeHelp("renameimages", "kipi-plugins");
}

}

#include "renameimagesdialog.moc"
