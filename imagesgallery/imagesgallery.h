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
#include <qcolor.h>
#include <qdatetime.h>

// Includes files for KDE.

#include <kurl.h>

// KIPI includes

#include <libkipi/interface.h>

class KProcess;

namespace KIPIImagesGalleryPlugin
{

class KIGPDialog;

class AlbumData
    {
    public:
        
        AlbumData(){}
        AlbumData( const QString& albumName,     const QString& albumCategory,
                   const QString& albumComments, const QDate& albumDate,
                   const KURL&    albumUrl,      const KURL::List& itemsUrl )
                 : m_albumName(albumName), m_albumCategory(albumCategory),
                   m_albumComments(albumComments), m_albumDate(albumDate),
                   m_albumUrl(albumUrl), m_itemsUrl(itemsUrl)
        {}

        QString    albumName()     const { return m_albumName;        }
        QString    albumCategory() const { return m_albumCategory;    }
        QString    albumComments() const { return m_albumComments;    }
        QDate      albumDate()     const { return m_albumDate;        }
        KURL       albumUrl()      const { return m_albumUrl;         }        
        KURL::List itemsUrl()      const { return m_itemsUrl;         }   
        int        countItems()          { return m_itemsUrl.count(); }   
        
    private:
        
        QString    m_albumName;
        QString    m_albumCategory;
        QString    m_albumComments;
        QDate      m_albumDate;
        KURL       m_albumUrl;
        KURL::List m_itemsUrl;
    };


const int NAV_THUMB_MAX_SIZE = 64;

// First field is the URL, represented with KURL::prettyURL. We can't use KURL
// directly because operator<(KURL,KURL) is not defined in KDE 3.1

typedef QMap<QString, QString>   CommentMap;  // List of Albums items comments.
typedef QMap<QString, AlbumData> AlbumsMap;   // Albums data list.


class ImagesGallery : public QObject
{
Q_OBJECT

public:

  ImagesGallery( KIPI::Interface* interface, QObject *parent=0 );
  ~ImagesGallery();
  
  bool prepare(void);
  void run(void);
  void stop(void);
  
  bool showDialog(void);
  void invokeWebBrowser(void);
  bool removeTargetGalleryFolder(void);
  
private:

  KURL                m_url4browser;

  KProcess           *m_webBrowserProc;
  KIPI::Interface    *m_interface;

  bool                m_cancelled;
  bool                m_recurseSubDirectories;
  bool                m_copyFiles;
  bool                m_useCommentFile;
  bool                m_useCommentsAlbum;
  bool                m_useCollectionAlbum;
  bool                m_useDateAlbum;
  bool                m_useNbImagesAlbum;
  bool                m_createPageForPhotos;
  bool                m_printImageName;
  bool                m_printImageProperty;
  bool                m_printImageSize;
  bool                m_printPageCreationDate;
  bool                m_useNotOriginalImageSize;
  bool                m_useSpecificThumbsCompression;
  bool                m_useSpecificTargetimageCompression;

  QString             m_hostName;
  QString             m_hostURL;
  QString             m_AlbumTitle;
  QString             m_AlbumComments;
  QString             m_AlbumCollection;
  QString             m_AlbumDate;
  QString             m_StreamMainPageAlbumPreview;
  QString             m_imagesFileFilter;
  QString             m_imageName;                       
  QString             m_mainTPath;
  QString             m_imageFormat;
  QString             m_targetImagesFormat;
  QString             m_mainTitle;
  QString             m_fontName;
  QString             m_fontSize;
  QString             m_bordersImagesSize;
  
  QColor              m_backgroundColor;
  QColor              m_foregroundColor;
  QColor              m_bordersImagesColor;
  
  QStringList         m_resizeImagesWithError;

  int                 m_imgWidth;
  int                 m_imgHeight;
  int                 m_imagesPerRow;
  int                 m_LevelRecursion;
  int                 m_targetImgWidth;
  int                 m_targetImgHeight;
  int                 m_imagesResize;
  int                 m_colorDepthSetTargetImages;
  int                 m_colorDepthTargetImages;
  int                 m_targetImagesCompression;
  int                 m_thumbnailsSize;
  int                 m_colorDepthSetThumbnails;
  int                 m_colorDepthThumbnails;
  int                 m_thumbsCompression;
  int                 m_albumListSize;
  
  KURL::List          m_albumUrlList; // Urls of Albums list from setup dialog.
  KURL                m_albumUrl;     // Current album Url use in the thread.
  
  CommentMap         *m_commentMap;
  AlbumsMap          *m_albumsMap;
  
  QObject            *m_parent;

  QDir                m_targetDir;    // Target directory from setup dialog.
  
  KIPIImagesGalleryPlugin::KIGPDialog  *m_configDlg;
  

private:
    
  bool createDirectory(QDir thumb_dir, QString imgGalleryDir, QString dirName);

  void createHead(QTextStream& stream);
  void createCSSSection(QTextStream& stream);

  void createBody(QTextStream& stream, const QStringList& subDirList,
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

  bool createHtml( const KURL& url, const QString& sourceDirName, int recursionLevel, 
                   const QString& imageFormat, const QString& TargetimagesFormat);

  void createBodyMainPage(QTextStream& stream, KURL& url);
  void loadComments(void);
  
  bool DeleteDir(QString dirname);
  bool deldir(QString dirname);

  void writeSettings(void);
  void readSettings(void);

  QString EscapeSgmlText(const QTextCodec* codec, const QString& strIn,
                         const bool quot = false, const bool apos = false );
};

}  // NameSpace KIPIImagesGalleryPlugin

#endif // IMAGESGALLERY_H
