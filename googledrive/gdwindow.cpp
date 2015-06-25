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

#include "gdwindow.moc"

// Qt includes

#include <QPushButton>
#include <QButtonGroup>
#include <QProgressDialog>
#include <QPixmap>
#include <QCheckBox>
#include <QStringList>
#include <QSpinBox>
#include <QPointer>

// KDE includes

#include <kcombobox.h>
#include <klineedit.h>
#include <kmenu.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <ktabwidget.h>
#include <krun.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kdeversion.h>
#include <kwallet.h>
#include <kpushbutton.h>
#include <kurl.h>
#include <ktoolinvocation.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kpimageslist.h"
#include "kprogressdialog.h"
#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpversion.h"
#include "kpprogresswidget.h"
#include "gdtalker.h"
#include "gditem.h"
#include "gdalbum.h"
#include "picasawebalbum.h"
#include "gdwidget.h"
#include "picasawebtalker.h"

namespace KIPIGoogleDrivePlugin
{

GDWindow::GDWindow(const QString& tmpFolder,QWidget* const /*parent*/, const QString& serviceName)
    : KPToolDialog(0)
{
    m_serviceName = serviceName;
    m_tmp         = tmpFolder;
    m_imagesCount = 0;
    m_imagesTotal = 0;

    m_widget      = new GoogleDriveWidget(this, iface(), m_serviceName);
    
    setMainWidget(m_widget);
    setButtons(Help | User1 | Close);
    setDefaultButton(Close);
    setModal(false);  
    KPAboutData* about;
    
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        about = new KPAboutData(ki18n("Google Drive Export"),0,
                                       KAboutData::License_GPL,
                                       ki18n("A Kipi-plugin to export images "
                                             "to Google Drive"),
                                       ki18n("(c) 2013, Saurabh Patel\n"
                                             "(c) 2015, Shourya Singh Gupta")); 

        about->addAuthor(ki18n("Saurabh Patel"),ki18n("Author and maintainer"),
                         "saurabhpatel7717 at gmail dot com");
 
        about->addAuthor(ki18n( "Shourya Singh Gupta" ), ki18n("Developer"),
                         "shouryasgupta at gmail dot com");

        about->setHandbookEntry("googledrive");
        setAboutData(about);

        setWindowIcon(KIcon("kipi-googledrive"));
        setWindowTitle(i18n("Export to Google Drive"));
        setButtonGuiItem(User1,KGuiItem(i18n("Start Upload"),"network-workgroup",i18n("Start upload to Google Drive")));
        m_widget->setMinimumSize(700,500);
    }
    else
    {
        about = new KPAboutData(ki18n("PicasaWeb Export"),
                                       0,
                                       KAboutData::License_GPL,
                                       ki18n("A Kipi plugin to export image collections to "
                                             "PicasaWeb web service."),
                                       ki18n( "(c) 2007-2009, Vardhman Jain\n"
                                              "(c) 2008-2013, Gilles Caulier\n"
                                              "(c) 2009, Luka Renko\n"
                                              "(c) 2010, Jens Mueller\n"
                                              "(c) 2015, Shourya Singh Gupta"));

        about->addAuthor(ki18n( "Vardhman Jain" ), ki18n("Author and maintainer"),
                         "Vardhman at gmail dot com");

        about->addAuthor(ki18n( "Gilles Caulier" ), ki18n("Developer"),
                         "caulier dot gilles at gmail dot com");

        about->addAuthor(ki18n( "Luka Renko" ), ki18n("Developer"),
                         "lure at kubuntu dot org");

        about->addAuthor(ki18n( "Jens Mueller" ), ki18n("Developer"),
                         "tschenser at gmx dot de");
       
        about->addAuthor(ki18n( "Shourya Singh Gupta" ), ki18n("Developer"),
                         "shouryasgupta at gmail dot com");

        about->setHandbookEntry("picasawebexport");
        setAboutData(about);

        setWindowIcon(KIcon("kipi-picasa"));
        if(QString::compare(m_serviceName, QString("picasawebexport"), Qt::CaseInsensitive) == 0)
        {
            setWindowTitle(i18n("Export to Picasa Web Service"));
            setButtonGuiItem(User1,KGuiItem(i18n("Start Upload"),"network-workgroup",i18n("Start upload to Picasa Web Service")));
            m_widget->setMinimumSize(700,500);
        }
        else
        {
            setWindowTitle(i18n("Import from Picasa Web Service"));
            setButtonGuiItem(User1,KGuiItem(i18n("Start Download"),"network-workgroup",i18n("Start download from Picasaweb service")));
            m_widget->setMinimumSize(300, 400);
        }
    }

