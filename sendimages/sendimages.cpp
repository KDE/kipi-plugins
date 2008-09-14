/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-02-25
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006 by Tom Albers <tomalbers at kde dot nl>
 * Copyright (C) 2006 by Michael Hoechstetter <michael dot hoechstetter at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes.

#include <qfileinfo.h>
#include <qdir.h>
#include <qimage.h>
#include <qprogressdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qtimer.h>
#include <qurl.h>
#include <qdeepcopy.h>

// KDE includes

#include <klocale.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kimageeffect.h>
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

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes.

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000106
#include <libkdcraw/dcrawbinary.h>
#endif

// Local includes.

#include "sendimagesdialog.h"
#include "listimageserrordialog.h"
#include "actions.h"
#include "pluginsversion.h"
#include "sendimages.h"
#include "sendimages.moc"

namespace KIPISendimagesPlugin
{

///Constructor: saves system handoff parameters in member variables
SendImages::SendImages(KIPI::Interface* interface, const QString &tmpFolder,
                       const KIPI::ImageCollection& imagesCollection, QObject *parent)
          : QObject(parent), QThread()
{
    m_invokedBefore = false;
    m_interface     = interface;
    m_tmp           = tmpFolder;
    m_collection    = imagesCollection;
    m_parent        = parent;
    m_mozillaTimer  = new QTimer(this);
    KImageIO::registerFormats();

    connect(m_mozillaTimer, SIGNAL(timeout()),
            this, SLOT(slotMozillaTimeout()));
}

///Destructor
SendImages::~SendImages()
{
    delete m_sendImagesDialog;
    wait();
}

///Invokes the User Dialog Window
void SendImages::showDialog()
{
    m_sendImagesDialog = new KIPISendimagesPlugin::SendImagesDialog(kapp->activeWindow(),
                             m_interface, m_collection);
    m_sendImagesDialog->show();

    connect(m_sendImagesDialog, SIGNAL(signalAccepted()),
            m_parent, SLOT(slotAcceptedConfigDlg()));
}

/** Execute the no threadable operations before the real thread.
    Gets input from the user dialog and store it into member variables */
void SendImages::prepare(void)
{
    m_filesSendList.clear();
    m_imagesResizedWithError.clear();
    m_imagesPackage.clear();
    m_images           = m_sendImagesDialog->m_images2send;
    m_changeProp       = m_sendImagesDialog->m_changeImagesProp->isChecked();
    m_imageFormat      = m_sendImagesDialog->m_imagesFormat->currentText();
    m_sizeFactor       = getSize( m_sendImagesDialog->m_imagesResize->currentItem() );
    m_imageCompression = m_sendImagesDialog->m_imageCompression->value();

    // Base64-encoding needs a lot of space.
    m_attachmentlimit = m_sendImagesDialog->m_attachmentlimit->value()*770000-2000; 
}

/** List of threaded operations.
    Prepares the image list. This includes resizing, copying, maintaining an image's exif, 
    and dropping evil characters out of filenames ;-) */
void SendImages::run()
{
    KIPISendimagesPlugin::EventData *d;

    d = new KIPISendimagesPlugin::EventData;
    d->action   = KIPISendimagesPlugin::Initialize;
    d->starting = true;
    d->success  = false;
    d->total    = m_images.count();
    QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

    for( KURL::List::Iterator it = m_images.begin() ; it != m_images.end() ; ++it )
    {
        QString imageName = (*it).path();
        QString ItemName  = imageName.section( '/', -1 );

        d = new KIPISendimagesPlugin::EventData;
        d->action    = KIPISendimagesPlugin::ResizeImages;
        d->fileName  = (*it).fileName();
        d->albumName = (*it).directory().section('/', -1);
        d->starting  = true;
        d->success   = false;
        QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

        // Prepare resized target images to send.
        QString imageFileName="";
        
        KIPI::ImageInfo info = m_interface->info( *it );

        ///Generate filename of destination image
        QString commentItem = info.description();

        if ((m_sendImagesDialog->m_comment2ImageName->isChecked() == true ) && 
                !commentItem.isEmpty() )
        {
                qDebug("commentItem: %s",commentItem.ascii());
        }
        else 
        {
                commentItem = ItemName.left(ItemName.findRev('.'));
                qDebug("commentItem is empty");
        }

        //QString TempFileName             = (*it).directory().section('/', -1);
        QString TempFileName=(*it).path().section('/', -2,-2)+"/"+commentItem+
                             +"."+(*it).path().section('.', -1,-1);
        qDebug("TempFileName: %s",TempFileName.ascii());
        
        
        // Thunderbird does not like (). Replace them, BUG:131343
        TempFileName.replace(QChar('('), "_").replace(QChar(')'), "_");
        // and these characters are better eliminated, too ;-)
        TempFileName.replace(QChar(','), "_").replace(QChar(' '), "_");
        TempFileName.replace(QChar(';'), "_").replace(QChar('%'), "_");
        TempFileName.replace(QChar('/'), "-").replace(QChar('?'), "");
        TempFileName.replace(QChar('"'), "");
        //If TempFileName already exists, add a number oder increase number
        if (entry_already_exists(m_filesSendList,m_tmp + TempFileName))
        {
            qDebug ("I entered");
            QString secondpart=(m_tmp+TempFileName).section(".",-1,-1);
            QString firstpart=
            (m_tmp+TempFileName).left((m_tmp+TempFileName).length()-secondpart.length()-1);
            qDebug("Firstpart: %s \n Secondpart: %s",firstpart.ascii(), secondpart.ascii());
            //Add _integer value in the end and prove again
            int int_index=2;
            QString index=QString::number(int_index,10);
            while (entry_already_exists(m_filesSendList,firstpart + "_"+index+"."+secondpart))
            {
                    int_index++;
                    index=QString::number(int_index,10);
                    qDebug("Index: %s",index.ascii());
            }
            QString temp=firstpart + "_"+index+"."+secondpart;
            TempFileName=temp.right(temp.length()-m_tmp.length());
            // .section("-",-2,-1);
            // m_tmp=(firstpart + "_"+index+"."+secondpart).section("-",-2);
        }

        if ( m_changeProp == true )
        {
            // Prepare resizing images.

            //QString imageNameFormat = TempFileName.replace(QChar('.'), "_") +
            //                          extension(m_imageFormat);
   
            qDebug( "Resizing %s-> '%s %s ' (%s ; %d )",imageName.ascii(),
            m_tmp.ascii(),TempFileName.ascii(),m_imageFormat.ascii(),m_sizeFactor);
 
            // Return value for resizeImageProcess-function, in order to avoid reopening 
            // the image for exiv-writing.
            QSize newsize;
    
            if ( resizeImageProcess( imageName, m_tmp, m_imageFormat, TempFileName,
                                     m_sizeFactor, m_imageCompression, newsize) == false )
            {
                // Resized images failed...
    
                d = new KIPISendimagesPlugin::EventData;
                d->action    = KIPISendimagesPlugin::ResizeImages;
                d->fileName  = (*it).fileName();
                d->albumName = (*it).directory().section('/', -1);
                d->starting  = false;
                d->success   = false;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));
    
                m_imagesResizedWithError.append(*it);
            } 
            else          // Resized images OK...
            {
                // Only try to write Exif if both src and destination are JPEG files.
    
                if (QString(QImageIO::imageFormat(imageName)).upper() == "JPEG" && 
                    m_imageFormat.upper() == "JPEG")
                {
                    QString targetFile = m_tmp + TempFileName;
                    KExiv2Iface::KExiv2 exiv2Iface;

                    if (exiv2Iface.load(imageName))
                    {
                        exiv2Iface.setImageProgramId(QString("Kipi-plugins"), QString(kipiplugins_version));
                        exiv2Iface.setImageDimensions(newsize);
                        exiv2Iface.save(targetFile);
                    }
                } 
                else 
                {
                    qWarning( "createThumb::No Exif Data Found") ;
                }

                d = new KIPISendimagesPlugin::EventData;
                d->action    = KIPISendimagesPlugin::ResizeImages;
                d->fileName  = (*it).fileName();
                d->albumName = (*it).directory().section('/', -1);
                d->starting  = false;
                d->success   = true;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

                m_filesSendList.append(m_tmp + TempFileName);
                m_imagesPackage.append(*it);
                m_imagesPackage.append(m_tmp + TempFileName);
            }
        }
        else     // No resize images operations...
        {
            if ( copyImageProcess( imageName, m_tmp, TempFileName) == true )
            {
                d = new KIPISendimagesPlugin::EventData;
                d->action    = KIPISendimagesPlugin::Progress;
                d->fileName  = (*it).fileName();
                d->albumName = (*it).directory().section('/', -1);
                d->starting  = true;
                d->success   = false;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

                m_filesSendList.append(m_tmp + TempFileName);
                m_imagesPackage.append(*it);
                m_imagesPackage.append(m_tmp + TempFileName);
            }
        }
    }

    d = new KIPISendimagesPlugin::EventData;
    d->action   = KIPISendimagesPlugin::Progress;
    d->starting = false;
    d->success  = true;
    QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));
}

