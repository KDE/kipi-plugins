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
#include <qdir.h>
#include <qthread.h>

// Include files for KDE

#include <kaction.h>

// KIPI includes

#include <libkipi/interface.h>


class QTimer;

class KURL;
class KConfig;
class KProcess;

namespace KIPICDArchivingPlugin
{

const int NAV_THUMB_MAX_SIZE = 64;

class KIGPDialog;
class CDArchivingDialog;

typedef QMap<QString,QString> CommentMap;

class CDArchiving : public QObject, public QThread
{
Q_OBJECT

public:
  CDArchiving( KIPI::Interface* interface, QObject *parent=0, KAction *action_cdarchiving=0);
  virtual ~CDArchiving();

  virtual void run();

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

  bool                m_recurseSubDirectories;
  bool                m_copyFiles;
  bool                m_useCommentFile;

  QString             m_AlbumTitle;
  QString             m_AlbumComments;
  QString             m_AlbumCollection;
  QString             m_AlbumDate;
  QString             m_StreamMainPageAlbumPreview;
  QString             m_imagesFileFilter;

  QString             m_HTMLInterfaceFolder;
  QString             m_HTMLInterfaceIndex;
  QString             m_HTMLInterfaceAutoRunInf;
  QString             m_HTMLInterfaceAutoRunFolder;
  QString             m_tmpFolder;

  int                 m_imgWidth;
  int                 m_imgHeight;
  int                 m_imagesPerRow;
  int                 m_LevelRecursion;

  int                 m_targetImgWidth;
  int                 m_targetImgHeight;

  CommentMap         *m_commentMap;

  QObject            *m_parent;
  
  bool buildHTMLInterface (void);

  bool createDirectory(QDir thumb_dir, QString imgGalleryDir, QString dirName);

  void createHead(QTextStream& stream);
  void createCSSSection(QTextStream& stream);

  void createBody(QTextStream& stream, const QString& sourceDirName,
                  const QStringList& subDirList, const QDir& imageDir,
                  const KURL& url, const QString& imageFormat);

  bool createThumb( const QString& imgName, const QString& sourceDirName,
                    const QString& imgGalleryDir, const QString& imageFormat);

  bool ResizeImage( const QString Path, const QString Directory, const QString ImageFormat,
                    const QString ImageNameFormat, int *Width, int *Height, int SizeFactor,
                    bool ColorDepthChange, int ColorDepthValue, bool CompressionSet, int ImageCompression);

  bool createHtml( const KURL& url, const QString& sourceDirName, int recursionLevel,
                   const QString& imageFormat);

  bool createPage(const QString& imgGalleryDir , const QString& imgName, const QString& previousImgName,
                  const QString& nextImgName, const QString& comment, const QString& imageFormat,
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
