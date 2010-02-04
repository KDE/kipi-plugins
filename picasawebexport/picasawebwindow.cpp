/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-16-07
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010 by Jens Mueller <tschenser at gmx dot de>
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

#include "picasawebwindow.h"
#include "picasawebwindow.moc"

// Qt includes

#include <QFileInfo>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QCloseEvent>

// KDE includes

#include <KDebug>
#include <KConfig>
#include <KLocale>
#include <KMenu>
#include <KHelpMenu>
#include <KComboBox>
#include <KLineEdit>
#include <KMessageBox>
#include <KPushButton>
#include <KPasswordDialog>
#include <KProgressDialog>
#include <KToolInvocation>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// Local includes

#include "imageslist.h"
#include "kpaboutdata.h"
#include "newalbumdialog.h"
#include "picasawebalbum.h"
#include "picasawebitem.h"
#include "picasawebtalker.h"
#include "picasawebwidget.h"
#include "pluginsversion.h"

namespace KIPIPicasawebExportPlugin
{

PicasawebWindow::PicasawebWindow(KIPI::Interface* interface, const QString& tmpFolder,
                                 bool import, QWidget* /*parent*/)
          : KDialog(0)
{
    m_tmpPath.clear();
    m_tmpDir      = tmpFolder;
    m_interface   = interface;
    m_import      = import;
    m_imagesCount = 0;
    m_imagesTotal = 0;
    m_widget      = new PicasawebWidget(this, interface, import);

    setMainWidget(m_widget);
    setWindowIcon(KIcon("picasa"));
    setButtons(Help|User1|Cancel);
    setDefaultButton(Cancel);
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


    connect(m_widget, SIGNAL( signalUserChangeRequest(bool) ),
            this, SLOT( slotUserChangeRequest(bool)) );

    connect(m_widget->m_imgList, SIGNAL( signalImageListChanged() ),
            this, SLOT( slotImageListChanged()) );

    connect(m_widget->m_reloadAlbumsBtn, SIGNAL( clicked() ),
            this, SLOT( slotReloadAlbumsRequest()) );

    connect(m_widget->m_newAlbumBtn, SIGNAL( clicked() ),
            this, SLOT( slotNewAlbumRequest()) );

    connect(this, SIGNAL( user1Clicked() ),
            this, SLOT( slotStartTransfer()) );

    // ------------------------------------------------------------------------

    m_about = new KIPIPlugins::KPAboutData(ki18n("PicasaWeb Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to export image collections to "
                                                 "PicasaWeb web service."),
                                           ki18n( "(c) 2007-2009, Vardhman Jain\n"
                                           "(c) 2008-2009, Gilles Caulier\n"
                                           "(c) 2009, Luka Renko\n"
                                           "(c) 2010, Jens Mueller" ));

    m_about->addAuthor(ki18n( "Vardhman Jain" ), ki18n("Author and maintainer"),
                       "Vardhman at gmail dot com");

    m_about->addAuthor(ki18n( "Gilles Caulier" ), ki18n("Developer"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor(ki18n( "Luka Renko" ), ki18n("Developer"),
                       "lure at kubuntu dot org");

    m_about->addAuthor(ki18n( "Jens Mueller" ), ki18n("Developer"),
                       "tschenser at gmx dot de");

    disconnect(this, SIGNAL( helpClicked() ),
               this, SLOT( slotHelp()) );

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    m_albumDlg  = new PicasawebNewAlbum(this);

    m_talker = new PicasawebTalker(this);

    connect(m_talker, SIGNAL( signalBusy(bool) ),
            this, SLOT( slotBusy(bool) ));

    connect(m_talker, SIGNAL( signalLoginProgress(int, int, const QString&) ),
            this, SLOT( slotLoginProgress(int, int, const QString&) ));

    connect(m_talker, SIGNAL( signalLoginDone(int, const QString&) ),
            this, SLOT( slotLoginDone(int, const QString&) ));

    connect(m_talker, SIGNAL( signalAddPhotoDone(int, const QString&) ),
            this, SLOT( slotAddPhotoDone(int, const QString&) ));

    connect(m_talker, SIGNAL( signalCreateAlbumDone(int, const QString&, int) ),
            this, SLOT( slotCreateAlbumDone(int, const QString&, int) ));

    connect(m_talker, SIGNAL( signalListAlbumsDone(int, const QString&, const QList <PicasaWebAlbum>&) ),
            this, SLOT( slotListAlbumsDone(int, const QString&, const QList <PicasaWebAlbum>&) ));

    connect(m_talker, SIGNAL( signalListPhotosDone(int, const QString&, const QList <PicasaWebPhoto>&) ),
            this, SLOT( slotListPhotosDone(int, const QString&, const QList <PicasaWebPhoto>&) ));

    connect(m_talker, SIGNAL( signalGetPhotoDone(int, const QString&, const QByteArray&) ),
            this, SLOT( slotGetPhotoDone(int, const QString&, const QByteArray&) ));
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
    delete m_about;
}

void PicasawebWindow::slotHelp()
{
    KToolInvocation::invokeHelp("picasawebexport", "kipi-plugins");
}

void PicasawebWindow::slotButtonClicked(int button)
{
    kDebug() << "TODO: FbWindow::slotButtonClicked";
    switch (button)
    {
        case Cancel:
            if (m_widget->progressBar()->isHidden())
            {
                writeSettings();
                m_widget->imagesList()->listView()->clear();
                done(Close);
            }
            else // cancel login/transfer
            {
                m_talker->cancel();
                m_transferQueue.clear();
                m_widget->m_imgList->processed(false);
                m_widget->progressBar()->hide();
            }
            break;
        case User1:
            slotStartTransfer();
            break;
        default:
             KDialog::slotButtonClicked(button);
    }
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
    //m_password       = grp.readEntry("password");
    m_currentAlbumID  = grp.readEntry("Current Album");

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
}

void PicasawebWindow::slotLoginProgress(int step, int maxStep, const QString &label)
{
    QProgressBar* progressBar = m_widget->progressBar();

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
        m_username = m_talker->getUserName();
        m_token = m_talker->token();
        m_widget->updateLabels(m_username);
        m_widget->m_albumsCoB->clear();
        m_talker->listAlbums(m_username);
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

void PicasawebWindow::slotListPhotosDone(int errCode, const QString &errMsg,
                                    const QList <PicasaWebPhoto>& photosList)
{
    if (errCode != 0)
    {
        KMessageBox::error(this, i18n("Picasaweb Call Failed: %1\n", errMsg));
        return;
    }

    typedef QPair<KUrl,FPhotoInfo> Pair;
    m_transferQueue.clear();
    for (int i = 0; i < photosList.size(); ++i)
    {
        FPhotoInfo info;
        info.title = photosList.at(i).title;
        info.description = photosList.at(i).description;
        m_transferQueue.push_back(Pair(photosList.at(i).originalURL, info));
    }

    if (m_transferQueue.isEmpty())
        return;

    m_imagesTotal = m_transferQueue.count();
    m_imagesCount = 0;

    m_widget->progressBar()->setFormat(i18n("%v / %m"));
    m_widget->progressBar()->show();

    // start download with first photo in queue
    downloadNextPhoto();
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
        m_talker->listPhotos(m_username,
                             m_widget->m_albumsCoB->itemData(m_widget->m_albumsCoB->currentIndex()).toString());
    }
    else
    {
        typedef QPair<KUrl,FPhotoInfo> Pair;

        m_transferQueue.clear();

        KUrl::List urlList = m_widget->m_imgList->imageUrls();

        if (urlList.isEmpty())
            return;

        for (KUrl::List::ConstIterator it = urlList.constBegin(); it != urlList.constEnd(); ++it)
        {
            KIPI::ImageInfo info = m_interface->info( *it );
            FPhotoInfo temp;

            temp.title=info.title();
            temp.description=info.description();

            QStringList allTags;

            QStringList::Iterator itTags;
            /*TODO
            QStringList tagsFromDialog = m_tagsLineEdit->text().split(' ', QString::SkipEmptyParts);

            // Tags from the interface
            itTags = tagsFromDialog.begin();

            while( itTags != tagsFromDialog.end() )
            {
                allTags.append( *itTags );
                ++itTags;
            }
            */

            //Tags from the database
            QMap <QString, QVariant> attribs = info.attributes();
            QStringList tagsFromDatabase;

            //TODOif(m_exportApplicationTags->isChecked())
            {
                tagsFromDatabase=attribs["tags"].toStringList();
            }

            itTags = tagsFromDatabase.begin();

            while( itTags != tagsFromDatabase.end() )
            {
                allTags.append( *itTags );
                ++itTags;
            }

            itTags = allTags.begin();

            while( itTags != allTags.end() )
            {
                ++itTags;
            }

            temp.tags=allTags;
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

        kDebug() << "m_currentAlbumID" << m_currentAlbumID;
        uploadNextPhoto();
        kDebug() << "slotStartTransfer done";
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
    KExiv2Iface::KExiv2 exiv2Iface;
    if (exiv2Iface.load(imgPath))
    {
        exiv2Iface.setImageDimensions(image.size());
        exiv2Iface.setImageProgramId("Kipi-plugins", kipiplugins_version);
        exiv2Iface.save(m_tmpPath);
    }

    return true;
}

void PicasawebWindow::uploadNextPhoto()
{
    if (m_transferQueue.isEmpty())
    {
        m_widget->progressBar()->hide();
        return;
    }

    typedef QPair<KUrl,FPhotoInfo> Pair;
    Pair pathComments = m_transferQueue.first();
    FPhotoInfo info   = m_transferQueue.first().second;

    m_widget->progressBar()->setMaximum(m_imagesTotal);
    m_widget->progressBar()->setValue(m_imagesCount);

    m_widget->m_imgList->processing(pathComments.first);
    QString imgPath = pathComments.first.toLocalFile();

    bool res;
    KMimeType::Ptr ptr = KMimeType::findByUrl(imgPath);
    if(((ptr->is("image/bmp") ||
         ptr->is("image/gif") ||
         ptr->is("image/jpeg") ||
         ptr->is("image/png")) &&
        !m_widget->m_resizeChB->isChecked()) ||
       ptr->name().startsWith("video"))
    {
        m_tmpPath.clear();
        res = m_talker->addPhoto(imgPath, info, m_currentAlbumID);
    }
    else
    {
        // check if we have to RAW file -> use preview image then
#if KDCRAW_VERSION < 0x000400
        QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
        QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
        QFileInfo fileInfo(imgPath);
        bool isRAW = rawFilesExt.toUpper().contains(fileInfo.suffix().toUpper());

        if (!prepareImageForUpload(imgPath, isRAW))
        {
            slotAddPhotoDone(666, i18n("Cannot open file"));
            return;
        }
        res = m_talker->addPhoto(m_tmpPath, info, m_currentAlbumID);
    }

    if (!res)
    {
        slotAddPhotoDone(666, i18n("Cannot open file"));
        return;
    }
}

void PicasawebWindow::slotAddPhotoDone(int errCode, const QString& errMsg)
{
    // Remove temporary file if it was used
    if (!m_tmpPath.isEmpty())
    {
        QFile::remove(m_tmpPath);
        m_tmpPath.clear();
    }

    m_widget->m_imgList->processed(errCode == 0);

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
    QString imgPath = m_widget->getDestinationPath() + '/'
                      + QFileInfo(m_transferQueue.first().first.path()).fileName();

    if (errCode == 0)
    {
        QString errText;
        QFile imgFile(imgPath);
        if (!imgFile.open(QIODevice::WriteOnly))
        {
            errText = imgFile.errorString();
        }
        else if (imgFile.write(photoData) != photoData.size())
        {
            errText = imgFile.errorString();
        }
        else
            imgFile.close();

        if (errText.isEmpty())
        {
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
                m_transferQueue.clear();
                m_widget->progressBar()->hide();
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
            m_transferQueue.clear();
            m_widget->progressBar()->hide();
            return;
        }
    }

    downloadNextPhoto();
}

void PicasawebWindow::slotTransferCancel()
{
    m_transferQueue.clear();
    m_widget->progressBar()->hide();

    m_talker->cancel();
}

void PicasawebWindow::slotCreateAlbumDone(int errCode, const QString& errMsg,
                                     int newAlbumID)
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
