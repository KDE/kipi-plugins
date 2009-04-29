/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "outputdialog.h"
#include "outputdialog.moc"

// Qt includes

#include <q3textview.h>
#include <q3frame.h>
#include <Q3VBoxLayout>
#include <qlayout.h>
#include <qlabel.h>
#include <QPushButton>

// KDElib includes

#include <klocale.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "pluginsversion.h"

namespace KIPIBatchProcessImagesPlugin
{

OutputDialog::OutputDialog(QWidget* parent, QString caption, QString Messages, QString Header )
            : KDialog(parent)
{
    setCaption(caption);
    setModal(true);
    setButtons(Ok | Help | User1);
    setButtonText(User1, i18n("Copy to Clip&board"));
    setDefaultButton(Ok);
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch processes images"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("An interface to show the output of the \"Batch Process "
                                                 "Images\" Kipi plugin.\n"
                                                 "This plugin uses the \"convert\" program from \"ImageMagick\" "
                                                 "package."),
                                           ki18n("(c) 2003-2009, Gilles Caulier\n"
                                                 "(c) 2007-2009, Aurélien Gateau"));

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor(ki18n("Aurelien Gateau"), ki18n("Maintainer"),
                       "aurelien dot gateau at free dot fr");

    DialogUtils::setupHelpButton(this, m_about);

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
    KToolInvocation::invokeHelp("", "kipi-plugins");
}

void OutputDialog::slotCopyToCliboard( void )
{
    debugView->selectAll(true);
    debugView->copy();
    debugView->selectAll(false);
}

}  // NameSpace KIPIBatchProcessImagesPlugin
