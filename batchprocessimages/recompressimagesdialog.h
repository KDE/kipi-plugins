//////////////////////////////////////////////////////////////////////////////
//
//    RECOMPRESSIMAGESDIALOG.H
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


#ifndef RECOMPRESSIMAGESDIALOG_H
#define RECOMPRESSIMAGESDIALOG_H

// Local includes

#include "batchprocessimagesdialog.h"

class BatchProcessImagesItem;

class RecompressImagesDialog : public BatchProcessImagesDialog
{
Q_OBJECT

 public:
   RecompressImagesDialog(QWidget *parent=0, QStringList filesList=0);
   ~RecompressImagesDialog();

 private slots:
   void slotAbout(void);
   void slotOptionsClicked(void);

 protected:
   int     m_JPEGCompression;
   int     m_PNGCompression;
   bool    m_compressLossLess;
   QString m_TIFFCompressionAlgo;
   QString m_TGACompressionAlgo;

   QString makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                       Digikam::AlbumInfo *albumDest);
                       
   void readSettings(void);
   void saveSettings(void);

   bool prepareStartProcess(BatchProcessImagesItem *item,
                            Digikam::AlbumInfo *albumDest);
};

#endif  // RECOMPRESSIMAGESDIALOG_H
