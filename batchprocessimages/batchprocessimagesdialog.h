//////////////////////////////////////////////////////////////////////////////
//
//    BATCHPROCESSIMAGESDIALOG.H
//
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
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

// This implementation is a tempate for all batch process images dialog derived implementations class
// who use ImageMagick tools.

#ifndef BATCHPROCESSIMAGESDIALOG_H
#define BATCHPROCESSIMAGESDIALOG_H

#define OVERWRITE_ASK    0
#define OVERWRITE_RENAME 1
#define OVERWRITE_SKIP   2
#define OVERWRITE_OVER   3

#define NO_PROCESS       0
#define UNDER_PROCESS    1
#define PROCESS_DONE     2
#define STOP_PROCESS     3


// Include files for Qt

#include <qstring.h>
#include <qpushbutton.h>
#include <qguardedptr.h>

// Include files for KDE

#include <kio/job.h>
#include <kdialogbase.h>
#include <kdebug.h>
#include <kurl.h>

// Include files for KIPI

#include <libkipi/interface.h>

class KFileItem;
namespace KIPI
{
    class UploadWidget;
}

// Local includes

#include "batchprocessimagesitem.h"
#include "batchprocessimageslist.h"

class QLabel;
class QComboBox;
class QCheckBox;
class QFileInfo;
class QPushButton;
class QListViewItemIterator;
class QListViewItem;
class QGroupBox;

class KProcess;
class KConfig;
class KProgress;
class KURL;

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesDialog : public KDialogBase
{
Q_OBJECT

 public:
   // Don't forget to add the 'm_Type' and 'm_labelType' implementation in the constructor of
   // children dialog class.

   BatchProcessImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent=0 );
   ~BatchProcessImagesDialog();

protected slots:
    void slotProcessStop(void);

private slots:

   // --------------------------------------------------------------------------------------------------------
   // Standards slots

   void closeEvent ( QCloseEvent *e );
   void slotOk( void );

   void slotProcessStart(void);
   void slotProcessDone(KProcess* proc);
   void slotReadStd(KProcess* proc, char *buffer, int buflen);

   void slotPreview(void);
   void slotPreviewProcessDone(KProcess* proc);
   void slotPreviewReadStd(KProcess* proc, char *buffer, int buflen);
   void slotPreviewStop( void );

   void slotListDoubleClicked(QListViewItem *itemClicked);

   void slotAddDropItems(QStringList filesPath);

   void slotImageSelected( QListViewItem * item );
   void slotGotPreview(const KFileItem* , const QPixmap &pixmap);

   void slotImagesFilesButtonAdd( void );
   void slotImagesFilesButtonRem( void );

   // --------------------------------------------------------------------------------------------------------
   // Standards virtuals slots for re-implementation

   virtual void slotAbout(void){};                          // Called when 'About' button is clicked.
   virtual void slotOptionsClicked(void){};                 // Called when 'Options' button is clicked.
   virtual void slotTypeChanged(const QString &/*string*/){};   // Called when the current type option is changed.

 protected:

   // --------------------------------------------------------------------------------------------------------
   // Standards widgets in the dialog

   QLabel                 *m_labelOverWrite;
   QLabel                 *m_labelType;
   QLabel                 *m_statusbar;
   QLabel                 *m_imageLabel;

   QComboBox              *m_overWriteMode;
   KIPI::UploadWidget     *m_upload;
   QComboBox              *m_Type;

   BatchProcessImagesList *m_listFiles;
   KProgress              *m_progress;

   QCheckBox              *m_removeOriginal;
   QCheckBox              *m_smallPreview;

   QGroupBox              *groupBox1;
   QGroupBox              *groupBox2;
   QGroupBox              *groupBox3;
   QGroupBox              *groupBox4;
   QGroupBox              *groupBox41;

   QPushButton            *m_optionsButton;
   QPushButton            *m_previewButton;
   QPushButton            *m_addNewAlbumButton;
   QPushButton            *m_addImagesButton;
   QPushButton            *m_remImagesButton;

   KConfig                *m_config;

   int                     m_convertStatus;
   int                     m_progressStatus;
   int                     m_nbItem;

   KProcess               *m_ProcessusProc;
   KProcess               *m_PreviewProc;

   QListViewItemIterator  *m_listFile2Process_iterator;

   QString                 whatsThis;
   QString                 m_ImagesFilesSort;
   QString                 m_newDir;
   QString                 m_commandLine;
   QString                 m_tmpFolder;
   QString                 m_previewOutput;
   KURL::List              m_selectedImageFiles;

   KIPI::Interface       *m_interface;

   // --------------------------------------------------------------------------------------------------------
   // Standards fonctions

   // Fonctions for ImageMAgick call implementations.

   void    processAborted(bool removeFlag=false); // Called when ImageMagick process is stopped by user.
   void    endPreview(void);                      // Called when preview process is done or stopped by user.

   // Called for to parse the files overwriting mode selected by user.

   int     overwriteMode(void);

   // Called for rename the target current image file name if already exist.

   QString RenameTargetImageFile(QFileInfo *fi);

   // Extract the arguments from a KProcess an merge that in a QString. Used by makeProcess for to
   // show the command line arguments (debugging).

   QString extractArguments(KProcess *proc);

   // --------------------------------------------------------------------------------------------------------
   // Standards virtuals fonctions for re-implementation.

   // Called when ImageMagick process is started or for another re-implementation without
   // ImageMagick calls (like 'RenameImages' plugin).

   virtual bool startProcess(void);

   // Called when ImageMagick process is done or stopped by user or for another re-implementation without
   // ImageMagick calls (like 'RenameImages' plugin).

   virtual void endProcess(QString endMessage);

   //  Fonctions for ImageMagick implementation calls only.

   // Called by 'startProcess' fonction at start. You can checking here some things about the current
   // 'item' before the 'startprocess' fonction execution.

    virtual bool prepareStartProcess(BatchProcessImagesItem */*item*/, const QString& /*albumDest*/) { return true; };

   // Called for to contruct the ImageMagick command line used for to process or preview the image traitements.
   // If 'albumDest' = 0L ==> preview process.

    virtual QString makeProcess(KProcess* /*proc*/, BatchProcessImagesItem */*item*/,
                                const QString& /*albumDest*/ = QString::null )
           { return QString::null; };

   // Fonctions used for to read and to save the settings in the configuration file.

   virtual void readSettings(void){};
   virtual void saveSettings(void){};

   // Called for add or update the list of images to process in the dialog.

   virtual void listImageFiles(void);

   // Called by 'listImageFiles' for renamed if necessary the target image file name.

   virtual QString oldFileName2NewFileName(QString fileName)
           { return (fileName); };
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // BATCHPROCESSIMAGESDIALOG_H
