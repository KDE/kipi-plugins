/* ============================================================
 * File  : imagesgallery.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-05
 * Description : Images gallery HTML export
 *
 * Adapted and improved for DigikamPlugins from the konqueror plugin
 * 'kdeaddons/konq-plugins/kimgalleryplugin/' by Gilles Caulier.
 *
 * Copyright 2001, 2003 by Lukas Tinkl <lukas at kde.org> and
 * Andreas Schlapbach <schlpbch at iam.unibe.ch> for orginal source
 * of 'kimgalleryplugin' from KDE CVS
 *
 * Copyright 2003-2004 by Gilles Caulier <caulier dot gilles at free.fr> for
 * DigikamPlugins port.
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

#ifndef IMAGESGALLERY_H
#define IMAGESGALLERY_H

// Include files for Qt

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>

// KIPI includes

#include <libkipi/interface.h>

#define NAV_THUMB_MAX_SIZE 64

class QProgressDialog;

class KURL;
class KConfig;
class KProcess;
class KIGPDialog;
class ResizeImage;

typedef QMap<KURL,QString> CommentMap;

class ImagesGallery : public QObject
{
Q_OBJECT

public:
  ImagesGallery( KIPI::Interface* interface );
  ~ImagesGallery();

protected slots:
  void slotCancelled();

private:
  KConfig            *m_config;
  KProcess           *m_webBrowserProc;
  KIPI::Interface    *m_interface;

  bool                m_cancelled;
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

  QProgressDialog    *m_progressDlg;
  KIGPDialog         *m_configDlg;
  CommentMap         *m_commentMap;
  ResizeImage        *m_threadedImageResizing;

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

  bool createThumb( const KURL& url, const QString& imgName,
                    const QString& imgGalleryDir, const QString& imageFormat,
                    const QString& TargetimagesFormat);

  bool createHtml( const KURL& url,
                   const QString& imageFormat, const QString& TargetimagesFormat);

  void createBodyMainPage(QTextStream& stream, KURL& url);
  void loadComments(void);
  static QString extension(const QString& imageFormat);
  void invokeWebBrowser(KURL url);
  void Activate();
  void writeSettings(void);
  void readSettings(void);

  QString EscapeSgmlText(const QTextCodec* codec, const QString& strIn,
                         const bool quot = false, const bool apos = false );
};


#endif // IMAGESGALLERY_H
