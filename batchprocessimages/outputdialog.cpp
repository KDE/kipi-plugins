/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <q3textview.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <q3frame.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

// KDElib includes

#include <klocale.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>

// Local includes

#include "pluginsversion.h"
#include "outputdialog.h"
#include "outputdialog.moc"

namespace KIPIBatchProcessImagesPlugin
{

OutputDialog::OutputDialog(QWidget* parent, QString caption, QString Messages, QString Header )
            : KDialog(parent)
{
    setCaption(caption);
    setModal(true);
    setButtons(Ok | Help | User1);
    setButtonGuiItem(User1, i18n("Copy to Clip&board"));
    setDefaultButton(Ok);
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Batch processes images"),
                                           0,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("An interface to show the output of the \"Batch Process "
                                                     "Images\" Kipi plugin.\n"
                                                     "This plugin uses the \"convert\" program from \"ImageMagick\" "
                                                     "package."),
                                           "(c) 2003-2007, Gilles Caulier");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                       "caulier dot gilles at gmail dot com");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbooks"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    //---------------------------------------------

    QWidget* box = new QWidget( this );
    setMainWidget(box);
    Q3VBoxLayout *dvlay = new Q3VBoxLayout( box, 10, spacingHint() );

    //---------------------------------------------

    QLabel *labelHeader = new QLabel( Header, box);
    dvlay->addWidget( labelHeader );

    debugView = new Q3TextView( box );
    debugView->append( Messages );
    dvlay->addWidget( debugView );

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotCopyToCliboard()));

    resize( 600, 400 );
}

OutputDialog::~OutputDialog()
{
    delete m_about;
}

void OutputDialog::slotHelp( void )
{
    KToolInvocation::invokeHelp("",
                                             "kipi-plugins");
}

void OutputDialog::slotCopyToCliboard( void )
{
    debugView->selectAll(true);
    debugView->copy();
    debugView->selectAll(false);
}

}  // NameSpace KIPIBatchProcessImagesPlugin
