//////////////////////////////////////////////////////////////////////////////
//
//    RENAMEIMAGESOPTIONSDIALOG.H
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


#ifndef RENAMEIMAGESOPTIONSDIALOG_H
#define RENAMEIMAGESOPTIONSDIALOG_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

class QLabel;
class QCheckBox;
class QComboBox;
class QPushButton;

class KIntSpinBox;
class KLineEdit;
class KDateWidget;

class RenameImagesOptionsDialog : public KDialogBase
{
Q_OBJECT

 public:
   RenameImagesOptionsDialog(QWidget *parent=0);
   ~RenameImagesOptionsDialog();

   QLabel          *m_labelPrefix;
   QLabel          *m_labelStart;
   QLabel          *m_labelSort;
   QLabel          *m_statusbar;

   KLineEdit       *m_prefix;
   KIntSpinBox     *m_enumeratorStart;

   QComboBox       *m_sortType;

   QCheckBox       *m_addOriginalFileName;
   QCheckBox       *m_addImageFileDate;
   QCheckBox       *m_addImageFileSize;
   QCheckBox       *m_reverseOrder;
   QCheckBox       *m_dateChange;

   KDateWidget     *m_kDatePicker;
};

#endif  // RENAMEIMAGESOPTIONSDIALOG_H
