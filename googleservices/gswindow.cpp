/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#include "gswindow.h"

// Qt includes

#include <QMessageBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QProgressDialog>
#include <QPixmap>
#include <QCheckBox>
#include <QStringList>
#include <QSpinBox>
#include <QFileInfo>
#include <QPointer>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

// KDE includes

#include <klocalizedstring.h>
#include <kconfig.h>
#include <kio/renamedialog.h>
#include <kwindowconfig.h>

// LibKIPI includes

#include <KIPI/Interface>

// Local includes

#include "kpimageslist.h"
#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpversion.h"
#include "kpprogresswidget.h"
#include "gdtalker.h"
#include "gsitem.h"
#include "newalbumdlg.h"
#include "gswidget.h"
#include "picasawebtalker.h"
#include "replacedialog.h"
#include "kipiplugins_debug.h"

namespace KIPIGoogleServicesPlugin
{

GSWindow::GSWindow(const QString& tmpFolder,QWidget* const /*parent*/, const QString& serviceName)
    : KPToolDialog(0)
{
    m_serviceName = serviceName;
    
    if(QString::compare(m_serviceName, QStringLiteral("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        name = PluginName::GDrive;
        m_pluginName = QStringLiteral("Google Drive");
    }
    else if(QString::compare(m_serviceName, QStringLiteral("picasawebexport"), Qt::CaseInsensitive) == 0)
    {
        name = PluginName::PicasaExport;
        m_pluginName = QStringLiteral("Google Photos/PicasaWeb");
    }
    else
    {
        name = PluginName::PicasaImport;
        m_pluginName = QStringLiteral("Google Photos/PicasaWeb");
    }
    
    m_tmp         = tmpFolder;
    m_imagesCount = 0;
    m_imagesTotal = 0;
    m_renamingOpt = 0;

    m_widget = new GoogleServicesWidget(this, iface(), name, m_pluginName);
    
    setMainWidget(m_widget);
    setModal(false);  
    KPAboutData* about;
    
    switch (name)
    {
        case PluginName::GDrive :
            
            about = new KPAboutData(ki18n("Google Drive Export"),0,
                                           KAboutLicense::GPL,
                                           ki18n("A Kipi-plugin to export images "
                                                 "to Google Drive"),
                                           ki18n("(c) 2013, Saurabh Patel\n"
                                                 "(c) 2015, Shourya Singh Gupta")); 

            about->addAuthor(i18n("Saurabh Patel"),i18n("Author and maintainer"),
                             QStringLiteral("saurabhpatel7717 at gmail dot com"));
 
            about->addAuthor(i18n( "Shourya Singh Gupta" ), i18n("Developer"),
                             QStringLiteral("shouryasgupta at gmail dot com"));

            about->setHandbookEntry(QStringLiteral("googledrive"));
            setAboutData(about);

            setWindowIcon(QIcon::fromTheme(QStringLiteral("kipi-googledrive")));
            setWindowTitle(i18n("Export to Google Drive"));

            startButton()->setText(i18n("Start Upload"));
            startButton()->setToolTip(i18n("Start upload to Google Drive"));

            m_widget->setMinimumSize(700,500);
            
            m_albumDlg = new NewAlbumDlg(this,m_serviceName,m_pluginName);
            m_talker = new GDTalker(this);

            connect(m_talker,SIGNAL(signalBusy(bool)),
                    this,SLOT(slotBusy(bool)));

            connect(m_talker,SIGNAL(signalTextBoxEmpty()),
                    this,SLOT(slotTextBoxEmpty()));

            connect(m_talker,SIGNAL(signalAccessTokenFailed(int,QString)),
                    this,SLOT(slotAccessTokenFailed(int,QString)));

            connect(m_talker,SIGNAL(signalAccessTokenObtained()),
                    this,SLOT(slotAccessTokenObtained()));

            connect(m_talker,SIGNAL(signalRefreshTokenObtained(QString)),
                    this,SLOT(slotRefreshTokenObtained(QString)));

            connect(m_talker,SIGNAL(signalSetUserName(QString)),
                    this,SLOT(slotSetUserName(QString)));

            connect(m_talker,SIGNAL(signalListAlbumsDone(int,QString,QList<GSFolder>)),
                    this,SLOT(slotListAlbumsDone(int,QString,QList<GSFolder>)));

            connect(m_talker,SIGNAL(signalCreateFolderDone(int,QString)),
                    this,SLOT(slotCreateFolderDone(int,QString)));

            connect(m_talker,SIGNAL(signalAddPhotoDone(int,QString,QString)),
                    this,SLOT(slotAddPhotoDone(int,QString,QString)));

            readSettings();
            buttonStateChange(false);

            if(refresh_token.isEmpty())
            {
                m_talker->doOAuth();
            }
            else
            {
                m_talker->getAccessTokenFromRefreshToken(refresh_token);
            }            
            
            break;
    
        case PluginName::PicasaImport :
        case PluginName::PicasaExport :
            
            about = new KPAboutData(ki18n("Google Photos/PicasaWeb Export"),
                                        0,
                                        KAboutLicense::GPL,
                                        ki18n("A Kipi plugin to export image collections to "
                                                "Google Photos/Picasa web service."),
                                        ki18n("(c) 2007-2009, Vardhman Jain\n"
                                                "(c) 2008-2013, Gilles Caulier\n"
                                                "(c) 2009, Luka Renko\n"
                                                "(c) 2010, Jens Mueller\n"
                                                "(c) 2015, Shourya Singh Gupta"));

            about->addAuthor(i18n( "Vardhman Jain" ), i18n("Author and maintainer"),
                             QStringLiteral("Vardhman at gmail dot com"));

            about->addAuthor(i18n( "Gilles Caulier" ), i18n("Developer"),
                             QStringLiteral("caulier dot gilles at gmail dot com"));

            about->addAuthor(i18n( "Luka Renko" ), i18n("Developer"),
                             QStringLiteral("lure at kubuntu dot org"));

            about->addAuthor(i18n( "Jens Mueller" ), i18n("Developer"),
                             QStringLiteral("tschenser at gmx dot de"));
        
            about->addAuthor(i18n( "Shourya Singh Gupta" ), i18n("Developer"),
                             QStringLiteral("shouryasgupta at gmail dot com"));

            about->setHandbookEntry(QStringLiteral("picasawebexport"));
            setAboutData(about);

            setWindowIcon(QIcon::fromTheme(QStringLiteral("kipi-picasa")));
            if(name == PluginName::PicasaExport)
            {
                setWindowTitle(i18n("Export to Google Photos/PicasaWeb Service"));

                startButton()->setText(i18n("Start Upload"));
                startButton()->setToolTip(i18n("Start upload to Google Photos/PicasaWeb Service"));

                m_widget->setMinimumSize(700, 500);
            }
            else
            {
                setWindowTitle(i18n("Import from Google Photos/PicasaWeb Service"));

                startButton()->setText(i18n("Start Download"));
                startButton()->setToolTip(i18n("Start download from Google Photos/PicasaWeb service"));

                m_widget->setMinimumSize(300, 400);
            }
            
            m_picasa_albumdlg = new NewAlbumDlg(this,m_serviceName,m_pluginName);
            m_picsasa_talker = new PicasawebTalker(this);
            
            connect(m_picsasa_talker,SIGNAL(signalBusy(bool)),
                    this,SLOT(slotBusy(bool)));

            connect(m_picsasa_talker,SIGNAL(signalTextBoxEmpty()),
                    this,SLOT(slotTextBoxEmpty()));

            connect(m_picsasa_talker,SIGNAL(signalAccessTokenFailed(int,QString)),
                    this,SLOT(slotAccessTokenFailed(int,QString)));

            connect(m_picsasa_talker,SIGNAL(signalAccessTokenObtained()),
                    this,SLOT(slotAccessTokenObtained()));

            connect(m_picsasa_talker,SIGNAL(signalRefreshTokenObtained(QString)),
                    this,SLOT(slotRefreshTokenObtained(QString)));

            connect(m_picsasa_talker,SIGNAL(signalListAlbumsDone(int,QString,QList<GSFolder>)),
                    this,SLOT(slotListAlbumsDone(int,QString,QList<GSFolder>)));

            connect(m_picsasa_talker,SIGNAL(signalCreateAlbumDone(int,QString,QString)),
                    this,SLOT(slotCreateFolderDone(int,QString,QString)));

            connect(m_picsasa_talker,SIGNAL(signalAddPhotoDone(int,QString,QString)),
                    this,SLOT(slotAddPhotoDone(int,QString,QString)));
            
            connect(m_picsasa_talker, SIGNAL(signalGetPhotoDone(int,QString,QByteArray)),
                    this, SLOT(slotGetPhotoDone(int,QString,QByteArray)));

            readSettings();
            buttonStateChange(false);

            if(refresh_token.isEmpty())
            {
                m_picsasa_talker->doOAuth();
            }
            else
            {
                m_picsasa_talker->getAccessTokenFromRefreshToken(refresh_token);
            }
            
            break;
    }

    connect(m_widget->imagesList(), SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    connect(m_widget->getChangeUserBtn(),SIGNAL(clicked()),
            this,SLOT(slotUserChangeRequest()));

    connect(m_widget->getNewAlbmBtn(),SIGNAL(clicked()),
            this,SLOT(slotNewAlbumRequest()));

    connect(m_widget->getReloadBtn(),SIGNAL(clicked()),
            this,SLOT(slotReloadAlbumsRequest()));

    connect(startButton(), SIGNAL(clicked()),
            this, SLOT(slotStartTransfer()));

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));
}

GSWindow::~GSWindow()
{
    delete m_widget;
    delete m_albumDlg;
    delete m_talker;
}

void GSWindow::reactivate()
{
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    m_widget->progressBar()->hide();

    show();
}

void GSWindow::readSettings()
{
    KConfig config(QStringLiteral("kipirc"));
    
    KConfigGroup grp;
    
    switch(name)
    {
        case PluginName::GDrive :
            grp = config.group("Google Drive Settings");
            break;
        default :
            grp = config.group("PicasawebExport Settings");
            break;
    }

    m_currentAlbumId = grp.readEntry("Current Album",QString());
    refresh_token = grp.readEntry("refresh_token");
    
    if (grp.readEntry("Resize", false))
    {
        m_widget->getResizeCheckBox()->setChecked(true);
        m_widget->getDimensionSpB()->setEnabled(true);
        m_widget->getImgQualitySpB()->setEnabled(true);
    }
    else
    {
        m_widget->getResizeCheckBox()->setChecked(false);
        m_widget->getDimensionSpB()->setEnabled(false);
        m_widget->getImgQualitySpB()->setEnabled(false);
    }

    m_widget->getDimensionSpB()->setValue(grp.readEntry("Maximum Width",    1600));
    m_widget->getImgQualitySpB()->setValue(grp.readEntry("Image Quality", 90));
    
    if(name == PluginName::PicasaExport)
        m_widget->m_tagsBGrp->button(grp.readEntry("Tag Paths", 0))->setChecked(true);

    KConfigGroup dialogGroup;
    
    switch(name)
    {
        case PluginName::GDrive :
            dialogGroup = config.group("Google Drive Export Dialog");
            break;
        case PluginName::PicasaExport :
            dialogGroup = config.group("Picasaweb Export Dialog"); 
            break;
        case PluginName::PicasaImport :
            dialogGroup = config.group("Picasaweb Import Dialog");
            break;
    }

    KWindowConfig::restoreWindowSize(windowHandle(), dialogGroup);
}

void GSWindow::writeSettings()
{
    KConfig config(QStringLiteral("kipirc"));
    
    KConfigGroup grp;
    switch(name)
    {
        case PluginName::GDrive :
            grp = config.group("Google Drive Settings");
            break;
        default :
            grp = config.group("PicasawebExport Settings");
            break;
    }
    
    grp.writeEntry("refresh_token",refresh_token);
    grp.writeEntry("Current Album",m_currentAlbumId);
    grp.writeEntry("Resize",          m_widget->getResizeCheckBox()->isChecked());
    grp.writeEntry("Maximum Width",   m_widget->getDimensionSpB()->value());
    grp.writeEntry("Image Quality",   m_widget->getImgQualitySpB()->value());
    
    if(name == PluginName::PicasaExport)
        grp.writeEntry("Tag Paths",     m_widget->m_tagsBGrp->checkedId());

    KConfigGroup dialogGroup;
    
    switch(name)
    {
        case PluginName::GDrive :
            dialogGroup = config.group("Google Drive Export Dialog");
            break;
        case PluginName::PicasaExport :
            dialogGroup = config.group("Picasaweb Export Dialog"); 
            break;
        case PluginName::PicasaImport :
            dialogGroup = config.group("Picasaweb Import Dialog");
            break;
    }

    KWindowConfig::saveWindowSize(windowHandle(), dialogGroup);
    config.sync();
}

void GSWindow::slotSetUserName(const QString& msg)
{
    m_widget->updateLabels(msg);
}

void GSWindow::slotListPhotosDoneForDownload(int errCode, const QString& errMsg, const QList <GSPhoto>& photosList)
{
    disconnect(m_picsasa_talker, SIGNAL(signalListPhotosDone(int,QString,QList<GSPhoto>)),
               this, SLOT(slotListPhotosDoneForDownload(int,QString,QList<GSPhoto>)));

    if (errCode == 0)
    {
        QMessageBox::critical(this, i18nc("@title:window", "Error"),
                              i18n("Google Photos/PicasaWeb Call Failed: %1\n", errMsg));
        return;
    }

    typedef QPair<QUrl,GSPhoto> Pair;
    m_transferQueue.clear();
    QList<GSPhoto>::const_iterator itPWP;

    for (itPWP = photosList.begin(); itPWP != photosList.end(); ++itPWP)
    {
        m_transferQueue.push_back(Pair((*itPWP).originalURL, (*itPWP)));
    }

    if (m_transferQueue.isEmpty())
        return;

    m_currentAlbumId = m_widget->getAlbumsCoB()->itemData(m_widget->getAlbumsCoB()->currentIndex()).toString();
    m_imagesTotal    = m_transferQueue.count();
    m_imagesCount    = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->show();

    m_renamingOpt = 0;

    // start download with first photo in queue
    downloadNextPhoto();
}

void GSWindow::slotListPhotosDoneForUpload(int errCode, const QString& errMsg, const QList <GSPhoto>& photosList)
{
    qCCritical(KIPIPLUGINS_LOG)<< "err Code is "<< errCode <<" Err Message is "<< errMsg;
    disconnect(m_picsasa_talker, SIGNAL(signalListPhotosDone(int,QString,QList<GSPhoto>)),
               this, SLOT(slotListPhotosDoneForUpload(int,QString,QList<GSPhoto>)));

    if (errCode == 0)
    {
        QMessageBox::critical(this, i18nc("@title:window", "Error"),
                              i18n("Google Photos/PicasaWeb Call Failed: %1\n", errMsg));
        return;
    }

    typedef QPair<QUrl,GSPhoto> Pair;

    m_transferQueue.clear();

    QList<QUrl> urlList = m_widget->imagesList()->imageUrls(true);

    if (urlList.isEmpty())
        return;

    for (QList<QUrl>::ConstIterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        KPImageInfo info(*it);
        GSPhoto temp;
        temp.title = info.name();

        // Picasa doesn't support image titles. Include it in descriptions if needed.
        QStringList descriptions = QStringList() << info.title() << info.description();
        descriptions.removeAll(QStringLiteral(""));
        temp.description         = descriptions.join(QStringLiteral("\n\n"));

        // check for existing items
        QString localId;
        KPMetadata meta;

        if (meta.load((*it).toLocalFile()))
        {
            localId = meta.getXmpTagString("Xmp.kipi.picasawebGPhotoId");
        }

        QList<GSPhoto>::const_iterator itPWP;

        for (itPWP = photosList.begin(); itPWP != photosList.end(); ++itPWP)
        {
            if ((*itPWP).id == localId)
            {
                temp.id       = localId;
                temp.editUrl  = (*itPWP).editUrl;
                temp.thumbURL = (*itPWP).thumbURL;
                break;
            }
        }

        //Tags from the database
        temp.gpsLat.setNum(info.latitude());
        temp.gpsLon.setNum(info.longitude());

        temp.tags = info.tagsPath();
        m_transferQueue.append( Pair( (*it), temp) );
    }

    if (m_transferQueue.isEmpty())
        return;

    m_currentAlbumId = m_widget->getAlbumsCoB()->itemData(m_widget->getAlbumsCoB()->currentIndex()).toString();
    m_imagesTotal    = m_transferQueue.count();
    m_imagesCount    = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(0);
    m_widget->progressBar()->show();
    m_widget->progressBar()->progressScheduled(i18n("Picasa Export"), true, true);
    m_widget->progressBar()->progressThumbnailChanged(
        QIcon::fromTheme(QStringLiteral("kipi")).pixmap(22, 22));

    m_renamingOpt = 0;

    uploadNextPhoto();
}

void GSWindow::slotListAlbumsDone(int code,const QString& errMsg ,const QList <GSFolder>& list)
{
    
    switch (name)
    {
        case PluginName::GDrive :
            if(code == 0)
            {
                QMessageBox::critical(this, i18nc("@title:window", "Error"),
                                      i18n("Google Drive Call Failed: %1\n", errMsg));
                return;   
            }
    
            m_widget->getAlbumsCoB()->clear();
            qCDebug(KIPIPLUGINS_LOG) << "slotListAlbumsDone1:" << list.size();

            for(int i=0;i<list.size();i++)
            {
                m_widget->getAlbumsCoB()->addItem(
                    QIcon::fromTheme(QStringLiteral("system-users")),
                    list.value(i).title, list.value(i).id);

                if (m_currentAlbumId == list.value(i).id)
                {
                    m_widget->getAlbumsCoB()->setCurrentIndex(i);
                }
            }

            buttonStateChange(true);
            m_talker->getUserName();
            break;
        
        default :
            if(code == 0)
            {
                QMessageBox::critical(this, i18nc("@title:window", "Error"),
                                      i18n("Google Photos/PicasaWeb Call Failed: %1\n", errMsg));
                return;
            }
            
            m_widget->updateLabels(m_picsasa_talker->getLoginName(), m_picsasa_talker->getUserName());
            m_widget->getAlbumsCoB()->clear();

            for (int i = 0; i < list.size(); ++i)
            {
                QString albumIcon;

                if (list.at(i).access == QStringLiteral("public"))
                    albumIcon = QStringLiteral("folder-image");
                else if (list.at(i).access == QStringLiteral("protected"))
                    albumIcon = QStringLiteral("folder-locked");
                else
                    albumIcon = QStringLiteral("folder");

                m_widget->getAlbumsCoB()->addItem(QIcon::fromTheme(albumIcon), list.at(i).title, list.at(i).id);

                if (m_currentAlbumId == list.at(i).id)
                    m_widget->getAlbumsCoB()->setCurrentIndex(i);
            
                buttonStateChange(true);  
            }
            break;
    }

}

void GSWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
        m_widget->getChangeUserBtn()->setEnabled(false);
        buttonStateChange(false);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        m_widget->getChangeUserBtn()->setEnabled(true);
        buttonStateChange(true);
    }
}