    connect(m_widget->m_imgList, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    connect(m_widget->m_changeUserBtn,SIGNAL(clicked()),
            this,SLOT(slotUserChangeRequest()));

    connect(m_widget->m_newAlbumBtn,SIGNAL(clicked()),
            this,SLOT(slotNewAlbumRequest()));

    connect(m_widget->m_reloadAlbumsBtn,SIGNAL(clicked()),
            this,SLOT(slotReloadAlbumsRequest()));

    connect(this,SIGNAL(user1Clicked()),
            this,SLOT(slotStartTransfer()));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotCloseClicked()));  

    //-------------------------------------------------------------------------

    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        m_albumDlg = new GDNewAlbum(this);
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

        connect(m_talker,SIGNAL(signalListAlbumsDone(int,QString,QList<GDFolder>)),
                this,SLOT(slotListAlbumsDone(int,QString,QList<GDFolder>)));

        connect(m_talker,SIGNAL(signalCreateFolderDone(int,QString)),
                this,SLOT(slotCreateFolderDone(int,QString)));

        connect(m_talker,SIGNAL(signalAddPhotoDone(int,QString)),
                this,SLOT(slotAddPhotoDone(int,QString)));

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
    }
    else
    {
	m_picasa_albumdlg = new PicasawebNewAlbum(this);
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

        connect(m_picsasa_talker,SIGNAL(signalListAlbumsDone(int,QString,QList<GDFolder>)),
                this,SLOT(slotListAlbumsDone(int,QString,QList<GDFolder>)));

        connect(m_picsasa_talker,SIGNAL(signalCreateAlbumDone(int,QString,QString)),
                this,SLOT(slotCreateFolderDone(int,QString,QString)));

        connect(m_picsasa_talker,SIGNAL(signalAddPhotoDone(int,QString)),
                this,SLOT(slotAddPhotoDone(int,QString)));

        connect(m_picsasa_talker, SIGNAL(signalListPhotosDone(int,QString,QList<GDPhoto>)),
               this, SLOT(slotListPhotosDoneForDownload(int,QString,QList<GDPhoto>)));

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
    }

}

GDWindow::~GDWindow()
{
    delete m_widget;
    delete m_albumDlg;
    delete m_talker;
}

void GDWindow::reactivate()
{
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    m_widget->progressBar()->hide();

    show();
}

void GDWindow::readSettings()
{
    KConfig config("kipirc");
    
    KConfigGroup grp;
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
        grp = config.group("Google Drive Settings");
    else
        grp = config.group("PicasawebExport Settings");

    m_currentAlbumId = grp.readEntry("Current Album",QString());
    refresh_token = grp.readEntry("refresh_token");
    
    if (grp.readEntry("Resize", false))
    {
        m_widget->m_resizeChB->setChecked(true);
        m_widget->m_dimensionSpB->setEnabled(true);
        m_widget->m_imageQualitySpB->setEnabled(true);
    }
    else
    {
        m_widget->m_resizeChB->setChecked(false);
        m_widget->m_dimensionSpB->setEnabled(false);
        m_widget->m_imageQualitySpB->setEnabled(false);
    }

    m_widget->m_dimensionSpB->setValue(grp.readEntry("Maximum Width",    1600));
    m_widget->m_imageQualitySpB->setValue(grp.readEntry("Image Quality", 90));
    
   if((QString::compare(m_serviceName, QString("picasawebexport"), Qt::CaseInsensitive) == 0) || (QString::compare(m_serviceName, QString("picasawebimport"), Qt::CaseInsensitive) == 0))
       m_widget->m_tagsBGrp->button(grp.readEntry("Tag Paths", 0))->setChecked(true);

    KConfigGroup dialogGroup;
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        dialogGroup = config.group("Google Drive Export Dialog");
    }
    else if(QString::compare(m_serviceName, QString("picasawebexport"), Qt::CaseInsensitive) == 0)
    {
        dialogGroup = config.group("Picasaweb Export Dialog"); 
    }
    else
    {
        dialogGroup = config.group("Picasaweb Import Dialog");
    }
    restoreDialogSize(dialogGroup);
}

