//////////////////////////////////////////////////////////////////////////////
//
//    EFFECTOPTIONSDIALOG.H
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


#ifndef EFFECTOPTIONSDIALOG_H
#define EFFECTOPTIONSDIALOG_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

class KIntNumInput;

namespace KIPIBatchProcessImagesPlugin
{

class EffectOptionsDialog : public KDialogBase
{
Q_OBJECT

 public:
   EffectOptionsDialog(QWidget *parent=0, int EffectType = 0);
   ~EffectOptionsDialog();

   KIntNumInput    *m_latWidth;
   KIntNumInput    *m_latHeight;
   KIntNumInput    *m_latOffset;

   KIntNumInput    *m_charcoalRadius;
   KIntNumInput    *m_charcoalDeviation;

   KIntNumInput    *m_edgeRadius;

   KIntNumInput    *m_embossRadius;
   KIntNumInput    *m_embossDeviation;

   KIntNumInput    *m_implodeFactor;

   KIntNumInput    *m_paintRadius;

   KIntNumInput    *m_shadeAzimuth;
   KIntNumInput    *m_shadeElevation;

   KIntNumInput    *m_solarizeFactor;

   KIntNumInput    *m_spreadRadius;

   KIntNumInput    *m_swirlDegrees;

   KIntNumInput    *m_waveAmplitude;
   KIntNumInput    *m_waveLenght;
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // EFFECTOPTIONSDIALOG_H
