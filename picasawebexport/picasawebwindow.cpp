/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010      by Jens Mueller <tschenser at gmx dot de>
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

#include "picasawebwindow.moc"

// Qt includes

#include <QFileInfo>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QCloseEvent>
#include <QButtonGroup>

// KDE includes

#include <kdeversion.h>
#include <kde_file.h>
#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmenu.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kpassworddialog.h>
#include <kprogressdialog.h>
#include <kio/renamedialog.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// LibKIPI includes

#include <libkipi/uploadwidget.h>

// Local includes

#include "kpimageslist.h"
#include "kpimageinfo.h"
#include "kpmetadata.h"
#include "kpversion.h"
#include "newalbumdialog.h"
#include "picasawebalbum.h"
#include "picasawebitem.h"
#include "picasawebtalker.h"
#include "picasawebwidget.h"
#include "picasawebreplacedialog.h"

namespace KIPIPicasawebExportPlugin
{

PicasawebWindow::PicasawebWindow(const QString& tmpFolder, bool import, QWidget* /*parent*/)
    : KPToolDialog(0)
{
    m_tmpPath.clear();
    m_tmpDir      = tmpFolder;
    m_import      = import;
    m_imagesCount = 0;
    m_imagesTotal = 0;
    m_widget      = new PicasawebWidget(this, iface(), import);

    setMainWidget(m_widget);
    setWindowIcon(KIcon("picasa"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);

    if (import)
    {
        setWindowTitle(i18n("Import from Picasa Web Service"));
        setButtonGuiItem(User1,
                         KGuiItem(i18n("Start Download"), "network-workgroup",
                                  i18n("Start download from Picasaweb service")));
        m_widget->setMinimumSize(300, 400);
    }
    else
    {
        setWindowTitle(i18n("Export to Picasa Web Service"));
        setButtonGuiItem(User1,
                         KGuiItem(i18n("Start Upload"), "network-workgroup",
                                  i18n("Start upload to Picasaweb service")));
        m_widget->setMinimumSize(700, 500);
    }

    connect(m_widget, SIGNAL(signalUserChangeRequest(bool)),
            this, SLOT(slotUserChangeRequest(bool)) );

    connect(m_widget->m_imgList, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()) );

    connect(m_widget->m_reloadAlbumsBtn, SIGNAL(clicked()),
            this, SLOT(slotReloadAlbumsRequest()) );

    connect(m_widget->m_newAlbumBtn, SIGNAL(clicked()),
            this, SLOT(slotNewAlbumRequest()) );

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartTransfer()) );

    // ------------------------------------------------------------------------

    KPAboutData* about = new KPAboutData(ki18n("PicasaWeb Export"),
                             0,
                             KAboutData::License_GPL,
                             ki18n("A Kipi plugin to export image collections to "
                                   "PicasaWeb web service."),
                             ki18n( "(c) 2007-2009, Vardhman Jain\n"
                             "(c) 2008-2012, Gilles Caulier\n"
                             "(c) 2009, Luka Renko\n"
                             "(c) 2010, Jens Mueller" ));

    about->addAuthor(ki18n( "Vardhman Jain" ), ki18n("Author and maintainer"),
                     "Vardhman at gmail dot com");

    about->addAuthor(ki18n( "Gilles Caulier" ), ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n( "Luka Renko" ), ki18n("Developer"),
                     "lure at kubuntu dot org");

    about->addAuthor(ki18n( "Jens Mueller" ), ki18n("Developer"),
                     "tschenser at gmx dot de");

    about->setHandbookEntry("picasawebexport");
    setAboutData(about);

    // ------------------------------------------------------------------------

    m_albumDlg = new PicasawebNewAlbum(this);
    m_talker   = new PicasawebTalker(this);

    connect(m_talker, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(m_talker, SIGNAL(signalLoginProgress(int,int,QString)),
            this, SLOT(slotLoginProgress(int,int,QString)));

    connect(m_talker, SIGNAL(signalLoginDone(int,QString)),
            this, SLOT(slotLoginDone(int,QString)));

    connect(m_talker, SIGNAL(signalAddPhotoDone(int,QString,QString)),
            this, SLOT(slotAddPhotoDone(int,QString,QString)));

    connect(m_talker, SIGNAL(signalCreateAlbumDone(int,QString,QString)),
            this, SLOT(slotCreateAlbumDone(int,QString,QString)));

    connect(m_talker, SIGNAL(signalListAlbumsDone(int,QString,QList<PicasaWebAlbum>)),
            this, SLOT(slotListAlbumsDone(int,QString,QList<PicasaWebAlbum>)));

    connect(m_talker, SIGNAL(signalGetPhotoDone(int,QString,QByteArray)),
            this, SLOT(slotGetPhotoDone(int,QString,QByteArray)));

    connect(m_widget->progressBar(), SIGNAL(signalProgressCanceled()),
            this, SLOT(slotStopAndCloseProgressBar()));

    // ------------------------------------------------------------------------

    readSettings();

    if (m_username.isEmpty())
        slotUserChangeRequest(false);
    else
        authenticate(m_token, m_username, m_password);
}

