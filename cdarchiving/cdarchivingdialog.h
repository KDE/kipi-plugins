/* ============================================================
 * File  : cdarchivingdialog.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-05
 * Description : CD archiving dialog
 *
 * Copyright 2003-2004 by Gregory Kokanosky <gregory dot kokanosky at free.fr>
 * for images navigation mode patchs.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
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

class KFilePreview;
class QString;
class QCheckBox;
class QLineEdit;
class QSpinBox;
class QListView;

class KFileItem;
class KSqueezedTextLabel;
class KURLRequester;
class KURL;
class KIntNumInput;
class KColorButton;

namespace KIPICDArchivingPlugin
{

class CDArchivingDialog : public KDialogBase
{
 Q_OBJECT

 public:
  CDArchivingDialog( KIPI::Interface* interface, QWidget *parent=0);
  ~CDArchivingDialog();

  const QString getK3bBinPathName() const;
  void  setK3bBinPathName(QString Value);

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

  QValueList<KIPI::ImageCollection> getAlbumsSelection(void);
  void ShowMediaCapacity(void);

 protected slots:
  void albumSelected( QListViewItem * item );
  void mediaFormatActived( const QString& item );
  void slotOk();
  void slotbuttonSelectAll(void);
  void slotbuttonInvertSelection(void);
  void slotbuttonSelectNone(void);
  void UrlChanged(const QString &url );
  void slotGotPreview(const KFileItem*, const QPixmap &pixmap);

 private:
  QCheckBox          *m_useHTMLInterface;
  QCheckBox          *m_useAutoRunWin32;
  QCheckBox          *m_burnOnTheFly;
  QCheckBox          *m_checkCDBurn;
  QCheckBox          *m_startBurningProcess;

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

  QListView          *m_AlbumsList;

  QFrame             *page_setupSelection;
  QFrame             *page_setupLook;
  QFrame             *page_CDInfos;
  QFrame             *page_burning;
  QFrame             *page_about;

  QString             m_ImagesFilesSort;
  QString             m_TempFolder;

  KIO::filesize_t     MaxMediaSize;

  KIPI::Interface    *m_interface;

 private:
  void setupSelection(void);
  void setupLookPage(void);
  void setupCDInfos(void);
  void setupBurning(void);
  void aboutPage(void);
  void setAlbumsList(void);
};

}  // NameSpace KIPICDArchivingPlugin

#endif /* CDARCHIVINGDIALOG_H */
