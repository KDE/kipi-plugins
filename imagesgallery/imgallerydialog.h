//////////////////////////////////////////////////////////////////////////////
//
//    IMGGALLERYDIALOG.H
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at free.fr>
//    Copyright (C) 2003-2004 by Gregory Kokanosky <gregory dot kokanosky at free.fr>
//    for images navigation mode.
//
//    Adapted and improved for DigikamPlugins from the konqueror plugin
//    'kdeaddons/konq-plugins/kimgalleryplugin/' by Gilles Caulier.
//
//    Copyright 2001, 2003 by Lukas Tinkl <lukas at kde.org> and
//    Andreas Schlapbach <schlpbch at iam.unibe.ch> for orginal source
//    of 'kimgalleryplugin' from KDE CVS
//
//    Copyright 2003-2004 by Gilles Caulier <caulier dot gilles at free.fr> for
//    DigikamPlugins port.
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

#ifndef IMGALLERYDIALOG_H
#define IMGALLERYDIALOG_H

// Include files for Qt

#include <qguardedptr.h>

// Include files for KDE

#include <kdialogbase.h>

// Include files for KIPI

#include <libkipi/interface.h>

class QProgressDialog;
class QCheckBox;
class QString;
class QLineEdit;
class QSpinBox;

class KFileItem;
class KSqueezedTextLabel;
class KURL;
class KIntNumInput;
class KColorButton;
class KURLRequester;
class KListView;

namespace KIPIImagesGalleryPlugin
{

class AlbumItem;

class KIGPDialog : public KDialogBase
{
 Q_OBJECT

 public:
  KIGPDialog( KIPI::Interface* interface, QWidget *parent=0);
  ~KIGPDialog();

  bool  setAlbumsList(void);

  const QString getImageName() const;
  void  setImageName(QString Value);

  bool  printImageName() const;
  void  setPrintImageName(bool Value);

  bool  printImageSize() const;
  void  setPrintImageSize(bool Value);

  bool  printImageProperty() const;
  void  setPrintImageProperty(bool Value);

  bool  OpenGalleryInWebBrowser() const;
  void  setOpenGalleryInWebBrowser(bool Value);

  const QString getWebBrowserName() const;
  void  setWebBrowserName(QString Value);

  bool  getCreatePageForPhotos() const;
  void  setCreatePageForPhotos(bool Value);

  bool  useCommentFile() const;
  void  setUseCommentFile(bool Value);

  bool  useCommentsAlbum() const;
  void  setUseCommentsAlbum(bool Value);

  bool  useCollectionAlbum() const;
  void  setUseCollectionAlbum(bool Value);

  bool  useDateAlbum() const;
  void  setUseDateAlbum(bool Value);

  bool  useNbImagesAlbum() const;
  void  setUseNbImagesAlbum(bool Value);

  int   getImagesPerRow() const;
  void  setImagesPerRow(int Value);

  int   getThumbnailsSize() const;
  void  setThumbnailsSize(int Value);

  bool  colorDepthSetThumbnails() const;
  void  setColorDepthSetThumbnails(bool Value);

  int   getColorDepthThumbnails() const;
  void  setColorDepthThumbnails(QString Value);

  bool  colorDepthSetTargetImages() const;
  void  setColorDepthSetTargetImages(bool Value);

  int   getColorDepthTargetImages() const;
  void  setColorDepthTargetImages(QString Value);

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

  int   getImagesResize() const;
  void  setImagesResizeFormat(int Value);

  const QString getTargetImagesFormat() const;
  void  setTargetImagesFormat(QString Value);

  bool  useNotOriginalImageSize() const;
  void  setNotUseOriginalImageSize(bool Value);

  bool  useSpecificTargetimageCompression() const;
  void  setUseSpecificTargetimageCompression(bool Value);

  int   getTargetImagesCompression() const;
  void  setTargetImagesCompression(int Value);

  bool  useSpecificThumbsCompression() const;
  void  setUseSpecificThumbsCompression(bool Value);

  int   getThumbsCompression() const;
  void  setThumbsCompression(int Value);

  bool  printPageCreationDate() const;
  void  setPrintPageCreationDate(bool Value);

  QValueList<KIPI::ImageCollection> getSelectedAlbums() const;

 protected slots:
  void GalleryUrlChanged(const QString & );
  void albumSelected( QListViewItem * item );
  void slotOk();
  void slotbuttonSelectAll(void);
  void slotbuttonInvertSelection(void);
  void slotbuttonSelectNone(void);
  void slotGotPreview(const KFileItem*, const QPixmap &pixmap);
  void slotStopParsingAlbums(void);

 private:
  KColorButton       *m_foregroundColor;
  KColorButton       *m_backgroundColor;
  KColorButton       *m_bordersImagesColor;

  QLineEdit          *m_title;

  KIntNumInput       *m_imagesPerRow;
  KIntNumInput       *m_thumbnailsSize;
  KIntNumInput       *m_imagesresize;
  KIntNumInput       *m_Targetimagescompression;
  KIntNumInput       *m_thumbscompression;

  QSpinBox           *m_fontSize;
  QSpinBox           *m_bordersImagesSize;

  QLabel             *m_albumPreview;
  
  QProgressDialog    *m_progressDlg;
  
  QCheckBox          *m_imageName;
  QCheckBox          *m_imageSize;
  QCheckBox          *m_imageProperty;
  QCheckBox          *m_useCommentFile;
  QCheckBox          *m_useCommentsAlbum;
  QCheckBox          *m_useCollectionAlbum;
  QCheckBox          *m_useDateAlbum;
  QCheckBox          *m_useNbImagesAlbum;
  QCheckBox          *m_colorDepthSetThumbnails;
  QCheckBox          *m_colorDepthSetTargetImages;
  QCheckBox          *m_useNotOriginalImageSize;
  QCheckBox          *m_PageCreationDate;
  QCheckBox          *m_openInWebBrowser;
  QCheckBox          *m_createPageForPhotos;
  QCheckBox          *m_useSpecificTargetimageCompression;
  QCheckBox          *m_useSpecificThumbsCompression;

  QComboBox          *m_fontName;
  QComboBox          *m_imageFormat;
  QComboBox          *m_colorDepthThumbnails;
  QComboBox          *m_colorDepthTargetImages;
  QComboBox          *m_targetimagesFormat;
  QComboBox          *m_webBrowserName;

  KURLRequester      *m_imageNameReq;

  KListView          *m_AlbumsList;

  KSqueezedTextLabel *m_AlbumComments;
  KSqueezedTextLabel *m_AlbumCollection;
  KSqueezedTextLabel *m_AlbumDate;
  KSqueezedTextLabel *m_AlbumItems;

  QFrame             *page_setupSelection;
  QFrame             *page_setupLook;
  QFrame             *page_setupAlbum;
  QFrame             *page_setupThumbnail;
  QFrame             *page_about;

  KIPI::Interface*    m_interface;
  
  QMap<AlbumItem*, KIPI::ImageCollection> m_albums;

  bool                m_stopParsingAlbum;
  
 private:
  void setupSelection(void);
  void setupLookPage(void);
  void setupAlbumPage(void);
  void setupThumbnailPage(void);
  void aboutPage(void);
};

}  // NameSpace KIPIImagesGalleryPlugin

#endif /* IMGALLERYDIALOG_H */

