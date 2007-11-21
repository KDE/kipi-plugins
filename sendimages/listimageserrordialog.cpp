/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
    QVBoxLayout* ml = new QVBoxLayout(box);

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
