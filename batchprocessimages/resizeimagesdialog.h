//////////////////////////////////////////////////////////////////////////////
//
//    RESIZEIMAGESDIALOG.H
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


#ifndef RESIZEIMAGESDIALOG_H
#define RESIZEIMAGESDIALOG_H

// Local includes

#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class ResizeImagesDialog : public BatchProcessImagesDialog
{
Q_OBJECT

 public:
 
   ResizeImagesDialog( KURL::List images, KIPI::Interface* interface, QWidget *parent=0 );
   ~ResizeImagesDialog();

 private slots:
 
   void slotHelp(void);
   void slotOptionsClicked(void);

 protected:
 
   QString                m_resizeFilter;
   QString                m_paperSize;
   QString                m_printDpi;

   QColor                 m_backgroundColor;
   QColor                 m_bgColor;

   bool                   m_customSettings;

   int                    m_customXSize;
   int                    m_customYSize;
   int                    m_marging;
   int                    m_customDpi;
   int                    m_size;
   int                    m_xPixels;
   int                    m_yPixels;
   int                    m_Width;
   int                    m_Height;
   int                    m_Border;
   int                    m_fixedWidth;
   int                    m_fixedHeight;
   int			  m_quality;

   QString makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                       const QString& albumDest, bool previewMode);

   void readSettings(void);
   void saveSettings(void);
   bool prepareStartProcess(BatchProcessImagesItem *item,
                            const QString& albumDest);

   bool ResizeImage( int &w, int &h, int SizeFactor);
   
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // RESIZEIMAGESDIALOG_H