void GSWindow::picasaTransferHandler()
{
    qCDebug(KIPIPLUGINS_LOG) << "Picasa Transfer invoked";
    
    switch (name)
    {
    case PluginName::PicasaImport:
        // list photos of the album, then start download
        connect(m_picsasa_talker, SIGNAL(signalListPhotosDone(int,QString,QList<GSPhoto>)),
                this, SLOT(slotListPhotosDoneForDownload(int,QString,QList<GSPhoto>)));

        m_picsasa_talker->listPhotos(
            m_widget->getAlbumsCoB()->itemData(m_widget->getAlbumsCoB()->currentIndex()).toString(),
            m_widget->getDimensionCoB()->itemData(m_widget->getDimensionCoB()->currentIndex()).toString());
        break;

    default:
        // list photos of the album, then start upload with add/update items
        connect(m_picsasa_talker, SIGNAL(signalListPhotosDone(int,QString,QList<GSPhoto>)),
                this, SLOT(slotListPhotosDoneForUpload(int,QString,QList<GSPhoto>)));

        m_picsasa_talker->listPhotos(
            m_widget->getAlbumsCoB()->itemData(m_widget->getAlbumsCoB()->currentIndex()).toString());
        break;
    }
}

void GSWindow::slotTextBoxEmpty()
{
    qCDebug(KIPIPLUGINS_LOG) << "in slotTextBoxEmpty";
    QMessageBox::critical(this, i18nc("@title:window", "Error"),
                          i18n("The textbox is empty, please enter the code from the browser in the textbox. "
                                  "To complete the authentication click \"Change Account\", "
                                  "or \"Start Upload\" to authenticate again."));
}

