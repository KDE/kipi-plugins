//////////////////////////////////////////////////////////////////////////////
//
//    CDARCHIVING.H
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at gmail dot com>
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
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef CDARCHIVING_H
#define CDARCHIVING_H

// C Ansi includes.

extern "C"
{
#include <unistd.h>
}

// Qt includes

#include <qcolor.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

// KDE includes

#include <kaction.h>
#include <kurl.h>

// KIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

class KProcess;

namespace KIPICDArchivingPlugin
{

class CDArchivingDialog;

const int NAV_THUMB_MAX_SIZE = 64;

// First field is the URL, represented with KURL::prettyURL. We can't use KURL
// directly because operator<(KURL,KURL) is not defined in KDE 3.1

class CDArchiving : public QObject
{
    Q_OBJECT

public:

    CDArchiving( KIPI::Interface* interface, QObject *parent=0,
                 KAction *action_cdarchiving=0 );
    ~CDArchiving();

    bool prepare(void);
    bool showDialog();

    void invokeK3b();
    void removeTmpFiles(void);

    void run(void);
    void stop(void);


public slots:

    void slotK3bDone(KProcess*);
    void slotK3bStartBurningProcess(void);

private:

    CDArchivingDialog  *m_configDlg;

    KAction            *m_actionCDArchiving;

    KIPI::Interface    *m_interface;

    KProcess           *m_Proc;

    KURL                m_albumUrl;     // Current album Url use in the thread.
    KURL::List          m_albumUrlList; // Urls of Albums list from setup dialog.

    QColor              m_backgroundColor;
    QColor              m_bordersImagesColor;
    QColor              m_foregroundColor;

    QObject            *m_parent;

    QString             m_AlbumCollection;
    QString             m_AlbumComments;
    QString             m_AlbumDate;
    QString             m_AlbumTitle;
    QString             m_HTMLInterfaceAutoRunFolder;
    QString             m_HTMLInterfaceAutoRunInf;
    QString             m_HTMLInterfaceFolder;
    QString             m_HTMLInterfaceIndex;
    QString             m_K3bBinPathName;
    QString             m_K3bParameters;
    QString             m_StreamMainPageAlbumPreview;
    QString             m_applicationID;
    QString             m_bordersImagesSize;
    QString             m_fontName;
    QString             m_fontSize;
    QString             m_hostName;
    QString             m_hostURL;
    QString             m_imageFormat;
    QString             m_imagesFileFilter;
    QString             m_mainTitle;
    QString             m_mediaFormat;
    QString             m_preparer;
    QString             m_publisher;
    QString             m_systemID;
    QString             m_tmpFolder;
    QString             m_volumeID;
    QString             m_volumeSetID;

    QStringList        m_collection_name_list;

    QValueList<KIPI::ImageCollection> m_albumsList;

    bool                m_cancelled;
    bool                m_copyFiles;
    bool                m_useAutoRunWin32;
    bool                m_useCheckCD;
    bool                m_useHTMLInterface;
    bool                m_useOnTheFly;
    bool                m_useStartBurningProcess;

    int                 m_albumListSize;
    int                 m_imagesPerRow;
    int                 m_imgHeight;
    int                 m_imgWidth;
    int                 m_targetImgHeight;
    int                 m_targetImgWidth;
    int                 m_thumbnailsSize;

    pid_t               m_k3bPid;

private:

    bool buildHTMLInterface (void);

    bool createDirectory(QDir thumb_dir, QString imgGalleryDir, QString dirName);

    void createHead(QTextStream& stream);
    void createCSSSection(QTextStream& stream);

    void createBody(QTextStream& stream,
                    const KIPI::ImageCollection& album,
                    const KURL& targetURL,
                    const QString& imageFormat);

    int  createThumb( const QString& imgName, const QString& sourceDirName, const QString& uniqueFileName,
                      const QString& imgGalleryDir, const QString& imageFormat);

    int  ResizeImage( const QString Path, const QString Directory, const QString ImageFormat,
                      const QString ImageNameFormat, int *Width, int *Height, int SizeFactor,
                      bool ColorDepthChange, int ColorDepthValue, bool CompressionSet, int ImageCompression);

    bool createHtml( const KIPI::ImageCollection& album,
                     const KURL& targetURL,
                     const QString& imageFormat );

    bool createPage(const QString& imgGalleryDir,
                    const KURL& imgURL,
                    const QString& uniqueImgName,
                    const KURL& prevImgURL,
                    const QString& prevUniqueImgName,
                    const KURL& nextImgURL,
                    const QString& nextUniqueImgName,
                    const QString& comment);

    void createBodyMainPage(QTextStream& stream, KURL& url);

    static QString extension(const QString& imageFormat);

    bool BuildK3bXMLprojectfile (QString HTMLinterfaceFolder, QString IndexHtm,
                                 QString AutoRunInf, QString AutorunFolder);

    bool AddFolderTreeToK3bXMLProjectFile (QString dirname, QTextStream* stream);
    bool addCollectionToK3bXMLProjectFile(const KIPI::ImageCollection& collection, QTextStream* stream);
    bool CreateAutoRunInfFile(void);
    bool DeleteDir(QString dirname);
    bool deldir(QString dirname);

    QString EscapeSgmlText(const QTextCodec* codec, const QString& strIn,
                           const bool quot = false, const bool apos = false );

    void writeSettings(void);
    void readSettings(void);

    QString webifyFileName(QString fileName);
    QString makeFileNameUnique(QStringList& list, QString fileName);

};

}  // NameSpace KIPICDArchivingPlugin

#endif // CDARCHIVING_H