PicasawebWindow::~PicasawebWindow()
{
    delete m_talker;
    delete m_albumDlg;
}

void PicasawebWindow::slotStopAndCloseProgressBar()
{
    m_talker->cancel();
    m_transferQueue.clear();
    m_widget->m_imgList->cancelProcess();
    writeSettings();
    m_widget->imagesList()->listView()->clear();
    m_widget->progressBar()->progressCompleted();
    done(Close);
}

void PicasawebWindow::slotButtonClicked(int button)
{
    switch (button)
    {
        case Close:
            if (m_widget->progressBar()->isHidden())
            {
                writeSettings();
                m_widget->imagesList()->listView()->clear();
                m_widget->progressBar()->progressCompleted();
                done(Close);
            }
            else // cancel login/transfer
            {
                m_widget->progressBar()->progressCompleted();
                cancelProcessing();
            }
            break;
        case User1:
            slotStartTransfer();
            break;
        default:
             KDialog::slotButtonClicked(button);
             break;
    }
}

void PicasawebWindow::cancelProcessing()
{
    m_talker->cancel();
    m_transferQueue.clear();
    m_widget->m_imgList->cancelProcess();
    m_widget->progressBar()->hide();
}

void PicasawebWindow::reactivate()
{
    m_widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

void PicasawebWindow::authenticate(const QString& token, const QString& username, const QString& password)
{
    m_widget->progressBar()->show();
    m_widget->progressBar()->setFormat("");

    m_talker->authenticate(token, username, password);
}

void PicasawebWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group( "PicasawebExport Settings");
    m_token          = grp.readEntry("token");
    m_username       = grp.readEntry("username");
    //m_password     = grp.readEntry("password");
    m_currentAlbumID = grp.readEntry("Current Album");

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

    m_widget->m_dimensionSpB->setValue(grp.readEntry("Maximum Width", 1600));
    m_widget->m_imageQualitySpB->setValue(grp.readEntry("Image Quality", 85));
    m_widget->m_tagsBGrp->button(grp.readEntry("Tag Paths", 0))->setChecked(true);
}

void PicasawebWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("PicasawebExport Settings");
    kDebug() << "Writing token value as ########### " << m_talker->token() << " #######" ;
    grp.writeEntry("token",         m_talker->token());
    grp.writeEntry("username",      m_username);
    grp.writeEntry("Current Album", m_currentAlbumID);
    grp.writeEntry("Resize",        m_widget->m_resizeChB->isChecked());
    grp.writeEntry("Maximum Width", m_widget->m_dimensionSpB->value());
    grp.writeEntry("Image Quality", m_widget->m_imageQualitySpB->value());
    grp.writeEntry("Tag Paths",     m_widget->m_tagsBGrp->checkedId());
}