void GSWindow::slotStartTransfer()
{
    m_widget->imagesList()->clearProcessedStatus();
    
    switch (name)
    {
        case PluginName::GDrive :
        case PluginName::PicasaExport :
            if(m_widget->imagesList()->imageUrls().isEmpty())
            {
                QMessageBox::critical(this, i18nc("@title:window", "Error"),
                                      i18n("No image selected. Please select which images should be uploaded."));
                return;                
            }   
            break;
        case PluginName::PicasaImport :
            break;
    }
    
    switch (name)
    {
        case PluginName::GDrive :
            if(!(m_talker->authenticated()))
            {
                QMessageBox warn(QMessageBox::Warning,
                                 i18n("Warning"),
                                 i18n("Authentication failed. Click \"Continue\" to authenticate."),
                                 QMessageBox::Yes | QMessageBox::No);
    
                (warn.button(QMessageBox::Yes))->setText(i18n("Continue"));
                (warn.button(QMessageBox::No))->setText(i18n("Cancel"));
            
                if (warn.exec() == QMessageBox::Yes)
                {
                    m_talker->doOAuth();
                    return;
                }
                else
                {
                    return;
                }
            }
            break;

        default :
            if(!(m_picsasa_talker->authenticated()))
            {
                QMessageBox warn(QMessageBox::Warning,
                                 i18n("Warning"),
                                 i18n("Authentication failed. Click \"Continue\" to authenticate."),
                                 QMessageBox::Yes | QMessageBox::No);
    
                (warn.button(QMessageBox::Yes))->setText(i18n("Continue"));
                (warn.button(QMessageBox::No))->setText(i18n("Cancel"));            
            
                if (warn.exec() == QMessageBox::Yes)
                {
                    m_picsasa_talker->doOAuth();
                    return;
                }
                else
                {
                    return;
                }
                
            }     
            picasaTransferHandler();
            return;
    }

    typedef QPair<QUrl, GSPhoto> Pair;

    for(int i=0 ;i < (m_widget->imagesList()->imageUrls().size()) ; i++)
    {
        KPImageInfo info(m_widget->imagesList()->imageUrls().value(i));
        GSPhoto temp;
        qCDebug(KIPIPLUGINS_LOG) << "in start transfer info " <<info.title() << info.description();
        
        switch (name)
        {
            case PluginName::GDrive :
                temp.title      = info.title();
                break;
            default :
                temp.title      = info.name();
                break;
        }
        
        temp.description    = info.description().section(QStringLiteral("\n"), 0, 0);
        temp.gpsLat.setNum(info.latitude());
        temp.gpsLon.setNum(info.longitude());
        temp.tags = info.tagsPath();

        m_transferQueue.append(Pair(m_widget->imagesList()->imageUrls().value(i),temp));
    }

    m_currentAlbumId = m_widget->getAlbumsCoB()->itemData(m_widget->getAlbumsCoB()->currentIndex()).toString();
    m_imagesTotal    = m_transferQueue.count();
    m_imagesCount    = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(0);
    m_widget->progressBar()->show();
    m_widget->progressBar()->progressScheduled(i18n("Google Drive export"), true, true);
    m_widget->progressBar()->progressThumbnailChanged(
        QIcon::fromTheme(QStringLiteral("kipi")).pixmap(22, 22));

    uploadNextPhoto();
}