void GDWindow::writeSettings()
{
    KConfig config("kipirc");
    
    KConfigGroup grp;
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
        grp = config.group("Google Drive Settings");
    else
        grp = config.group("PicasawebExport Settings");
    
    grp.writeEntry("refresh_token",refresh_token);
    grp.writeEntry("Current Album",m_currentAlbumId);
    grp.writeEntry("Resize",          m_widget->m_resizeChB->isChecked());
    grp.writeEntry("Maximum Width",   m_widget->m_dimensionSpB->value());
    grp.writeEntry("Image Quality",   m_widget->m_imageQualitySpB->value());
    
   if((QString::compare(m_serviceName, QString("picasawebexport"), Qt::CaseInsensitive) == 0) || (QString::compare(m_serviceName, QString("picasawebimport"), Qt::CaseInsensitive) == 0))
       grp.writeEntry("Tag Paths",     m_widget->m_tagsBGrp->checkedId());

    KConfigGroup dialogGroup;
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        dialogGroup = config.group("Google Drive Export Dialog");
    }
    else if(QString::compare(m_serviceName, QString("picasawebexport"), Qt::CaseInsensitive) == 0)
    {
        dialogGroup = config.group("Picasaweb Export Dialog"); 
    }
    else
    {
        dialogGroup = config.group("Picasaweb Import Dialog");
    }
    
    saveDialogSize(dialogGroup);  
    config.sync();
}

void GDWindow::slotSetUserName(const QString& msg)
{
    m_widget->updateLabels(msg,"");
}

void GDWindow::slotListPhotosDoneForDownload(int errCode, const QString& errMsg, const QList <GDPhoto>& photosList)
{
    
}

void GDWindow::slotListAlbumsDone(int code,const QString& errMsg ,const QList <GDFolder>& list)
{
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        if(code == 0)
        {
            KMessageBox::error(this, i18n("Google Drive call failed:\n%1", errMsg));
            return;   
        }
    
        m_widget->m_albumsCoB->clear();
        kDebug() << "slotListAlbumsDone1:" << list.size();

        for(int i=0;i<list.size();i++)
        {
            m_widget->m_albumsCoB->addItem(KIcon("system-users"),list.value(i).title,
                                           list.value(i).id);

            if (m_currentAlbumId == list.value(i).id)
            {
                m_widget->m_albumsCoB->setCurrentIndex(i);
            }
        }

        buttonStateChange(true);
        m_talker->getUserName();              
    }
    else
    {
        if(code == 0)
        {
            KMessageBox::error(this, i18n("Picasaweb Call Failed: %1\n", errMsg));
            return;
        }
            
        m_widget->updateLabels(m_picsasa_talker->getLoginName(), m_picsasa_talker->getUserName());
        m_widget->m_albumsCoB->clear();

        for (int i = 0; i < list.size(); ++i)
        {
            QString albumIcon;

            if (list.at(i).access == "public")
                albumIcon = "folder-image";
            else if (list.at(i).access == "protected")
                albumIcon = "folder-locked";
            else
                albumIcon = "folder";

            m_widget->m_albumsCoB->addItem(KIcon(albumIcon), list.at(i).title, list.at(i).id);

            if (m_currentAlbumId == list.at(i).id)
                m_widget->m_albumsCoB->setCurrentIndex(i);
            
            buttonStateChange(true);  
        }
    }
}

void GDWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
        m_widget->m_changeUserBtn->setEnabled(false);
        buttonStateChange(false);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        m_widget->m_changeUserBtn->setEnabled(true);
        buttonStateChange(true);
    }
}

void GDWindow::slotTextBoxEmpty()
{
    kDebug() << "in slotTextBoxEmpty";
    KMessageBox::error(this, i18n("The textbox is empty, please enter the code from the browser in the textbox. "
                                  "To complete the authentication click \"Change Account\", "
                                  "or \"Start Upload\" to authenticate again."));
}