bool SendImages::entry_already_exists(KURL::List filenamelist,QString entry)
{
        KURL::List::Iterator it = filenamelist.begin();
        while( it != filenamelist.end() )
        {
                if ((*it)==entry) return true;
                it++;
        }
        return false;
}

/// Creates a text file with the images comments.
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
                commentItem = i18n("no caption");
            else
                anyCommentsPresent = true;

            ImageCommentsText += i18n("Caption for image \"%1\": %2\n")
                                      .arg(targetFile).arg(commentItem);
            
            //Tags from the database
            if (m_interface->hasFeature(KIPI::HostSupportsTags))
            {
                QMap <QString, QVariant> attribs=info.attributes();
                if (attribs["tags"].asStringList().count() > 0)
                {
                    ImageCommentsText += i18n("Tags: %2\n").arg(attribs["tags"].asStringList().join(","));
                    anyCommentsPresent = true;
                }
            }
        
            ImageCommentsText += "\n";
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

///Shows up an error dialog and the problematic images
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
            case KDialogBase::Yes :        // Added source image files instead resized images...
        
                for ( KURL::List::Iterator it = m_imagesResizedWithError.begin();
                      it != m_imagesResizedWithError.end(); ++it )
                {
                    m_filesSendList.append(*it);
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

/** Returns a list of Filenames, whose sum filesize is smaller than the quota
    The returned images are deleted from the m_filesSendList */
KURL::List SendImages::divideEmails(void)
{
    unsigned long mylistsize=0;
    
    KURL::List sendnow;
    KURL::List filesSendList;

    for ( KURL::List::Iterator it = m_filesSendList.begin() ; it != m_filesSendList.end() ; ++it )
    {
        qDebug("m_attachmentlimit: %lu ", m_attachmentlimit);
        QString imageName = (*it).path();       
        qDebug("Imagename: %s", imageName.ascii());
        QFile file(imageName);
        qDebug("filesize: %lu", file.size());

        if ((mylistsize + file.size()) <= m_attachmentlimit)
        {
            mylistsize+=file.size();
            sendnow.append(*it);
            qDebug("mylistsize: %lu; attachmentlimit: %lu",mylistsize, m_attachmentlimit);
        }
        else 
        {
            qDebug("file %s is out of %lu",imageName.ascii(),m_attachmentlimit);
            filesSendList.append(*it);
        }
        }
        m_filesSendList = filesSendList;
  
        return sendnow;
}

/** Invokes mail agent. Depending on which mail agent to be used, we have different
    proceedings. Easy for every agent except of mozilla derivates */
bool SendImages::invokeMailAgent(void)
{
    bool agentInvoked = false;

    KURL::List filelist;
    kurllistdeepcopy(m_filesSendList_copy,m_filesSendList);

    qDebug("invokeMailagent1: Number of elements in m_filesSendList=%d, and in m_filesSendList_copy=%d)",(int)m_filesSendList.size(),(int)m_filesSendList_copy.size());

    while (!((filelist=divideEmails()).empty()))
    {
        qDebug("invokeMailagent2: Number of elements in m_filesSendList=%d, and in m_filesSendList_copy=%d)",(int) m_filesSendList.size(),(int)m_filesSendList_copy.size());
        qDebug("number of elements in filelist %d",(int)filelist.size());
        qDebug("number of elements in m_filelist %d", (int)m_filesSendList.size());     
        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Default" )
        {
            KApplication::kApplication()->invokeMailer(
                QString::null,                     // Destination address.
                QString::null,                     // Carbon Copy address.
                QString::null,                     // Blind Carbon Copy address
                QString::null,                     // Message Subject.
                QString::null,                     // Message Body.
                QString::null,                     // Message Body File.
                filelist.toStringList());          // Images attachments (+ comments).
            agentInvoked = true;
        }
        
        // KMail mail agent call.
    
        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "KMail" )
        {
            m_mailAgentProc = new KProcess;
            *m_mailAgentProc << "kmail";
    
        for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
        {
            *m_mailAgentProc << "--attach";
            *m_mailAgentProc << QFile::encodeName((*it).path());
        }
    
        if ( m_mailAgentProc->start() == false )
            KMessageBox::error(kapp->activeWindow(), 
                               i18n("Cannot start '%1' program;\nplease "
                                    "check your installation.")
                                    .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
        else
            agentInvoked = true;
        }
    
        // Claws Mail and Sylpheed mail agent call.
    
        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Claws Mail" ||
	         m_sendImagesDialog->m_mailAgentName->currentText() == "Sylpheed" ||
             m_sendImagesDialog->m_mailAgentName->currentText() == "Sylpheed-Claws" )
        {
            m_mailAgentProc = new KProcess;

            if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Claws Mail")
                *m_mailAgentProc << "claws-mail";
            else if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Sylpheed")
                *m_mailAgentProc << "sylpheed";
            else
                *m_mailAgentProc << "sylpheed-claws";
        
            *m_mailAgentProc << "--compose" << "--attach";
        
            for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
                *m_mailAgentProc << QFile::encodeName((*it).path());
        
            if ( m_mailAgentProc->start() == false )
                KMessageBox::error(kapp->activeWindow(), 
                                   i18n("Cannot start '%1' program;\nplease "
                                        "check your installation.")
                                        .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
            else
                agentInvoked = true;
        }
    
        // Balsa mail agent call.
    
        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Balsa" )
        {
            m_mailAgentProc = new KProcess;
            *m_mailAgentProc << "balsa" << "-m" << "mailto:";
        
            for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
            {
                *m_mailAgentProc << "-a";
                *m_mailAgentProc << QFile::encodeName((*it).path());
            }
        
            if ( m_mailAgentProc->start() == false )
                KMessageBox::error(kapp->activeWindow(), 
                                   i18n("Cannot start '%1' program;\nplease "
                                        "check your installation.")
                                        .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
            else
                agentInvoked = true;
        }
    
        // Evolution mail agent call.

        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Evolution" )    
        {
            m_mailAgentProc = new KProcess;
            *m_mailAgentProc << "evolution";
        
            QString Temp = "mailto:?subject=";
        
            for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
            {
                Temp.append("&attach=");
                Temp.append( QFile::encodeName((*it).path()) );
            }
        
            *m_mailAgentProc << Temp;
        
            if ( m_mailAgentProc->start() == false )
                KMessageBox::error(kapp->activeWindow(), 
                                   i18n("Cannot start '%1' program;\nplease "
                                        "check your installation.")
                                        .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
            else
                agentInvoked = true;
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
                qDebug("URL: %s", m_thunderbirdUrl.ascii());
            }
            else if ( m_sendImagesDialog->m_mailAgentName->currentText() == "GmailAgent" )
            {
                *m_mailAgentProc << "gmailagent" << "-remote";
            }
            else
            {
                *m_mailAgentProc << "netscape" << "-remote";
            }
        
            QString Temp = " xfeDoCommand(composeMessage,attachment='";
        
            for ( KURL::List::Iterator it = filelist.begin() ; it != filelist.end() ; ++it )
            {
                Temp.append( "file://" );
                QString toencode=(*it).encodedPathAndQuery();
                Temp.append(toencode);
                Temp.append( "," );
            }
    
            Temp.remove(Temp.length()-1,1);
            Temp.append("')");
            
            *m_mailAgentProc << Temp;
       
            if (!m_invokedBefore)
            {
                connect(m_mailAgentProc, SIGNAL(processExited(KProcess *)),
                        this, SLOT(slotMozillaExited(KProcess*)));
        
                connect(m_mailAgentProc, SIGNAL(receivedStderr(KProcess *, char*, int)),
                        this, SLOT(slotMozillaReadStderr(KProcess*, char*, int)));
            }
            qDebug ("%s", Temp.ascii());
        
            if ( m_mailAgentProc->start(KProcess::NotifyOnExit , KProcess::All) == false )
                KMessageBox::error(kapp->activeWindow(), 
                                   i18n("Cannot start '%1' program;\nplease "
                                        "check your installation.")
                                        .arg(m_sendImagesDialog->m_mailAgentName->currentText()));
            else
            {   agentInvoked = true;
                m_invokedBefore=true;
            }
        }
    }
    
    return agentInvoked;
}

///Cleans up the temp directory
void SendImages::removeTmpFiles(void)
{
    if (DeleteDir(m_tmp) == false)
       KMessageBox::error(kapp->activeWindow(), i18n("Cannot remove temporary folder %1.").arg(m_tmp));
}

///Checks if directory is empty and invokes its deletion
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

///Deletes a directory and all its contents - Please call it using "DeleteDir"
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

///Returns the file-extension of the corresponding fileformat
QString SendImages::extension(const QString& imageFileFormat)
{
    if (imageFileFormat == "PNG")
        return ".png";

    if (imageFileFormat == "JPEG")
        return ".jpg";

    Q_ASSERT(false);
    return "";
}

/** in sendimagesplugin dialog the user can select a compression of images
    this function returns the pixel-size of the selected entry */
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
       case 5:
          return (1600);
              break;
       default:
          return (800); // Default value...
          break;
    }
}