void GSWindow::uploadNextPhoto()
{
    qCDebug(KIPIPLUGINS_LOG) << "in upload nextphoto " << m_transferQueue.count();

    if (m_transferQueue.isEmpty())
    {
        //m_widget->progressBar()->hide();
        m_widget->progressBar()->progressCompleted();
        return;
    }

    typedef QPair<QUrl,GSPhoto> Pair;
    Pair pathComments = m_transferQueue.first();
    GSPhoto info      = pathComments.second;
    m_widget->imagesList()->processing(pathComments.first);
    bool res = true;
    
    switch (name)
    {
        case PluginName::GDrive :
        {
            res = m_talker->addPhoto(pathComments.first.toLocalFile(),info,m_currentAlbumId,
                                    m_widget->getResizeCheckBox()->isChecked(),
                                    m_widget->getDimensionSpB()->value(),
                                    m_widget->getImgQualitySpB()->value());
            break;
        }

        case PluginName::PicasaExport :
        {    
            bool bCancel = false;
            bool bAdd    = true;

            if (!info.id.isEmpty() && !info.editUrl.isEmpty())
            {
                switch(m_renamingOpt)
                {
                    case PWR_ADD_ALL:
                        bAdd = true;
                        break;
                    case PWR_REPLACE_ALL:
                        bAdd = false;
                        break;
                    default:
                    {
                        ReplaceDialog dlg(this, QStringLiteral(""), iface(), pathComments.first, info.thumbURL);
                        dlg.exec();
                        
                        switch(dlg.getResult())
                        {
                            case PWR_ADD_ALL:
                                m_renamingOpt = PWR_ADD_ALL;
                                break;
                            case PWR_ADD:
                                bAdd = true;
                                break;
                            case PWR_REPLACE_ALL:
                                m_renamingOpt = PWR_REPLACE_ALL;
                                break;
                            case PWR_REPLACE:
                                bAdd = false;
                                break;
                            case PWR_CANCEL:
                            default:
                                bCancel = true;
                                break;
                        }

                        break;
                    }
                }
            }
            
            //adjust tags according to radio button clicked
            switch (m_widget->m_tagsBGrp->checkedId())
            {
                case PwTagLeaf:
                {
                    QStringList newTags;
                    QStringList::const_iterator itT;

                    for (itT = info.tags.constBegin(); itT != info.tags.constEnd(); ++itT)
                    {
                        QString strTmp = *itT;
                        int idx        = strTmp.lastIndexOf(QStringLiteral("/"));

                        if (idx > 0)
                        {
                            strTmp.remove(0, idx + 1);
                        }

                        newTags.append(strTmp);
                    }

                    info.tags = newTags;
                    break;
                }

                case PwTagSplit:
                {
                    QSet<QString> newTagsSet;
                    QStringList::const_iterator itT;

                    for (itT = info.tags.constBegin(); itT != info.tags.constEnd(); ++itT)
                    {
                        QStringList strListTmp = itT->split(QLatin1Char('/'));
                        QStringList::const_iterator itT2;

                        for (itT2 = strListTmp.constBegin(); itT2 != strListTmp.constEnd(); ++itT2)
                        {
                            if (!newTagsSet.contains(*itT2))
                            {
                                newTagsSet.insert(*itT2);
                            }
                        }
                    }

                    info.tags.clear();
                    QSet<QString>::const_iterator itT3;

                    for (itT3 = newTagsSet.begin(); itT3 != newTagsSet.end(); ++itT3)
                    {
                        info.tags.append(*itT3);
                    }

                    break;
                }

                case PwTagCombined:
                default:
                    break;
            }
            
            if (bCancel)
            {
                slotTransferCancel();
                res = true;
            }
            else
            {
                if (bAdd)
                {
                    res = m_picsasa_talker->addPhoto(pathComments.first.toLocalFile(),info,m_currentAlbumId,
                                                    m_widget->getResizeCheckBox()->isChecked(),
                                                    m_widget->getDimensionSpB()->value(),
                                                    m_widget->getImgQualitySpB()->value());     
                }
                else
                {
                    res = m_picsasa_talker->updatePhoto(pathComments.first.toLocalFile(), info,
                                                        m_widget->getResizeCheckBox()->isChecked(),
                                                        m_widget->getDimensionSpB()->value(),
                                                        m_widget->getImgQualitySpB()->value());
                }
            }
            break;
        }

        case PluginName::PicasaImport :
            break;
    }
    
    if (!res)
    {
        slotAddPhotoDone(0, QStringLiteral(""), QStringLiteral("-1"));
        return;
    }
}

