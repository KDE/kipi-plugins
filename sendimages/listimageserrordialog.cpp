//////////////////////////////////////////////////////////////////////////////
//
//    LISTIMAGESERRORDIALOG.CPP
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

#include "listimageserrordialog.h"

// Qt includes

#include <qpushbutton.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qlabel.h>
#include <qfileinfo.h>

// KDElib includes

#include <klocale.h>
#include <klistview.h>

/////////////////////////////////////////////////////////////////////////////////////////////

listImagesErrorDialog::listImagesErrorDialog(QWidget* parent, QString Caption, const QString &Mess1,
                                             const QString &Mess2, QStringList ListOfiles)
                     : KDialogBase( Caption, Yes|No|Cancel, Yes, Cancel, parent, "listImagesErrorDialog",
                                    true, true )
{
  QWidget* box = new QWidget( this );
  setMainWidget(box);
  QVBoxLayout* ml = new QVBoxLayout( box, 10 );
  QHBoxLayout* h1 = new QHBoxLayout( ml );
  QVBoxLayout* v1 = new QVBoxLayout( h1 );
  h1->addSpacing( 5 );
  QGridLayout* g1 = new QGridLayout( v1, 1, 3 );

  QLabel *labelMess1 = new QLabel ( Mess1, box);
  m_listFiles = new KListView( box );
  m_listFiles->addColumn(i18n("Image file name"));
  m_listFiles->addColumn(i18n("Album"));
  m_listFiles->setSorting(1);
  m_listFiles->setItemMargin(3);
  m_listFiles->setResizeMode(QListView::LastColumn);
  QLabel *labelMess2 = new QLabel ( Mess2, box);

  g1->addWidget (labelMess1, 1, 1);
  g1->addWidget (m_listFiles, 2, 1);
  g1->addWidget (labelMess2, 3, 1);

  for ( QStringList::Iterator it = ListOfiles.begin() ; it != ListOfiles.end() ; ++it )
      {
      QString currentFile = *it;
      QFileInfo fi(currentFile);
      QString Temp = fi.dirPath();

      QListViewItem *item = new QListViewItem( m_listFiles,
                                               fi.fileName(),             // Filename.
                                               Temp.section('/', -1)      // Album.
                                             );
      }

  resize( 500, 200 );
}


/////////////////////////////////////////////////////////////////////////////////////////////

listImagesErrorDialog::~listImagesErrorDialog()
{
}


