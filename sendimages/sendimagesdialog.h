//////////////////////////////////////////////////////////////////////////////
//
//    SENDIMAGESDIALOG.H
//
//    Copyright (C) 2003 Gilles Caulier <caulier dot gilles at free.fr>
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
#include <kio/previewjob.h>
#include <kurl.h>

// Include files for KIPI

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>
#include <libkipi/imageinfo.h>

class QComboBox;
class QGroupBox;
class QLabel;
class QCheckBox;
class QFrame;
class QPushButton;
class QFileInfo;

class KFileItem;
class KConfig;
class KIntNumInput;
class KListBox;
class KSqueezedTextLabel;

namespace KIPISendimagesPlugin
{

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
   SendImagesDialog(QWidget *parent, 
                    KIPI::Interface* interface, 
                    const KIPI::ImageCollection& images );
   ~SendImagesDialog();

   KIntNumInput   *m_imageCompression;

   QComboBox      *m_imagesFormat;
   QComboBox      *m_imagesResize;
   QComboBox      *m_mailAgentName;

   QCheckBox      *m_addComments;
   QCheckBox      *m_changeImagesProp;
     
   KURL::List      m_images2send;
   
public slots:
   void slotAddDropItems(QStringList filesPath);

private slots:
   void slotOk();
   void slotImageSelected( QListBoxItem * item );
   void slotGotPreview(const KFileItem* , const QPixmap &pixmap);
   void slotFailedPreview(const KFileItem*);
   void slotImagesFilesButtonAdd(void);
   void slotImagesFilesButtonRem(void);

protected:
   QLabel             *m_labelImageFormat;
   QLabel             *m_labelImageSize;
   QLabel             *m_mailAgentLabel;
      
   KConfig            *m_config;

   QPushButton        *m_addImageButton;
   QPushButton        *m_remImagesButton;

   ListImageItems     *m_ImagesFilesListBox;
   
   bool                m_cancelled;

   QProgressDialog    *m_progressDlg;

   QGroupBox          *m_groupBoxImageList;

   QLabel             *m_imageLabel;

   QFrame             *page_setupImagesList;
   QFrame             *page_setupEmailOptions;
   QFrame             *page_about;

   KSqueezedTextLabel *m_ImageComments;
   KSqueezedTextLabel *m_ImageAlbum;
   KIPI::Interface    *m_interface;
   KIO::PreviewJob    *m_thumbJob;

 private:
   void setupImagesList(void);
   void setupEmailOptions(void);
   void aboutPage(void);
   void setImagesList( const KURL::List& images );
   void writeSettings(void);
   void readSettings(void);
   void setNbItems(void);
};

}  // NameSpace KIPISendimagesPlugin

#endif  // SENDIMAGESDIALOG_H