void GSWindow::downloadNextPhoto()
{
    if (m_transferQueue.isEmpty())
    {
        m_widget->progressBar()->hide();
        m_widget->progressBar()->progressCompleted();
        return;
    }

    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(m_imagesCount);

    QString imgPath = m_transferQueue.first().first.url();

    m_picsasa_talker->getPhoto(imgPath);
}

void GSWindow::slotGetPhotoDone(int errCode, const QString& errMsg, const QByteArray& photoData)
{
    GSPhoto item = m_transferQueue.first().second;
    QUrl tmpUrl = QUrl::fromLocalFile(QString(m_tmp + item.title));

    if (item.mimeType == QStringLiteral("video/mpeg4"))
    {
        tmpUrl = tmpUrl.adjusted(QUrl::RemoveFilename);
        tmpUrl.setPath(tmpUrl.path() + item.title + QStringLiteral(".mp4"));
    }

    if (errCode == 1)
    {
        QString errText;
        QFile imgFile(tmpUrl.toLocalFile());

        if (!imgFile.open(QIODevice::WriteOnly))
        {
            errText = imgFile.errorString();
        }
        else if (imgFile.write(photoData) != photoData.size())
        {
            errText = imgFile.errorString();
        }
        else
        {
            imgFile.close();
        }

        if (errText.isEmpty())
        {
            KPMetadata meta;
            if (meta.load(tmpUrl.toLocalFile()))
            {
                if (meta.supportXmp() && meta.canWriteXmp(tmpUrl.toLocalFile()))
                {
                    meta.setXmpTagString("Xmp.kipi.picasawebGPhotoId", item.id, false);
                    meta.setXmpKeywords(item.tags, false);
                }


                if (!item.gpsLat.isEmpty() && !item.gpsLon.isEmpty())
                {
                    meta.setGPSInfo(0.0, item.gpsLat.toDouble(), item.gpsLon.toDouble(), false);
                }

                meta.save(tmpUrl.toLocalFile());
            }

            m_transferQueue.pop_front();
            m_imagesCount++;
        }
        else
        {
            QMessageBox warn(QMessageBox::Warning,
                             i18n("Warning"),
                             i18n("Failed to save photo: %1\n"
                                  "Do you want to continue?", errText),
                             QMessageBox::Yes | QMessageBox::No);
    
            (warn.button(QMessageBox::Yes))->setText(i18n("Continue"));
            (warn.button(QMessageBox::No))->setText(i18n("Cancel")); 
            
            if (warn.exec() != QMessageBox::Yes)
            {
                slotTransferCancel();
                return;
            }
        }
    }
    else
    {
        QMessageBox warn(QMessageBox::Warning,
                         i18n("Warning"),
                         i18n("Failed to download photo: %1\n"
                              "Do you want to continue?", errMsg),
                         QMessageBox::Yes | QMessageBox::No);
    
        (warn.button(QMessageBox::Yes))->setText(i18n("Continue"));
        (warn.button(QMessageBox::No))->setText(i18n("Cancel"));         
        
        if (warn.exec() != QMessageBox::Yes)
        {
            slotTransferCancel();
            return;
        }
    }

    QUrl newUrl = QUrl::fromLocalFile(QString(m_widget->getDestinationPath() + tmpUrl.fileName()));
    bool bSkip  = false;

    QFileInfo targetInfo(newUrl.toLocalFile());

    if (targetInfo.exists())
    {
        switch (m_renamingOpt)
        {
            case KIO::R_AUTO_SKIP:
                bSkip = true;
                break;

            case KIO::R_OVERWRITE_ALL:
                break;

            default:
            {
                KIO::RenameDialog dlg(this, i18n("A file named \"%1\" already "
                        "exists. Are you sure you want "
                        "to overwrite it?",
                        newUrl.fileName()),
                        tmpUrl, newUrl,
                        KIO::RenameDialog_Mode(KIO::M_MULTI | KIO::M_OVERWRITE | KIO::M_SKIP));

                switch (dlg.exec())
                {
                    case KIO::R_CANCEL:
                        m_transferQueue.clear();
                        bSkip = true;
                        break;

                    case KIO::R_AUTO_SKIP:
                        m_renamingOpt = KIO::R_AUTO_SKIP;
                    case KIO::R_SKIP:
                        bSkip = true;
                        break;

                    case KIO::R_RENAME:
                        newUrl = dlg.newDestUrl();
                        break;

                    case KIO::R_OVERWRITE_ALL:
                        m_renamingOpt = KIO::R_OVERWRITE_ALL;
                    case KIO::R_OVERWRITE:
                    default:    // Overwrite.
                        break;
                }
                break;
            }
        }
    }

    if (bSkip == true)
    {
        QFile::remove(tmpUrl.toLocalFile());
    }
    else
    {
        if (QFile::exists(newUrl.toLocalFile()))
        {
            QFile::remove(newUrl.toLocalFile());
        }
        //jmueller: rename from tmpDir to home does not work for me
        /*
        int ret;
#if KDE_IS_VERSION(4,2,85)
        // KDE 4.3.0
        // KDE::rename() takes care of QString -> bytestring encoding
        ret = KDE::rename(tmpUrl.toLocalFile(),
                          newUrl.toLocalFile());
#else
        // KDE 4.2.x or 4.1.x
        ret = KDE_rename(QFile::encodeName(tmpUrl.toLocalFile()),
                         newUrl.toLocalFile());
#endif
        if (ret != 0)
        */
        if (QFile::rename(tmpUrl.toLocalFile(), newUrl.toLocalFile()) == false)
        {
            QMessageBox::critical(this, i18nc("@title:window", "Error"),
                                  i18n("Failed to save image to %1", newUrl.toLocalFile()));
        }
        else
        {
            KPImageInfo info(newUrl);
            info.setName(item.title);
            info.setDescription(item.description);
            info.setTagsPath(item.tags);

            if (!item.gpsLat.isEmpty() && !item.gpsLon.isEmpty())
            {
                info.setLatitude(item.gpsLat.toDouble());
                info.setLongitude(item.gpsLon.toDouble());
            }
        }
    }

    downloadNextPhoto();
}