void PicasawebWindow::slotLoginProgress(int step, int maxStep, const QString &label)
{
    KPProgressWidget* progressBar = m_widget->progressBar();

    if (!label.isEmpty())
        progressBar->setFormat(label);

    if (maxStep > 0)
        progressBar->setMaximum(maxStep);

    progressBar->setValue(step);
}

void PicasawebWindow::slotLoginDone(int errCode, const QString& errMsg)
{
    m_widget->progressBar()->hide();

    if (errCode == 0 /*TODO&& m_talker->loggedIn()*/)
    {
        buttonStateChange(true);
        m_token = m_talker->token();
        m_widget->m_albumsCoB->clear();
        m_talker->listAlbums(m_talker->getUserName());
    }
    else
    {
        KMessageBox::error(this, i18n("Picasaweb Call Failed: %1\n", errMsg));
    }
}

void PicasawebWindow::slotListAlbumsDone(int errCode, const QString &errMsg,
                                    const QList <PicasaWebAlbum>& albumsList)
{
    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Picasaweb Call Failed: %1\n", errMsg));
        return;
    }

    m_username = m_talker->getUserName();
    m_widget->updateLabels(m_username);

    m_widget->m_albumsCoB->clear();
    for (int i = 0; i < albumsList.size(); ++i)
    {
        QString albumIcon;
        if (albumsList.at(i).access == "public")
            albumIcon = "folder-image";
        else if (albumsList.at(i).access == "protected")
            albumIcon = "folder-locked";
        else
            albumIcon = "folder";

        m_widget->m_albumsCoB->addItem(KIcon(albumIcon), albumsList.at(i).title, albumsList.at(i).id);

        if (m_currentAlbumID == albumsList.at(i).id)
            m_widget->m_albumsCoB->setCurrentIndex(i);
    }
}

void PicasawebWindow::slotListPhotosDoneForDownload(int errCode, const QString &errMsg,
                                                    const QList <PicasaWebPhoto>& photosList)
{
    disconnect(m_talker, SIGNAL(signalListPhotosDone(int,QString,QList<PicasaWebPhoto>)),
               this, SLOT(slotListPhotosDoneForDownload(int,QString,QList<PicasaWebPhoto>)));

    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Picasaweb Call Failed: %1\n", errMsg));
        return;
    }

    typedef QPair<KUrl,PicasaWebPhoto> Pair;
    m_transferQueue.clear();
    QList<PicasaWebPhoto>::const_iterator itPWP;
    for (itPWP = photosList.begin(); itPWP != photosList.end(); ++itPWP)
    {
        m_transferQueue.push_back(Pair((*itPWP).originalURL, (*itPWP)));
    }

    if (m_transferQueue.isEmpty())
        return;

    m_currentAlbumID = m_widget->m_albumsCoB->itemData(
                                 m_widget->m_albumsCoB->currentIndex()).toString();
    m_imagesTotal = m_transferQueue.count();
    m_imagesCount = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->show();

    m_renamingOpt = 0;

    // start download with first photo in queue
    downloadNextPhoto();
}

