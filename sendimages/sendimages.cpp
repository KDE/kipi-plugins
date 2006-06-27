/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 *         from digiKam project.
 * Date  : 2004-02-25
 * Description : a kipi plugin for e-mailing images
 * 
 * Copyright 2004-2005 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// Include files for Qt

#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qimage.h>
#include <qprogressdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qtimer.h>

// Include files for KDE

#include <klocale.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kimageeffect.h>
#include <kdebug.h>
#include <kprogress.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <kio/global.h>
#include <kprocess.h>
#include <kimageio.h>
#include <knuminput.h>
#include <kurlrequester.h>

// Local include files

#include "sendimages.h"
#include "sendimagesdialog.h"
#include "listimageserrordialog.h"
#include "exifrestorer.h"
#include "actions.h"

namespace KIPISendimagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

SendImages::SendImages(KIPI::Interface* interface, const QString &tmpFolder,
                       const KIPI::ImageCollection& imagesCollection, QObject *parent)
          : QObject(parent), QThread(), m_interface( interface ), m_tmp( tmpFolder ),
            m_collection( imagesCollection )
{
    m_parent = parent;
    KImageIO::registerFormats();
    m_mozillaTimer = new QTimer(this);

    connect(m_mozillaTimer, SIGNAL(timeout()),
            this, SLOT(slotMozillaTimeout()));
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

SendImages::~SendImages()
{
    delete m_sendImagesDialog;
    wait();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImages::showDialog()
{
    m_sendImagesDialog = new KIPISendimagesPlugin::SendImagesDialog(kapp->activeWindow(),
                             m_interface, m_collection);
    m_sendImagesDialog->show();

    connect(m_sendImagesDialog, SIGNAL(signalAccepted()),
            m_parent, SLOT(slotAcceptedConfigDlg()));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Execute the no threadable operations before the real thread.

void SendImages::prepare(void)
{
    m_filesSendList.clear();
    m_imagesResizedWithError.clear();
    m_imagesPackage.clear();
    m_images = m_sendImagesDialog->m_images2send;
    m_changeProp = m_sendImagesDialog->m_changeImagesProp->isChecked();
    m_imageFormat = m_sendImagesDialog->m_imagesFormat->currentText();
    m_sizeFactor = getSize( m_sendImagesDialog->m_imagesResize->currentItem() );
    m_imageCompression = m_sendImagesDialog->m_imageCompression->value();
}


/////////////////////////////////////////////////////////////////////////////////////////////
// List of threaded operations.

void SendImages::run()
{
    KIPISendimagesPlugin::EventData *d;

    d = new KIPISendimagesPlugin::EventData;
    d->action = KIPISendimagesPlugin::Initialize;
    d->starting = true;
    d->success = false;
    d->total = m_images.count();
    QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

    for( KURL::List::Iterator it = m_images.begin() ; it != m_images.end() ; ++it )
        {
        QString imageName = (*it).path();
        QString ItemName = imageName.section( '/', -1 );

        d = new KIPISendimagesPlugin::EventData;
        d->action   = KIPISendimagesPlugin::ResizeImages;
        d->fileName = (*it).fileName();
        d->albumName = (*it).directory().section('/', -1);
        d->starting = true;
        d->success = false;
        QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

        // Prepare resized target images to send.

        if ( m_changeProp == true )
           {
           // Prepare resizing images.

           QString imageFileName = ItemName;

           QString imageNameFormat = (*it).directory().section('/', -1) + "-" +
                                     imageFileName.replace(QChar('.'), "_") +
                                     extension(m_imageFormat);

           kdDebug (51000) << "Resizing ' " << imageName.ascii() << "-> '"
                           << m_tmp.ascii() << imageNameFormat.ascii()
                           << "' (" << m_imageFormat.ascii() << ")" << endl;

           if ( resizeImageProcess( imageName, m_tmp, m_imageFormat, imageNameFormat,
                                    m_sizeFactor, m_imageCompression) == false )
               {
               // Resized images failed...

               d = new KIPISendimagesPlugin::EventData;
               d->action   = KIPISendimagesPlugin::ResizeImages;
               d->fileName = (*it).fileName();
               d->albumName = (*it).directory().section('/', -1);
               d->starting = false;
               d->success = false;
               QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

               m_imagesResizedWithError.append(*it);
               }
           else          // Resized images OK...
               {
               // Only try to write Exif if both src and destination are JPEG files.

               if (QString(QImageIO::imageFormat(imageName)).upper() == "JPEG" &&
                   m_imageFormat.upper() == "JPEG")
                  {
                  ExifRestorer exifHolder;
                  exifHolder.readFile(imageName, ExifRestorer::ExifOnly);

                  QString targetFile = m_tmp + imageNameFormat;

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

               d = new KIPISendimagesPlugin::EventData;
               d->action   = KIPISendimagesPlugin::ResizeImages;
               d->fileName = (*it).fileName();
               d->albumName = (*it).directory().section('/', -1);
               d->starting = false;
               d->success = true;
               QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

               m_filesSendList.append(m_tmp + imageNameFormat);
               m_imagesPackage.append(*it);
               m_imagesPackage.append(m_tmp + imageNameFormat);
               }
           }
        else     // No resize images operations...
           {
           d = new KIPISendimagesPlugin::EventData;
           d->action   = KIPISendimagesPlugin::Progress;
           d->fileName = (*it).fileName();
           d->albumName = (*it).directory().section('/', -1);
           d->starting = true;
           d->success = false;
           QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

           m_filesSendList.append(imageName);
           m_imagesPackage.append(imageName);
           m_imagesPackage.append(imageName);
           }
        }

     d = new KIPISendimagesPlugin::EventData;
     d->action   = KIPISendimagesPlugin::Progress;
     d->starting = false;
     d->success = true;
     QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Create a text file with the images comments.

void SendImages::makeCommentsFile(void)
{
    if ( m_sendImagesDialog->m_addComments->isChecked() == true )
    {
        QString ImageCommentsText;

        KURL::List::Iterator it = m_imagesPackage.begin();

        bool anyCommentsPresent = false;
       
        while( it != m_imagesPackage.end() )
        {
            KIPI::ImageInfo info = m_interface->info( *it );

            QString commentItem = info.description();
            ++it;
            QString targetFile = (*it).filename();

            if ( commentItem.isEmpty() )
                commentItem = i18n("no comment");
            else
                anyCommentsPresent = true;

            ImageCommentsText = ImageCommentsText +
                                i18n("Comments for image \"%1\": %2\n\n")
                                .arg(targetFile)
                                .arg(commentItem);
            ++it;
        }

        if ( anyCommentsPresent )
        {
            QFile commentsFile( m_tmp + i18n("comments.txt") );
            QTextStream stream( &commentsFile );
            stream.setEncoding( QTextStream::UnicodeUTF8 );
            commentsFile.open( IO_WriteOnly );
            stream << ImageCommentsText << "\n";
            commentsFile.close();
            m_filesSendList.append( m_tmp + i18n("comments.txt") );
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool SendImages::showErrors()
{
    if ( m_imagesResizedWithError.isEmpty() == false )
       {
       listImagesErrorDialog *ErrorImagesDialog = new listImagesErrorDialog(kapp->activeWindow(),
                                                  i18n("Error during resize images process."),
                                                  i18n("Cannot resize the following image files:"),
                                                  i18n("Do you want them to be added as attachments "
                                                       "(without resizing)?"),
                                                  m_imagesResizedWithError);

       int ValRet = ErrorImagesDialog->exec();

       switch (ValRet)
          {
          case KDialogBase::Yes :         // Added source image files instead resized images...

             for ( KURL::List::Iterator it = m_imagesResizedWithError.begin();
                                       it != m_imagesResizedWithError.end(); ++it )
                 {
                 m_filesSendList.append (*it);
                 m_imagesPackage.append(*it);
                 m_imagesPackage.append(*it);
                 }
             break;

          case KDialogBase::No :         // Do nothing...
             break;

          case KDialogBase::Cancel :     // Stop process...
             removeTmpFiles();
             return false;
             break;
          }
       }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Invoke mailer agent.

void SendImages::invokeMailAgent(void)
{
    // default agent call
    // FIXME: seems to fail for thunderbird. Fix kdelibs or maybe work around it.

    if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Default" )
       {
       KApplication::kApplication()->invokeMailer(
                       QString::null,                     // Destination address.
                       QString::null,                     // Carbon Copy address.
                       QString::null,                     // Blind Carbon Copy address
                       QString::null,                     // Message Subject.
                       QString::null,                     // Message Body.
                       QString::null,                     // Message Body File.
                       m_filesSendList.toStringList());   // Images attachments (+ comments).
       }


    // KMail mail agent call.

    if ( m_sendImagesDialog->m_mailAgentName->currentText() == "KMail" )
    {
        m_mailAgentProc = new KProcess;
        *m_mailAgentProc << "kmail";

        for ( KURL::List::Iterator it = m_filesSendList.begin() ; it != m_filesSendList.end() ; ++it )
        {
            *m_mailAgentProc << "--attach";
            *m_mailAgentProc << QFile::encodeName((*it).path());
        }

        if ( m_mailAgentProc->start() == false )
            KMessageBox::error(kapp->activeWindow(), i18n("Cannot start '%1' program;\nplease "
                                                          "check your installation.")
                               .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
    }

    // Sylpheed mail agent call.

    if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Sylpheed" ||
         m_sendImagesDialog->m_mailAgentName->currentText() == "Sylpheed-Claws" )
    {
        m_mailAgentProc = new KProcess;
        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Sylpheed")
            *m_mailAgentProc << "sylpheed";
        else
            *m_mailAgentProc << "sylpheed-claws";

        *m_mailAgentProc << "--compose" << "--attach";

        for ( KURL::List::Iterator it = m_filesSendList.begin() ; it != m_filesSendList.end() ; ++it )
            *m_mailAgentProc << QFile::encodeName((*it).path());

        if ( m_mailAgentProc->start() == false )
            KMessageBox::error(kapp->activeWindow(), i18n("Cannot start '%1' program;\nplease "
                                                          "check your installation.")
                               .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
    }

    // Balsa mail agent call.

    if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Balsa" )
    {
        m_mailAgentProc = new KProcess;
        *m_mailAgentProc << "balsa" << "-m" << "mailto:";

        for ( KURL::List::Iterator it = m_filesSendList.begin() ; it != m_filesSendList.end() ; ++it )
        {
            *m_mailAgentProc << "-a";
            *m_mailAgentProc << QFile::encodeName((*it).path());
        }

        if ( m_mailAgentProc->start() == false )
            KMessageBox::error(kapp->activeWindow(), i18n("Cannot start '%1' program;\nplease "
                                                          "check your installation.")
                               .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
    }

    if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Evolution" )    // Evolution mail agent call.
    {
        m_mailAgentProc = new KProcess;
        *m_mailAgentProc << "evolution";

        QString Temp = "mailto:?subject=";

        for ( KURL::List::Iterator it = m_filesSendList.begin() ; it != m_filesSendList.end() ; ++it )
        {
            Temp.append("&attach=");
            Temp.append( QFile::encodeName((*it).path()) );
        }

        *m_mailAgentProc << Temp;

        if ( m_mailAgentProc->start() == false )
            KMessageBox::error(kapp->activeWindow(), i18n("Cannot start '%1' program;\nplease "
                                                          "check your installation.")
                               .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
    }

    // Mozilla | Netscape | Thunderbird mail agent call.

    if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Mozilla" ||
         m_sendImagesDialog->m_mailAgentName->currentText() == "Netscape" ||
         m_sendImagesDialog->m_mailAgentName->currentText() == "Thunderbird" ||
         m_sendImagesDialog->m_mailAgentName->currentText() == "GmailAgent")
    {
        m_mailAgentProc = new KProcess;

        m_thunderbirdUrl = m_sendImagesDialog->m_ThunderbirdBinPath->url();

        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Mozilla" )
            {
            *m_mailAgentProc << "mozilla" << "-remote";
            }
        else if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Thunderbird" )
            {
            *m_mailAgentProc << m_thunderbirdUrl << "-remote";
            kdDebug (51000) << m_thunderbirdUrl << endl;
            }
        else if ( m_sendImagesDialog->m_mailAgentName->currentText() == "GmailAgent" )
            {
            *m_mailAgentProc << "gmailagent" << "-remote";
            }
        else
            {
            *m_mailAgentProc << "netscape" << "-remote";
            }

        QString Temp = "xfeDoCommand(composeMessage,attachment='";

        for ( KURL::List::Iterator it = m_filesSendList.begin() ; it != m_filesSendList.end() ; ++it )
        {
            Temp.append( "file://" );
            Temp.append( QFile::encodeName((*it).path()) );
            Temp.append( "," );
        }

        Temp.append("')");

        *m_mailAgentProc << Temp;

        connect(m_mailAgentProc, SIGNAL(processExited(KProcess *)),
                this, SLOT(slotMozillaExited(KProcess*)));

        connect(m_mailAgentProc, SIGNAL(receivedStderr(KProcess *, char*, int)),
                this, SLOT(slotMozillaReadStderr(KProcess*, char*, int)));

        kdDebug (51000) << Temp << endl;

        if ( m_mailAgentProc->start(KProcess::NotifyOnExit , KProcess::All) == false )
            KMessageBox::error(kapp->activeWindow(), i18n("Cannot start '%1' program;\nplease "
                                                          "check your installation.")
                               .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
        else return;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SendImages::removeTmpFiles(void)
{
    if (DeleteDir(m_tmp) == false)
       KMessageBox::error(kapp->activeWindow(), i18n("Cannot remove temporary folder %1.").arg(m_tmp));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool SendImages::DeleteDir(QString dirname)
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

bool SendImages::deldir(QString dirname)
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

         kapp->processEvents();
         ++it;
         }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString SendImages::extension(const QString& imageFileFormat)
{
    if (imageFileFormat == "PNG")
        return ".png";

    if (imageFileFormat == "JPEG")
        return ".jpg";

    Q_ASSERT(false);
    return "";
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SendImages::getSize ( int choice )
{
    switch (choice)
       {
       case 0:
          return (320);
          break;
       case 1:
          return (640);
          break;
       case 2:
          return (800);
          break;
       case 3:
          return (1024);
          break;
       case 4:
          return (1280);
          break;
       default:
          return (800); // Default value...
          break;
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool SendImages::resizeImageProcess( const QString &SourcePath, const QString &DestPath,
                                     const QString &ImageFormat, const QString &ImageName,
                                     int SizeFactor, int ImageCompression)
{
    QImage img;

    if ( img.load(SourcePath) == true )
       {
       int w = img.width();
       int h = img.height();

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
               kdDebug (51000) << "Resizing failed. Aborting." << endl;
               return false;
               }

           img = scaleImg;
           }

        if ( !img.save(DestPath + ImageName, ImageFormat.latin1(), ImageCompression) )
           {
           kdDebug (51000) << "Saving failed with specific compression value. Aborting." << endl;
           return false;
           }

        return true;
        }

    return false;
}


/////////////////////////////////////// SLOTS ///////////////////////////////////////////////////////

void SendImages::slotMozillaExited(KProcess*)
{
    if ( m_mozillaStdErr.find("No running window found") != -1 )   // No remote Mozilla | Netscape |
       {                                                                  // Thunderbird env. loaded !
       m_mailAgentProc2 = new KProcess;                                   // Init a new env.

       if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Mozilla" )
          *m_mailAgentProc2 << "mozilla" << "-mail";
       else
          if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Thunderbird" )
             *m_mailAgentProc2 << m_thunderbirdUrl << "-mail";
          else
             *m_mailAgentProc2 << "netscape" << "-mail";

       // Start an instance of mozilla mail agent before a remote call.

       if ( m_mailAgentProc2->start() == false )
          {
          KMessageBox::error(kapp->activeWindow(), i18n("Cannot start '%1' program;\nplease "
                                     "check your installation.")
                                     .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
          }
       else
          {
          // Mozilla | Netscape | Thunderbird mail agent started correctly
          // -> start a remote mail agent with multiple attachments after the env. is loaded !

          m_mozillaTimer->start(5000, true);
          return;
          }
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SendImages::slotMozillaTimeout(void)
{
    m_mailAgentProc3 = new KProcess;

    if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Mozilla" )
       *m_mailAgentProc3 << "mozilla" << "-remote";
    else
       if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Thunderbird" )
          *m_mailAgentProc3 << m_thunderbirdUrl << "-remote";
       else
          *m_mailAgentProc3 << "netscape" << "-remote";

    QString Temp = "xfeDoCommand(composeMessage,attachment='";

    for ( KURL::List::Iterator it = m_filesSendList.begin() ; it != m_filesSendList.end() ; ++it )
        {
        Temp.append( "file://" );
        Temp.append( QFile::encodeName((*it).path()) );
        Temp.append( "," );
        }

    Temp.append("')");

    *m_mailAgentProc3 << Temp;

    if ( m_mailAgentProc3->start() == false )
       KMessageBox::error(kapp->activeWindow(), i18n("Cannot start '%1' program;\nplease "
                                  "check your installation.")
                                  .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SendImages::slotMozillaReadStderr(KProcess*, char *buffer, int buflen)
{
    m_mozillaStdErr = QString::fromLocal8Bit(buffer, buflen);
}

}  // NameSpace KIPISendimagesPlugin

#include "sendimages.moc"
