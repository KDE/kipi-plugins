//////////////////////////////////////////////////////////////////////////////
//
//    RESIZEOPTIONSDIALOG.H
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
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


#ifndef RESIZEOPTIONSDIALOG_H
#define RESIZEOPTIONSDIALOG_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

class QLabel;
class QComboBox;
class QCheckBox;

class KIntNumInput;
class KColorButton;

namespace KIPIBatchProcessImagesPlugin
{

class ResizeOptionsDialog : public KDialogBase
{
Q_OBJECT

 public:
   ResizeOptionsDialog(QWidget *parent=0, QString ResizeType = 0);
   ~ResizeOptionsDialog();

   QLabel          *m_label_size;
   QLabel          *m_label_resizeFilter;
   QLabel          *m_label_paperSize;
   QLabel          *m_label_printDpi;
   QLabel          *m_label_customXSize;
   QLabel          *m_label_customYSize;
   QLabel          *m_label_customDpi;
   QLabel          *m_label_backgroundColor;
   QLabel          *m_label_marging;
   QLabel          *m_label_Width;
   QLabel          *m_label_Height;
   QLabel          *m_label_bgColor;
   QLabel          *m_label_border;

   KIntNumInput    *m_size;
   KIntNumInput    *m_customXSize;
   KIntNumInput    *m_customYSize;
   KIntNumInput    *m_customDpi;
   KIntNumInput    *m_marging;
   KIntNumInput    *m_Width;
   KIntNumInput    *m_Height;
   KIntNumInput    *m_Border;
   KIntNumInput    *m_fixedWidth;
   KIntNumInput    *m_fixedHeight;

   QCheckBox       *m_customSettings;

   KColorButton    *m_button_backgroundColor;
   KColorButton    *m_button_bgColor;

   QComboBox       *m_resizeFilter;
   QComboBox       *m_paperSize;
   QComboBox       *m_printDpi;

 protected slots:
   void slotCustomSettingsEnabled(bool val);
   void slotOk();

 protected:
   QString          m_Type;
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // RESIZEOPTIONSDIALOG_H