void PicasawebWindow::slotListPhotosDoneForUpload(int errCode, const QString &errMsg,
                                                  const QList <PicasaWebPhoto>& photosList)
{
    disconnect(m_talker, SIGNAL(signalListPhotosDone(int,QString,QList<PicasaWebPhoto>)),
               this, SLOT(slotListPhotosDoneForUpload(int,QString,QList<PicasaWebPhoto>)));

    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Picasaweb Call Failed: %1\n", errMsg));
        return;
    }

    typedef QPair<KUrl,PicasaWebPhoto> Pair;

    m_transferQueue.clear();

    KUrl::List urlList = m_widget->m_imgList->imageUrls(true);

    if (urlList.isEmpty())
        return;

    for (KUrl::List::ConstIterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
    {
        KPImageInfo info(*it);
        PicasaWebPhoto temp;
        temp.title = info.name();

        // Picasa doesn't support image titles. Include it in descriptions if needed.
        QStringList descriptions = QStringList() << info.title() << info.description();
        descriptions.removeAll("");
        temp.description = descriptions.join("\n\n");

        // check for existing items
        QString localId;
        KPMetadata meta;
        if (meta.load((*it).toLocalFile()))
        {
            localId = meta.getXmpTagString("Xmp.kipi.picasawebGPhotoId");
        }
        QList<PicasaWebPhoto>::const_iterator itPWP;
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

    m_currentAlbumID = m_widget->m_albumsCoB->itemData(
                                 m_widget->m_albumsCoB->currentIndex()).toString();
    m_imagesTotal = m_transferQueue.count();
    m_imagesCount = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(0);
    m_widget->progressBar()->show();
    m_widget->progressBar()->progressScheduled(i18n("Picasa Export"), true, true);
    m_widget->progressBar()->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));


    m_renamingOpt = 0;

    uploadNextPhoto();
}

void PicasawebWindow::buttonStateChange(bool state)
{
    m_widget->m_newAlbumBtn->setEnabled(state);
    m_widget->m_reloadAlbumsBtn->setEnabled(state);
    enableButton(User1, state);
}

void PicasawebWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
    m_widget->m_changeUserBtn->setEnabled(!val);
    buttonStateChange(!val);
}

void PicasawebWindow::slotUserChangeRequest(bool /*anonymous*/)
{
    kDebug() << "Slot Change User Request ";
    m_talker->authenticate();
}

void PicasawebWindow::slotReloadAlbumsRequest()
{
    m_talker->listAlbums(m_username);
}

void PicasawebWindow::slotNewAlbumRequest()
{
    kDebug() << "Slot New Album Request";

    if (m_albumDlg->exec() == QDialog::Accepted)
    {
        PicasaWebAlbum newAlbum;
        m_albumDlg->getAlbumProperties(newAlbum);
        m_talker->createAlbum(newAlbum);
    }
 }

void PicasawebWindow::slotStartTransfer()
{
    kDebug() << "slotStartTransfer invoked";

    if (m_import)
    {
        // list photos of the album, then start download
        connect(m_talker, SIGNAL(signalListPhotosDone(int,QString,QList<PicasaWebPhoto>)),
                this, SLOT(slotListPhotosDoneForDownload(int,QString,QList<PicasaWebPhoto>)));

        m_talker->listPhotos(m_username,
                             m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toString(),
                             m_widget->m_dlDimensionCoB->itemData(m_widget->m_dlDimensionCoB->currentIndex()).toString());

    }
    else
    {
        // list photos of the album, then start upload with add/update items
        connect(m_talker, SIGNAL(signalListPhotosDone(int,QString,QList<PicasaWebPhoto>)),
                this, SLOT(slotListPhotosDoneForUpload(int,QString,QList<PicasaWebPhoto>)));

        m_talker->listPhotos(m_username,
                             m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toString());

    }
}

bool PicasawebWindow::prepareImageForUpload(const QString& imgPath, bool isRAW)
{
    QImage image;
    if (isRAW)
    {
        kDebug() << "Get RAW preview " << imgPath;
        KDcrawIface::KDcraw::loadDcrawPreview(image, imgPath);
    }
    else
    {
       image.load(imgPath);
    }

    if (image.isNull())
        return false;

    // get temporary file name
    m_tmpPath  = m_tmpDir + QFileInfo(imgPath).baseName().trimmed() + ".jpg";

    // rescale image if requested
    int maxDim = m_widget->m_dimensionSpB->value();

    if (m_widget->m_resizeChB->isChecked()
        && (image.width() > maxDim || image.height() > maxDim))
    {
        kDebug() << "Resizing to " << maxDim;
        image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);
    }

    kDebug() << "Saving to temp file: " << m_tmpPath;
    image.save(m_tmpPath, "JPEG", m_widget->m_imageQualitySpB->value());

    // copy meta data to temporary image
    KPMetadata meta;
    if (meta.load(imgPath))
    {
        meta.setImageDimensions(image.size());
        meta.setImageProgramId("Kipi-plugins", kipiplugins_version);
        // #225161 Picasaweb do not like XMP (exif-GPS is ignored if set)
        // follow Picasa 3 and remove XMP
        //meta.clearXmp();
        meta.save(m_tmpPath);
    }

    return true;
}

