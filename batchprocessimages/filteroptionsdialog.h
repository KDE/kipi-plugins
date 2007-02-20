//////////////////////////////////////////////////////////////////////////////
//
//    FILTEROPTIONSDIALOG.H
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at gmail dot com>
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


#ifndef FILTEROPTIONSDIALOG_H
#define FILTEROPTIONSDIALOG_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

class QComboBox;

class KIntNumInput;

namespace KIPIBatchProcessImagesPlugin
{

class FilterOptionsDialog : public KDialogBase
{
Q_OBJECT

 public:
   FilterOptionsDialog(QWidget *parent=0, int FilterType = 0);
   ~FilterOptionsDialog();

   QComboBox       *m_noiseType;

   KIntNumInput    *m_blurRadius;
   KIntNumInput    *m_blurDeviation;

   KIntNumInput    *m_medianRadius;

   KIntNumInput    *m_noiseRadius;

   KIntNumInput    *m_sharpenRadius;
   KIntNumInput    *m_sharpenDeviation;

   KIntNumInput    *m_unsharpenRadius;
   KIntNumInput    *m_unsharpenDeviation;
   KIntNumInput    *m_unsharpenPercent;
   KIntNumInput    *m_unsharpenThreshold;
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // FILTEROPTIONSDIALOG_H
