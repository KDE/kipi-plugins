//////////////////////////////////////////////////////////////////////////////
//
//    EFFECTIMAGESDIALOG.H
//
//    Copyright (C) 2003-2004 Gilles CAULIER <caulier dot gilles at free.fr>
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


#ifndef EFFECTIMAGESDIALOG_H
#define EFFECTIMAGESDIALOG_H

// Local includes

#include "batchprocessimagesdialog.h"

class BatchProcessImagesItem;

class EffectImagesDialog : public BatchProcessImagesDialog
{
Q_OBJECT

 public:
   EffectImagesDialog(QWidget *parent=0, QStringList filesList=0);
   ~EffectImagesDialog();

 private slots:
   void slotAbout(void);
   void slotOptionsClicked(void);

 protected:
   int  m_latWidth;
   int  m_latHeight;
   int  m_latOffset;
   int  m_charcoalRadius;
   int  m_charcoalDeviation;
   int  m_edgeRadius;
   int  m_embossRadius;
   int  m_embossDeviation;
   int  m_implodeFactor;
   int  m_paintRadius;
   int  m_shadeAzimuth;
   int  m_shadeElevation;
   int  m_solarizeFactor;
   int  m_spreadRadius;
   int  m_swirlDegrees;
   int  m_waveAmplitude;
   int  m_waveLenght;

   QString makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                       Digikam::AlbumInfo *albumDest);
                       
   void readSettings(void);
   void saveSettings(void);
};

#endif  // EFFECTIMAGESDIALOG_H