void GSWindow::slotAddPhotoDone(int err, const QString& msg, const QString& photoId)
{
    if(err == 0)
    {
        m_widget->imagesList()->processed(m_transferQueue.first().first,false);
        
        QMessageBox warn(QMessageBox::Warning,
                         i18n("Warning"),
                         i18n("Failed to upload photo to %1.\n%2\nDo you want to continue?",m_pluginName,msg),
                         QMessageBox::Yes | QMessageBox::No);
    
        (warn.button(QMessageBox::Yes))->setText(i18n("Continue"));
        (warn.button(QMessageBox::No))->setText(i18n("Cancel"));              
            
        if (warn.exec() != QMessageBox::Yes)
        {
            m_transferQueue.clear();
            m_widget->progressBar()->hide();
        }
        else
        {
            m_transferQueue.pop_front();
            m_imagesTotal--;
            m_widget->progressBar()->setMaximum(m_imagesTotal);
            m_widget->progressBar()->setValue(m_imagesCount);
            uploadNextPhoto();
        }
    }
    else
    {
        KPMetadata meta;
        QString fileName = m_transferQueue.first().first.path();

        if (!photoId.isEmpty() && meta.supportXmp() && meta.canWriteXmp(fileName) && meta.load(fileName))
        {
            meta.setXmpTagString("Xmp.kipi.picasawebGPhotoId", photoId, false);
            meta.save(fileName);
        }
        // Remove photo uploaded from the list
        m_widget->imagesList()->removeItemByUrl(m_transferQueue.first().first);
        m_transferQueue.pop_front();
        m_imagesCount++;
        qCDebug(KIPIPLUGINS_LOG) << "In slotAddPhotoSucceeded" << m_imagesCount;
        m_widget->progressBar()->setMaximum(m_imagesTotal);
        m_widget->progressBar()->setValue(m_imagesCount);
        uploadNextPhoto();
    }
}

