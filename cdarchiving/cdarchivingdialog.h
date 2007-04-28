/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at gmail dot com>
 *         from digiKam project.
 * Date  : 2003-10-01
 * Description : a kipi plugin to export image collections
 *               on CD/DVD.
 * 
 * Copyright 2003-2005 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright 2003-2004 by Gregory Kokanosky 
 *                        <gregory dot kokanosky at free.fr>
 *                        for HTML interface navigation mode
 * Copyright 2005      by Owen Hirst <n8rider at sbcglobal.net>
 *                        about bugfix.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef CDARCHIVINGDIALOG_H
#define CDARCHIVINGDIALOG_H

// Include files for Qt

#include <qguardedptr.h>

// Include files for KDE

#include <kdialogbase.h>
#include <kio/global.h>

// Include files for KIPI

#include <libkipi/interface.h>
#include "kpaboutdata.h"

class QString;
class QCheckBox;
class QLineEdit;
class QSpinBox;
class QProgressDialog;
class QPushButton;

class KFileItem;
class KSqueezedTextLabel;
class KURLRequester;
class KURL;
class KIntNumInput;
class KColorButton;
class KListView;
class KFilePreview;

namespace KIPI
{
    class ImageCollectionSelector;
}

namespace KIPICDArchivingPlugin
{

class CDArchivingDialog : public KDialogBase
{
Q_OBJECT

public:
 
    CDArchivingDialog( KIPI::Interface* interface, QWidget *parent=0);
    ~CDArchivingDialog();
  
    QString getK3bBinPathName() const;
    QString getK3bParameters() const;
    
    void  setK3bBinPathName(const QString &Value);
    void  setK3bParameters(const QString &Value);
  
    int   getImagesPerRow() const;
    void  setImagesPerRow(int Value);
  
    int   getThumbnailsSize() const;
    void  setThumbnailsSize(int Value);
  
    const QString getFontName() const;
    void  setFontName(QString Value);
  
    const QString getFontSize() const;
    void  setFontSize(int Value);
  
    const QString getBordersImagesSize() const;
    void  setBordersImagesSize(int Value);
  
    const QColor getBackgroundColor() const;
    void  setBackgroundColor(QColor Value);
  
    const QColor getForegroundColor() const;
    void  setForegroundColor(QColor Value);
  
    const QColor getBordersImagesColor() const;
    void  setBordersImagesColor(QColor Value);
  
    const QString getImageFormat() const;
    void  setImageFormat(QString Value);
  
    const QString getMainTitle() const;
    void  setMainTitle(QString Value);
  
    bool  getUseHTMLInterface() const;
    void  setUseHTMLInterface(bool Value);
  
    bool  getUseAutoRunWin32() const;
    void  setUseAutoRunWin32(bool Value);
  
    const QString getVolumeID() const;
    void  setVolumeID(QString Value);
  
    const QString getVolumeSetID() const;
    void  setVolumeSetID(QString Value);
  
    const QString getSystemID() const;
    void  setSystemID(QString Value);
  
    const QString getApplicationID() const;
    void  setApplicationID(QString Value);
  
    const QString getPublisher() const;
    void  setPublisher(QString Value);
  
    const QString getPreparer() const;
    void  setPreparer(QString Value);
  
    const QString getMediaFormat() const;
    void  setMediaFormat(QString Value);
  
    bool  getUseOnTheFly() const;
    void  setUseUseOnTheFly(bool Value);
  
    bool  getUseCheckCD() const;
    void  setUseCheckCD(bool Value);
  
    bool  getUseStartBurningProcess() const;
    void  setUseStartBurningProcess(bool Value);
  
    QValueList<KIPI::ImageCollection> getSelectedAlbums() const { return m_selectedAlbums; }
    
    void ShowMediaCapacity(void);

protected slots:
  
    void slotHelp();
    void slotAlbumSelected(void);
    void mediaFormatActived( const QString& item );
    void slotOk();
    void UrlChanged(const QString &url );
  
private:
 
    QPushButton        *m_helpButton;
      
    QCheckBox          *m_useHTMLInterface;
    QCheckBox          *m_useAutoRunWin32;
    QCheckBox          *m_burnOnTheFly;
    QCheckBox          *m_checkCDBurn;
    QCheckBox          *m_startBurningProcess;
  
    QProgressDialog    *m_progressDlg;
    
    QSpinBox           *m_fontSize;
    QSpinBox           *m_bordersImagesSize;
  
    QComboBox          *m_fontName;
    QComboBox          *m_imageFormat;
    QComboBox          *m_mediaFormat;
  
    KIntNumInput       *m_imagesPerRow;
    KIntNumInput       *m_thumbnailsSize;
  
    KColorButton       *m_foregroundColor;
    KColorButton       *m_backgroundColor;
    KColorButton       *m_bordersImagesColor;
  
    QLabel             *m_albumPreview;
    QLabel             *m_labelTitle;
    QLabel             *m_labelThumbsFileFormat;
    QLabel             *m_labelFontName;
    QLabel             *m_labelFontSize;
    QLabel             *m_labelForegroundColor;
    QLabel             *m_labelBackgroundColor;
    QLabel             *m_labelImageBorderSize;
    QLabel             *m_labelImageBorderSizeColor;
  
    KSqueezedTextLabel *m_AlbumComments;
    KSqueezedTextLabel *m_AlbumCollection;
    KSqueezedTextLabel *m_AlbumDate;
    KSqueezedTextLabel *m_AlbumSize;
    KSqueezedTextLabel *m_AlbumItems;
    QLabel             *m_mediaSize;
  
    QLineEdit          *m_title;
    QLineEdit          *m_volume_id;
    QLineEdit          *m_volume_set_id;
    QLineEdit          *m_system_id;
    QLineEdit          *m_application_id;
    QLineEdit          *m_publisher;
    QLineEdit          *m_preparer;
  
    KURLRequester      *m_K3bBinPath;
    QLineEdit          *m_K3bParameters;
  
    KListView          *m_AlbumsList;
  
    QFrame             *page_setupSelection;
    QFrame             *page_setupLook;
    QFrame             *page_CDInfos;
    QFrame             *page_burning;
  
    QString             m_ImagesFilesSort;
    QString             m_TempFolder;
  
    KIO::filesize_t     MaxMediaSize;
  
    KIPI::Interface    *m_interface;
  
    QValueList<KIPI::ImageCollection>  m_selectedAlbums;
    KIPI::ImageCollectionSelector     *m_imageCollectionSelector;

    KIPIPlugins::KPAboutData *m_about;

private:
 
    void setupSelection(void);
    void setupLookPage(void);
    void setupCDInfos(void);
    void setupBurning(void);
};

}  // NameSpace KIPICDArchivingPlugin

#endif /* CDARCHIVINGDIALOG_H */
