//////////////////////////////////////////////////////////////////////////////
//
//    CDARCHIVING.H
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at free.fr>
//    Copyright (C) 2003-2004 by Gregory Kokanosky <gregory dot kokanosky at free.fr>
//    for images navigation mode.
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

#ifndef CDARCHIVING_H
#define CDARCHIVING_H

// C Ansi includes.

extern "C"
{
#include <unistd.h>
}

// Include files for Qt

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qcolor.h>
#include <qdatetime.h>

// Include files for KDE

#include <kaction.h>
#include <kurl.h>

// KIPI includes

#include <libkipi/interface.h>

class QTimer;

class KConfig;
class KProcess;

namespace KIPICDArchivingPlugin
{

class CDArchivingDialog;

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

class CDArchiving : public QObject
{
Q_OBJECT

public:

  CDArchiving( KIPI::Interface* interface, QObject *parent=0,
               KAction *action_cdarchiving=0 );
  ~CDArchiving();

  bool prepare(void);
  void run(void);
  void stop(void);
  
  bool showDialog();
  void invokeK3b();
  void removeTmpFiles(void);

public slots:

  void slotK3bDone(KProcess*);
  void slotK3bStartBurningProcess(void);

private:
                           
  KIPI::Interface    *m_interface;

  KConfig            *m_config;
  CDArchivingDialog  *m_configDlg;
  
  QString             m_hostName;
  QString             m_hostURL;
  
  KAction            *m_actionCDArchiving;
  KProcess           *m_Proc;
  QTimer             *m_K3bTimer;
  pid_t               m_k3bPid;

  bool                m_cancelled;
  bool                m_useHTMLInterface;
  bool                m_useAutoRunWin32;
  bool                m_useStartBurningProcess;
  bool                m_recurseSubDirectories;
  bool                m_copyFiles;
  bool                m_useCommentFile;
  bool                m_useOnTheFly;
  bool                m_useCheckCD;
  
  QString             m_K3bBinPathName;
  QString             m_AlbumTitle;
  QString             m_AlbumComments;
  QString             m_AlbumCollection;
  QString             m_AlbumDate;
  QString             m_StreamMainPageAlbumPreview;
  QString             m_imagesFileFilter;
  QString             m_imageFormat;
  QString             m_HTMLInterfaceFolder;
  QString             m_HTMLInterfaceIndex;
  QString             m_HTMLInterfaceAutoRunInf;
  QString             m_HTMLInterfaceAutoRunFolder;
  QString             m_tmpFolder;
  QString             m_mainTitle;
  QString             m_fontName;
  QString             m_fontSize;
  QString             m_bordersImagesSize;
  QString             m_mediaFormat;
  QString             m_volumeID;
  QString             m_volumeSetID;
  QString             m_systemID;
  QString             m_applicationID;
  QString             m_publisher;
  QString             m_preparer;
  
  QColor              m_backgroundColor;
  QColor              m_foregroundColor;
  QColor              m_bordersImagesColor;
  
  int                 m_imgWidth;
  int                 m_imgHeight;
  int                 m_imagesPerRow;
  int                 m_LevelRecursion;
  int                 m_targetImgWidth;
  int                 m_targetImgHeight;
  int                 m_thumbnailsSize;
  int                 m_albumListSize;
  
  KURL::List          m_albumUrlList; // Urls of Albums list from setup dialog.
  KURL                m_albumUrl;     // Current album Url use in the thread.
  
  CommentMap         *m_commentMap;
  AlbumsMap          *m_albumsMap;
  
  QObject            *m_parent;
  
private:
  
  bool buildHTMLInterface (void);

  bool createDirectory(QDir thumb_dir, QString imgGalleryDir, QString dirName);

  void createHead(QTextStream& stream);
  void createCSSSection(QTextStream& stream);

  void createBody(QTextStream& stream, const QString& sourceDirName,
                  const QStringList& subDirList, const QDir& imageDir,
                  const KURL& url, const QString& imageFormat);

  int  createThumb( const QString& imgName, const QString& sourceDirName,
                    const QString& imgGalleryDir, const QString& imageFormat);

  int  ResizeImage( const QString Path, const QString Directory, const QString ImageFormat,
                    const QString ImageNameFormat, int *Width, int *Height, int SizeFactor,
                    bool ColorDepthChange, int ColorDepthValue, bool CompressionSet, int ImageCompression);

  bool createHtml( const KURL& url, const QString& sourceDirName, int recursionLevel,
                   const QString& imageFormat);

  bool createPage(const QString& imgGalleryDir , const QString& imgName, const QString& previousImgName,
                  const QString& nextImgName, const QString& comment, 
                  const QString& AlbumTitle, const QString& sourceDirName);

  void createBodyMainPage(QTextStream& stream, KURL& url);
  void loadComments(void);
  static QString extension(const QString& imageFormat);

  bool BuildK3bXMLprojectfile (QString HTMLinterfaceFolder, QString IndexHtm,
                               QString AutoRunInf, QString AutorunFolder);

  bool AddFolderTreeToK3bXMLProjectFile (QString dirname, QTextStream* stream);
  bool CreateAutoRunInfFile(void);
  bool DeleteDir(QString dirname);
  bool deldir(QString dirname);

  QString EscapeSgmlText(const QTextCodec* codec, const QString& strIn,
                         const bool quot = false, const bool apos = false );

  void writeSettings(void);
  void readSettings(void);
};

}  // NameSpace KIPICDArchivingPlugin

#endif // CDARCHIVING_H