void GDWindow::slotStartTransfer()
{
    m_widget->m_imgList->clearProcessedStatus();

    if(m_widget->m_imgList->imageUrls().isEmpty())
    {
        if (KMessageBox::warningContinueCancel(this, i18n("No image selected. Please select which images should be uploaded."))
            == KMessageBox::Continue)
        {
             return;
        }

        return;
    }
 
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        if(!(m_talker->authenticated()))
        {
            if (KMessageBox::warningContinueCancel(this, i18n("Authentication failed. Click \"Continue\" to authenticate."))
                == KMessageBox::Continue)
            {
                m_talker->doOAuth();
                return;
            }
            else
            {
                return;
            }
        }  
    }
    else
    {
        if(!(m_picsasa_talker->authenticated()))
        {
            if (KMessageBox::warningContinueCancel(this, i18n("Authentication failed. Click \"Continue\" to authenticate."))
                == KMessageBox::Continue)
            {
                m_picsasa_talker->doOAuth();
                return;
            }
            else
            {
                return;
            }
        }      
    }

    typedef QPair<KUrl, GDPhoto> Pair;

    for(int i=0 ;i < (m_widget->m_imgList->imageUrls().size()) ; i++)
    {
        KPImageInfo info(m_widget->m_imgList->imageUrls().value(i));
        GDPhoto temp;
        kDebug() << "in start transfer info " <<info.title() << info.description();
        
        if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
            temp.title      = info.title();
        else
            temp.title      = info.name();
        
        temp.description    = info.description().section("\n",0,0);
        temp.gpsLat.setNum(info.latitude());
        temp.gpsLon.setNum(info.longitude());
        temp.tags = info.tagsPath();

        m_transferQueue.append(Pair(m_widget->m_imgList->imageUrls().value(i),temp));
    }

    m_currentAlbumId = m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toString();
    m_imagesTotal    = m_transferQueue.count();
    m_imagesCount    = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(0);
    m_widget->progressBar()->show();
    m_widget->progressBar()->progressScheduled(i18n("Google Drive export"), true, true);
    m_widget->progressBar()->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));

    uploadNextPhoto();
}

void GDWindow::uploadNextPhoto()
{
    kDebug() << "in upload nextphoto " << m_transferQueue.count();

    if(m_transferQueue.isEmpty())
    {
        //m_widget->progressBar()->hide();
        m_widget->progressBar()->progressCompleted();
        return;
    }

    typedef QPair<KUrl,GDPhoto> Pair;
    Pair pathComments = m_transferQueue.first();
    GDPhoto info      = pathComments.second;
    bool res;
    
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        res = m_talker->addPhoto(pathComments.first.toLocalFile(),info,m_currentAlbumId,
                                 m_widget->m_resizeChB->isChecked(),
                                 m_widget->m_dimensionSpB->value(),
                                 m_widget->m_imageQualitySpB->value());
      
    }
    else
    {
        //adjust tags according to radio button clicked
        switch (m_widget->m_tagsBGrp->checkedId())
        {
            case PwTagLeaf:
            {
                QStringList newTags;
                QStringList::const_iterator itT;

                for(itT = info.tags.constBegin(); itT != info.tags.constEnd(); ++itT)
                {
                    QString strTmp = *itT;
                    int idx        = strTmp.lastIndexOf("/");

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

                for(itT = info.tags.constBegin(); itT != info.tags.constEnd(); ++itT)
                {
                    QStringList strListTmp = itT->split('/');
                    QStringList::const_iterator itT2;

                    for(itT2 = strListTmp.constBegin(); itT2 != strListTmp.constEnd(); ++itT2)
                    {
                        if (!newTagsSet.contains(*itT2))
                        {
                            newTagsSet.insert(*itT2);
                        }
                    }
                }

                info.tags.clear();
                QSet<QString>::const_iterator itT3;

                for(itT3 = newTagsSet.begin(); itT3 != newTagsSet.end(); ++itT3)
                {
                    info.tags.append(*itT3);
                }

                break;
            }

            case PwTagCombined:
            default:
                break;
        }
        
        res = m_picsasa_talker->addPhoto(pathComments.first.toLocalFile(),info,m_currentAlbumId,
                                         m_widget->m_resizeChB->isChecked(),
                                         m_widget->m_dimensionSpB->value(),
                                         m_widget->m_imageQualitySpB->value());  
    }
    
    if (!res)
    {
        slotAddPhotoDone(0,"");
        return;
    }
}

