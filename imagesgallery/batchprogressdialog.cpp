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
#include <qheader.h>

// Include files for KDE

#include <klocale.h>
#include <kprogress.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <klistview.h>
#include <kiconloader.h>

// Local includes

#include "batchprogressdialog.h"

namespace KIPIImagesGalleryPlugin
{

class BatchProgressItem : public KListViewItem
{
public:
   BatchProgressItem(QListView * parent, QListViewItem *after, QString message, int messageType)
                   : KListViewItem( parent, after), m_messagetype(messageType)
   {
   // Set the icon.
   
   switch( messageType ) 
     {
     case KIPIImagesGalleryPlugin::StartingMessage:
        setPixmap( 0, SmallIcon( "run" ) );
        break;
     case KIPIImagesGalleryPlugin::SucessMessage:
        setPixmap( 0, SmallIcon( "ok" ) );
        break;
     case KIPIImagesGalleryPlugin::WarningMessage:
        setPixmap( 0, SmallIcon( "flag" ) );
        break;
     case KIPIImagesGalleryPlugin::ErrorMessage:
        setPixmap( 0, SmallIcon( "stop" ) );
        break;
     case KIPIImagesGalleryPlugin::ProgressMessage:
        setPixmap( 0, SmallIcon( "info" ) );
        break;
     default:
        setPixmap( 0, SmallIcon( "info" ) );
     }

   // Set the message text.
           
   setText(1, message);
   }

private:
   int m_messagetype;   
    
   void paintCell (QPainter *p, const QColorGroup &cg, int column, int width, int alignment)
      {
      QColorGroup _cg( cg );

      if ( m_messagetype == KIPIImagesGalleryPlugin::ErrorMessage )
          {
          _cg.setColor( QColorGroup::Text, Qt::red );
          KListViewItem::paintCell( p, _cg, column, width, alignment );
          return;
          }

      if ( m_messagetype == KIPIImagesGalleryPlugin::WarningMessage )
          {
          _cg.setColor( QColorGroup::Text, Qt::darkYellow );
          KListViewItem::paintCell( p, _cg, column, width, alignment );
          return;
          }
              
      KListViewItem::paintCell( p, cg, column, width, alignment );
      }
};


/////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

BatchProgressDialog::BatchProgressDialog( QWidget *parent )
                   : KDialogBase( KDialogBase::Plain, i18n("Albums HTML Export process"), Cancel,
                                  Cancel, parent, "HTMLExportActionsDialog", true )
{
    QWidget* box = plainPage();
    QVBoxLayout *dvlay = new QVBoxLayout( box, 6 );

    //---------------------------------------------

    groupBox1 = new QGroupBox( 2, Qt::Horizontal, box );
    
    m_actionsList = new KListView( groupBox1 );
    m_actionsList->addColumn("Status");
    m_actionsList->addColumn("Current actions");
    m_actionsList->setSorting(-1);
    m_actionsList->setItemMargin(1);
    //m_actionsList->setShowSortIndicator(false);
    m_actionsList->header()->hide();
    m_actionsList->setResizeMode(QListView::LastColumn);
    QWhatsThis::add( m_actionsList, i18n("<p>This is the current tasks list released.") );
    dvlay->addWidget( groupBox1 );
    
    //---------------------------------------------

    m_progress = new KProgress( box, "Progress" );
    m_progress->setTotalSteps(100);
    m_progress->setValue(0);
    QWhatsThis::add( m_progress, i18n("<p>This is the list current percent task released.") );
    dvlay->addWidget( m_progress );
    resize( 600, 400 );
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

BatchProgressDialog::~BatchProgressDialog()
{
}


///////////////////////////////////// FONCTIONS /////////////////////////////////////////////

void BatchProgressDialog::addedAction(QString text, int type)
{
    BatchProgressItem *item = new BatchProgressItem(m_actionsList, 
                                                    m_actionsList->lastItem(),
                                                    text, type);
    
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

}  // NameSpace KIPIImagesGalleryPlugin

#include "batchprogressdialog.moc"
