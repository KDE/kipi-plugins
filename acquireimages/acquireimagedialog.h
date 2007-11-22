/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : Acquire image dialog
 *
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

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
#include <kurl.h>

// Include files for KIPI

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// Local includes

#include "kpaboutdata.h"

class QPushButton;
class QLineEdit;
class QLabel;
class QTextEdit;
class QComboBox;
class QFrame;
class QPushButton;

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
 
   void slotHelp();
   void slotOk();
   void slotAlbumSelected( const KURL &url );
   void slotImageFormatChanged(const QString &string);

private:
 
   KIPI::Interface    *m_interface;
   QImage              m_qimageScanned;

   QPushButton        *m_addNewAlbumButton;
   QPushButton        *m_helpButton;

   KConfig            *m_config;

   QString             m_newDir;
   QString             m_ImagesFilesSort;

   KIPI::UploadWidget* m_uploadPath;

   KIPIPlugins::KPAboutData *m_about; 

   KIntNumInput       *m_imageCompression;

   QComboBox          *m_imagesFormat;

   QLineEdit          *m_FileName;

   QTextEdit          *m_CommentsEdit;

   QLabel             *m_ImageComments;
   QLabel             *m_imageLabel;
   QLabel             *m_labelImageFormat;
   QLabel             *m_ImageFileName;
   QLabel             *m_preview;

   QFrame             *page_setupImageOptions;
   QFrame             *page_setupAlbumsList;

   KSqueezedTextLabel *m_AlbumComments;
   KSqueezedTextLabel *m_AlbumCollection;
   KSqueezedTextLabel *m_AlbumDate;
   KSqueezedTextLabel *m_AlbumItems;

private:
 
   QString extension(const QString& imageFormat);
   bool QImageToTiff(const QImage& image, const QString& dst);
   void setupImageOptions(void);
   void setupAlbumsList(void);
   void writeSettings(void);
   void readSettings(void);
};

}  // NameSpace KIPIAcquireImagesPlugin

#endif  // ACQUIREIMAGEDIALOG_H
