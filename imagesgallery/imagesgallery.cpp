//////////////////////////////////////////////////////////////////////////////
//
//    IMAGESGALLERY.CPP
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

extern "C"
{
#include <unistd.h>
}

// Include files for Qt

#include <qtextstream.h>
#include <qfile.h>
#include <qfont.h>
#include <qimage.h>
#include <qtextcodec.h>

// Include files for KDE

#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/global.h>
#include <kmessagebox.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcharsets.h>
#include <kapplication.h>
#include <kprocess.h>
#include <kimageio.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>
#include <kaboutdata.h>

// KIPI includes

#include <libkipi/imagecollection.h>

// Local includes

#include "actions.h"
#include "imgallerydialog.h"
#include "exifrestorer.h"
#include "imagesgallery.h"

namespace KIPIImagesGalleryPlugin
{

ImagesGallery::ImagesGallery( KIPI::Interface* interface, QObject *parent )
             : QObject(parent)
{
    KImageIO::registerFormats();
    
    const KAboutData *data = KApplication::kApplication()->aboutData();
    m_hostName = QString::QString( data->appName() );
       
    m_hostURL = data->homepage();
    
    if (m_hostURL.isEmpty())
       {
       m_hostName = "Kipi";
       m_hostURL = "http://extragear.kde.org/apps/kipi.php";
       }

    m_interface = interface;
    m_parent = parent;
    m_commentMap = 0L;
    m_albumsMap = 0L;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

ImagesGallery::~ImagesGallery()
{
    if (m_commentMap) delete m_commentMap;
    if (m_albumsMap) delete m_albumsMap;
    delete m_configDlg;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::writeSettings(void)
{
    m_config = new KConfig("kipirc");
    m_config->setGroup("ImagesGallery Settings");

    // HTML Look dialogbox setup tab

    m_config->writeEntry("MainPageTitle", m_configDlg->getMainTitle());
    m_config->writeEntry("ImagesPerRow", m_configDlg->getImagesPerRow());
    m_config->writeEntry("PrintImageName", m_configDlg->printImageName());
    m_config->writeEntry("PrintImageSize", m_configDlg->printImageSize());
    m_config->writeEntry("PrintFileSize", m_configDlg->printImageProperty());
    m_config->writeEntry("PrintPageCreationDate", m_configDlg->printPageCreationDate());
    m_config->writeEntry("CreatePageForPhotos", m_configDlg->getCreatePageForPhotos());
    m_config->writeEntry("OpenInWebBrowser", m_configDlg->OpenGalleryInWebBrowser());
    m_config->writePathEntry("WebBrowserName", m_configDlg->getWebBrowserName());
    m_config->writeEntry("FontName", m_configDlg->getFontName());
    m_config->writeEntry("FontColor", m_configDlg->getForegroundColor());
    m_config->writeEntry("BackgroundColor", m_configDlg->getBackgroundColor());
    m_config->writeEntry("BordersImagesSize", m_configDlg->getBordersImagesSize());
    m_config->writeEntry("BordersImagesColor", m_configDlg->getBordersImagesColor());

    // ALBUM dialogbox setup tab

    m_config->writePathEntry("GalleryPath", m_configDlg->getImageName());
    m_config->writeEntry("NotUseOriginalImageSize", m_configDlg->useNotOriginalImageSize());
    m_config->writeEntry("ImagesResize", m_configDlg->getImagesResize());
    m_config->writeEntry("TargetImagesCompressionSet", m_configDlg->useSpecificTargetimageCompression());
    m_config->writeEntry("TargetImagesCompression", m_configDlg->getTargetImagesCompression());
    m_config->writeEntry("TargetImagesFormat", m_configDlg->getTargetImagesFormat());
    m_config->writeEntry("TargetImagesColorDepthSet", m_configDlg->colorDepthSetTargetImages());
    m_config->writeEntry("TargetImagesColorDepthValue", m_configDlg->getColorDepthTargetImages());
    m_config->writeEntry("UseCommentFile", m_configDlg->useCommentFile());
    m_config->writeEntry("UseCommentsAlbum", m_configDlg->useCommentsAlbum());
    m_config->writeEntry("UseCollectionAlbum", m_configDlg->useCollectionAlbum());
    m_config->writeEntry("UseDateAlbum", m_configDlg->useDateAlbum());
    m_config->writeEntry("PrintImageNb", m_configDlg->useNbImagesAlbum());

    // THUMNAILS dialogbox setup tab

    m_config->writeEntry("ThumbnailsSize", m_configDlg->getThumbnailsSize());
    m_config->writeEntry("ThumbnailsCompressionSet", m_configDlg->useSpecificThumbsCompression());
    m_config->writeEntry("ThumbnailsCompression", m_configDlg->getThumbsCompression());
    m_config->writeEntry("ThumbnailsFormat", m_configDlg->getImageFormat());
    m_config->writeEntry("ThumbnailsColorDepthSet", m_configDlg->colorDepthSetThumbnails());
    m_config->writeEntry("ThumbnailsColorDepthValue", m_configDlg->getColorDepthThumbnails());

    m_config->sync();
    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::readSettings(void)
{
  QColor* ColorFont;
  QColor* ColorBackground;
  QColor* ColorBordersImages;

  m_config = new KConfig("kipirc");
  m_config->setGroup("ImagesGallery Settings");

  // HTML Look dialogbox setup tab

  m_configDlg->setMainTitle( m_config->readEntry("MainPageTitle", i18n("KIPI Album Image Gallery")) );
  m_configDlg->setImagesPerRow( m_config->readEntry("ImagesPerRow", "4").toInt() );

  if (m_config->readEntry("PrintImageName", "true") == "true")
     m_configDlg->setPrintImageName( true );
  else
     m_configDlg->setPrintImageName( false );

  if (m_config->readEntry("PrintImageSize", "true") == "true")
     m_configDlg->setPrintImageSize( true );
  else
     m_configDlg->setPrintImageSize( false );

  if (m_config->readEntry("PrintFileSize", "true") == "true")
     m_configDlg->setPrintImageProperty( true );
  else
     m_configDlg->setPrintImageProperty( false );

  if (m_config->readEntry("PrintPageCreationDate", "true") == "true")
     m_configDlg->setPrintPageCreationDate( true );
  else
     m_configDlg->setPrintPageCreationDate( false );

  if(m_config->readEntry("CreatePageForPhotos", "true") == "true")
     m_configDlg->setCreatePageForPhotos( true );
  else
     m_configDlg->setCreatePageForPhotos( false );

  if (m_config->readEntry("OpenInWebBrowser", "true") == "true")
     m_configDlg->setOpenGalleryInWebBrowser( true );
  else
     m_configDlg->setOpenGalleryInWebBrowser( false );

  m_configDlg->setWebBrowserName( m_config->readPathEntry("WebBrowserName", "Konqueror") );

  m_configDlg->setFontName( m_config->readEntry("FontName", "Helvetica") );
  m_configDlg->setFontSize( m_config->readEntry("FontSize", "14").toInt() );
  ColorFont = new QColor( 208, 255, 208 );
  m_configDlg->setForegroundColor( m_config->readColorEntry("FontColor", ColorFont));
  ColorBackground = new QColor( 51, 51, 51 );
  m_configDlg->setBackgroundColor( m_config->readColorEntry("BackgroundColor", ColorBackground));
  m_configDlg->setBordersImagesSize( m_config->readEntry("BordersImagesSize", "1").toInt() );
  ColorBordersImages = new QColor( 208, 255, 208 );
  m_configDlg->setBordersImagesColor( m_config->readColorEntry("BordersImagesColor", ColorBordersImages));

  delete ColorFont;
  delete ColorBackground;
  delete ColorBordersImages;

  // ALBUM dialogbox setup tab

  m_configDlg->setImageName( m_config->readPathEntry("GalleryPath", KGlobalSettings::documentPath()) );

  if (m_config->readEntry("NotUseOriginalImageSize", "true") == "true")
     m_configDlg->setNotUseOriginalImageSize( true );
  else
     m_configDlg->setNotUseOriginalImageSize( false );

  m_configDlg->setImagesResizeFormat( m_config->readEntry("ImagesResize", "640").toInt() );

  if (m_config->readEntry("TargetImagesCompressionSet", "false") == "true")
     m_configDlg->setUseSpecificTargetimageCompression( true );
  else
     m_configDlg->setUseSpecificTargetimageCompression( false );

  m_configDlg->setTargetImagesCompression( m_config->readEntry("TargetImagesCompression", "75").toInt() );

  m_configDlg->setTargetImagesFormat( m_config->readEntry("TargetImagesFormat", "JPEG") );

  if (m_config->readEntry("TargetImagesColorDepthSet", "false") == "true")
     m_configDlg->setColorDepthSetTargetImages( true );
  else
     m_configDlg->setColorDepthSetTargetImages( false );

  m_configDlg->setColorDepthTargetImages( m_config->readEntry("TargetImagesColorDepthValue", "32") );

  if (m_config->readEntry("UseCommentFile", "true") == "true")
     m_configDlg->setUseCommentFile( true );
  else
     m_configDlg->setUseCommentFile( false );

  if (m_config->readEntry("UseCommentsAlbum", "true") == "true")
     m_configDlg->setUseCommentsAlbum( true );
  else
     m_configDlg->setUseCommentsAlbum( false );

  if (m_config->readEntry("UseCollectionAlbum", "true") == "true")
     m_configDlg->setUseCollectionAlbum( true );
  else
     m_configDlg->setUseCollectionAlbum( false );

  if (m_config->readEntry("UseDateAlbum", "true") == "true")
     m_configDlg->setUseDateAlbum( true );
  else
     m_configDlg->setUseDateAlbum( false );

  if (m_config->readEntry("PrintImageNb", "true") == "true")
     m_configDlg->setUseNbImagesAlbum( true );
  else
     m_configDlg->setUseNbImagesAlbum( false );

  // THUMNAILS dialogbox setup tab

  m_configDlg->setThumbnailsSize( m_config->readEntry("ThumbnailsSize", "140").toInt() );

  if (m_config->readEntry("ThumbnailsCompressionSet", "false") == "true")
     m_configDlg->setUseSpecificThumbsCompression( true );
  else
     m_configDlg->setUseSpecificThumbsCompression( false );

  m_configDlg->setThumbsCompression( m_config->readEntry("ThumbnailsCompression", "75").toInt() );

  m_configDlg->setImageFormat( m_config->readEntry("ThumbnailsFormat", "JPEG") );

  if (m_config->readEntry("ThumbnailsColorDepthSet", "false") == "true")
     m_configDlg->setColorDepthSetThumbnails( true );
  else
     m_configDlg->setColorDepthSetThumbnails( false );

  m_configDlg->setColorDepthThumbnails( m_config->readEntry("ThumbnailsColorDepthValue", "32") );

  delete m_config;
    
  // Get the image files filters from the hosts app.
     
  m_imagesFileFilter = m_interface->fileExtensions();
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool ImagesGallery::showDialog()
{
    m_configDlg = new KIGPDialog( m_interface, 0 );
    readSettings();

    if ( m_configDlg->exec() == QDialog::Accepted )
        {
        writeSettings();
        return true;
        }
       
    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool ImagesGallery::removeTargetGalleryFolder(void)
{
    QDir TargetDir;
    QString MainTPath = m_configDlg->getImageName() + "/KIPIHTMLExport";

    if (TargetDir.exists (MainTPath) == true)
       {
       if (KMessageBox::warningYesNo(0,
           i18n("The target directory\n'%1'\nalready exists; do you want overwrite it? (all data "
                "in this directory will be lost.)").arg(MainTPath)) == KMessageBox::Yes)
          {
          if ( DeleteDir(MainTPath) == false )
             {
             KMessageBox::error(0, i18n("Cannot remove folder '%1'.").arg(MainTPath));
             return false;
             }
          }
       }
       
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool ImagesGallery::prepare(void)
{
    QValueList<KIPI::ImageCollection> albumsList;  
    KIPIImagesGalleryPlugin::EventData *d;
    
    // This variable are used for the recursive sub directories parsing
    // during the HTML pages creation. (TODO: Checked this mode)
    
    m_recurseSubDirectories = false;
    m_LevelRecursion = 1;

    m_cancelled = false;
    m_resizeImagesWithError.clear();
    m_StreamMainPageAlbumPreview = "";
    
    // Get config from setup dialog.
    
    m_imagesPerRow = m_configDlg->getImagesPerRow();
    albumsList = m_configDlg->getSelectedAlbums();
    m_imageName = m_configDlg->getImageName();
    m_useCommentFile = m_configDlg->useCommentFile();
    m_imageFormat = m_configDlg->getImageFormat();
    m_targetImagesFormat = m_configDlg->getTargetImagesFormat();
    m_mainTitle = m_configDlg->getMainTitle();
    m_backgroundColor = m_configDlg->getBackgroundColor();
    m_foregroundColor = m_configDlg->getForegroundColor(); 
    m_bordersImagesColor = m_configDlg->getBordersImagesColor();
    m_fontName = m_configDlg->getFontName();    
    m_fontSize = m_configDlg->getFontSize();
    m_bordersImagesSize = m_configDlg->getBordersImagesSize();
    m_useCommentsAlbum = m_configDlg->useCommentsAlbum();
    m_useCollectionAlbum = m_configDlg->useCollectionAlbum();
    m_useDateAlbum = m_configDlg->useDateAlbum();
    m_useNbImagesAlbum = m_configDlg->useNbImagesAlbum();
    m_createPageForPhotos = m_configDlg->getCreatePageForPhotos();   
    m_printImageName = m_configDlg->printImageName();
    m_printImageProperty = m_configDlg->printImageProperty();
    m_printImageSize = m_configDlg->printImageSize();
    m_printPageCreationDate = m_configDlg->printPageCreationDate();
    m_useNotOriginalImageSize = m_configDlg->useNotOriginalImageSize();
    m_imagesResize = m_configDlg->getImagesResize();
    m_colorDepthSetTargetImages = m_configDlg->colorDepthSetTargetImages();
    m_colorDepthTargetImages = m_configDlg->getColorDepthTargetImages();    
    m_useSpecificTargetimageCompression = m_configDlg->useSpecificTargetimageCompression();
    m_targetImagesCompression = m_configDlg->getTargetImagesCompression();            
    m_thumbnailsSize = m_configDlg->getThumbnailsSize();
    m_colorDepthSetThumbnails = m_configDlg->colorDepthSetThumbnails();
    m_colorDepthThumbnails = m_configDlg->getColorDepthThumbnails();
    m_useSpecificThumbsCompression = m_configDlg->useSpecificThumbsCompression();
    m_thumbsCompression = m_configDlg->getThumbsCompression();
    m_albumListSize = albumsList.count();
                        
    // Estimate the number of actions for the KIPI progress dialog. 
    
    int nbActions = m_albumListSize;
    
    for( QValueList<KIPI::ImageCollection>::Iterator it = albumsList.begin() ;
         !m_cancelled && (it != albumsList.end()) ; ++it ) 
       nbActions = nbActions + (*it).images().count();
    
    d = new KIPIImagesGalleryPlugin::EventData;
    d->action = KIPIImagesGalleryPlugin::Initialize;
    d->starting = true;
    d->success = false;
    d->total = nbActions; 
    QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
    usleep(1000);
    
    
    // Create the main target folder.
        
    m_mainTPath = m_imageName + "/KIPIHTMLExport";

    if (m_targetDir.mkdir( m_mainTPath ) == false)
       {
       d = new KIPIImagesGalleryPlugin::EventData;
       d->action = KIPIImagesGalleryPlugin::Error;
       d->starting = false;
       d->success = false;
       d->message = i18n("Could not create directory '%1'").arg(m_mainTPath);
       QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
       usleep(1000);
       
       return(false);
       }

    // Create data maps to use in the thread.

    m_albumsMap = new AlbumsMap;
        
    for( QValueList<KIPI::ImageCollection>::Iterator albumIt = albumsList.begin() ;
         !m_cancelled && (albumIt != albumsList.end()) ; ++albumIt )
        {
        AlbumData data((*albumIt).name(),    (*albumIt).category(),
                       (*albumIt).comment(), (*albumIt).date(), 
                       (*albumIt).path(),    (*albumIt).images());
        
        m_albumsMap->insert( (*albumIt).path().prettyURL(), data );
        m_albumUrlList.append( (*albumIt).path() );
        }
    
    // Load images comments if necessary.
                          
    if ( m_useCommentFile )
       loadComments();
           
    return(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::stop()
{
    m_cancelled = true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::run()
{
    KIPIImagesGalleryPlugin::EventData *d;
    QString Path;
    KURL SubUrl, MainUrl;
       
    // Build all Albums HTML export.

    if ( m_albumListSize > 1 )
       {
       KGlobal::dirs()->addResourceType("kipi_data", KGlobal::dirs()->kde_default("data") + "kipi");
       QString dir = KGlobal::dirs()->findResourceDir("kipi_data", "gohome.png");
       dir = dir + "gohome.png";

       KURL srcURL(dir);
       KURL destURL(m_imageName + "/KIPIHTMLExport/gohome.png");
       KIO::file_copy(srcURL, destURL, -1, true, false, false);           
       }

       for( KURL::List::Iterator albumsUrlIt = m_albumUrlList.begin() ;
            !m_cancelled && (albumsUrlIt != m_albumUrlList.end()) ; ++albumsUrlIt )
          {
          m_albumUrl = *albumsUrlIt;
          AlbumData data = (*m_albumsMap)[m_albumUrl.prettyURL()];
          KURL::List images = data.itemsUrl();

          for( KURL::List::Iterator urlIt = images.begin(); urlIt != images.end(); ++urlIt ) 
             {
             kdDebug( 51000 ) << "URL:" << (*urlIt).prettyURL() << endl;
             }

          m_AlbumTitle      = data.albumName();
          m_AlbumComments   = data.albumComments();
          m_AlbumCollection = data.albumCategory();
          m_AlbumDate       = data.albumDate().toString();
          Path              = data.albumUrl().path();

          SubUrl = m_imageName + "/KIPIHTMLExport/" + m_AlbumTitle + "/" + "index.html";

          if ( !SubUrl.isEmpty() && SubUrl.isValid())
             {
             // Create the target sub folder for the current album.

             QString SubTPath= m_imageName + "/KIPIHTMLExport/" + m_AlbumTitle;
 
             if (m_targetDir.mkdir( SubTPath ) == false)
                 {
                 d = new KIPIImagesGalleryPlugin::EventData;
                 d->action = KIPIImagesGalleryPlugin::Error;
                 d->starting = false;
                 d->success = false;
                 d->message = i18n("Could not create directory '%1'").arg(SubTPath);
                 QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
                 usleep(1000);
                 
                 return;
                 }

             d = new KIPIImagesGalleryPlugin::EventData;
             d->action = KIPIImagesGalleryPlugin::BuildAlbumHTMLPage;
             d->starting = true;
             d->success = false;
             d->albumName = m_AlbumTitle;
             QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
             usleep(1000);
                         
             if ( !createHtml( SubUrl, Path, 
                               m_LevelRecursion > 0 ? m_LevelRecursion + 1 : 0,
                               m_imageFormat,
                               m_targetImagesFormat) )
                {
                d = new KIPIImagesGalleryPlugin::EventData;
                d->action = KIPIImagesGalleryPlugin::BuildAlbumHTMLPage;
                d->starting = false;
                d->success = false;
                d->albumName = m_AlbumTitle;
                QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d)); 
                usleep(1000);
                
                if ( !DeleteDir(m_mainTPath) )
                   {
                   d = new KIPIImagesGalleryPlugin::EventData;
                   d->action = KIPIImagesGalleryPlugin::Error;
                   d->starting = false;
                   d->success = false;
                   d->message = i18n("Cannot remove folder '%1'.").arg(m_mainTPath);
                   QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
                   usleep(1000);
                   
                   return;
                   }

                return;
                }
             else 
                {
                d = new KIPIImagesGalleryPlugin::EventData;
                d->action = KIPIImagesGalleryPlugin::BuildAlbumHTMLPage;
                d->starting = false;
                d->success = true;
                d->albumName = m_AlbumTitle;
                QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
                usleep(1000);
                
                }
             }
         }

    // Create the main HTML page if many Albums selected.

    d = new KIPIImagesGalleryPlugin::EventData;
    d->action = KIPIImagesGalleryPlugin::BuildHTMLiface;
    d->starting = true;
    d->success = false;
    QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
    usleep(1000);
    

    if ( m_albumListSize > 1 )
       {
       MainUrl.setPath( m_imageName + "/KIPIHTMLExport/" + "index.html" );
       QFile MainPageFile( MainUrl.path() );

       if ( MainPageFile.open(IO_WriteOnly) )
          {
          QTextStream stream(&MainPageFile);
          stream.setEncoding(QTextStream::UnicodeUTF8);
          createHead(stream);
          createBodyMainPage(stream, MainUrl);
          MainPageFile.close();
          m_url4browser = MainUrl.url();
          }
       else
          {
          d = new KIPIImagesGalleryPlugin::EventData;
          d->action = KIPIImagesGalleryPlugin::Error;
          d->starting = false;
          d->success = false;
          d->message = i18n("Couldn't open file '%1'").arg(MainUrl.path());
          QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
          usleep(1000);
          
          return;
          }
       }
    else
       {
       m_url4browser = SubUrl.url();
       }

    d = new KIPIImagesGalleryPlugin::EventData;
    d->action = KIPIImagesGalleryPlugin::BuildHTMLiface;
    d->success = true;
    d->starting = false;
    QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
    usleep(1000);
    
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool ImagesGallery::createDirectory(QDir thumb_dir, QString imgGalleryDir, QString dirName)
{
    if (!thumb_dir.exists())
        {
        thumb_dir.setPath( imgGalleryDir );

        if (!(thumb_dir.mkdir(dirName, false)))
            {
            KIPIImagesGalleryPlugin::EventData* d = new KIPIImagesGalleryPlugin::EventData;
            d->action = KIPIImagesGalleryPlugin::Error;
            d->starting = false;
            d->success = false;
            d->message = i18n("Could not create directory '%1' in '%2'")
                         .arg(dirName).arg(imgGalleryDir);
            QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
            usleep(1000);
            
            return false;
            }
        else
            {
            thumb_dir.setPath( imgGalleryDir + "/" + dirName + "/" );
            return true;
            }
        }
    else
        return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::createHead(QTextStream& stream)
{
    QString chsetName = QTextCodec::codecForLocale()->mimeName();

    stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">"
           << endl;
    stream << "<html>" << endl;
    stream << "<head>" << endl;
    stream << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">" << endl;

    stream << "<meta name=\"Generator\" content=\"Albums Images gallery generated by "
           << m_hostName << " [" << m_hostURL << "]\">"  << endl;
    stream << "<meta name=\"date\" content=\"" + KGlobal::locale()->formatDate(QDate::currentDate()) + "\">"
           << endl;
    stream << "<title>" << m_mainTitle << "</title>" << endl;
    
    createCSSSection(stream);
    
    stream << "</head>" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::createCSSSection(QTextStream& stream)
{
    QString backgroundColor = m_backgroundColor.name();
    QString foregroundColor = m_foregroundColor.name();
    QString bordersImagesColor = m_bordersImagesColor.name();

    // Adding a touch of style

    stream << "<style type='text/css'>\n";
    stream << "BODY {color: " << foregroundColor << "; background: " << backgroundColor << ";" << endl;
    stream << "          font-family: " << m_fontName << ", sans-serif;" << endl;
    stream << "          font-size: " << m_fontSize << "pt; margin: 4%; }" << endl;
    stream << "H1       {color: " << foregroundColor << ";}" << endl;
    stream << "TABLE    {text-align: center; margin-left: auto; margin-right: auto;}" << endl;
    stream << "TD       { color: " << foregroundColor << "; padding: 1em}" << endl;
    stream << "IMG.photo      { border: " << m_bordersImagesSize << "px solid "
           << bordersImagesColor << "; }" << endl;
    stream << "</style>" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString ImagesGallery::extension(const QString& imageFormat)
{
    if (imageFormat == "PNG")
        return ".png";

    if (imageFormat == "JPEG")
        return ".jpg";

    Q_ASSERT(false);
    return "";
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::createBody(QTextStream& stream, const QStringList& subDirList,
                               const KURL& url, const QString& imageFormat,
                               const QString& TargetimagesFormat)
{
    KIPIImagesGalleryPlugin::EventData *d;
    AlbumData data = (*m_albumsMap)[m_albumUrl.prettyURL()];
    int numOfImages = data.countItems();
    
    kdDebug( 51000 ) << "Num of images in " << data.albumName().ascii() << " : " 
                     << numOfImages << endl;
    
    const QString imgGalleryDir = url.directory();
    const QString today(KGlobal::locale()->formatDate(QDate::currentDate()));

    stream << "<body>"<<endl;

    // Display gohome icon

    if (QFile::exists(imgGalleryDir + QString::fromLatin1("/../gohome.png")))
       {
       stream << "<p><a href=\"../index.html\"><img src=\"../gohome.png\" border=\"0\"  title=\""
              << i18n("Album list") << "\" alt=\"" << i18n("Album list") << "\"></a></p>"
              << endl;
       }

    stream << "<h1>" << i18n("Image Gallery for Album ") << "&quot;" << m_AlbumTitle << "&quot;"
           << "</h1>" << endl;

    if (m_useCommentsAlbum == true ||
        m_useCollectionAlbum == true ||
        m_useDateAlbum == true ||
        m_useNbImagesAlbum == true)
       {
       stream << "<table width=\"100%\" border=1 cellpadding=0 cellspacing=0 "
                 "style=\"page-break-before: always\">\n" << endl;
       stream << "<col width=\"20%\"><col width=\"80%\">"<<endl;
       stream << "<tr valign=top><td align=left>\n" << endl;

       if (m_useCommentsAlbum && !m_AlbumComments.isEmpty())
           stream << i18n("<i>Comment:</i>") << "<br>\n" << endl;

       if (m_useCollectionAlbum && !m_AlbumCollection.isEmpty())
           stream << i18n("<i>Collection:</i>") << "<br>\n" << endl;

       if (m_useDateAlbum == true)
           stream << i18n("<i>Date:</i>") << "<br>\n" << endl;

       if (m_useNbImagesAlbum == true)
           stream << i18n("<i>Images:</i>") << "\n" << endl;

       stream << "</td><td align=left>\n" << endl;

       if (m_useCommentsAlbum && !m_AlbumComments.isEmpty())
           {
           stream << EscapeSgmlText(QTextCodec::codecForLocale(), m_AlbumComments, true, true)
                  << "<br>\n" << endl;
           }

       if (m_useCollectionAlbum && !m_AlbumCollection.isEmpty())
           stream << m_AlbumCollection << "<br>\n" << endl;

       if (m_useDateAlbum == true)
           stream << m_AlbumDate << "<br>\n" << endl;

       if (m_useNbImagesAlbum == true)
           stream << numOfImages << "\n" << endl;

       stream << "</td></tr></table>\n" << endl;
       
       if (m_recurseSubDirectories && subDirList.count() > 2)  
          {                                                   
          // subDirList.count() is always >= 2 because of the "." and ".." directories

          QString Temp = i18n("<i>Subdirectories:</i>");
          stream << Temp << "<br>" << endl;

          for (QStringList::ConstIterator it = subDirList.begin() ; 
               !m_cancelled && (it != subDirList.end()); it++)
              {
              if (*it == "." || *it == "..")
                  continue;                        // Disregard the "." and ".." directories

              stream << "<a href=\"" << *it << "/" << url.fileName() << "\">" << *it << "</a><br>" << endl;
              }
          }
       }
    else
       stream << "<hr>\n" << endl;

    stream << "<table>" << endl;

    // Table with images

    int imgIndex = 0;
    QFileInfo imginfo;
    QPixmap imgProp;

    KURL::List images = data.itemsUrl();
    
    for( KURL::List::Iterator urlIt = images.begin() ; !m_cancelled && (urlIt != images.end()) ; )
        {
        stream << "<tr>" << endl;

        for (int col = 0 ;
             !m_cancelled && (urlIt!=images.end()) && (col < m_imagesPerRow) ;
             ++col, ++urlIt, ++imgIndex)
            {
            const QString imgName = (*urlIt).fileName();

            const QString targetImgName = imgName + extension(TargetimagesFormat);

            QDir targetImagesDir( imgGalleryDir + QString::fromLatin1("/images/"));

            kdDebug( 51000 ) << "Creating thumbnail for " << imgName.ascii() << endl;

            stream << "<td align='center'>\n";
            
            d = new KIPIImagesGalleryPlugin::EventData;
            d->action = KIPIImagesGalleryPlugin::ResizeImages;
            d->starting = true;
            d->success = false;
            d->fileName = imgName;
            QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
            usleep(1000);
            
           
            int valRet = createThumb(*urlIt, imgName, imgGalleryDir, imageFormat, TargetimagesFormat);
            
            if ( valRet != -1 )
                {
                // user requested the creation of html pages for each photo

                if ( m_createPageForPhotos )
                   stream << "<a href=\"pages/" << targetImgName << ".html\">";
                else
                   stream << "<a href=\"images/" << targetImgName << "\">";

                const QString imgNameFormat = imgName;

                const QString imgPath("thumbs/" + imgNameFormat + extension(imageFormat));
                stream << "<img class=\"photo\" src=\"" << imgPath << "\" width=\"" << m_imgWidth << "\" ";
                stream << "height=\"" << m_imgHeight << "\" alt=\"" << imgPath;

                QString sep = "\" title=\"";

                if ( m_printImageName )
                    {
                    stream << sep << imgName;
                    sep = ", ";
                    }

                if ( m_printImageProperty )
                    {
                    imgProp.load( targetImagesDir.absFilePath(targetImgName, true) );
                    stream << sep << imgProp.width() << "&nbsp;x&nbsp;" << imgProp.height();
                    sep = ", ";
                    }

                if ( m_printImageSize)
                    {
                    imginfo.setFile( targetImagesDir, targetImgName );
                    stream << sep << (imginfo.size() / 1024) << "&nbsp;" <<  i18n("KB");
                    sep = ", ";
                    }

                if ( m_useCommentFile )
                   {
                   QString imgComment = (*m_commentMap)[(*urlIt).prettyURL()];

                   if ( !imgComment.isEmpty() )
                      {
                      stream << sep
                             << EscapeSgmlText(QTextCodec::codecForLocale(), imgComment, true, true);
                      }
                   }

                stream << "\">" << endl;

                // For each first image of current Album we add a preview in main HTML page.

                if ( imgIndex == 0)
                   {
                   QString Temp, Temp2;
                   Temp2 = "<a href=\"" + m_AlbumTitle + "/" + "index.html" + "\">";
                   m_StreamMainPageAlbumPreview.append ( Temp2 );
                   Temp2 = "<img class=\"photo\" src=\"" + m_AlbumTitle + "/" + imgPath + "\" width=\""
                           + Temp.setNum(m_imgWidth) + "\" ";
                   m_StreamMainPageAlbumPreview.append ( Temp2 );
                   Temp2 = "height=\"" + Temp.setNum(m_imgHeight) + "\" alt=\"" + imgPath + "\" ";
                   m_StreamMainPageAlbumPreview.append ( Temp2 );
                   Temp2 = "title=\"" + m_AlbumTitle + " [ " + Temp.setNum(numOfImages) + i18n(" images")
                           + " ]\"></a>\n";
                   m_StreamMainPageAlbumPreview.append ( Temp2 );
                   Temp2 = "<a href=\"" + m_AlbumTitle + "/" + "index.html" + "\">" + m_AlbumTitle + "</a>"
                           + " [ " + Temp.setNum(numOfImages) + i18n(" images") + " ]" + "<br>\n";
                   m_StreamMainPageAlbumPreview.append ( Temp2 );
                   }
                }
            
            if ( valRet == -1 || valRet == 0 )
                {
                kdDebug( 51000 ) << "Creating thumbnail for " << imgName.ascii() 
                                 << " failed !" << endl;
                
                d = new KIPIImagesGalleryPlugin::EventData;
                d->action = KIPIImagesGalleryPlugin::ResizeImages;
                d->starting = false;
                d->success = false;
                d->fileName = imgName;
                QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
                usleep(1000);
                
                }
            else
                {
                d = new KIPIImagesGalleryPlugin::EventData;
                d->action = KIPIImagesGalleryPlugin::ResizeImages;
                d->starting = false;
                d->success = true;
                d->fileName = imgName;
                QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
                usleep(1000);
                
                }
                
            stream << "</a>" << endl;

            if ( m_printImageName )
                {
                stream << "<div>" << imgName << "</div>" << endl;
                }

            if ( m_printImageProperty )
                {
                imgProp.load( targetImagesDir.absFilePath(targetImgName, true) );
                stream << "<div>" << imgProp.width() << " x " << imgProp.height() << "</div>" << endl;
                }

            if ( m_printImageSize )
                {
                imginfo.setFile( targetImagesDir, targetImgName );
                stream << "<div>(" << (imginfo.size() / 1024) << " " <<  i18n("KB") << ") "
                       << "</div>" << endl;
                }
                
            stream << "</td>" << endl;
            }

        stream << "</tr>" << endl;
        }

    // Close the HTML and page creation info if necessary.

    stream << "</table>\n<hr>\n" << endl;

    // create HTML pages if requested.

    if( m_createPageForPhotos )
      {
      KGlobal::dirs()->addResourceType("kipi_data", KGlobal::dirs()->kde_default("data") + "kipi");
      QString dir = KGlobal::dirs()->findResourceDir("kipi_data", "up.png");
      dir = dir + "up.png";

      KURL srcURL(dir);
      KURL destURL(imgGalleryDir + QString::fromLatin1("/up.png"));
      KIO::file_copy(srcURL, destURL, -1, true, false, false);

      int imgIndex = 0;
      KURL::List images = data.itemsUrl();
      
      for( KURL::List::Iterator urlIt = images.begin();
           !m_cancelled && (urlIt != images.end()) ; ++urlIt, ++imgIndex )
        {
        const QString imgName = (*urlIt).fileName();

        const QString targetImgName = imgName + extension(TargetimagesFormat);

        QString previousImgName = "";

        if ( imgIndex != 0 )
           {
           KURL::List::Iterator it = urlIt;
           --it;
           previousImgName = (*it).fileName();
           previousImgName = previousImgName + extension(TargetimagesFormat);
           }

        QString nextImgName = "" ;

        if ( imgIndex != numOfImages -1)
           {
           KURL::List::Iterator it = urlIt;
           ++it;
           nextImgName = (*it).fileName();
           nextImgName = nextImgName + extension(TargetimagesFormat);
           }

        QString imgComment = "";

        if ( m_useCommentFile )
           imgComment = (*m_commentMap)[(*urlIt).prettyURL()];
           
        d = new KIPIImagesGalleryPlugin::EventData;
        d->action = KIPIImagesGalleryPlugin::BuildImageHTMLPage;
        d->starting = true;
        d->success = false;
        d->fileName = imgName;
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
        

        if ( createPage(imgGalleryDir,  targetImgName , previousImgName , nextImgName , imgComment) == false )
           {
           d = new KIPIImagesGalleryPlugin::EventData;
           d->action = KIPIImagesGalleryPlugin::BuildImageHTMLPage;
           d->starting = false;
           d->success = false;
           d->fileName = imgName;
           QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
           usleep(1000);
           
           }
        }
      }

    if ( m_printPageCreationDate )
        {
        QString Temp;
        KGlobal::dirs()->addResourceType("kipi_data", KGlobal::dirs()->kde_default("data") + "kipi");
        QString dir = KGlobal::dirs()->findResourceDir("kipi_data", "valid-html401.png");
        dir = dir + "valid-html401.png";

        KURL srcURL(dir);
        KURL destURL(imgGalleryDir + QString::fromLatin1("/thumbs/valid-html401.png"));
        KIO::file_copy(srcURL, destURL, -1, true, false, false);
        
        stream << "<p>"  << endl;
        Temp = i18n("Valid HTML 4.01");
        stream << "<img src=\"thumbs/valid-html401.png\" alt=\"" << Temp
               << "\" height=\"31\" width=\"88\"  title=\"" << Temp <<  "\" />" << endl;
        Temp = i18n("Image gallery created with "
                    "<a href=\"%1\">%2</a> on %3").arg(m_hostURL).arg(m_hostName).arg(today);
        stream << Temp << endl;
        stream << "</p>" << endl;
        }

    stream << "</body>\n</html>\n" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::createBodyMainPage(QTextStream& stream, KURL& url)
{
    QString Temp;
    const QString today(KGlobal::locale()->formatDate(QDate::currentDate()));

    Temp = m_mainTitle;
    stream << "<body>\n<h1>" << Temp << "</h1><p>\n" << endl;

    Temp = i18n("<i>Album list:</i>");
    stream << Temp << "<br>" << endl;
    stream << "<hr>" << endl;

    stream << "<p> " << m_StreamMainPageAlbumPreview << "</p>" << endl;

    stream << "<hr>" << endl;

    if ( m_printPageCreationDate )
        {
        QString Temp;
        KGlobal::dirs()->addResourceType("kipi_data", KGlobal::dirs()->kde_default("data") + "kipi");
        QString dir = KGlobal::dirs()->findResourceDir("kipi_data", "valid-html401.png");
        dir = dir + "valid-html401.png";

        KURL srcURL(dir);
        KURL destURL(url.directory() + QString::fromLatin1("/valid-html401.png"));
        KIO::file_copy(srcURL, destURL, -1, true, false, false);

        stream << "<p>"  << endl;
        Temp = i18n("Valid HTML 4.01");
        stream << "<img src=\"valid-html401.png\" alt=\"" << Temp
               << "\" height=\"31\" width=\"88\" title=\"" << Temp <<  "\" />" << endl;

        Temp = i18n("Image gallery created with "
                    "<a href=\"%1\">%2</a> on %3").arg(m_hostURL).arg(m_hostName).arg(today);
               
        stream << Temp << endl;
        stream << "</p>" << endl;
        }

    stream << "</body>\n</html>\n" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool ImagesGallery::createHtml(const KURL& url, const QString& sourceDirName, int recursionLevel, 
                               const QString& imageFormat, const QString& TargetimagesFormat)
{
    if (m_cancelled) return false;
    
    KIPIImagesGalleryPlugin::EventData *d;
    QStringList subDirList;
    
    if (m_recurseSubDirectories && (recursionLevel >= 0))  
        {                                                  
        QDir toplevel_dir = QDir( sourceDirName );
        toplevel_dir.setFilter( QDir::Dirs | QDir::Readable | QDir::Writable );
        subDirList = toplevel_dir.entryList();

        for (QStringList::ConstIterator it = subDirList.begin() ; 
             !m_cancelled && (it != subDirList.end()) ; it++)
            {
            const QString currentDir = *it;

            if (currentDir == "." || currentDir == "..")   // Disregard the "." and ".." directories
                continue;

            QDir subDir = QDir( url.directory() + "/" + currentDir );

            if (!subDir.exists())
                {
                subDir.setPath( url.directory() );

                if (!(subDir.mkdir(currentDir, false)))
                    {
                    d = new KIPIImagesGalleryPlugin::EventData;
                    d->action = KIPIImagesGalleryPlugin::Error;
                    d->starting = false;
                    d->success = false;
                    d->message = i18n("Could not create directory '%1' in '%2'.")
                                        .arg(currentDir).arg(url.directory());
                    QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
                    usleep(1000);
                    
                    continue;
                    }
                else
                    subDir.setPath( url.directory() + "/" + currentDir );
                }

            if (!createHtml( KURL( subDir.path() + "/" + url.fileName() ),
                             sourceDirName + "/" + currentDir,
                             recursionLevel > 1 ? recursionLevel - 1 : 0,
                             imageFormat,
                             TargetimagesFormat
                             ))
               return false;
            }
       }

    const QString imgGalleryDir = url.directory();
    kdDebug( 51000 ) << "imgGalleryDir: " << imgGalleryDir << endl;

    // Create the "thumbs" subdirectory

    QDir thumb_dir( imgGalleryDir + QString::fromLatin1("/thumbs/"));

    if (createDirectory(thumb_dir, imgGalleryDir, "thumbs") == false)
        return false;

    // Create the "images" subdirectory

    QDir images_dir( imgGalleryDir + QString::fromLatin1("/images/"));

    if (createDirectory(images_dir, imgGalleryDir, "images") == false)
        return false;

    QDir pages_dir( imgGalleryDir + QString::fromLatin1("/pages/"));

    if (m_createPageForPhotos )
       {
       kdDebug( 51000 ) << "Create photos :" << m_createPageForPhotos << endl;

       if (createDirectory(pages_dir, imgGalleryDir, "pages") == false)
         return false;
       }

    // Create HTML page.

    QFile file( url.path() );
    kdDebug( 51000 ) << "url.path(): " << url.path() << ", thumb_dir: "<< thumb_dir.path()
                     << ", pagesDir: "<< pages_dir.path()<<endl;

    if ( file.open(IO_WriteOnly) )
        {
        QTextStream stream(&file);
        stream.setEncoding(QTextStream::UnicodeUTF8);
        createHead(stream);
        createBody(stream, subDirList, url, imageFormat, TargetimagesFormat);
        file.close();
        return true;
        }
    else
        {
        d = new KIPIImagesGalleryPlugin::EventData;
        d->action = KIPIImagesGalleryPlugin::Error;
        d->starting = false;
        d->success = false;
        d->message = i18n("Could not open file '%1'").arg(url.path(+1));
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
        
        return false;
        }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::loadComments(void)
{
    // We considering no default images comments for the current album.

    m_useCommentFile = false;
    m_commentMap = new CommentMap;

    QValueList<KIPI::ImageCollection> albums = m_interface->allAlbums();       

    for( QValueList<KIPI::ImageCollection>::Iterator albumIt = albums.begin() ;
         !m_cancelled && (albumIt != albums.end()) ; ++albumIt )
        {
        KURL::List images = (*albumIt).images();

        for( KURL::List::Iterator urlIt = images.begin(); urlIt != images.end(); ++urlIt )
            {
            KIPI::ImageInfo info = m_interface->info( *urlIt );                
            QString comment=info.description();
            
            if (!comment.isEmpty())
               {
               m_useCommentFile = true;
               m_commentMap->insert((*urlIt).prettyURL(), comment);
               }
            }
        }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool ImagesGallery::createPage(const QString& imgGalleryDir, const QString& imgName,
                               const QString& previousImgName, const QString& nextImgName,
                               const QString& comment)
{
    const QDir pagesDir(imgGalleryDir + QString::fromLatin1("/pages/"));
    const QDir targetImagesDir(imgGalleryDir + QString::fromLatin1("/images/"));
    const QDir thumbsDir(imgGalleryDir + QString::fromLatin1("/thumbs/"));

    // Html pages filenames

    const QString pageFilename = pagesDir.path() + QString::fromLatin1("/") + imgName
                                 + QString::fromLatin1(".html");
    const QString nextPageFilename =  nextImgName + QString::fromLatin1(".html");
    const QString previousPageFilename =  previousImgName + QString::fromLatin1(".html");

    // Thumbs filenames

    const QString previousThumb = QString::fromLatin1("../thumbs/")
                                  + previousImgName.left( previousImgName.findRev('.', -1) )
                                  + extension(m_imageFormat);

    const QString nextThumb = QString::fromLatin1("../thumbs/")
                              + nextImgName.left( nextImgName.findRev('.', -1) )
                              + extension(m_imageFormat);

    QFile file( pageFilename );

    if ( pagesDir.exists() && file.open(IO_WriteOnly) )
       {
       QTextStream stream(&file);
       stream.setEncoding(QTextStream::UnicodeUTF8);

       QString chsetName = QTextCodec::codecForLocale()->mimeName();
       stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 "
                 "Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">" << endl;
       stream << "<html>" << endl;
       stream << "<head>" << endl;
       stream << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">" << endl;

       stream << "<meta name=\"Generator\" content=\"Albums Images gallery generated by "
              << m_hostName << " [" << m_hostURL << "]\">"  << endl;
       stream << "<meta name=\"date\" content=\""
                 + KGlobal::locale()->formatDate(QDate::currentDate()) + "\">" << endl;
       stream << "<title>" << m_mainTitle << " : "<< imgName <<"</title>" << endl;
       
       createCSSSection(stream);
       
       stream << "</head>" << endl;
       stream<<"<body>"<< endl;;

       stream << "<div align=\"center\">" << endl;

       QPixmap imgProp;

       int prevW = 0;
       int prevH = 0;
       int nextW = 0;
       int nextH = 0;

       if (imgProp.load( targetImagesDir.absFilePath(previousImgName, true) ))
          {
          prevW = imgProp.width();
          prevH = imgProp.height();
          }

       if (imgProp.load( targetImagesDir.absFilePath(nextImgName, true) ))
          {
          nextW = imgProp.width();
          nextH = imgProp.height();
          }

       kdDebug( 51000 ) << previousImgName << ":"<<prevW<<"/"<<prevH << "       "
                        <<  nextImgName << ":"<<nextW<<"/"<<nextH<< endl;

       // Navigation thumbs need to be 64x64 at most

       if ( prevW < prevH )
          {
          prevH = (NAV_THUMB_MAX_SIZE  * prevH) / prevW;
          prevW = NAV_THUMB_MAX_SIZE;
          }
       else if ( prevW == prevH )
          {
          prevH = NAV_THUMB_MAX_SIZE;
          prevW = NAV_THUMB_MAX_SIZE;
          }
       else
          {
          prevW = (NAV_THUMB_MAX_SIZE  * prevW) / prevH;
          prevH = NAV_THUMB_MAX_SIZE;
          }

       if ( nextW < nextH )
          {
          nextH = (NAV_THUMB_MAX_SIZE  * nextH) / nextW;
          nextW = NAV_THUMB_MAX_SIZE;
          }
       else if ( nextW == nextH )
          {
          nextH = NAV_THUMB_MAX_SIZE ;
          nextW = NAV_THUMB_MAX_SIZE;
          }
       else
          {
          nextW = (NAV_THUMB_MAX_SIZE  * nextW) / nextH;
          nextH = NAV_THUMB_MAX_SIZE;
          }

       kdDebug( 51000 ) << previousImgName << ":"<<prevW<<"/"<<prevH << "       "
                        <<  nextImgName << ":"<<nextW<<"/"<<nextH<< endl;

       if ( !previousImgName.isEmpty() )
          {
          stream << "<a href=\"" << previousPageFilename << "\"><img class=\"photo\" src=\""
                 << previousThumb << "\" alt=\"" << i18n("Previous") <<  "\" title=\""
                 << i18n("Previous") << "\" height=\""<< prevH << "\" width=\"" << prevW
                 << "\"></a>&nbsp; | &nbsp;" << endl;
          }

       stream << "<a href=\"../index.html\"><img src=\"../up.png\" border=\"0\" title=\""
              << i18n("Album index") << "\" alt=\"" << i18n("Album index") << "\"></a>" << endl;

       if (QFile::exists(imgGalleryDir + QString::fromLatin1("/../gohome.png")))
          {
          stream << "&nbsp; | &nbsp;<a href=\"../../index.html\"><img src=\"../../gohome.png\" "
                    "border=\"0\" title=\"" << i18n("Albums list") << "\" alt=\""
                 << i18n("Albums list") << "\"></a>" <<endl;
          }

       if ( !nextImgName.isEmpty() )
          {
          stream << "&nbsp; | &nbsp;<a href=\"" << nextPageFilename << "\"><img class=\"photo\" src=\""
                 << nextThumb << "\" alt=\"" << i18n("Next") <<"\" title=\"" << i18n("Next")
                 << "\" height=\"" << nextH << "\" width=\"" << nextW << "\"></a>" << endl;
          }

       stream << "<br><hr><br>" << endl;

       // Add comment if it exists

       if ( !comment.isEmpty() )
          {
          stream << "<div align=\"center\">"
                 << EscapeSgmlText(QTextCodec::codecForLocale(), comment, true, true)
                 << "</div>" << endl;
          }

       stream <<"<br>" << endl;

       stream << "<img class=\"photo\" src=\"../images/" << imgName << "\" alt=\"" << imgName;

       // Add info about image if requested

       QString sep = "\" title=\"";

       QFileInfo imginfo;

       if ( m_printImageName )
          {
          stream << sep << imgName;
          sep = ", ";
          }

       if ( m_printImageProperty )
          {
          imgProp.load( targetImagesDir.absFilePath(imgName, true) );
          
          kdDebug( 51000 ) << targetImagesDir.path() << "/" << imgName << endl;
          
          stream << sep << imgProp.width() << "&nbsp;x&nbsp;" << imgProp.height();
          sep = ", ";
          }

       if ( m_printImageSize )
          {
          imginfo.setFile( targetImagesDir, imgName );
          stream << sep << (imginfo.size() / 1024) << "&nbsp;" <<  i18n("KB");
          }

       stream << "\"><br><br></div>" << endl;

       // Footer

       if ( m_printPageCreationDate )
          {
          stream << "<hr>" << endl;
          QString valid = i18n("Valid HTML 4.01");
          const QString today(KGlobal::locale()->formatDate(QDate::currentDate()));
          stream << "<div><img src=\"../thumbs/valid-html401.png\" alt=\"" << valid
                 << "\" height=\"31\" width=\"88\"  title=\"" << valid <<  "\" />" << endl;

          valid =  i18n("Image gallery created with "
                        "<a href=\"%1\">%2</a> on %3").arg(m_hostURL).arg(m_hostName).arg(today);
          stream << valid << "</div>" << endl;
          }

       stream << "</body></html>" << endl;
       file.close();

       return true;
       }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int ImagesGallery::createThumb( const KURL& url, const QString& imgName,
                                const QString& imgGalleryDir, const QString& imageFormat,
                                const QString& TargetimagesFormat)
{
    const QString pixPath = url.path();

    // Create the target images with resizing factor.

    const QString TargetImageNameFormat = imgName + extension(TargetimagesFormat);
    
    const QString TargetImagesbDir = imgGalleryDir + QString::fromLatin1("/images/");
    int extentTargetImages;

    if ( m_useNotOriginalImageSize == true)
        extentTargetImages = m_imagesResize;
    else
        extentTargetImages = -1;    // Use original image size.

    m_targetImgWidth = 640;         // Default resize values.
    m_targetImgHeight = 480;

    int valRet = ResizeImage(pixPath, TargetImagesbDir, TargetimagesFormat, TargetImageNameFormat,
                             &m_targetImgWidth, &m_targetImgHeight, extentTargetImages,
                             m_colorDepthSetTargetImages,
                             m_colorDepthTargetImages,
                             m_useSpecificTargetimageCompression,
                             m_targetImagesCompression);

    // Only try to write Exif if both src and destination are JPEG files.
    
    if (valRet == 1 && 
        QString(QImageIO::imageFormat(pixPath)).upper() == "JPEG" &&
        TargetimagesFormat.upper() == "JPEG")
       {
       ExifRestorer exifHolder;
       exifHolder.readFile(pixPath, ExifRestorer::ExifOnly);
       
       QString targetFile = TargetImagesbDir + TargetImageNameFormat;
       
       if (exifHolder.hasExif()) 
          {
          ExifRestorer restorer;
          restorer.readFile(targetFile, ExifRestorer::EntireImage);
          restorer.insertExifData(exifHolder.exifData());
          restorer.writeFile(targetFile);
          }
       else 
          kdWarning( 51000 ) << ("createThumb::No Exif Data Found") << endl;
       }              
                     
    // Create the thumbnails.

    const QString ImageNameFormat = imgName + extension(imageFormat);
    const QString thumbDir = imgGalleryDir + QString::fromLatin1("/thumbs/");
    int extent = m_thumbnailsSize;

    m_imgWidth = 120; // Setting the size of the images is
    m_imgHeight = 90; // required to generate faster 'loading' pages

    return (ResizeImage(pixPath, thumbDir, imageFormat, ImageNameFormat,
                        &m_imgWidth, &m_imgHeight, extent,
                        m_colorDepthSetThumbnails,
                        m_colorDepthThumbnails,
                        m_useSpecificThumbsCompression,
                        m_thumbsCompression));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Return -1 if failed, 0 if used Broken image, 1 if done !

int ImagesGallery::ResizeImage( const QString Path, const QString Directory, const QString ImageFormat,
                                const QString ImageNameFormat, int *Width, int *Height, int SizeFactor,
                                bool ColorDepthChange, int ColorDepthValue, bool CompressionSet,
                                int ImageCompression)
{
    QImage img;
    bool ValRet;
    bool usingBrokenImage = false;

    ValRet = img.load(Path);

    if ( ValRet == false )        // Cannot load the src image.
       {
       KGlobal::dirs()->addResourceType("kipi_imagebroken", KGlobal::dirs()->kde_default("data") + "kipi/data");
       QString dir = KGlobal::dirs()->findResourceDir("kipi_imagebroken", "image_broken.png");
       dir = dir + "image_broken.png";
       kdDebug ( 51000 ) << "Loading " << Path.ascii() << " failed ! Using " << dir.ascii() 
                         << " instead..." << endl;
       ValRet = img.load(dir);    // Try broken image icon...
       usingBrokenImage = true;
       }

    if ( ValRet == true )
       {
       int w = img.width();
       int h = img.height();

       if (SizeFactor != -1)      // Use original image size ?
          {
          // scale to pixie size
          // kdDebug( 51000 ) << "w: " << w << " h: " << h << endl;
          // Resizing if to big

          if( w > SizeFactor || h > SizeFactor )
              {
              if( w > h )
                  {
                  h = (int)( (double)( h * SizeFactor ) / w );
  
                  if ( h == 0 ) h = 1;

                  w = SizeFactor;
                  Q_ASSERT( h <= SizeFactor );
                  }
              else
                  {
                  w = (int)( (double)( w * SizeFactor ) / h );

                  if ( w == 0 ) w = 1;

                  h = SizeFactor;
                  Q_ASSERT( w <= SizeFactor );
                  }
 
              const QImage scaleImg(img.smoothScale( w, h ));

              if ( scaleImg.width() != w || scaleImg.height() != h )
                  {
                  kdDebug( 51000 ) << "Resizing failed. Aborting." << endl;
                  return -1;
                  }

              img = scaleImg;
              }
              
           if ( ColorDepthChange == true )
               {
               const QImage depthImg(img.convertDepth( ColorDepthValue ));
               img = depthImg;
               }
           }

       kdDebug( 51000 ) << "Saving resized image to: " << Directory + ImageFormat  << endl;

       if ( CompressionSet == true )
          {
          if ( !img.save(Directory + ImageNameFormat, ImageFormat.latin1(), ImageCompression) )
             {
             kdDebug( 51000 ) << "Saving failed with specific compression value. Aborting." << endl;
             return -1;
             }
          }
       else
          {
          if ( !img.save(Directory + ImageNameFormat, ImageFormat.latin1(), -1) )
             {
             kdDebug( 51000 ) << "Saving failed with no compression value. Aborting." << endl;
             return -1;
             }
          }

       *Width = w;
       *Height = h;
   
       if ( usingBrokenImage == true )
          return 0;
       else 
          return 1;
       }

    return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ImagesGallery::invokeWebBrowser(void)
{
    if (m_cancelled) return;

    if (m_configDlg->OpenGalleryInWebBrowser() == false)
       return;
    
    if (m_configDlg->getWebBrowserName() == "Konqueror")
       kapp->invokeBrowser(m_url4browser.url());       // Open Konqueror browser to show the main HTML page.

    if (m_configDlg->getWebBrowserName() == "Mozilla")
       {
       m_webBrowserProc = new KProcess;
       *m_webBrowserProc << "mozilla";
       *m_webBrowserProc << m_url4browser.url();

       if (m_webBrowserProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start 'mozilla' web browser.\nPlease check your installation."));
       }

    if (m_configDlg->getWebBrowserName() == "Netscape")
       {
       m_webBrowserProc = new KProcess;
       *m_webBrowserProc << "netscape";
       *m_webBrowserProc << m_url4browser.url();

       if (m_webBrowserProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start 'netscape' web browser.\nPlease check your installation."));
       }

    if (m_configDlg->getWebBrowserName() == "Opera")
       {
       m_webBrowserProc = new KProcess;
       *m_webBrowserProc << "opera";
       *m_webBrowserProc << m_url4browser.url();

       if (m_webBrowserProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start 'opera' web browser.\nPlease check your installation."));
       }

    if (m_configDlg->getWebBrowserName() == "Dillo")
       {
       m_webBrowserProc = new KProcess;
       *m_webBrowserProc << "dillo";
       *m_webBrowserProc << m_url4browser.url();

       if (m_webBrowserProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start 'dillo' web browser.\nPlease check your installation."));
       }

    if (m_configDlg->getWebBrowserName() == "Galeon")
       {
       m_webBrowserProc = new KProcess;
       *m_webBrowserProc << "galeon";
       *m_webBrowserProc << m_url4browser.url();

       if (m_webBrowserProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start 'galeon' web browser.\nPlease check your installation."));
       }

    if (m_configDlg->getWebBrowserName() == "Amaya")
       {
       m_webBrowserProc = new KProcess;
       *m_webBrowserProc << "amaya";
       *m_webBrowserProc << m_url4browser.url();

       if (m_webBrowserProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start 'amaya' web browser.\nPlease check your installation."));
       }

    if (m_configDlg->getWebBrowserName() == "Quanta")
       {
       m_webBrowserProc = new KProcess;
       *m_webBrowserProc << "quanta";
       *m_webBrowserProc << m_url4browser.url();

       if (m_webBrowserProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start 'quanta' web editor.\nPlease check your installation."));
       }

    if (m_configDlg->getWebBrowserName() == "Screem")
       {
       m_webBrowserProc = new KProcess;
       *m_webBrowserProc << "screem";
       *m_webBrowserProc << m_url4browser.url();

       if (m_webBrowserProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start 'screem' web editor.\nPlease check your installation."));
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// This code can be multithreaded (in opposite to KIO::netaccess::delete().

bool ImagesGallery::DeleteDir(QString dirname)
{
    if ( !dirname.isEmpty() )
        {
        QDir dir;

        if (dir.exists ( dirname ) == true)
           {
           if (deldir(dirname) == false)
               return false;

           if (dir.rmdir( dirname ) == false )
               return false;
           }
        else
           return false;
        }
    else
        return false;

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// This code can be multithreaded (in opposite to KIO::netaccess::delete().

bool ImagesGallery::deldir(QString dirname)
{
    QDir *dir = new QDir(dirname);
    dir->setFilter ( QDir::Dirs | QDir::Files | QDir::NoSymLinks );

    const QFileInfoList* fileinfolist = dir->entryInfoList();
    QFileInfoListIterator it(*fileinfolist);
    QFileInfo* fi;

    while( (fi = it.current() ) )
         {
         if(fi->fileName() == "." || fi->fileName() == ".." )
              {
              ++it;
              continue;
              }

         if( fi->isDir() )
              {
              if (deldir( fi->absFilePath() ) == false)
                  return false;
              if (dir->rmdir( fi->absFilePath() ) == false)
                  return false;
              }
         else
              if( fi->isFile() )
                   if (dir->remove(fi->absFilePath() ) == false)
                       return false;
         
         ++it;
         }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Source code from Koffice 1.3

QString ImagesGallery::EscapeSgmlText(const QTextCodec* codec,
                      const QString& strIn,
                      const bool quot /* = false */ ,
                      const bool apos /* = false */ )
{
    QString strReturn;
    QChar ch;

    for (uint i = 0 ; i < strIn.length() ; ++i)
    {
        ch=strIn[i];
        switch (ch.unicode())
        {
        case 38: // &
            {
                strReturn+="&amp;";
                break;
            }
        case 60: // <
            {
                strReturn+="&lt;";
                break;
            }
        case 62: // >
            {
                strReturn+="&gt;";
                break;
            }
        case 34: // "
            {
                if (quot)
                    strReturn+="&quot;";
                else
                    strReturn+=ch;
                break;
            }
        case 39: // '
            {
                // NOTE: HTML does not define &apos; by default (only XML/XHTML does)
                if (apos)
                    strReturn+="&apos;";
                else
                    strReturn+=ch;
                break;
            }
        default:
            {
                // verify that the character ch can be expressed in the
                // encoding in which we will write the HTML file.
                if (codec)
                {
                    if (!codec->canEncode(ch))
                    {
                        strReturn+=QString("&#%1;").arg(ch.unicode());
                        break;
                    }
                }
                strReturn+=ch;
                break;
            }
        }
    }

    return strReturn;
}

}  // NameSpace KIPIImagesGalleryPlugin

#include "imagesgallery.moc"
