//////////////////////////////////////////////////////////////////////////////
//
//    BORDERIMAGESDIALOG.H
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


#ifndef BORDERIMAGESDIALOG_H
#define BORDERIMAGESDIALOG_H

// Local includes

#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class BorderImagesDialog : public BatchProcessImagesDialog
{
Q_OBJECT

 public:
   BorderImagesDialog( KURL::List images, KIPI::Interface* interface, QWidget *parent=0 );
   ~BorderImagesDialog();

 private slots:
   void slotAbout(void);
   void slotOptionsClicked(void);

 protected:
   int                    m_solidWidth;
   QColor                 m_solidColor;

   int                    m_lineNiepceWidth;
   QColor                 m_lineNiepceColor;
   int                    m_NiepceWidth;
   QColor                 m_NiepceColor;

   int                    m_raiseWidth;

   int                    m_frameWidth;
   int                    m_bevelWidth;
   QColor                 m_frameColor;

    QString makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                       const QString& albumDest);

   void readSettings(void);
   void saveSettings(void);
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // BORDERIMAGESDIALOG_H