void GDWindow::slotAddPhotoDone(int err, const QString& msg)
{
    if(err == 0)
    {
        if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
        {
            if (KMessageBox::warningContinueCancel(this, i18n("Failed to upload photo to Google Drive.\n%1\nDo you want to continue?",msg))
                != KMessageBox::Continue)
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
            if (KMessageBox::warningContinueCancel(this, i18n("Failed to upload photo to Picasaweb.\n%1\nDo you want to continue?",msg))
                != KMessageBox::Continue)
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
    }
    else
    {
        // Remove photo uploaded from the list
        m_widget->m_imgList->removeItemByUrl(m_transferQueue.first().first);
        m_transferQueue.pop_front();
        m_imagesCount++;
        kDebug() << "In slotAddPhotoSucceeded" << m_imagesCount;
        m_widget->progressBar()->setMaximum(m_imagesTotal);
        m_widget->progressBar()->setValue(m_imagesCount);
        uploadNextPhoto();
    }
}

void GDWindow::slotImageListChanged()
{
    enableButton(User1, !(m_widget->m_imgList->imageUrls().isEmpty()));
}

void GDWindow::slotNewAlbumRequest()
{
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        if (m_albumDlg->exec() == QDialog::Accepted)
        {
            GDFolder newFolder;
            m_albumDlg->getAlbumTitle(newFolder);
            m_currentAlbumId = m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toString();
            m_talker->createFolder(newFolder.title,m_currentAlbumId);
        }
    }
    else
    {        
        if (m_picasa_albumdlg->exec() == QDialog::Accepted)
        {
            GDFolder newFolder;
            m_picasa_albumdlg->getAlbumProperties(newFolder);
            m_picsasa_talker->createAlbum(newFolder);
        }
    }
}

void GDWindow::slotReloadAlbumsRequest()
{
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
        m_talker->listFolders();
    else
        m_picsasa_talker->listAlbums();
}

void GDWindow::slotAccessTokenFailed(int errCode,const QString& errMsg)
{
    KMessageBox::error(this, i18nc("%1 is the error string, %2 is the error code",
                                   "An authentication error occurred: %1 (%2)",errMsg,errCode));
    return;
}

void GDWindow::slotAccessTokenObtained()
{
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
        m_talker->listFolders();
    else
        m_picsasa_talker->listAlbums();
}

void GDWindow::slotRefreshTokenObtained(const QString& msg)
{
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        refresh_token = msg;
        m_talker->listFolders();        
    }
    else
    {
        refresh_token = msg;
        m_picsasa_talker->listAlbums(); 
    }
}

void GDWindow::slotCreateFolderDone(int code, const QString& msg, const QString& albumId)
{
    if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
    {
        if(code == 0)
            KMessageBox::error(this, i18n("Google Drive call failed:\n%1", msg));
        else
            m_talker->listFolders();        
    }
    else
    {
        if(code == 0)
            KMessageBox::error(this, i18n("Picasaweb call failed:\n%1", msg));
        else
            m_currentAlbumId = albumId;
            m_picsasa_talker->listAlbums();
    }
    
}

void GDWindow::slotTransferCancel()
{
    m_transferQueue.clear();
    m_progressDlg->hide();
    m_talker->cancel();
}

void GDWindow::slotUserChangeRequest()
{
    KUrl url("https://accounts.google.com/logout");
    KToolInvocation::invokeBrowser(url.url());

    if (KMessageBox::warningContinueCancel(this, i18n("After you have been logged out in the browser, "
                                                      "click \"Continue\" to authenticate for another account"))
        == KMessageBox::Continue)
    {
        refresh_token = "";
        if(QString::compare(m_serviceName, QString("googledriveexport"), Qt::CaseInsensitive) == 0)
            m_talker->doOAuth();
        else
            m_picsasa_talker->doOAuth();
    }
}

void GDWindow::buttonStateChange(bool state)
{
    m_widget->m_newAlbumBtn->setEnabled(state);
    m_widget->m_reloadAlbumsBtn->setEnabled(state);
    enableButton(User1, state);
}

void GDWindow::slotCloseClicked()
{
    writeSettings();
    m_widget->imagesList()->listView()->clear();   
}

void GDWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
        return;

    writeSettings();
    m_widget->imagesList()->listView()->clear();
    e->accept();
}

} // namespace KIPIGoogleDrivePlugin
