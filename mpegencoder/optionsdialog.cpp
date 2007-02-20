//////////////////////////////////////////////////////////////////////////////
//
//    OPTIONSDIALOG.CPP
//
//    Copyright (C) 2003 Gilles CAULIER <caulier dot gilles at gmail dot com>
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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Include files for Qt

#include <qvbox.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qstring.h>

// Include files for KDE

#include <klocale.h>
#include <klineedit.h>
#include <kiconloader.h>
#include <kfiledialog.h>

// Local includes.

#include "optionsdialog.h"
#include "kimg2mpg.h"

namespace KIPIMPEGEncoderPlugin
{

////////////////////////////////// CONSTRUCTOR //////////////////////////////////////////////

OptionsDialog::OptionsDialog(QWidget *parent)
                 : KDialogBase( parent, "MPEGEncoderOptionsDialog", true,
                 i18n("MPEG Encoder Plugin Settings"), Ok|Cancel, Ok, false)
{
  Icons = new KIconLoader( QString( "MenuDlg" ) );

  QWidget* box = new QWidget( this );
  setMainWidget(box);
  QVBoxLayout* vbox = new QVBoxLayout (box, 10);
  QHBoxLayout* h1 = new QHBoxLayout( vbox );
  QVBoxLayout* v1 = new QVBoxLayout( h1 );
  h1->addSpacing( 5 );
  QGridLayout* g1 = new QGridLayout( v1, 3, 3 );

  // ImageMagick binary programs folder

  QGroupBox* IMBinFolderGroup = new QGroupBox( 3, Qt::Horizontal, i18n( "ImageMagick Binary Programs Path" ), box);
  g1->addWidget( IMBinFolderGroup, 1, 1, Qt::AlignLeft);
  IMBinFolderEditFilename = new KLineEdit( IMBinFolderGroup );
  IMBinFolderEditFilename->setMinimumWidth( 300 );
  QPushButton* IMBinFolderButtonFilename = new QPushButton( IMBinFolderGroup );
  IMBinFolderButtonFilename->setPixmap( LoadIcon( QString( "fileopen" ), KIcon::Toolbar ) );
  connect( IMBinFolderButtonFilename, SIGNAL( clicked() ), this, SLOT( slotIMBinFolderFilenameDialog()));

  // MjpegTools binary programs folder

  QGroupBox* MJBinFolderGroup = new QGroupBox( 3, Qt::Horizontal, i18n( "MjpegTools Binary Programs Path" ), box);
  g1->addWidget( MJBinFolderGroup, 2, 1, Qt::AlignLeft);
  MJBinFolderEditFilename = new KLineEdit( MJBinFolderGroup );
  MJBinFolderEditFilename->setMinimumWidth( 300 );
  QPushButton* MJBinFolderButtonFilename = new QPushButton( MJBinFolderGroup );
  MJBinFolderButtonFilename->setPixmap( LoadIcon( QString( "fileopen" ), KIcon::Toolbar ) );
  connect( MJBinFolderButtonFilename, SIGNAL( clicked() ), this, SLOT( slotMJBinFolderFilenameDialog()));
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

OptionsDialog::~OptionsDialog()
{
}


//////////////////////////////////// FONCTIONS //////////////////////////////////////////////

QPixmap OptionsDialog::LoadIcon( QString Name, int Group )
{
  return Icons->loadIcon( Name, (KIcon::Group)Group );
}

////////////////////////////////// SLOTS ////////////////////////////////////////////////////


void OptionsDialog::slotIMBinFolderFilenameDialog( void )
{
  QString temp;

  temp = KFileDialog::getExistingDirectory(IMBinFolderEditFilename->text(),
                                           this,
                                           i18n("Select path to ImageMagick binary programs..."));

  if( temp.isEmpty() )
    return;

  IMBinFolderEditFilename->setText( temp );
}


void OptionsDialog::slotMJBinFolderFilenameDialog( void )
{
  QString temp;

  temp = KFileDialog::getExistingDirectory(MJBinFolderEditFilename->text(),
                                           this,
                                           i18n("Select path to MjpegTools binary programs..."));

  if( temp.isEmpty() )
    return;

  MJBinFolderEditFilename->setText( temp );
}

}  // NameSpace KIPIMPEGEncoderPlugin

#include "optionsdialog.moc"
