//////////////////////////////////////////////////////////////////////////////
//
//    IMAGESGALLERY.H
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

#ifndef IMAGESGALLERY_H
#define IMAGESGALLERY_H

// Include files for Qt

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qthread.h>

// Includes files for KDE.

#include <kurl.h>

// KIPI includes

#include <libkipi/interface.h>

class KURL;
class KConfig;
class KProcess;

namespace KIPIImagesGalleryPlugin
{

class KIGPDialog;

const int NAV_THUMB_MAX_SIZE = 64;

// First field is the URL, represented with KURL::prettyURL. We can't use KURL
// directly because operator<(KURL,KURL) is not defined in KDE 3.1

typedef QMap<QString,QString> CommentMap;

class ImagesGallery : public QObject, public QThread
{
Q_OBJECT

public:
  ImagesGallery( KIPI::Interface* interface, QObject *parent=0 );
  ~ImagesGallery();
  
  virtual void run();

  bool showDialog();
  void invokeWebBrowser(void);
  bool removeTargetGalleryFolder(void);
  
private:
  KURL                m_url4browser;

  KConfig            *m_config;
  KProcess           *m_webBrowserProc;
  KIPI::Interface    *m_interface;
  
  QString             m_hostName;
  QString             m_hostURL;
  
  bool                m_recurseSubDirectories;
  bool                m_copyFiles;
  bool                m_useCommentFile;

  QString             m_AlbumTitle;
  QString             m_AlbumComments;
  QString             m_AlbumCollection;
  QString             m_AlbumDate;
  QString             m_StreamMainPageAlbumPreview;
  QString             m_imagesFileFilter;
  QStringList         m_resizeImagesWithError;

  int                 m_imgWidth;
  int                 m_imgHeight;
  int                 m_imagesPerRow;
  int                 m_LevelRecursion;

  int                 m_targetImgWidth;
  int                 m_targetImgHeight;

  CommentMap         *m_commentMap;

  QObject            *m_parent;
    
  KIPIImagesGalleryPlugin::KIGPDialog  *m_configDlg;

  KIPI::ImageCollection m_album;

  bool createDirectory(QDir thumb_dir, QString imgGalleryDir, QString dirName);

  void createHead(QTextStream& stream);
  void createCSSSection(QTextStream& stream);

  void createBody(QTextStream& stream,
                  const KURL& url, const QString& imageFormat,
                  const QString& TargetimagesFormat);

  bool createPage( const QString& imgGalleryDir, const QString& imgName,
                   const QString& previousImgName , const QString& nextImgName,
                   const QString& comment);

  int  createThumb( const KURL& url, const QString& imgName,
                    const QString& imgGalleryDir, const QString& imageFormat,
                    const QString& TargetimagesFormat);
                    
  int  ResizeImage( const QString Path, const QString Directory, const QString ImageFormat,
                    const QString ImageNameFormat, int *Width, int *Height, int SizeFactor,
                    bool ColorDepthChange, int ColorDepthValue, bool CompressionSet,
                    int ImageCompression);                    

  bool createHtml( const KURL& url,
                   const QString& imageFormat, const QString& TargetimagesFormat);

  void createBodyMainPage(QTextStream& stream, KURL& url);
  void loadComments(void);
  static QString extension(const QString& imageFormat);
  
  bool DeleteDir(QString dirname);
  bool deldir(QString dirname);

  void writeSettings(void);
  void readSettings(void);

  QString EscapeSgmlText(const QTextCodec* codec, const QString& strIn,
                         const bool quot = false, const bool apos = false );
};

}  // NameSpace KIPIImagesGalleryPlugin

#endif // IMAGESGALLERY_H
