//////////////////////////////////////////////////////////////////////////////
//
//    CONVERTOPTIONSDIALOG.H
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


#ifndef CONVERTOPTIONSDIALOG_H
#define CONVERTOPTIONSDIALOG_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

class QLabel;
class QCheckBox;
class QComboBox;

class KIntNumInput;

namespace KIPIBatchProcessImagesPlugin
{

class ConvertOptionsDialog : public KDialogBase
{
Q_OBJECT

 public:
   ConvertOptionsDialog(QWidget *parent=0, int ImageFormatType = 0);
   ~ConvertOptionsDialog();

   QLabel          *m_label_imageCompression;
   KIntNumInput    *m_JPEGPNGCompression;
   QCheckBox       *m_compressLossLess;
   QComboBox       *m_TIFFCompressionAlgo;
   QComboBox       *m_TGACompressionAlgo;

 public slots:
   void slotCompressLossLessEnabled(bool val);
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // CONVERTOPTIONSDIALOG_H
