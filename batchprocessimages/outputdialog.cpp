//////////////////////////////////////////////////////////////////////////////
//
//    OUTPUTDIALOG.CPP
//
//    Copyright (C) 2003 Gilles CAULIER <caulier dot gilles at free.fr>
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

// Qt includes

#include <qtextview.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qframe.h>

// KDElib includes

#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>

// Include files for libKipi.

#include <libkipi/version.h>

// Local includes

#include "outputdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////

OutputDialog::OutputDialog(QWidget* parent, QString caption, QString Messages, QString Header )
            : KDialogBase( parent, "OutputDialog", true, caption, Help|User1|Ok, Ok, false,
                           i18n("Copy to Clip&board"))
{
    // About data and help button.
    
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Batch processes images"), 
                                       kipi_version,
                                       I18N_NOOP("An interface for to show the output of batch processes "
                                                 "images Kipi plugins\n"
                                                 "This plugin use the \"convert\" program from \"ImageMagick\" "
                                                 "package."),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Gilles Caulier", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi.php");
    
    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at free.fr");
                        
    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Kipi Plugins Handbooks"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    //---------------------------------------------
  
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );

    //---------------------------------------------
   
    QFrame *headerFrame = new QFrame( box );
    headerFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QHBoxLayout* layout = new QHBoxLayout( headerFrame );
    layout->setMargin( 2 ); // to make sure the frame gets displayed
    layout->setSpacing( 0 );
    QLabel *pixmapLabelLeft = new QLabel( headerFrame, "pixmapLabelLeft" );
    pixmapLabelLeft->setScaledContents( false );
    layout->addWidget( pixmapLabelLeft );
    QLabel *labelTitle = new QLabel( caption, headerFrame, "labelTitle" );
    layout->addWidget( labelTitle );
    layout->setStretchFactor( labelTitle, 1 );
    dvlay->addWidget( headerFrame );
    
    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");
    
    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    //---------------------------------------------
    
    QLabel *labelHeader = new QLabel( Header, box);
    dvlay->addWidget( labelHeader );

    debugView = new QTextView( box );
    debugView->append( Messages );
    dvlay->addWidget( debugView );

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotCopyToCliboard()));

    resize( 600, 400 );
}


/////////////////////////////////////////////////////////////////////////////////////////////

OutputDialog::~OutputDialog()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////

void OutputDialog::slotHelp( void )
{
    KApplication::kApplication()->invokeHelp("",
                                             "kipi-plugins");
}


/////////////////////////////////////////////////////////////////////////////////////////////

void OutputDialog::slotCopyToCliboard( void )
{
    debugView->selectAll(true);
    debugView->copy();
    debugView->selectAll(false);
}

}  // NameSpace KIPIBatchProcessImagesPlugin

#include "outputdialog.moc"
