/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at free.fr>
 * Date   : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 * 
 * Copyright 2003-2005 by Gilles Caulier
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
 * ============================================================ */

// Qt includes

#include <qpushbutton.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qframe.h>

// KDElib includes

#include <klocale.h>
#include <klistview.h>
#include <kurl.h>
#include <kstandarddirs.h>

// Local includes.

#include "listimageserrordialog.h"
#include "listimageserrordialog.moc"

namespace KIPISendimagesPlugin
{

listImagesErrorDialog::listImagesErrorDialog(QWidget* parent, QString Caption, 
                                             const QString &Mess1, const QString &Mess2,
                                             KURL::List ListOfiles)
                     : KDialogBase( Caption, Yes|No|Cancel, Yes, Cancel, parent,
                                    "listImagesErrorDialog", true, false )
{
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout* ml = new QVBoxLayout( box, 10 );
  
    //---------------------------------------------
   
    QFrame *headerFrame = new QFrame( box );
    headerFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QHBoxLayout* layout = new QHBoxLayout( headerFrame );
    layout->setMargin( 2 ); // to make sure the frame gets displayed
    layout->setSpacing( 0 );
    QLabel *pixmapLabelLeft = new QLabel( headerFrame, "pixmapLabelLeft" );
    pixmapLabelLeft->setScaledContents( false );
    layout->addWidget( pixmapLabelLeft );
    QLabel *labelTitle = new QLabel( Caption, headerFrame, "labelTitle" );
    layout->addWidget( labelTitle );
    layout->setStretchFactor( labelTitle, 1 );
    ml->addWidget( headerFrame );
    
    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");
    
    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    //---------------------------------------------
  
    QHBoxLayout* h1 = new QHBoxLayout( ml );
    QVBoxLayout* v1 = new QVBoxLayout( h1 );
    h1->addSpacing( 5 );
    QGridLayout* g1 = new QGridLayout( v1, 1, 3 );

    QLabel *labelMess1 = new QLabel ( Mess1, box);
    m_listFiles = new KListView( box );
    m_listFiles->addColumn(i18n("Image File Name"));
    m_listFiles->addColumn(i18n("From Album"));
    m_listFiles->setSorting(1);
    m_listFiles->setItemMargin(3);
    m_listFiles->setResizeMode(QListView::LastColumn);
    QLabel *labelMess2 = new QLabel ( Mess2, box);

    g1->addWidget (labelMess1, 1, 1);
    g1->addWidget (m_listFiles, 2, 1);
    g1->addWidget (labelMess2, 3, 1);

    for ( KURL::List::Iterator it = ListOfiles.begin() ; it != ListOfiles.end() ; ++it )
    {
        new KListViewItem( m_listFiles,
                           (*it).fileName(),    
                           (*it).directory().section('/', -1) );
    }

   resize( 500, 400 );
}

listImagesErrorDialog::~listImagesErrorDialog()
{
}

}  // NameSpace KIPISendimagesPlugin