void GSWindow::slotImageListChanged()
{
    startButton()->setEnabled(!(m_widget->imagesList()->imageUrls().isEmpty()));
}

void GSWindow::slotNewAlbumRequest()
{
    switch(name)
    {
        case PluginName::GDrive :
            if (m_albumDlg->exec() == QDialog::Accepted)
            {
                GSFolder newFolder;
                m_albumDlg->getAlbumProperties(newFolder);
                m_currentAlbumId = m_widget->getAlbumsCoB()->itemData(m_widget->getAlbumsCoB()->currentIndex()).toString();
                m_talker->createFolder(newFolder.title,m_currentAlbumId);
            }
            break;
            
        default :
            if (m_picasa_albumdlg->exec() == QDialog::Accepted)
            {
                GSFolder newFolder;
                m_picasa_albumdlg->getAlbumProperties(newFolder);
                m_picsasa_talker->createAlbum(newFolder);
            }
            break;
    }
}

void GSWindow::slotReloadAlbumsRequest()
{
    switch (name)
    {
        case PluginName::GDrive :
            break;
        case PluginName::PicasaImport :
        case PluginName::PicasaExport :
            break;
    }
}

void GSWindow::slotAccessTokenFailed(int errCode,const QString& errMsg)
{
    QMessageBox::critical(this, i18nc("@title:window", "Error"),
                          i18nc("%1 is the error string, %2 is the error code",
                                "An authentication error occurred: %1 (%2)",
                                errMsg, errCode));
    return;
}

