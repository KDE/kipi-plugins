//////////////////////////////////////////////////////////////////////////////
//
//    RENAMEIMAGESDIALOG.H
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


#ifndef RENAMEIMAGESDIALOG_H
#define RENAMEIMAGESDIALOG_H

#define SORT_BY_NAME 0
#define SORT_BY_SIZE 1
#define SORT_BY_DATE 3

// Include files for Qt

#include <qdatetime.h>

// Local includes

#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;
class RenameImagesOptionsDialog;

class RenameImagesDialog : public BatchProcessImagesDialog
{
Q_OBJECT

 public:
   RenameImagesDialog( KURL::List images, KIPI::Interface* interface, QWidget *parent=0);
   ~RenameImagesDialog();

 private slots:
   void slotAbout(void);
   void slotOptionsClicked(void);

   void slotResult( KIO::Job *job );
   void slotTemplateSortListChanged(const QString &text);
   void slotTemplateSortListChanged(int index);
   void slotTemplateSortListChanged(bool on);

 protected:
   bool    m_addOriginalFileName;
   bool    m_addImageFileDate;
   bool    m_addImageFileSize;
   bool    m_reverseOrder;
   QString m_prefix;
   int     m_sortType;
   int     m_enumeratorStart;
   bool    m_dateChange;

   QDate   m_newDate;
   bool    m_changeModification;
   bool    m_changeAccess;
   int     m_hour;
   int     m_minute;
   int     m_second;

   RenameImagesOptionsDialog *optionsDialog;

   void    listImageFiles(void);

   bool    startProcess(void);

   void    readSettings(void);
   void    saveSettings(void);

   QString oldFileName2NewFileName(QFileInfo *fi, int id);
   void    copyItemOperations(void);
   void    updateOptions(void);
   bool    changeDate(QString file);
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // RENAMEIMAGESDIALOG_H