void PicasawebWindow::uploadNextPhoto()
{
    if (m_transferQueue.isEmpty())
    {
        m_widget->progressBar()->hide();
        m_widget->progressBar()->progressCompleted();
        return;
    }

    typedef QPair<KUrl,PicasaWebPhoto> Pair;
    Pair pathComments   = m_transferQueue.first();
    PicasaWebPhoto info = m_transferQueue.first().second;

    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(m_imagesCount);

    m_widget->m_imgList->processing(pathComments.first);
    QString imgPath  = pathComments.first.toLocalFile();
    QString itemPath = imgPath;

    bool res = false;
    KMimeType::Ptr ptr = KMimeType::findByUrl(imgPath);
    if(((ptr->is("image/bmp") ||
         ptr->is("image/gif") ||
         ptr->is("image/jpeg") ||
         ptr->is("image/png")) &&
        !m_widget->m_resizeChB->isChecked()) ||
       ptr->name().startsWith("video"))
    {
        m_tmpPath.clear();
    }
    else
    {
        // check if we have to RAW file -> use preview image then
        bool isRAW = KPMetadata::isRawFile(imgPath);

        if (!prepareImageForUpload(imgPath, isRAW))
        {
            slotAddPhotoDone(666, i18n("Cannot open file"), "");
            return;
        }
        itemPath = m_tmpPath;
    }

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
                PicasawebReplaceDialog dlg(this, "", m_interface, imgPath, info.thumbURL);
                switch(dlg.exec())
                {
                case PWR_ADD_ALL:
                    m_renamingOpt = PWR_ADD_ALL;
                case PWR_ADD:
                    bAdd = true;
                    break;
                case PWR_REPLACE_ALL:
                    m_renamingOpt = PWR_REPLACE_ALL;
                case PWR_REPLACE:
                    bAdd = false;
                    break;
                case PWR_CANCEL:
                default:
                    bCancel = true;
                    break;
                }
            }
            break;
        }
    }

    //modify tags
    switch (m_widget->m_tagsBGrp->checkedId())
    {
    case PwTagLeaf:
        {
            QStringList newTags;
            QStringList::const_iterator itT;
            for(itT = info.tags.constBegin(); itT != info.tags.constEnd(); ++itT)
            {
                QString strTmp = *itT;
                int idx = strTmp.lastIndexOf("/");
                if (idx > 0)
                {
                    strTmp.remove(0, idx + 1);
                }
                newTags.append(strTmp);
            }
            info.tags = newTags;
        }
        break;
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
        }
        break;
    case PwTagCombined:
    default:
        break;
    }

    if (bCancel)
    {
        cancelProcessing();
        res = true;
    }
    else
    {
        if(bAdd)
        {
            res = m_talker->addPhoto(itemPath, info, m_currentAlbumID);
        }
        else
        {
            res = m_talker->updatePhoto(itemPath, info);
        }
    }

    if (!res)
    {
        slotAddPhotoDone(666, i18n("Cannot open file"), "");
        return;
    }
}

