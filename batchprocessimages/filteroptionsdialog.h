//////////////////////////////////////////////////////////////////////////////
//
//    FILTEROPTIONSDIALOG.H
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


#ifndef FILTEROPTIONSDIALOG_H
#define FILTEROPTIONSDIALOG_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

class QComboBox;

class KIntNumInput;

class FilterOptionsDialog : public KDialogBase
{
Q_OBJECT

 public:
   FilterOptionsDialog(QWidget *parent=0, QString FilterType = 0);
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

#endif  // FILTEROPTIONSDIALOG_H