/** This function should copy the images to tempfolder in order to avoid suspicious filenames
    It is used, when no resizing should take place
    This function can be replaced with Qt4 QFile.copy */
bool SendImages::copyImageProcess(const QString &oldFilePath, const QString &DestPath,
                                  const QString &ImageName)
{
        //same file, no need to copy
        qDebug("DestPath: %s",(DestPath).ascii());
        qDebug("ImageName: %s",(ImageName).ascii());
        if(oldFilePath.compare(DestPath+ImageName) == 0)
                return true;

        //load both files
        QFile oldFile(oldFilePath);
        QFile newFile(DestPath+ImageName);
        bool openOld = oldFile.open( IO_ReadOnly );
        bool openNew = newFile.open( IO_WriteOnly );

        //if either file fails to open bail
        if(!openOld || !openNew) { return false; }

        //copy contents
        uint BUFFER_SIZE = 16000;
        char* buffer = new char[BUFFER_SIZE];
        while(!oldFile.atEnd())
        {
                Q_LONG len = oldFile.readBlock( buffer, BUFFER_SIZE );
                newFile.writeBlock( buffer, len );
        }

        //deallocate buffer
        delete[] buffer;
        buffer = NULL;
        return true;
}

///Resizes the Images before Sending...
bool SendImages::resizeImageProcess(const QString &SourcePath, const QString &DestPath,
                                    const QString &ImageFormat, const QString &ImageName,
                                    int SizeFactor, int ImageCompression, QSize &newsize)
{
    QImage img;

    // Check if RAW file.
#if KDCRAW_VERSION < 0x000106
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    QFileInfo fileInfo(SourcePath);
    if (rawFilesExt.upper().contains( fileInfo.extension(false).upper() ))
        KDcrawIface::KDcraw::loadDcrawPreview(img, SourcePath);
    else
        img.load(SourcePath);

    if ( !img.isNull() )
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
                qDebug ("Resizing failed. Aborting.");
                return false;
            }
    
            img = scaleImg;
            newsize=img.size();
        }
    
        if ( !img.save(DestPath + ImageName, ImageFormat.latin1(), ImageCompression) )
        {
            qDebug("Saving failed with specific compression value. Aborting.");
            return false;
        }
    
        return true;
    }

    return false;
}