void PicasawebWindow::slotAddPhotoDone(int errCode, const QString& errMsg, const QString& photoId)
{
    // Remove temporary file if it was used
    if (!m_tmpPath.isEmpty())
    {
        QFile::remove(m_tmpPath);
        m_tmpPath.clear();
    }

    KPMetadata meta;
    bool bRet        = false;
    QString fileName = m_transferQueue.first().first.path();

    if (!photoId.isEmpty() && meta.supportXmp() && meta.canWriteXmp(fileName) && meta.load(fileName))
    {
        bRet = meta.setXmpTagString("Xmp.kipi.picasawebGPhotoId", photoId, false);
        bRet = meta.save(fileName);
    }

    m_widget->m_imgList->processed(m_transferQueue.first().first, (errCode == 0));

    if (errCode == 0)
    {
        m_transferQueue.pop_front();
        m_imagesCount++;
    }
    else
    {
        if (KMessageBox::warningContinueCancel(this,
                         i18n("Failed to upload photo into Picasaweb: %1\n"
                              "Do you want to continue?", errMsg))
                         != KMessageBox::Continue)
        {
            m_transferQueue.clear();
            m_widget->progressBar()->hide();
            m_widget->progressBar()->progressCompleted();
            return;
        }
    }

    uploadNextPhoto();
}

void PicasawebWindow::downloadNextPhoto()
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

    m_talker->getPhoto(imgPath);
}

void PicasawebWindow::slotGetPhotoDone(int errCode, const QString& errMsg,
                                  const QByteArray& photoData)
{
    PicasaWebPhoto item = m_transferQueue.first().second;
    KUrl tmpUrl = QString(m_tmpDir + item.title);
    if (item.mimeType == "video/mpeg4")
    {
        tmpUrl.setFileName(item.title + ".mp4");
    }

    if (errCode == 0)
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
            bool bRet = false;
            if (meta.load(tmpUrl.toLocalFile()))
            {
                if (meta.supportXmp() && meta.canWriteXmp(tmpUrl.toLocalFile()))
                {
                    bRet = meta.setXmpTagString("Xmp.kipi.picasawebGPhotoId", item.id, false);
                    bRet = meta.setXmpKeywords(item.tags, false);
                }


                if (!item.gpsLat.isEmpty() && !item.gpsLon.isEmpty())
                {
                    bRet = meta.setGPSInfo(0.0, item.gpsLat.toDouble(), item.gpsLon.toDouble(), false);
                }
                bRet = meta.save(tmpUrl.toLocalFile());
            }

            m_transferQueue.pop_front();
            m_imagesCount++;
        }
        else
        {
            if (KMessageBox::warningContinueCancel(this,
                             i18n("Failed to save photo: %1\n"
                                  "Do you want to continue?", errText))
                             != KMessageBox::Continue)
            {
                cancelProcessing();
                return;
            }
        }
    }
    else
    {
        if (KMessageBox::warningContinueCancel(this,
                         i18n("Failed to download photo: %1\n"
                              "Do you want to continue?", errMsg))
                         != KMessageBox::Continue)
        {
            cancelProcessing();
            return;
        }
    }

    KUrl newUrl = QString(m_widget->getDestinationPath() + tmpUrl.fileName());
    bool bSkip = false;

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
            KMessageBox::error(this, i18n("Failed to save image to %1", newUrl.toLocalFile()));
        }
        else
        {
            KPImageInfo info(newUrl);
            info.setName(item.description);
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

void PicasawebWindow::slotTransferCancel()
{
    m_transferQueue.clear();
    m_widget->progressBar()->hide();
    m_widget->progressBar()->progressCompleted();

    m_talker->cancel();
}

void PicasawebWindow::slotCreateAlbumDone(int errCode, const QString& errMsg,
                                          const QString& newAlbumID)
{
    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Picasaweb Call Failed: %1\n", errMsg));
        return;
    }

    // reload album list and automatically select new album
    m_currentAlbumID = newAlbumID;
    m_talker->listAlbums(m_username);
}

void PicasawebWindow::slotImageListChanged()
{
    enableButton(User1, !(m_widget->m_imgList->imageUrls().isEmpty()));
}

} // namespace KIPIPicasawebExportPlugin
