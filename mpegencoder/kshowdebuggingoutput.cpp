//////////////////////////////////////////////////////////////////////////////
//
//    KSHOWDEBUGGINGOUTPUT.CPP
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

// Local includes.

#include "kshowdebuggingoutput.h"

namespace KIPIMPEGEncoderPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////

KShowDebuggingOutput::KShowDebuggingOutput(QString Messages, QString Header, QString Foot, QWidget* parent )
  : KDialog( parent, "debugViewDialog", true )
{
  setCaption( i18n("Debugging output") );

  okButton = new QPushButton( i18n("&OK"), this );
  ClipBoardCopy = new QPushButton( i18n("Copy to clip&board"), this );
  debugView = new QTextView( this );
  grid = new QGridLayout( this );

  grid->addMultiCellWidget( debugView, 0, 0, 0, 2 );
  grid->addWidget( okButton, 1, 1 );
  grid->addWidget( ClipBoardCopy, 1, 2 );
  grid->setSpacing( spacingHint() );
  grid->setMargin( marginHint() );
  grid->setColStretch( 0, 1 );

  connect( okButton, SIGNAL(pressed()), this, SLOT(accept()) );
  connect( ClipBoardCopy, SIGNAL(pressed()), this, SLOT(slotCopyToCliboard()) );

  // add the debugging output

  debugView->append( Header );
  debugView->append( "-----------------------------------------------\n" );
  debugView->append( Messages );
  debugView->append( "-----------------------------------------------\n" );
  debugView->append( Foot );
  
  resize( 600, 300 );
}


/////////////////////////////////////////////////////////////////////////////////////////////

KShowDebuggingOutput::~KShowDebuggingOutput()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KShowDebuggingOutput::slotCopyToCliboard( void )
{
  debugView->selectAll(TRUE);
  debugView->copy();
  debugView->selectAll(FALSE);
}

}  // NameSpace KIPIMPEGEncoderPlugin

#include "kshowdebuggingoutput.moc"