void GSWindow::slotAccessTokenObtained()
{
    switch (name)
    {
        case PluginName::GDrive :
            m_talker->listFolders();
            break;
        case PluginName::PicasaImport :
        case PluginName::PicasaExport :
            m_picsasa_talker->listAlbums();
            break;
    }
}

void GSWindow::slotRefreshTokenObtained(const QString& msg)
{
    
    switch (name)
    {
        case PluginName::GDrive :
            refresh_token = msg;
            m_talker->listFolders();  
            break;
        case PluginName::PicasaImport :
        case PluginName::PicasaExport :
            refresh_token = msg;
            m_picsasa_talker->listAlbums(); 
            break;
    }
}

void GSWindow::slotCreateFolderDone(int code, const QString& msg, const QString& albumId)
{
    switch (name)
    {
        case PluginName::GDrive :
            if(code == 0)
                QMessageBox::critical(this, i18nc("@title:window", "Error"),
                                      i18n("Google Drive call failed:\n%1", msg));
            else
                m_talker->listFolders();
            break;
        case PluginName::PicasaImport :
        case PluginName::PicasaExport :
            if(code == 0)
                QMessageBox::critical(this, i18nc("@title:window", "Error"),
                                      i18n("Google Photos/PicasaWeb call failed:\n%1", msg));
            else
            {
                m_currentAlbumId = albumId;
                m_picsasa_talker->listAlbums();            
            }            
            break;
    }
    
}

void GSWindow::slotTransferCancel()
{
    m_transferQueue.clear();
    m_widget->progressBar()->hide();
    
    switch (name)
    {
        case PluginName::GDrive :
            m_talker->cancel();
            break;
        case PluginName::PicasaImport :
        case PluginName::PicasaExport :
            m_picsasa_talker->cancel();
            break;
    }
}

void GSWindow::slotUserChangeRequest()
{
    QUrl url(QStringLiteral("https://accounts.google.com/logout"));
    QDesktopServices::openUrl(url);

    QMessageBox warn(QMessageBox::Warning,
                     i18nc("@title:window", "Warning"),
                     i18n("After you have been logged out in the browser, "
                          "click \"Continue\" to authenticate for another account"),
                     QMessageBox::Yes | QMessageBox::No);
    
    (warn.button(QMessageBox::Yes))->setText(i18n("Continue"));
    (warn.button(QMessageBox::No))->setText(i18n("Cancel"));      
    
    if (warn.exec() == QMessageBox::Yes)
    {
        refresh_token = QStringLiteral("");
        
        switch (name)
        {
            case PluginName::GDrive :
                m_talker->doOAuth();
                break;
            case PluginName::PicasaImport :
            case PluginName::PicasaExport :
                m_picsasa_talker->doOAuth();
                break;
        }
    }
}

void GSWindow::buttonStateChange(bool state)
{
    m_widget->getNewAlbmBtn()->setEnabled(state);
    m_widget->getReloadBtn()->setEnabled(state);
    startButton()->setEnabled(state);
}

void GSWindow::slotFinished()
{
    writeSettings();
    m_widget->imagesList()->listView()->clear();
}

void GSWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

} // namespace KIPIGoogleServicesPlugin
