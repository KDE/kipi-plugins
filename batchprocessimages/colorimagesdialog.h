//////////////////////////////////////////////////////////////////////////////
//
//    COLORIMAGESDIALOG.H
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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef COLORIMAGESDIALOG_H
#define COLORIMAGESDIALOG_H

// Local includes

#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class ColorImagesDialog : public BatchProcessImagesDialog
{
Q_OBJECT

 public:
 
   ColorImagesDialog( KURL::List images, KIPI::Interface* interface, QWidget *parent=0 );
   ~ColorImagesDialog();

 private slots:
 
   void slotHelp(void);
   void slotOptionsClicked(void);
   void slotTypeChanged(int type);

 protected:
 
   QString                m_depthValue;
   int                    m_fuzzDistance;
   int                    m_segmentCluster;
   int                    m_segmentSmooth;

   QString makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                       const QString& albumDest, bool previewMode);

   void readSettings(void);
   void saveSettings(void);
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // COLORIMAGESDIALOG_H
