//////////////////////////////////////////////////////////////////////////////
//
//    BATCHPROGRESSDIALOG.CPP
//
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////


// Include files for Qt

#include <qvbox.h>
#include <qlayout.h>
#include <qdir.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qcolor.h>
#include <qhgroupbox.h>
#include <qvgroupbox.h>

// Include files for KDE

#include <klocale.h>
#include <kprogress.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <klistview.h>

// Local includes

#include "batchprogressdialog.h"

namespace KIPICDArchivingPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

BatchProgressDialog::BatchProgressDialog( QWidget *parent )
                   : KDialogBase( KDialogBase::Plain, i18n("Preparing archive to CD"), Cancel,
                                  Cancel, parent, "ArchiveToCDActionsDialog", true, true )
{
    QWidget* box = plainPage();
    QVBoxLayout *dvlay = new QVBoxLayout( box, 6 );

    //---------------------------------------------

    groupBox1 = new QGroupBox( 2, Qt::Horizontal, box );
    
    m_actionsList = new KListView( groupBox1 );
    m_actionsList->addColumn(i18n("Current actions"));
    m_actionsList->setSorting(-1);
    m_actionsList->setItemMargin(1);
    m_actionsList->setResizeMode(QListView::LastColumn);

    dvlay->addWidget( groupBox1 );
    
    //---------------------------------------------

    m_progress = new KProgress( box, "Progress" );
    m_progress->setTotalSteps(100);
    m_progress->setValue(0);
    QWhatsThis::add( m_progress, i18n("<p>This is the current percent task released.") );

    dvlay->addWidget( m_progress );
    resize( 600, 400 );
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

BatchProgressDialog::~BatchProgressDialog()
{
}


///////////////////////////////////// FONCTIONS /////////////////////////////////////////////

void BatchProgressDialog::addedAction(QString text)
{
    QListViewItem *item;
    
    if ( !m_actionsList->lastItem() )
        item = new QListViewItem(m_actionsList);
    else 
        item = new QListViewItem(m_actionsList, m_actionsList->lastItem());
    
    item->setText(0, text);
    m_actionsList->ensureItemVisible(item);
}


void BatchProgressDialog::reset()
{
    m_actionsList->clear();
    m_progress->setValue(0);
}


void BatchProgressDialog::setProgress(int current, int total)
{
    m_progress->setTotalSteps(total);
    m_progress->setValue(current);
}

}  // NameSpace KIPICDArchivingPlugin

#include "batchprogressdialog.moc"
