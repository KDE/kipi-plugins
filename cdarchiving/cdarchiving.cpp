//////////////////////////////////////////////////////////////////////////////
//
//    CDARCHIVING.CPP
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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

// C Ansi includes

extern "C"
{
#include <sys/types.h>
}

// Include files for Qt

#include <qtextstream.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qimage.h>
#include <qtextcodec.h>
#include <qtimer.h>

// Include files for KDE

#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/global.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kcharsets.h>
#include <kapplication.h>
#include <kimageio.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <krun.h>
#include <kaboutdata.h>

// Local includes

#include "actions.h"
#include "cdarchiving.h"
#include "cdarchivingdialog.h"
#include "plugin_cdarchiving.h"

namespace KIPICDArchivingPlugin
{

CDArchiving::CDArchiving( KIPI::Interface* interface, QObject *parent, KAction *action_cdarchiving )
           : QObject(parent)
{
    KImageIO::registerFormats();
    const KAboutData *data = KApplication::kApplication()->aboutData();
    m_hostName = QString::QString( data->appName() );
    
    m_hostURL = data->homepage();
    
    if (m_hostURL.isEmpty())
       {
       m_hostName = "Kipi";
       m_hostURL = "http://extragear.kde.org/apps/kipi";
       }

    m_actionCDArchiving = action_cdarchiving;
    m_interface = interface;
    m_parent = parent;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

CDArchiving::~CDArchiving()
{
    delete m_configDlg;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::writeSettings(void)
{
    KConfig config("kipirc");
    config.setGroup("CDArchiving Settings");

    // Albums selection dialogbox setup tab

    config.writeEntry("MediaFormat", m_configDlg->getMediaFormat());

    // HTML interface Look dialogbox setup tab

    config.writeEntry("UseHTMLInterface", m_configDlg->getUseHTMLInterface());
    config.writeEntry("UseAutoRun", m_configDlg->getUseAutoRunWin32());
    config.writeEntry("MainPageTitle", m_configDlg->getMainTitle());
    config.writeEntry("ImagesPerRow", m_configDlg->getImagesPerRow());
    config.writeEntry("FontName", m_configDlg->getFontName());
    config.writeEntry("FontSize", m_configDlg->getFontSize());
    config.writeEntry("FontColor", m_configDlg->getForegroundColor());
    config.writeEntry("BackgroundColor", m_configDlg->getBackgroundColor());
    config.writeEntry("ThumbnailsSize", m_configDlg->getThumbnailsSize());  
    config.writeEntry("ThumbnailsFormat", m_configDlg->getImageFormat());
    config.writeEntry("BordersImagesSize", m_configDlg->getBordersImagesSize());
    config.writeEntry("BordersImagesColor", m_configDlg->getBordersImagesColor());

    // CD Informations setup tab

    config.writeEntry("VolumeID", m_configDlg->getVolumeID());
    config.writeEntry("VolumeSetID", m_configDlg->getVolumeSetID());
    config.writeEntry("SystemID", m_configDlg->getSystemID());
    config.writeEntry("ApplicationID", m_configDlg->getApplicationID());
    config.writeEntry("Publisher", m_configDlg->getPublisher());
    config.writeEntry("Preparer", m_configDlg->getPreparer());

    // Misc dialogbox setup tab

    config.writeEntry("K3bBinPath", m_configDlg->getK3bBinPathName());
    config.writeEntry("K3bParameters", m_configDlg->getK3bParameters());
    config.writeEntry("UseOnTheFly", m_configDlg->getUseOnTheFly());
    config.writeEntry("UseCheckCD", m_configDlg->getUseCheckCD());
    config.writeEntry("UseStartWrintingProcess", m_configDlg->getUseStartBurningProcess());

    config.sync();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::readSettings(void)
{
    KConfig config("kipirc");
    config.setGroup("CDArchiving Settings");

    // Albums selection dialogbox setup tab

    m_configDlg->setMediaFormat( config.readEntry("MediaFormat", i18n("CD (650Mb)")) );

    // HTML interface Look dialogbox setup tab

    m_configDlg->setUseHTMLInterface( config.readBoolEntry("UseHTMLInterface", "true") );
    m_configDlg->setUseAutoRunWin32( config.readBoolEntry("UseAutoRun", "true") );
    m_configDlg->setMainTitle( config.readEntry("MainPageTitle", i18n("KIPI Albums Archiving")) );
    m_configDlg->setImagesPerRow( config.readEntry("ImagesPerRow", "4").toInt() );
    m_configDlg->setFontName( config.readEntry("FontName", "Helvetica") );
    m_configDlg->setFontSize( config.readEntry("FontSize", "14").toInt() );
    QColor ColorFont( 208, 255, 208 );
    m_configDlg->setForegroundColor( config.readColorEntry("FontColor", &ColorFont));
    QColor ColorBackground( 51, 51, 51 );
    m_configDlg->setBackgroundColor( config.readColorEntry("BackgroundColor", &ColorBackground));
    m_configDlg->setThumbnailsSize( config.readEntry("ThumbnailsSize", "140").toInt() );
    m_configDlg->setImageFormat( config.readEntry("ThumbnailsFormat", "JPEG") );
    m_configDlg->setBordersImagesSize( config.readEntry("BordersImagesSize", "1").toInt() );
    QColor ColorBordersImages( 208, 255, 208 );
    m_configDlg->setBordersImagesColor( config.readColorEntry("BordersImagesColor", &ColorBordersImages));

    // CD Informations setup tab

    m_configDlg->setVolumeID( config.readEntry("VolumeID", i18n("CD Albums")) );
    m_configDlg->setVolumeSetID( config.readEntry("VolumeSetIDeTitle", i18n("KIPI Album CD archiving")) );
    m_configDlg->setSystemID( config.readEntry("SystemID", i18n("LINUX")) );
    m_configDlg->setApplicationID( config.readEntry("ApplicationID", i18n("K3b CD-DVD Burning application")) );
    m_configDlg->setPublisher( config.readEntry("Publisher", m_hostName + " [" + m_hostURL + "]") );
    m_configDlg->setPreparer( config.readEntry("Preparer", i18n("KIPI CD-Archiving plugin")) );

    // Misc dialogbox setup tab

    m_configDlg->setK3bBinPathName( config.readEntry("K3bBinPath", "k3b") );
    m_configDlg->setK3bParameters( config.readEntry("K3bParameters", "--nofork") );
    m_configDlg->setUseUseOnTheFly( config.readBoolEntry("UseOnTheFly", "true") );
    m_configDlg->setUseCheckCD( config.readBoolEntry("UseCheckCD", "true") );
    m_configDlg->setUseStartBurningProcess( config.readBoolEntry("UseStartWrintingProcess", "false") );
  
    // Get the image files filters from the hosts app.
     
    m_imagesFileFilter = m_interface->fileExtensions();
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchiving::showDialog()
{
    KStandardDirs dir;
    m_tmpFolder = dir.saveLocation("tmp", "kipi-cdarchivingplugin-" + QString::number(getpid()) + "/");

    m_HTMLInterfaceFolder = "";
    m_HTMLInterfaceIndex = "";
    m_HTMLInterfaceAutoRunInf = "";
    m_HTMLInterfaceAutoRunFolder = "";

    m_configDlg = new CDArchivingDialog( m_interface, kapp->activeWindow() );
    readSettings();
    
    if ( m_configDlg->exec() == QDialog::Accepted )
       {
       writeSettings();
       return true;
       }
       
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchiving::prepare(void)
{
    QValueList<KIPI::ImageCollection> albumsList;  
    KIPICDArchivingPlugin::EventData *d;
    
    m_cancelled = false;
    m_StreamMainPageAlbumPreview = "";
        
    // Get config from setup dialog.
    
    albumsList = m_configDlg->getSelectedAlbums();
    m_useHTMLInterface = m_configDlg->getUseHTMLInterface();
    m_useAutoRunWin32 = m_configDlg->getUseAutoRunWin32();
    m_K3bBinPathName = m_configDlg->getK3bBinPathName();
    m_K3bParameters = m_configDlg->getK3bParameters();
    m_useStartBurningProcess = m_configDlg->getUseStartBurningProcess();
    m_imagesPerRow = m_configDlg->getImagesPerRow();
    m_imageFormat = m_configDlg->getImageFormat();
    m_mainTitle = m_configDlg->getMainTitle();
    m_backgroundColor = m_configDlg->getBackgroundColor();
    m_foregroundColor = m_configDlg->getForegroundColor(); 
    m_bordersImagesColor = m_configDlg->getBordersImagesColor();
    m_fontName = m_configDlg->getFontName();    
    m_fontSize = m_configDlg->getFontSize();
    m_bordersImagesSize = m_configDlg->getBordersImagesSize();
    m_thumbnailsSize = m_configDlg->getThumbnailsSize();
    m_mediaFormat = m_configDlg->getMediaFormat();
    m_useOnTheFly = m_configDlg->getUseOnTheFly();
    m_useCheckCD = m_configDlg->getUseCheckCD();
    m_volumeID = m_configDlg->getVolumeID();
    m_volumeSetID = m_configDlg->getVolumeSetID();
    m_systemID = m_configDlg->getSystemID();
    m_applicationID = m_configDlg->getApplicationID();
    m_publisher = m_configDlg->getPublisher();
    m_preparer = m_configDlg->getPreparer();
    m_albumListSize = albumsList.count();
    m_albumsList = albumsList;
    
    // Estimate the number of actions for the KIPI progress dialog. 
    
    int nbActions = 1;
    
    if ( m_useHTMLInterface == true )    
    {
        nbActions = nbActions + m_albumListSize + 1;
    
        if ( m_useAutoRunWin32 == true ) 
            ++nbActions;
    }

    d = new KIPICDArchivingPlugin::EventData;
    d->action = KIPICDArchivingPlugin::Initialize;
    d->starting = true;
    d->success = false;
    d->total = nbActions; 
    QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
    usleep(1000);

    return(true);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::stop()
{
    m_cancelled = true;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::run()
{
    KIPICDArchivingPlugin::EventData *d;
    
    // Making HTML interface.
        
    if ( m_useHTMLInterface == true )
    {
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::BuildHTMLiface;
        d->starting = true;
        d->success = false;
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
       
        if ( buildHTMLInterface() == true )
        {
            m_HTMLInterfaceFolder = m_tmpFolder + "/HTMLInterface";
            QString dir;
            KGlobal::dirs()->addResourceType("kipi_autorun",
                                             KGlobal::dirs()->kde_default("data") + "kipi/data");
            dir = KGlobal::dirs()->findResourceDir("kipi_autorun", "index.htm");
            m_HTMLInterfaceIndex = dir + "index.htm";

            d = new KIPICDArchivingPlugin::EventData;
            d->action = KIPICDArchivingPlugin::BuildHTMLiface;
            d->success = true;
            d->starting = false;
            QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
            usleep(1000);

            // Making AutoRun options.
          
            if ( m_useAutoRunWin32 == true )
            {
                d = new KIPICDArchivingPlugin::EventData;
                d->action = KIPICDArchivingPlugin::BuildAutoRuniface;
                d->starting = true;
                d->success = false;
                QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
                usleep(1000);
       
                CreateAutoRunInfFile();
                m_HTMLInterfaceAutoRunInf = m_tmpFolder + "/autorun.inf";
                m_HTMLInterfaceAutoRunFolder = dir + "/autorun";
             
                d = new KIPICDArchivingPlugin::EventData;
                d->action = KIPICDArchivingPlugin::BuildAutoRuniface;
                d->starting = false;
                d->success = true;
                QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
                usleep(1000);
            }
        }
    }

    // Making K3b project file.
       
    d = new KIPICDArchivingPlugin::EventData;
    d->action = KIPICDArchivingPlugin::BuildK3bProject;
    d->starting = true;
    d->success = false;
    QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
    usleep(1000);
           
    if ( BuildK3bXMLprojectfile(m_HTMLInterfaceFolder, m_HTMLInterfaceIndex,
                                m_HTMLInterfaceAutoRunInf, m_HTMLInterfaceAutoRunFolder) == false )
    {
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::BuildK3bProject;
        d->starting = false;
        d->success = false;
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
        return;
    }
    else 
    {
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::BuildK3bProject;
        d->starting = false;
        d->success = true;
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::invokeK3b()
{
    if (m_cancelled) return;

    m_Proc = new KProcess();

    *m_Proc << m_K3bBinPathName << m_K3bParameters;
    *m_Proc << m_tmpFolder + "/KIPICDArchiving.xml";

    QString K3bCommandLine = m_K3bBinPathName + " " +
                             m_K3bParameters + " " +
                             m_tmpFolder + "/KIPICDArchiving.xml";
    kdDebug(51000) << "K3b is started : " << K3bCommandLine.ascii() << endl;

    connect(m_Proc, SIGNAL(processExited(KProcess *)),
            this, SLOT(slotK3bDone(KProcess*)));

    if ( !m_Proc->start(KProcess::NotifyOnExit, KProcess::All ) )
       {
       KIPICDArchivingPlugin::EventData *d = new KIPICDArchivingPlugin::EventData;
       d->action = KIPICDArchivingPlugin::Error;
       d->starting = false;
       d->success = false;
       d->message = i18n("Cannot start K3b program : fork failed.");
       QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
       usleep(1000);
       return;
       }

    m_actionCDArchiving->setEnabled(false);

    if ( m_useStartBurningProcess == true )
       {
       QTimer::singleShot(10000, 
               this, SLOT(slotK3bStartBurningProcess()));
       m_k3bPid = m_Proc->pid();
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::slotK3bStartBurningProcess(void)
{
    QString temp, cmd;
    temp.setNum(m_k3bPid);
    cmd = "dcop k3b-" + temp + " K3bProject-0 burn";

    KRun::runCommand(cmd);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::slotK3bDone(KProcess*)
{
    kdDebug(51000) << "K3b is done !!! Removing temporary folder..." << endl;

    KIPICDArchivingPlugin::EventData *d = new KIPICDArchivingPlugin::EventData;
    d->action = KIPICDArchivingPlugin::Progress;
    d->starting = true;
    d->success = true;
    d->message = i18n("K3b is done; removing temporary folder....");
    QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
    usleep(1000);
    
    if (DeleteDir(m_tmpFolder) == false)
        {
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::Error;
        d->starting = false;
        d->success = false;
        d->message = i18n("Cannot remove temporary folder '%1'.").arg(m_tmpFolder);
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
        }

    m_actionCDArchiving->setEnabled(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchiving::buildHTMLInterface (void)
{
    QString Path;
    KIPICDArchivingPlugin::EventData *d;
    KURL MainUrl;

    // Create the main target folder.

    QDir TargetDir;
    QString MainTPath= m_tmpFolder + "/HTMLInterface";

    if (TargetDir.exists (MainTPath) == true)
    {
        if (DeleteDir (MainTPath) == false)
        {
            d = new KIPICDArchivingPlugin::EventData;
            d->action = KIPICDArchivingPlugin::Error;
            d->starting = false;
            d->success = false;
            d->message = i18n("Cannot remove folder '%1'.").arg(MainTPath);
            QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
            usleep(1000);
            return false;
        }
    }
    
    if (TargetDir.mkdir( MainTPath ) == false)
    {
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::Error;
        d->starting = false;
        d->success = false;
        d->message = i18n("Could not create folder '%1'.").arg(MainTPath);
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
        return false;
    }

    // Build all Albums interface HTML.

    // Adding go home icon if there is more than
    KGlobal::dirs()->addResourceType("kipi_data", KGlobal::dirs()->kde_default("data") + "kipi");
    QString dir = KGlobal::dirs()->findResourceDir("kipi_data", "gohome.png");
    dir = dir + "gohome.png";
    KURL srcURL(dir);
    KURL destURL( MainTPath + "/gohome.png");
    KIO::file_copy(srcURL, destURL, -1, true, false, false);

    // Adding up icon
    KGlobal::dirs()->addResourceType("kipi_data", KGlobal::dirs()->kde_default("data") + "kipi");
    dir = KGlobal::dirs()->findResourceDir("kipi_data", "up.png");
    dir = dir + "up.png";
    srcURL = dir;
    destURL = MainTPath + QString::fromLatin1("/up.png");
    KIO::file_copy(srcURL, destURL, -1, true, false, false);

    for (QValueList<KIPI::ImageCollection>::iterator it = m_albumsList.begin();
         it != m_albumsList.end(); ++it)
    {
        KIPI::ImageCollection album = *it;
        kdDebug( 51000 ) << "HTML Interface for Album: " << album.name() << endl;

        m_AlbumTitle      = album.name();
        m_AlbumComments   = m_interface->hasFeature(KIPI::AlbumsHaveComments) ?
                            album.comment() : QString();
        m_AlbumCollection = m_interface->hasFeature(KIPI::AlbumsHaveCategory) ?
                            album.category() : QString();
        m_AlbumDate       = m_interface->hasFeature(KIPI::AlbumsHaveCreationDate) ?
                            album.date().toString() : QString();

        // Create the target sub folder for the current album.
        QString SubTPath = m_tmpFolder + "/HTMLInterface/" + m_AlbumTitle;
        KURL SubURL      = SubTPath + "/index.htm";
        if (TargetDir.mkdir( SubTPath ) == false)
        {
            d = new KIPICDArchivingPlugin::EventData;
            d->action = KIPICDArchivingPlugin::Error;
            d->starting = false;
            d->success = false;
            d->message = i18n("Could not create folder '%1'.").arg(SubTPath);
            QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
            usleep(1000);
            return false;
        }

        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::BuildAlbumHTMLPage;
        d->starting = true;
        d->success = false;
        d->albumName = m_AlbumTitle;
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);

        if ( createHtml( album, SubURL, m_imageFormat ) == false)
        {
            d = new KIPICDArchivingPlugin::EventData;
            d->action = KIPICDArchivingPlugin::BuildAlbumHTMLPage;
            d->starting = false;
            d->success = false;
            d->albumName = m_AlbumTitle;
            QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));        
            usleep(1000);               
               
            if (DeleteDir (MainTPath) == false)
            {
                d = new KIPICDArchivingPlugin::EventData;
                d->action = KIPICDArchivingPlugin::Error;
                d->starting = false;
                d->success = false;
                d->message = i18n("Cannot remove folder '%1'.").arg(MainTPath);
                QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
                usleep(1000);
                return false;
            }

            return false;
        }
           
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::BuildAlbumHTMLPage;
        d->starting = false;
        d->success = true;
        d->albumName = m_AlbumTitle;
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));        
        usleep(1000);
    }

    // Create the main interface HTML page.

    MainUrl = m_tmpFolder + "/HTMLInterface/" + "index.htm";
    QFile MainPageFile( MainUrl.path() );

    if ( MainPageFile.open(IO_WriteOnly) )
    {
        QTextStream stream(&MainPageFile);
        stream.setEncoding(QTextStream::UnicodeUTF8);
        createHead(stream);
        createBodyMainPage(stream, MainUrl);
        MainPageFile.close();
    }
    else
    {
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::Error;
        d->starting = false;
        d->success = false;
        d->message = i18n("Could not open file '%1'.").arg(MainUrl.path(+1));
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
        return false;
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchiving::createDirectory(QDir thumb_dir, QString imgGalleryDir, QString dirName)
{

    if (!thumb_dir.exists())
        {
        thumb_dir.setPath( imgGalleryDir );

        if (!(thumb_dir.mkdir(dirName, false)))
            {
            KIPICDArchivingPlugin::EventData *d = new KIPICDArchivingPlugin::EventData;
            d->action = KIPICDArchivingPlugin::Error;
            d->starting = false;
            d->success = false;
            d->message = i18n("Could not create folder '%1' in '%2'.")
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

void CDArchiving::createHead(QTextStream& stream)
{
    QString chsetName = QTextCodec::codecForLocale()->mimeName();

    stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">"
           << endl;
    stream << "<html>" << endl;
    stream << "<head>" << endl;
    stream << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">" << endl;
    stream << "<meta name=\"Generator\" content=\"Albums HTML interface for CD archiving generated by "
           << m_hostName << " [" << m_hostURL << "]\">"  << endl;
    stream << "<meta name=\"date\" content=\"" + KGlobal::locale()->formatDate(QDate::currentDate())
           + "\">" << endl;
    stream << "<title>" << m_mainTitle << "</title>" << endl;
    createCSSSection(stream);
    stream << "</head>" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::createCSSSection(QTextStream& stream)
{
    QString backgroundColor = m_backgroundColor.name();
    QString foregroundColor = m_foregroundColor.name();
    QString bordersImagesColor = m_bordersImagesColor.name();

    // Adding a touch of style

    stream << "<style type='text/css'>\n";
    stream << "BODY {color: " << foregroundColor << "; background: " << backgroundColor << ";" << endl;
    stream << "          font-family: " << m_fontName << ", sans-serif;" << endl;
    stream << "          font-size: " << m_fontSize << "pt; margin: 8%; }" << endl;
    stream << "H1       {color: " << foregroundColor << ";}" << endl;
    stream << "TABLE    {text-align: center; margin-left: auto; margin-right: auto;}" << endl;
    stream << "TD       { color: " << foregroundColor << "; padding: 1em}" << endl;
    stream << "IMG      { border: 0px ; }" << endl;
    stream << "IMG.photo      { border: " << m_bordersImagesSize << "px solid "
           << bordersImagesColor << "; }" << endl;
    stream << "</style>" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString CDArchiving::extension(const QString& imageFormat)
{
    if (imageFormat == "PNG")
        return ".png";

    if (imageFormat == "JPEG")
        return ".jpg";

    Q_ASSERT(false);
    return "";
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::createBody(QTextStream& stream,
                             const KIPI::ImageCollection& album,
                             const KURL& targetURL,
                             const QString& imageFormat)
{
    KURL::List images = album.images();
    int numOfImages   = images.count();
    
    const QString imgGalleryDir = targetURL.directory();
    const QString today(KGlobal::locale()->formatDate(QDate::currentDate()));

    stream << "<body>\n" << endl;

    stream << "<p><a href=\"../index.htm\"><img src=\"../gohome.png\" border=\"0\"  title=\""
           << i18n("Album list") << "\" alt=\"" << i18n("Album list") << "\"></a></p>" << endl;


    // Page Top -------------------------------------------------------------------------
    
    stream << "<h1>" << i18n("Album ") << "&quot;" << m_AlbumTitle << "&quot;"
           << "</h1>" << endl;

    stream << "<table width=\"100%\" border=1 cellpadding=0 cellspacing=0 "
              "style=\"page-break-before: always\">\n" << endl;

    stream << "<col width=\"20%\"><col width=\"80%\">" << endl;
    stream << "<tr valign=top><td align=left>\n" << endl;

    if (m_interface->hasFeature(KIPI::AlbumsHaveComments))
        stream << i18n("<i>Comment:</i>") << "<br>\n" << endl;

    if (m_interface->hasFeature(KIPI::AlbumsHaveCategory))        
        stream << i18n("<i>Collection:</i>") << "<br>\n" << endl;

    if (m_interface->hasFeature(KIPI::AlbumsHaveCreationDate))
        stream << i18n("<i>Date:</i>") << "<br>\n" << endl;

    stream << i18n("<i>Images:</i>") << "\n" << endl;

    stream << "</td><td align=left>\n" << endl;

    if (m_interface->hasFeature(KIPI::AlbumsHaveComments))
        stream << EscapeSgmlText(QTextCodec::codecForLocale(),
                                 m_AlbumComments, true, true)
               << "<br>\n" << endl;

    if (m_interface->hasFeature(KIPI::AlbumsHaveCategory))        
        stream << m_AlbumCollection << "<br>\n" << endl;

    if (m_interface->hasFeature(KIPI::AlbumsHaveCreationDate))
        stream << m_AlbumDate << "<br>\n" << endl;

    stream << numOfImages << "\n" << endl;

    stream << "</td></tr></table>\n" << endl;

    
    // Page Center -----------------------------------------------------------------------
    
    stream << "<table>" << endl;
    
    // Table with images

    int        imgIndex = 0;
    EventData* d = 0;
    
    for( KURL::List::iterator urlIt = images.begin() ; 
         !m_cancelled && (urlIt != images.end());
         ++urlIt, ++imgIndex)
    {
        // Row Start
        if ((imgIndex % m_imagesPerRow) == 0)
        {
            stream << "<tr>" << endl;
        }
        
        QString   imgName = (*urlIt).fileName();
        QString   imgPath = (*urlIt).path();
        QFileInfo imgInfo(imgPath);
        QImage    imgProp = QImage(imgPath);

        stream << "<td align='center'>\n<a href=\"pages/"  << imgName << ".htm\">";
        kdDebug(51000) << "Creating thumbnail for " << imgName << endl;
            
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::ResizeImages;
        d->starting = true;
        d->success = false;
        d->fileName = imgName;
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
            
        int valRet = createThumb(imgName, (*urlIt).directory(),
                                 imgGalleryDir, imageFormat);
            
        if ( valRet != -1 )
        {
            QString thumbPath("thumbs/" + imgName + extension(imageFormat));
            stream << "<img class=\"photo\" src=\"" << thumbPath
                   << "\" width=\"" << m_imgWidth
                   << "\" "
                   << "height=\"" << m_imgHeight
                   << "\" alt=\"" << imgPath;

            QString sep = "\" title=\"";

            stream << sep << imgName;
            sep = ", ";

            stream << sep << imgProp.width() << "&nbsp;x&nbsp;" << imgProp.height();
            sep = ", ";

            stream << sep << (imgInfo.size() / 1024) << "&nbsp;" << i18n("KB");
            sep = ", ";

            QString imgPageComment = m_interface->info(*urlIt).description();
            
            if ( !imgPageComment.isEmpty() )
            {
                stream << sep
                       << EscapeSgmlText(QTextCodec::codecForLocale(),
                                         imgPageComment, true, true);
                
            }

            stream << "\">" << endl;

            QString prevImgName = "";
            QString nextImgName = "";

            if (imgIndex != 0)
                prevImgName = images[imgIndex].fileName();

            if (imgIndex < numOfImages-1)
                nextImgName = images[imgIndex].fileName();


            createPage(imgGalleryDir,  (*urlIt),
                       (imgIndex > 0) ? images[imgIndex-1] : KURL(),
                       (imgIndex < (int)(images.count()-1)) ? images[imgIndex+1] : KURL(),
                       imgPageComment);

            // For each first image of current Album we add a preview in main HTML page.      
                
            if ( imgIndex == 0) 
            {
                QString Temp, Temp2;
                Temp2 = "<a href=\"./" + m_AlbumTitle + "/" + "index.htm" + "\">";
                m_StreamMainPageAlbumPreview.append ( Temp2 );
                Temp2 = "<img class=\"photo\" src=\"./" + m_AlbumTitle + "/"
                        + thumbPath + "\" width=\"" + Temp.setNum(m_imgWidth) + "\" ";
                m_StreamMainPageAlbumPreview.append ( Temp2 );
                Temp2 = "height=\"" + Temp.setNum(m_imgHeight) + "\" alt=\"" + thumbPath + "\" ";
                m_StreamMainPageAlbumPreview.append ( Temp2 );
                Temp2 = "title=\"" + m_AlbumTitle + " [ " + Temp.setNum(numOfImages)
                        + i18n(" images") + " ]\"></a>\n";
                m_StreamMainPageAlbumPreview.append ( Temp2 );
                Temp2 = "<a href=\"./" + m_AlbumTitle + "/" + "index.htm" + "\">"
                        + m_AlbumTitle + "</a>" + " [ " + Temp.setNum(numOfImages) + i18n(" images")
                        + " ]" + "<br>\n";
                m_StreamMainPageAlbumPreview.append ( Temp2 );
            }
        }
            
        if ( valRet == -1 || valRet == 0 )
        {
            kdDebug(51000) << "Creating thumbnail for " << imgName
                           <<  "failed !" << endl;
                
            d = new KIPICDArchivingPlugin::EventData;
            d->action = KIPICDArchivingPlugin::ResizeImages;
            d->starting = false;
            d->success = false;
            d->fileName = imgName;
            QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
            usleep(1000);
        }
        else
        {
            d = new KIPICDArchivingPlugin::EventData;
            d->action = KIPICDArchivingPlugin::ResizeImages;
            d->starting = false;
            d->success = true;
            d->fileName = imgName;
            QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
            usleep(1000);
        }
                
        stream << "</a>" << endl;

        stream << "<div>" << imgName << "</div>" << endl;
        
        stream << "<div>" << imgProp.width() << " x " << imgProp.height() << "</div>" << endl;
        stream << "<div>(" << (imgInfo.size() / 1024) << " " <<  i18n("KB") << ") " << "</div>" << endl;

        stream << "</td>" << endl;

        // Row End
        if ( ((imgIndex+1) % m_imagesPerRow) == 0 ||
             ((imgIndex+1) == (int)(images.count())) )
        {            
            stream << "</tr>" << endl;
        }
    }

    // Close the HTML and page creation info if necessary.

    stream << "</table>\n<hr>\n" << endl;

    QString Temp;
    KGlobal::dirs()->addResourceType("kipi_data", KGlobal::dirs()->kde_default("data") + "kipi");
    QString dir = KGlobal::dirs()->findResourceDir("kipi_data", "valid-html401.png");
    dir = dir + "valid-html401.png";

    KURL srcURL(dir);
    KURL destURL(imgGalleryDir + QString::fromLatin1("/thumbs/valid-html401.png"));
    KIO::file_copy(srcURL, destURL, -1, true, false, false);

    stream << "<p>"  << endl;
    Temp = i18n("Valid HTML 4.01.");
    stream << "<img src=\"thumbs/valid-html401.png\" alt=\"" << Temp
           << "\" height=\"31\" width=\"88\" title=\"" << Temp << "\" />" << endl;
                
    Temp = i18n("Album archive created with "
                "<a href=\"%1\">%2</a> on %3").arg(m_hostURL).arg(m_hostName).arg(today);
                
    stream << Temp << endl;
    stream << "</p>" << endl;
    stream << "</body>\n</html>\n" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::createBodyMainPage(QTextStream& stream, KURL& url)
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

    KGlobal::dirs()->addResourceType("kipi_data", KGlobal::dirs()->kde_default("data") + "kipi");
    QString dir = KGlobal::dirs()->findResourceDir("kipi_data", "valid-html401.png");
    dir = dir + "valid-html401.png";

    KURL srcURL(dir);
    KURL destURL(url.directory() + QString::fromLatin1("/valid-html401.png"));
    KIO::file_copy(srcURL, destURL, -1, true, false, false);

    stream << "<p>"  << endl;
    Temp = i18n("Valid HTML 4.01.");
    stream << "<img src=\"valid-html401.png\" alt=\"" << Temp << "\" height=\"31\" width=\"88\" title=\""
           << Temp <<  "\" />" << endl;
           
    Temp = i18n("Album archive created with "
                "<a href=\"%1\">%2</a> on %3").arg(m_hostURL).arg(m_hostName).arg(today);                
    stream << Temp << endl;
    stream << "</p>" << endl;
    stream << "</body>\n</html>\n" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchiving::createHtml( const KIPI::ImageCollection& album,
                              const KURL& targetURL,
                              const QString& imageFormat )
{
    if (m_cancelled)
        return false;
    
    // Sort the images files formats running with thumbnails construction.

    const QString imgGalleryDir = targetURL.directory();

    // Create the "thumbs" subdirectory

    QDir thumb_dir( imgGalleryDir + QString::fromLatin1("/thumbs/"));

    if (createDirectory(thumb_dir, imgGalleryDir, "thumbs") == false)
        return false;

    QDir pages_dir( imgGalleryDir + QString::fromLatin1("/pages/"));

    if (createDirectory(pages_dir, imgGalleryDir, "pages") == false)
        return false;

    // Create HTML page.

    QFile file( targetURL.path() );

    if ( file.open(IO_WriteOnly) )
    {
        QTextStream stream(&file);
        stream.setEncoding(QTextStream::UnicodeUTF8);
        createHead(stream);
        createBody(stream, album, targetURL, imageFormat);
        file.close();
        return true;        
    }
    else
    {
        KIPICDArchivingPlugin::EventData *d;
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::Error;
        d->starting = false;
        d->success = false;
        d->message = i18n("Could not open file '%1'.").arg(targetURL.path(+1));
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
        return false;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchiving::createPage(const QString& imgGalleryDir,
                             const KURL& imgURL,
                             const KURL& prevImgURL,
                             const KURL& nextImgURL,
                             const QString& comment)
{

    const QDir pagesDir(imgGalleryDir + QString::fromLatin1("/pages/"));
    const QDir thumbsDir(imgGalleryDir + QString::fromLatin1("/thumbs/"));
    const QString imgName(imgURL.fileName());
    
    // Html pages filenames

    const QString pageFilename = pagesDir.path() +
                                 QString::fromLatin1("/") +
                                 imgName +
                                 QString::fromLatin1(".htm");
    const QString nextPageFilename =  nextImgURL.fileName() +
                                      QString::fromLatin1(".htm");
    const QString prevPageFilename =  prevImgURL.fileName() +
                                      QString::fromLatin1(".htm");

    // Thumbs filenames

    const QString prevThumb = QString::fromLatin1("../thumbs/") +
                              prevImgURL.fileName() +
                              extension(m_imageFormat);

    const QString nextThumb = QString::fromLatin1("../thumbs/") +
                              nextImgURL.fileName() +
                              extension(m_imageFormat);

    QFile file( pageFilename );

    if ( pagesDir.exists() && file.open(IO_WriteOnly) )
    {
        QTextStream stream(&file);
        stream.setEncoding(QTextStream::UnicodeUTF8);

        QString chsetName = QTextCodec::codecForLocale()->mimeName();
        stream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\""
            " \"http://www.w3.org/TR/html4/loose.dtd\">" << endl;
        stream << "<html>" << endl;
        stream << "<head>" << endl;
        stream << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">" << endl;
                 
        stream << "<meta name=\"Generator\" content=\"Albums Images gallery generated by "
               << m_hostName << " [" << m_hostURL << "]\">"  << endl;
                 
        stream << "<meta name=\"date\" content=\""
               << KGlobal::locale()->formatDate(QDate::currentDate())
               << "\">" << endl;
        stream << "<title>" << m_mainTitle << " : "<< imgURL.fileName() <<"</title>" << endl;

        createCSSSection(stream);

        stream << "</head>" << endl;
        stream<<"<body>" << endl;;
        stream << "<div align=\"center\">"<< endl;

        QImage imgProp;
        int prevW = 0;
        int prevH = 0;
        int nextW = 0;
        int nextH = 0;

        if (imgProp.load(prevImgURL.path()))
        {
            prevW = imgProp.width();
            prevH = imgProp.height();
        }

        if (imgProp.load(nextImgURL.path()))
        {
            nextW = imgProp.width();
            nextH = imgProp.height();
        }

        // Navigation thumbs need to be 64x64 at most

        if ( prevW < prevH )
        {
            prevH = (NAV_THUMB_MAX_SIZE  * prevH) / prevW;
            prevW = NAV_THUMB_MAX_SIZE;
        }
        else if ( prevW==prevH )
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
        else if ( nextW==nextH )
        {
            nextH = NAV_THUMB_MAX_SIZE ;
            nextW = NAV_THUMB_MAX_SIZE;
        }
        else
        {
            nextW = (NAV_THUMB_MAX_SIZE  * nextW) / nextH;
            nextH = NAV_THUMB_MAX_SIZE;
        }

        if ( prevImgURL.isValid() )
        {
            stream << "<a href=\"" << prevPageFilename << "\"><img class=\"photo\" src=\""
                   << prevThumb<<"\" alt=\"" << i18n("Previous") <<  "\" title=\"" << i18n("Previous")
                   << "\" height=\"" << prevH << "\" width=\"" << prevW << "\"></a>&nbsp; | &nbsp;" << endl;
        }

        stream << "<a href=\"../index.htm\"><img src=\"../../up.png\" border=\"0\" title=\""
               << i18n("Album index") << "\" alt=\"" << i18n("Album index") << "\"></a>" << endl;
        stream << "&nbsp; | &nbsp;<a href=\"../../index.htm\"><img src=\"../../gohome.png\" border=\"0\" title=\""
               << i18n("Album list") << "\" alt=\"" << i18n("Album list") << "\"></a>" << endl;

        if ( nextImgURL.isValid() )
        {
            stream << "&nbsp; | &nbsp;<a href=\"" << nextPageFilename<<"\"><img class=\"photo\" src=\""
                   << nextThumb << "\" alt=\"" << i18n("Next") << "\" title=\"" << i18n("Next")
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

        stream << "<img class=\"photo\" src=\"../../../" << m_AlbumTitle << "/" << imgName
               << "\" alt=\"" << imgName;

        // Add info about image if requested

        QString sep="\" title=\"";
        QFileInfo imgInfo;

        stream << sep << imgName;
        sep = ", ";

        imgProp.load( imgURL.path() );
        stream << sep << imgProp.width() << "&nbsp;x&nbsp;" << imgProp.height();
        sep = ", ";

        imgInfo.setFile( imgURL.path() );
        stream << sep << (imgInfo.size() / 1024) << "&nbsp;" << i18n("KB");

        stream << "\"><br><br></div>" << endl;

        // Footer

        QString valid = i18n("Valid HTML 4.01.");
        const QString today(KGlobal::locale()->formatDate(QDate::currentDate()));

        stream << "<div align=\"center\"><hr><img src=\"../thumbs/valid-html401.png\" alt=\""
               << valid << "\" height=\"31\" width=\"88\"  title=\"" << valid <<  "\" />" << endl;
                    
        valid =  i18n("Image gallery created with "
                      "<a href=\"%1\">%2</a> on %3").arg(m_hostURL).arg(m_hostName).arg(today);
                    
        stream << valid << "</div>" << endl;

        stream << "</body></html>" << endl;
        file.close();

        return true;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int CDArchiving::createThumb( const QString& imgName, const QString& sourceDirName,
                              const QString& imgGalleryDir, const QString& imageFormat)
{
    const QString pixPath = sourceDirName + "/" + imgName;

    // Create the thumbnails for the HTML interface.

    const QString ImageNameFormat = imgName + extension(imageFormat);
    const QString thumbDir = imgGalleryDir + QString::fromLatin1("/thumbs/");
    int extent = m_thumbnailsSize;

    m_imgWidth = 120; // Setting the size of the images is
    m_imgHeight = 90; // required to generate faster 'loading' pages

    return (ResizeImage(pixPath, thumbDir, imageFormat, ImageNameFormat,
                        &m_imgWidth, &m_imgHeight, extent, false, 16, false, 100));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int CDArchiving::ResizeImage( const QString Path, const QString Directory, const QString ImageFormat,
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

bool CDArchiving::BuildK3bXMLprojectfile (QString HTMLinterfaceFolder, QString IndexHtm,
                                          QString AutoRunInf, QString AutorunFolder)
{
    QString Temp;
    KIPICDArchivingPlugin::EventData *d;
    QFile XMLK3bProjectFile;

    // open the K3b XML project file.

    XMLK3bProjectFile.setName ( m_tmpFolder + "/KIPICDArchiving.xml" );

    if ( XMLK3bProjectFile.open ( IO_WriteOnly | IO_Truncate ) == false )
        return false;
    
    d = new KIPICDArchivingPlugin::EventData;
    d->action = KIPICDArchivingPlugin::Progress;
    d->starting = true;
    d->success = false;
    d->message = i18n("Creating project header...");
    QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
    usleep(1000);

    // Build K3b XML project File.

    QTextStream stream( &XMLK3bProjectFile );
    stream.setEncoding(QTextStream::UnicodeUTF8);

    // XML Header.

    Temp = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    stream << Temp;

    if (m_mediaFormat == i18n("DVD (4,7Gb)"))
        Temp = "<!DOCTYPE k3b_dvd_project>\n"
               "<k3b_dvd_project>\n";                                     // Build a Data DVD project file.
    else
        Temp = "<!DOCTYPE k3b_data_project>\n"
               "<k3b_data_project>\n";                                    // Build a Data CD project file.

    stream << Temp;

    // General section.

    Temp = "<general>\n"
           "<writing_mode>auto</writing_mode>\n"                          // Let K3b selected the good mode.
           "<dummy activated=\"no\" />\n";                                // Simulation always disactived.

    stream << Temp;

    if (m_useOnTheFly == false)                          // Burning CD On The Fly ?
       Temp = "<on_the_fly activated=\"no\" />\n";
    else
       Temp = "<on_the_fly activated=\"yes\" />\n";

    stream << Temp;

    Temp = "<only_create_images activated=\"no\" />\n"                    // Always desactived.
           "<remove_images activated=\"yes\" />\n"                        // Always actived.
           "</general>\n";

    stream << Temp;

    // Option section.

    Temp = "<options>\n"
           "<rock_ridge activated=\"yes\" />\n"                           // Always actived for Linux.
           "<joliet activated=\"yes\" />\n";                              // Always actived for Win32.

    stream << Temp;

    if (m_mediaFormat == i18n("DVD (4,7Gb)"))
        Temp = "<udf activated=\"yes\" />\n";                             // Need this option for DVDR/RW.
    else
        Temp = "<udf activated=\"no\" />\n";                              // Don't need this option for CDR/RW

    stream << Temp;

    Temp = "<iso_allow_lowercase activated=\"no\" />\n"
           "<iso_allow_period_at_begin activated=\"no\" />\n"
           "<iso_allow_31_char activated=\"yes\" />\n"
           "<iso_omit_version_numbers activated=\"no\" />\n"
           "<iso_omit_trailing_period activated=\"no\" />\n"
           "<iso_max_filename_length activated=\"no\" />\n"
           "<iso_relaxed_filenames activated=\"no\" />\n"
           "<iso_no_iso_translate activated=\"no\" />\n"
           "<iso_allow_multidot activated=\"no\" />\n"
           "<iso_untranslated_filenames activated=\"no\" />\n"
           "<follow_symbolic_links activated=\"no\" />\n"                 // Always desactived.
           "<create_trans_tbl activated=\"no\" />\n"
           "<hide_trans_tbl activated=\"no\" />\n"
           "<iso_level>2</iso_level>\n"                                   // Always ISO level 2.
           "<discard_symlinks activated=\"no\" />\n"                      // Always desactived.
           "<discard_broken_symlinks activated=\"no\" />\n"               // Always desactived.
           "<preserve_file_permissions activated=\"yes\" />\n"            // Actived : backup.
           "<force_input_charset activated=\"no\" />\n"                   // Disabled.
           "<input_charset>iso8859-1</input_charset>\n"                   // Disabled (see before).
           "<whitespace_treatment>noChange</whitespace_treatment>\n"
           "<whitespace_replace_string>_</whitespace_replace_string>\n"
           "<data_track_mode>auto</data_track_mode>\n"                    // Let K3b selected the good mode.
           "<multisession>none</multisession>\n";                         // Always 1 session for backup CD.

    stream << Temp;

    if (m_useCheckCD == false)                           // Checking CD after burning process ?
       Temp = "<verify_data activated=\"no\" />\n";
    else
       Temp = "<verify_data activated=\"yes\" />\n";

    stream << Temp;

    Temp = "</options>\n";

    stream << Temp;

    // Header section.

    Temp = "<header>\n"
           "<volume_id>"
           + EscapeSgmlText(QTextCodec::codecForLocale(), m_volumeID, true, true)
           + "</volume_id>\n"
           "<volume_set_id>"
           + EscapeSgmlText(QTextCodec::codecForLocale(), m_volumeSetID, true, true)
           + "</volume_set_id>\n"
           "<volume_set_size>1</volume_set_size>\n"
           "<volume_set_number>1</volume_set_number>\n"
           "<system_id>"
           + EscapeSgmlText(QTextCodec::codecForLocale(), m_systemID, true, true)
           + "</system_id>\n"
           "<application_id>"
           + EscapeSgmlText(QTextCodec::codecForLocale(), m_applicationID, true, true)
           + "</application_id>\n"
           "<publisher>"
           + EscapeSgmlText(QTextCodec::codecForLocale(), m_publisher, true, true)
           + "</publisher>\n"
           "<preparer>"
           + EscapeSgmlText(QTextCodec::codecForLocale(), m_preparer, true, true)
           + "</preparer>\n"
           "</header>\n";

    stream << Temp;

    // Files and folders section.

    Temp = "<files>\n";

    stream << Temp;

    if ( IndexHtm.isEmpty() == false )          // index.htm file in CD root.
       {
       Temp = "<file name=\"index.htm\" >\n"
              "<url>"
              + EscapeSgmlText(QTextCodec::codecForLocale(), IndexHtm, true, true)
              + "</url>\n"
              "</file>\n";

       stream << Temp;
       }

    if ( AutoRunInf.isEmpty() == false )        // Autorun.inf file in CD root.
       {
       Temp = "<file name=\"autorun.inf\" >\n"
              "<url>"
              + EscapeSgmlText(QTextCodec::codecForLocale(), AutoRunInf, true, true)
              + "</url>\n"
              "</file>\n";

       stream << Temp;
       }

    // Add Autorun folder name and files.

    if ( AutorunFolder.isEmpty() == false )
       AddFolderTreeToK3bXMLProjectFile(AutorunFolder, &stream);

    // Add HTMLInterface folders name and files.

    if ( HTMLinterfaceFolder.isEmpty() == false )
       AddFolderTreeToK3bXMLProjectFile(HTMLinterfaceFolder, &stream);

    for (QValueList<KIPI::ImageCollection>::iterator it = m_albumsList.begin();
         !m_cancelled && (it != m_albumsList.end()); ++it)
    {
        d = new KIPICDArchivingPlugin::EventData;
        d->action = KIPICDArchivingPlugin::Progress;
        d->starting = true;
        d->success = false;
        d->message = i18n("Adding Album '%1' into project...").arg( (*it).name() );
        QApplication::sendEvent(m_parent, new QCustomEvent(QEvent::User, d));
        usleep(1000);
        addCollectionToK3bXMLProjectFile( *it, &stream); 
    }

    Temp = "</files>\n";

    stream << Temp;

    if (m_mediaFormat == i18n("DVD (4,7Gb)"))
        Temp = "</k3b_dvd_project>\n";                                     // Close the Data DVD project file.
    else
        Temp = "</k3b_data_project>\n";                                    // Close the Data CD project file.

    stream << Temp;

    // Close K3b XML project File.

    XMLK3bProjectFile.close();
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchiving::AddFolderTreeToK3bXMLProjectFile (QString dirname, QTextStream* stream)
{
   QString Temp;

   QDir dir(dirname);
   dir.setFilter ( QDir::Dirs | QDir::Files | QDir::NoSymLinks );

   Temp = "<directory name=\""
          + EscapeSgmlText(QTextCodec::codecForLocale(), dir.dirName(), true, true)
          + "\" >\n";

   *stream << Temp;
   
   kdDebug( 51000 ) << "Directory: " << dir.dirName().latin1 () << endl;

   const QFileInfoList* fileinfolist = dir.entryInfoList();
   QFileInfoListIterator it_files(*fileinfolist);
   QFileInfoListIterator it_folders(*fileinfolist);
   QFileInfo* fi_files;
   QFileInfo* fi_folders;

   while( (fi_files = it_files.current()) && !m_cancelled )      // Check all files in folder.
     {
     if ( fi_files->fileName() == "." || fi_files->fileName() == ".." )
          {
          ++it_files;
          continue;
          }

     if( fi_files->isFile() )
          {
          kdDebug( 51000 ) << "   Filename: " << fi_files->fileName().latin1() << endl;
          
          Temp = "<file name=\""
                 + EscapeSgmlText(QTextCodec::codecForLocale(), fi_files->fileName(), true, true)
                 + "\" >\n"
                 "<url>"
                 + EscapeSgmlText(QTextCodec::codecForLocale(), fi_files->absFilePath(), true, true)
                 + "</url>\n"
                 "</file>\n";

          *stream << Temp;
          }

     ++it_files;
     }

   while( (fi_folders = it_folders.current()) && !m_cancelled )           // Check all sub-folders in folder.
     {
     if ( fi_folders->fileName() == "." || fi_folders->fileName() == ".." )
          {
          ++it_folders;
          continue;
          }

     if ( fi_folders->isDir() )
          {
          kdDebug( 51000 ) << "   folder: " << fi_folders->fileName().latin1() << endl;
          
          AddFolderTreeToK3bXMLProjectFile ( fi_folders->absFilePath(), stream );
          }

     ++it_folders;
     }

   Temp = "</directory>\n";
   *stream << Temp;

   return true;
}

bool CDArchiving::addCollectionToK3bXMLProjectFile(const KIPI::ImageCollection& collection,
                                                   QTextStream* stream)
{
   kdDebug( 51000 ) << "Adding Collection: " << collection.name() << endl;

   QString Temp;

   Temp = "<directory name=\""
          + EscapeSgmlText(QTextCodec::codecForLocale(), collection.name(), true, true)
          + "\" >\n";
   *stream << Temp;
   
   KURL::List images = collection.images();

   for (KURL::List::iterator it = images.begin();
        (it != images.end()) && !m_cancelled;
        ++it)
   {

       kdDebug( 51000 ) << "   Filename: " << (*it).fileName() << endl;
          
       Temp = "<file name=\""
              + EscapeSgmlText(QTextCodec::codecForLocale(), (*it).fileName(), true, true)
              + "\" >\n"
              "<url>"
              + EscapeSgmlText(QTextCodec::codecForLocale(), (*it).path(), true, true)
              + "</url>\n"
              "</file>\n";

       *stream << Temp;
   }

   Temp = "</directory>\n";
   *stream << Temp;

   return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchiving::CreateAutoRunInfFile(void)
{
   QString Temp;
   QFile AutoRunInf;

   AutoRunInf.setName ( m_tmpFolder + "/autorun.inf" );

   if ( AutoRunInf.open ( IO_WriteOnly | IO_Truncate ) == false )
       return false;

   QTextStream stream( &AutoRunInf );

   Temp = "[autorun]\r\n"
          "OPEN=autorun\\ShellExecute.bat HTMLInterface\\index.htm\r\n"
          "ICON=autorun\\cdalbums.ico\r\n";

   stream << Temp;

   Temp = "LABEL=" + m_volumeID + "\r\n";
   stream << Temp;

   AutoRunInf.close();
   return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void CDArchiving::removeTmpFiles(void)
{
    DeleteDir(m_tmpFolder);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// This code can be multithreaded (in opposite to KIO::netaccess::delete().

bool CDArchiving::DeleteDir(QString dirname)
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

bool CDArchiving::deldir(QString dirname)
{
    QDir dir(dirname);
    dir.setFilter ( QDir::Dirs | QDir::Files | QDir::NoSymLinks );

    const QFileInfoList* fileinfolist = dir.entryInfoList();
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
              if (dir.rmdir( fi->absFilePath() ) == false)
                  return false;
              }
         else
              if( fi->isFile() )
                   if (dir.remove(fi->absFilePath() ) == false)
                       return false;
         
         ++it;
         }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Source code from Koffice 1.3

QString CDArchiving::EscapeSgmlText(const QTextCodec* codec,
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
                strReturn += "&amp;";
                break;
            }
        case 60: // <
            {
                strReturn += "&lt;";
                break;
            }
        case 62: // >
            {
                strReturn += "&gt;";
                break;
            }
        case 34: // "
            {
                if (quot)
                    strReturn += "&quot;";
                else
                    strReturn += ch;
                break;
            }
        case 39: // '
            {
                // NOTE:  HTML does not define &apos; by default (only XML/XHTML does)
                if (apos)
                    strReturn += "&apos;";
                else
                    strReturn += ch;
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
                        strReturn += QString("&#%1;").arg(ch.unicode());
                        break;
                    }
                }
                strReturn += ch;
                break;
            }
        }
    }

    return strReturn;
}

}  // NameSpace KIPICDArchivingPlugin

#include "cdarchiving.moc"
