//////////////////////////////////////////////////////////////////////////////
//
//    SENDIMAGESDIALOG.H
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


#ifndef SENDIMAGESDIALOG_H
#define SENDIMAGESDIALOG_H

// Include files for Qt

#include <qstring.h>
#include <qguardedptr.h>

// Include files for KDE

#include <kdialogbase.h>
#include <klistbox.h>

// Include files for Digikam

#include <digikam/thumbnailjob.h>

class QComboBox;
class QGroupBox;
class QProgressDialog;
class QLabel;
class QCheckBox;
class QTimer;
class QFrame;
class QPushButton;
class QFileInfo;

class KConfig;
class KProcess;
class KIntNumInput;
class KListBox;
class KSqueezedTextLabel;

class ListImageItems : public KListBox
{
Q_OBJECT

public:
    ListImageItems(QWidget *parent=0, const char *name=0);

signals:
    void addedDropItems(QStringList filesPath);

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
};


class SendImagesDialog : public KDialogBase
{
Q_OBJECT

public:
   SendImagesDialog(QWidget *parent = 0, QString TmpPath = 0, QStringList imagesfileList = 0);
   ~SendImagesDialog();

   KIntNumInput *m_imageCompression;

   QComboBox    *m_imagesFormat;
   QComboBox    *m_imagesResize;
   QComboBox    *m_mailAgentName;

   QLabel       *m_labelImageFormat;
   QLabel       *m_labelImageSize;
   QLabel       *m_mailAgentLabel;

   QCheckBox    *m_addComments;
   QCheckBox    *m_changeImagesProp;

public slots:
   void slotAddDropItems(QStringList filesPath);

private slots:
   void slotOk();
   void slotCancelled();
   void slotMozillaTimeout(void);
   void slotMozillaExited(KProcess* proc);
   void slotMozillaReadStderr(KProcess* proc, char *buffer, int buflen);
   void slotImageSelected( QListBoxItem * item );
   void slotGotPreview(const KURL &url, const QPixmap &pixmap);
   void slotImagesFilesButtonAdd(void);
   void slotImagesFilesButtonRem(void);

protected:
   KConfig            *m_config;

   QPushButton        *m_addImageButton;
   QPushButton        *m_remImagesButton;

   bool                m_cancelled;

   QProgressDialog    *m_progressDlg;

   QGroupBox          *m_groupBoxImageList;

   QStringList         m_imagesSendList;

   KProcess           *m_mailAgentProc;
   KProcess           *m_mailAgentProc2;
   KProcess           *m_mailAgentProc3;

   ListImageItems     *m_ImagesFilesListBox;

   QTimer             *m_mozillaTimer;

   QString             m_mozillaStdErr;
   QString             m_tempPath;
   QString             m_ImagesFilesSort;

   QLabel             *m_imageLabel;

   QFrame             *page_setupImagesList;
   QFrame             *page_setupEmailOptions;
   QFrame             *page_about;

   KSqueezedTextLabel *m_ImageComments;
   KSqueezedTextLabel *m_ImageAlbum;

   QGuardedPtr<Digikam::ThumbnailJob> m_thumbJob;

   QString extension(const QString& imageFileFormat);
   void removeTmpFiles(void);
   bool DeleteDir(QString dirname);
   bool deldir(QString dirname);
   int  getSize ( int choice );
   bool ResizeImage( const QString &SourcePath, const QString &DestPath, const QString &ImageFormat,
                     const QString &ImageName, int SizeFactor, int ImageCompression);

 private:
   void setupImagesList(void);
   void setupEmailOptions(void);
   void aboutPage(void);
   void setImagesList(QStringList Files);
   void writeSettings(void);
   void readSettings(void);
   void setNbItems(void);
};

#endif  // SENDIMAGESDIALOG_H
