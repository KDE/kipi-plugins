//////////////////////////////////////////////////////////////////////////////
//
//    OUTPUTDIALOG.H
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


#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

class QTextView;

class OutputDialog : public KDialogBase
{
Q_OBJECT

public:
  OutputDialog( QWidget* parent=0, QString caption=QString::null,
                QString Messages=QString::null, QString Header=QString::null );
  ~OutputDialog();

private slots:
  void slotCopyToCliboard( void );

private:
  QTextView *debugView;
};

#endif  // OUTPUTDIALOG_H

