//////////////////////////////////////////////////////////////////////////////
//
//    ACQUIREIMAGEDIALOG.H
//
//    Copyright (C) 2003 Gilles CAULIER <caulier.gilles at free.fr>
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


#ifndef ACQUIREIMAGEDIALOG_H
#define ACQUIREIMAGEDIALOG_H

// Include files for Qt

#include <qimage.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qguardedptr.h>

// Include files for KDE

#include <kdialogbase.h>
#include <kio/job.h>

// Include files for KIPI

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

class QPushButton;
class QLineEdit;
class QLabel;
class QTextEdit;
class QComboBox;
class QFrame;

class KListBox;
class KConfig;
class KIntNumInput;
class KSqueezedTextLabel;

namespace KIPIAcquireImagesPlugin
{

class AcquireImageDialog : public KDialogBase
{
Q_OBJECT

 public:
   AcquireImageDialog( KIPI::Interface* interface, QWidget *parent=0, const QImage &img=0);
   ~AcquireImageDialog();

 private slots:
   void slotOk();

// // PENDING(blackie) see implementation of these methods to see why this was removed.
//    void albumSelected( QListBoxItem * item );
//   void slotGotPreview(const KURL &url, const QPixmap &pixmap);
    void slotImageFormatChanged(const QString &string);

 protected:
   KIPI::Interface    *m_interface;
   QImage              m_qimageScanned;

   QPushButton        *m_addNewAlbumButton;

   KConfig            *m_config;

   QString             m_newDir;
   QString             m_ImagesFilesSort;

   KIPI::UploadWidget* m_uploadPath;
    //KListBox           *m_AlbumList;

   KIntNumInput       *m_imageCompression;

   QComboBox          *m_imagesFormat;

   QLineEdit          *m_FileName;

   QTextEdit          *m_CommentsEdit;

   QLabel             *m_ImageComments;
   QLabel             *m_imageLabel;
   QLabel             *m_labelImageFormat;
   QLabel             *m_ImageFileName;
   QLabel             *m_preview;
   QLabel             *m_albumPreview;

   QFrame             *page_setupImageOptions;
   QFrame             *page_setupAlbumsList;
   QFrame             *page_about;

   KSqueezedTextLabel *m_AlbumComments;
   KSqueezedTextLabel *m_AlbumCollection;
   KSqueezedTextLabel *m_AlbumDate;
   KSqueezedTextLabel *m_AlbumItems;

   bool                m_dialogOk;

   QString extension(const QString& imageFormat);
   bool QImageToTiff(const QImage& image, const QString& dst);

 private:
   void setupImageOptions(void);
   void setupAlbumsList(void);
   void aboutPage(void);
   void writeSettings(void);
   void readSettings(void);
};

}  // NameSpace KIPIAcquireImagesPlugin

#endif  // ACQUIREIMAGEDIALOG_H