/** If mozilla, or thunderbird or any derivate isn't already running, start it now, 
    wait 5 seconds and start SlotMozillaTimeout() */
void SendImages::slotMozillaExited(KProcess*)
{
    qDebug("slotMozillaExited");
    ///Here would be the right point to clear the sendlist in order to avoid infinite resendings!!
    m_filesSendList.clear();
    ///Also disconnect SLOT
    m_mailAgentProc->disconnect(SIGNAL(processExited(KProcess *)), this, SLOT(slotMozillaExited(KProcess*)));

    qDebug("Number of elements in m_filesSendList=%d, and in m_filesSendList_copy=%d)",(int)m_filesSendList.size(),(int)m_filesSendList_copy.size()); 
    if ( m_mozillaStdErr.find("No running window found") != -1 )   // No remote Mozilla | Netscape |
    {                                                              // Thunderbird env. loaded !
        m_mailAgentProc2 = new KProcess;                           // Init a new env.

        if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Mozilla" )
           *m_mailAgentProc2 << "mozilla" << "-mail";
        else if ( m_sendImagesDialog->m_mailAgentName->currentText() == "Thunderbird" )
           *m_mailAgentProc2 << m_thunderbirdUrl << "-mail";  ///for new versions of thunderbird, we don't need it anymore
           //*m_mailAgentProc2 << m_thunderbirdUrl;
        else
           *m_mailAgentProc2 << "netscape" << "-mail";

        // Start an instance of mozilla mail agent before a remote call.

        if ( m_mailAgentProc2->start() == false )
        {
            KMessageBox::error(kapp->activeWindow(), 
                               i18n("Cannot start '%1' program;\nplease "
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

///If Mozilla wasn't started before, now it is and so we can begin with the transmission
void SendImages::slotMozillaTimeout(void)
{        
    m_mozillaTimer->disconnect(SIGNAL(timeout()), this, SLOT(slotMozillaTimeout()));

    qDebug("slotMozillaTimeout: Number of elements in m_filesSendList=%d, and in m_filesSendList_copy=%d)",(int)m_filesSendList.size(),(int)m_filesSendList_copy.size());
    kurllistdeepcopy(m_filesSendList,m_filesSendList_copy);
    invokeMailAgent();

}

///Handles mozillas errors
void SendImages::slotMozillaReadStderr(KProcess*, char *buffer, int buflen)
{
    m_mozillaStdErr = QString::fromLocal8Bit(buffer, buflen);
}

///Makes a deep copy of a KURL-list: Real and slow copying instead of only pointer arithmetics
bool SendImages::kurllistdeepcopy(KURL::List &Destination, KURL::List Source)
{
    Destination.clear();
    qDebug("kurllistdeepcopy started");
    for ( KURL::List::Iterator it = Source.begin() ; it != Source.end() ; ++it )
    {
        //QString Tempstring;
        QString Getstring=(*it).path();
        QString Tempstring=Getstring.copy();
        Destination.append(Tempstring);
        qDebug("%s",Tempstring.ascii());
    }
    
    /*
    qDebug("deepcopytest");
    Source.clear();
    for ( KURL::List::Iterator it = Destination.begin() ; it != Destination.end() ; ++it )
    {
        qDebug("%s",(*it).path().ascii());
    }
    */

    qDebug("kurllistdeepcopy ended\n");
    return true;
}

}  // NameSpace KIPISendimagesPlugin
