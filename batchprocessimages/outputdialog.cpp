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

// KDElib includes

#include <klocale.h>

// Local includes

#include "outputdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////

OutputDialog::OutputDialog(QWidget* parent, QString caption, QString Messages, QString Header )
            : KDialogBase( parent, "OutputDialog", true, caption, User1|Ok, Ok, false,
                           i18n("Copy to Clip&board"))
{
  QWidget* box = new QWidget( this );
  setMainWidget(box);
  QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );

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

void OutputDialog::slotCopyToCliboard( void )
{
  debugView->selectAll(TRUE);
  debugView->copy();
  debugView->selectAll(FALSE);
}

}  // NameSpace KIPIBatchProcessImagesPlugin

#include "outputdialog.moc"
