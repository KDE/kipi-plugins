/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to Yandex.Fotki web service
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
 *
 * GUI based on PicasaWeb KIPI Plugin
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "yfwindow.h"

// C ANSI includes

extern "C"
{
// TODO: remove this include
#include <unistd.h> // getpid
}

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QMenu>
#include <QComboBox>
#include <QApplication>
#include <QMessageBox>

// KDE includes

#include <kconfig.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/UploadWidget>
#include <KIPI/ImageCollection>

// Local includes

#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpversion.h"
#include "kpimageslist.h"
#include "yftalker.h"
#include "yfalbumdialog.h"
#include "kipiplugins_debug.h"
#include "kputil.h"
#include "kplogindialog.h"
#include "yfwidget.h"

using namespace KIPI;

namespace KIPIYandexFotkiPlugin
{

/*
 * This tag added to our images after uploading to Fotki web service
 */
const char* YandexFotkiWindow::XMP_SERVICE_ID = "Xmp.kipi.yandexGPhotoId";

YandexFotkiWindow::YandexFotkiWindow(bool import, QWidget* const parent)
    : KPToolDialog(parent)
{
    m_import = import;
    m_tmpDir = makeTemporaryDir("kipi-yandexfotki").absolutePath() + QString::fromLatin1("/");
    m_widget = new YandexFotkiWidget(this, iface(), QString::fromLatin1("Yandex.Fotki"));

    m_loginLabel           = m_widget->getUserNameLabel();
    m_headerLabel          = m_widget->getHeaderLbl();
    m_changeUserButton     = m_widget->getChangeUserBtn();
    m_newAlbumButton       = m_widget->getNewAlbmBtn();
    m_reloadAlbumsButton   = m_widget->getReloadBtn();
    m_albumsCombo          = m_widget->getAlbumsCoB();
    m_resizeCheck          = m_widget->getResizeCheckBox();
    m_dimensionSpin        = m_widget->getDimensionSpB();
    m_imageQualitySpin     = m_widget->getImgQualitySpB();
    m_imgList              = m_widget->imagesList();
    m_progressBar          = m_widget->progressBar();
    m_accessCombo          = m_widget->m_accessCombo;
    m_hideOriginalCheck    = m_widget->m_hideOriginalCheck;
    m_disableCommentsCheck = m_widget->m_disableCommentsCheck;
    m_adultCheck           = m_widget->m_adultCheck;
    m_policyGroup          = m_widget->m_policyGroup;
    m_albumsBox            = m_widget->getAlbumBox();
    m_meta                 = 0;

    if (iface())
    {
        m_meta = iface()->createMetadataProcessor();
    }

    connect(m_changeUserButton, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));

    connect(m_newAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotNewAlbumRequest()) );


    connect(m_reloadAlbumsButton, SIGNAL(clicked()),
            this, SLOT(slotReloadAlbumsRequest()) );

    setMainWidget(m_widget);
    m_widget->setMinimumSize(800, 700);

    KPAboutData* const about = new KPAboutData(ki18n("Yandex.Fotki Plugin"),
                                               0,
                                               KAboutLicense::GPL,
                                               ki18n("A Kipi plugin to export image collections to "
                                                     "Yandex.Fotki web service."),
                                               ki18n("(c) 2007-2009, Vardhman Jain\n"
                                                     "(c) 2008-2015, Gilles Caulier\n"
                                                     "(c) 2009, Luka Renko\n"
                                                     "(c) 2010, Roman Tsisyk"));

    about->addAuthor(ki18n( "Roman Tsisyk" ).toString(),
                     ki18n("Author").toString(),
                     QString::fromLatin1("roman at tsisyk dot com"));

    about->setHandbookEntry(QString::fromLatin1("YandexFotki"));
    setAboutData(about);

    // -- UI slots -----------------------------------------------------------------------

    connect(startButton(), &QPushButton::clicked,
            this, &YandexFotkiWindow::slotStartTransfer);

    connect(this, &KPToolDialog::cancelClicked,
            this, &YandexFotkiWindow::slotCancelClicked);

    connect(this, &QDialog::finished,
            this, &YandexFotkiWindow::slotFinished);

    // -- Talker slots -------------------------------------------------------------------

    connect(&m_talker, SIGNAL(signalError()),
            this, SLOT(slotError()));

    connect(&m_talker, SIGNAL(signalGetSessionDone()),
            this, SLOT(slotGetSessionDone()));

    connect(&m_talker, SIGNAL(signalGetTokenDone()),
            this, SLOT(slotGetTokenDone()));

    connect(&m_talker, SIGNAL(signalGetServiceDone()),
            this, SLOT(slotGetServiceDone()));

    connect(&m_talker, SIGNAL(signalListAlbumsDone(QList<YandexFotkiAlbum>)),
            this, SLOT(slotListAlbumsDone(QList<YandexFotkiAlbum>)));

    connect(&m_talker, SIGNAL(signalListPhotosDone(QList<YandexFotkiPhoto>)),
            this, SLOT(slotListPhotosDone(QList<YandexFotkiPhoto>)));

    connect(&m_talker, SIGNAL(signalUpdatePhotoDone(YandexFotkiPhoto&)),
            this, SLOT(slotUpdatePhotoDone(YandexFotkiPhoto&)));

    connect(&m_talker, SIGNAL(signalUpdateAlbumDone()),
            this, SLOT(slotUpdateAlbumDone()));

    // read settings from file
    readSettings();
}

YandexFotkiWindow::~YandexFotkiWindow()
{
    reset();
}

void YandexFotkiWindow::reactivate()
{
    m_imgList->loadImagesFromCurrentSelection();

    reset();
    authenticate(false);
    show();
}

void YandexFotkiWindow::reset()
{
    m_talker.reset();
    updateControls(true);
    updateLabels();
}

void YandexFotkiWindow::updateControls(bool val)
{
    if (val)
    {
        if (m_talker.isAuthenticated())
        {
            m_albumsBox->setEnabled(true);
            startButton()->setEnabled(true);
        }
        else
        {
            m_albumsBox->setEnabled(false);
            startButton()->setEnabled(false);
        }

        m_changeUserButton->setEnabled(true);
        setCursor(Qt::ArrowCursor);

        setRejectButtonMode(QDialogButtonBox::Close);
    }
    else
    {
        setCursor(Qt::WaitCursor);
        m_albumsBox->setEnabled(false);
        m_changeUserButton->setEnabled(false);
        startButton()->setEnabled(false);

        setRejectButtonMode(QDialogButtonBox::Cancel);
    }
}

void YandexFotkiWindow::updateLabels()
{
    QString urltext;
    QString logintext;

    if (m_talker.isAuthenticated())
    {
        logintext = m_talker.login();
        urltext = YandexFotkiTalker::USERPAGE_URL.arg(m_talker.login());
        m_albumsBox->setEnabled(true);
    }
    else
    {
        logintext = i18n("Unauthorized");
        urltext = YandexFotkiTalker::USERPAGE_DEFAULT_URL;
        m_albumsCombo->clear();
    }

    m_loginLabel->setText(QString::fromLatin1("<b>%1</b>").arg(logintext));
    m_headerLabel->setText(QString::fromLatin1(
        "<b><h2><a href=\"%1\">"
        "<font color=\"#ff000a\">%2</font>"
        "<font color=\"black\">%3</font>"
        "<font color=\"#009d00\">%4</font>"
        "</a></h2></b>")
        .arg(urltext)
        .arg(i18nc("Yandex.Fotki", "Y"))
        .arg(i18nc("Yandex.Fotki", "andex."))
        .arg(i18nc("Yandex.Fotki", "Fotki")));
}

void YandexFotkiWindow::readSettings()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup grp = config.group("YandexFotki Settings");

    m_talker.setLogin(grp.readEntry("login", ""));
    // don't store tokens in plaintext
    //m_talker.setToken(grp.readEntry("token", ""));

    if (grp.readEntry("Resize", false))
    {
        m_resizeCheck->setChecked(true);
        m_dimensionSpin->setEnabled(true);
        m_imageQualitySpin->setEnabled(true);
    }
    else
    {
        m_resizeCheck->setChecked(false);
        m_dimensionSpin->setEnabled(false);
        m_imageQualitySpin->setEnabled(false);
    }

    m_dimensionSpin->setValue(grp.readEntry("Maximum Width", 1600));
    m_imageQualitySpin->setValue(grp.readEntry("Image Quality", 85));
    m_policyGroup->button(grp.readEntry("Sync policy", 0))->setChecked(true);
}

void YandexFotkiWindow::writeSettings()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup grp = config.group("YandexFotki Settings");

    grp.writeEntry("token", m_talker.token());
    // don't store tokens in plaintext
    //grp.writeEntry("login", m_talker.login());

    grp.writeEntry("Resize", m_resizeCheck->isChecked());
    grp.writeEntry("Maximum Width", m_dimensionSpin->value());
    grp.writeEntry("Image Quality", m_imageQualitySpin->value());
    grp.writeEntry("Sync policy", m_policyGroup->checkedId());
}

void YandexFotkiWindow::slotChangeUserClicked()
{
    // force authenticate window
    authenticate(true);
}

void YandexFotkiWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void YandexFotkiWindow::slotFinished()
{
    writeSettings();
    reset();
}

void YandexFotkiWindow::slotCancelClicked()
{
    m_talker.cancel();
    updateControls(true);
}

/*
void YandexFotkiWindow::cancelProcessing()
{
    m_talker.cancel();
    m_transferQueue.clear();
    m_imgList->processed(false);
    progressBar()->hide();
}
*/

void YandexFotkiWindow::authenticate(bool forceAuthWindow)
{
    // update credentials
    if (forceAuthWindow || m_talker.login().isNull() || m_talker.password().isNull())
    {
        KPLoginDialog* const dlg = new KPLoginDialog(this, QString::fromLatin1("Yandex.Fotki"), m_talker.login(), QString());

        if (dlg->exec() == QDialog::Accepted)
        {
            m_talker.setLogin(dlg->login());
            m_talker.setPassword(dlg->password());
        }
        else
        {
            // don't change anything
            return;
        }

        delete dlg;
    }

    /*else
    {
        qCDebug(KIPIPLUGINS_LOG) << "Checking old token...";
        m_talker.checkToken();
        return;
    }
    */

    // if new credentials non-empty, authenticate
    if (!m_talker.login().isEmpty() && !m_talker.password().isEmpty())
    {
        // cancel all tasks first
        reset();

        // start authentication chain
        updateControls(false);
        m_talker.getService();
    }
    else
    {
        // we don't have valid credentials, so cancel all transfers and reset
        reset();
    }

/*
        progressBar()->show();
        progressBar()->setFormat("");
*/
}

void YandexFotkiWindow::slotListPhotosDone(const QList <YandexFotkiPhoto>& photosList)
{
    if (m_import)
    {
        slotListPhotosDoneForDownload(photosList);
    }
    else
    {
        slotListPhotosDoneForUpload(photosList);
    }
}

void YandexFotkiWindow::slotListPhotosDoneForDownload(const QList <YandexFotkiPhoto>& photosList)
{
    Q_UNUSED(photosList);
    updateControls(true);
}

void YandexFotkiWindow::slotListPhotosDoneForUpload(const QList <YandexFotkiPhoto>& photosList)
{
    updateControls(true);

    QMap<QString, int> dups;
    int i = 0;

    foreach(const YandexFotkiPhoto& photo, photosList)
    {
        dups.insert(photo.urn(), i);
        i++;
    }

    YandexFotkiWidget::UpdatePolicy policy = static_cast<YandexFotkiWidget::UpdatePolicy>(m_policyGroup->checkedId());
    const YandexFotkiPhoto::Access access = static_cast<YandexFotkiPhoto::Access>(
                                            m_accessCombo->itemData(m_accessCombo->currentIndex()).toInt());

    qCDebug(KIPIPLUGINS_LOG) << "";
    qCDebug(KIPIPLUGINS_LOG) << "----";
    m_transferQueue.clear();

    foreach(const QUrl& url, m_imgList->imageUrls(true))
    {
        KPImageInfo info(url);

        // check if photo alredy uploaded

        int oldPhotoId = -1;

        if (m_meta && m_meta->load(url))
        {
            QString localId = m_meta->getXmpTagString(QLatin1String(XMP_SERVICE_ID));
            oldPhotoId      = dups.value(localId, -1);
        }

        // get tags
        QStringList tags = info.tagsPath();
        bool updateFile  = true;

        QSet<QString> oldtags;

        if (oldPhotoId != -1)
        {
            if (policy == YandexFotkiWidget::UpdatePolicy::POLICY_SKIP)
            {
                qCDebug(KIPIPLUGINS_LOG) << "SKIP: " << url;
                continue;
            }

            // old photo copy
            m_transferQueue.push(photosList[oldPhotoId]);

            if (policy == YandexFotkiWidget::UpdatePolicy::POLICY_UPDATE_MERGE)
            {
                foreach(const QString& t, m_transferQueue.top().tags)
                {
                    oldtags.insert(t);
                }
            }

            if (policy != YandexFotkiWidget::UpdatePolicy::POLICY_ADDNEW)
            {
                updateFile = false;
            }
        }
        else
        {
            // empty photo
            m_transferQueue.push(YandexFotkiPhoto());
        }

        YandexFotkiPhoto& photo = m_transferQueue.top();
        // TODO: updateFile is not used
        photo.setOriginalUrl(url.toLocalFile());
        photo.setTitle(info.name());
        photo.setSummary(info.description());
        photo.setAccess(access);
        photo.setHideOriginal(m_hideOriginalCheck->isChecked());
        photo.setDisableComments(m_disableCommentsCheck->isChecked());

        // adult flag can't be removed, API restrictions
        if (!photo.isAdult())
            photo.setAdult(m_adultCheck->isChecked());

        foreach(const QString& t, tags)
        {
            if (!oldtags.contains(t))
            {
                photo.tags.append(t);
            }
        }

        if (updateFile)
        {
            qCDebug(KIPIPLUGINS_LOG) << "METADATA + IMAGE: " << url;
        }
        else
        {
            qCDebug(KIPIPLUGINS_LOG) << "METADATA: " << url;
        }
    }

    if (m_transferQueue.isEmpty())
    {
        return;    // nothing to do
    }

    qCDebug(KIPIPLUGINS_LOG) << "----";
    qCDebug(KIPIPLUGINS_LOG) << "";

    updateControls(false);
    updateNextPhoto();
}

void YandexFotkiWindow::updateNextPhoto()
{
    // select only one image from stack
    while (!m_transferQueue.isEmpty())
    {
        YandexFotkiPhoto& photo = m_transferQueue.top();

        if (!photo.originalUrl().isNull())
        {
            QImage image;

            if (iface())
            {
                image = iface()->preview(QUrl::fromLocalFile(photo.originalUrl()));
            }

            if (image.isNull())
            {
                image.load(photo.originalUrl());
            }

            photo.setLocalUrl(m_tmpDir + QFileInfo(photo.originalUrl())
                              .baseName()
                              .trimmed() + QString::fromLatin1(".jpg"));

            bool prepared = false;

            if (!image.isNull())
            {
                // get temporary file name

                // rescale image if requested
                int maxDim = m_dimensionSpin->value();

                if (m_resizeCheck->isChecked() && (image.width() > maxDim || image.height() > maxDim))
                {
                    qCDebug(KIPIPLUGINS_LOG) << "Resizing to " << maxDim;
                    image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation);
                }

                // copy meta data to temporary image

                if (image.save(photo.localUrl(), "JPEG", m_imageQualitySpin->value()))
                {
                    if (m_meta && m_meta->load(QUrl::fromLocalFile(photo.originalUrl())))
                    {
                        m_meta->setImageDimensions(image.size());
                        m_meta->setImageProgramId(QString::fromLatin1("Kipi-plugins"), kipipluginsVersion());
                        m_meta->save(QUrl::fromLocalFile(photo.localUrl()));
                        prepared = true;
                    }
                }
            }

            if (!prepared)
            {
                if (QMessageBox::question(this, i18n("Processing Failed"),
                                  i18n("Failed to prepare image %1\n"
                                       "Do you want to continue?", photo.originalUrl()))
                    != QMessageBox::Yes)
                {
                    // stop uploading
                    m_transferQueue.clear();
                    continue;
                }
                else
                {
                    m_transferQueue.pop();
                    continue;
                }
            }
        }

        const YandexFotkiAlbum& album = m_talker.albums().at(m_albumsCombo->currentIndex());

        qCDebug(KIPIPLUGINS_LOG) << photo.originalUrl();

        m_talker.updatePhoto(photo, album);

        return;
    }

    updateControls(true);

    QMessageBox::information(this, QString(), i18n("Images has been uploaded"));
    return;
}

void YandexFotkiWindow::slotNewAlbumRequest()
{
    YandexFotkiAlbum album;
    QPointer<YandexFotkiAlbumDialog> dlg = new YandexFotkiAlbumDialog(this, album);

    if (dlg->exec() == QDialog::Accepted)
    {
        updateControls(false);
        m_talker.updateAlbum(album);
    }

    delete dlg;
}

void YandexFotkiWindow::slotReloadAlbumsRequest()
{
    updateControls(false);
    m_talker.listAlbums();
}

void YandexFotkiWindow::slotStartTransfer()
{
    qCDebug(KIPIPLUGINS_LOG) << "slotStartTransfer invoked";

    if (m_albumsCombo->currentIndex() == -1 || m_albumsCombo->count() == 0)
    {
        QMessageBox::information(this, QString(), i18n("Please select album first"));
        return;
    }

    // TODO: import support
    if (!m_import)
    {
        // list photos of the album, then start upload
        const YandexFotkiAlbum& album = m_talker.albums().at(m_albumsCombo->currentIndex());

        qCDebug(KIPIPLUGINS_LOG) << "Album selected" << album;

        updateControls(false);
        m_talker.listPhotos(album);
    }
}

void YandexFotkiWindow::slotError()
{
    switch (m_talker.state())
    {
        case YandexFotkiTalker::STATE_GETSESSION_ERROR:
            QMessageBox::critical(this, QString(), i18n("Session error"));
            break;
        case YandexFotkiTalker::STATE_GETTOKEN_ERROR:
            QMessageBox::critical(this, QString(), i18n("Token error"));
            break;
        case YandexFotkiTalker::STATE_INVALID_CREDENTIALS:
            QMessageBox::critical(this, QString(), i18n("Invalid credentials"));
//            authenticate(true);
            break;
        case YandexFotkiTalker::STATE_GETSERVICE_ERROR:
            QMessageBox::critical(this, QString(), i18n("Cannot get service document"));
            break;
/*
        case YandexFotkiTalker::STATE_CHECKTOKEN_INVALID:
            // remove old expired token
            qCDebug(KIPIPLUGINS_LOG) << "CheckToken invalid";
            m_talker.setToken(QString());
            // don't say anything, simple show new auth window
            authenticate(true);
            break;
*/
        case YandexFotkiTalker::STATE_LISTALBUMS_ERROR:
            m_albumsCombo->clear();
            QMessageBox::critical(this, QString(), i18n("Cannot list albums"));
            break;
        case YandexFotkiTalker::STATE_LISTPHOTOS_ERROR:
            QMessageBox::critical(this, QString(), i18n("Cannot list photos"));
            break;
        case YandexFotkiTalker::STATE_UPDATEALBUM_ERROR:
            QMessageBox::critical(this, QString(), i18n("Cannot update album info"));
            break;
        case YandexFotkiTalker::STATE_UPDATEPHOTO_FILE_ERROR:
        case YandexFotkiTalker::STATE_UPDATEPHOTO_INFO_ERROR:
            qCDebug(KIPIPLUGINS_LOG) << "UpdatePhotoError";

            if (QMessageBox::question(this, i18n("Uploading Failed"),
                                      i18n("Failed to upload image %1\n"
                                           "Do you want to continue?",
                                      m_transferQueue.top().originalUrl()))
                != QMessageBox::Yes)
            {
                // clear upload stack
                m_transferQueue.clear();
            }
            else
            {
                // cancel current operation
                m_talker.cancel();
                // remove only bad image
                m_transferQueue.pop();
                // and try next
                updateNextPhoto();
                return;
            }
            break;
        default:
            qCDebug(KIPIPLUGINS_LOG) << "Unhandled error" << m_talker.state();
            QMessageBox::critical(this, QString(), i18n("Unknown error"));
    }

    // cancel current operation
    m_talker.cancel();
    updateControls(true);
}

void YandexFotkiWindow::slotGetServiceDone()
{
    qCDebug(KIPIPLUGINS_LOG) << "GetService Done";
    m_talker.getSession();
}

void YandexFotkiWindow::slotGetSessionDone()
{
    qCDebug(KIPIPLUGINS_LOG) << "GetSession Done";
    m_talker.getToken();
}

void YandexFotkiWindow::slotGetTokenDone()
{
    updateLabels();
    slotReloadAlbumsRequest();
}

void YandexFotkiWindow::slotListAlbumsDone(const QList<YandexFotkiAlbum>& albumsList)
{
    m_albumsCombo->clear();

    foreach(const YandexFotkiAlbum& album, albumsList)
    {
        QString albumIcon;

        if (album.isProtected())
        {
            albumIcon = QString::fromLatin1("folder-locked");
        }
        else
        {
            albumIcon = QString::fromLatin1("folder-image");
        }

        m_albumsCombo->addItem(QIcon::fromTheme(albumIcon), album.toString());
    }

    m_albumsCombo->setEnabled(true);
    updateControls(true);
}

void YandexFotkiWindow::slotUpdatePhotoDone(YandexFotkiPhoto& photo)
{
    qCDebug(KIPIPLUGINS_LOG) << "photoUploaded" << photo;

    if (m_meta && m_meta->supportXmp() && m_meta->canWriteXmp(QUrl::fromLocalFile(photo.originalUrl())) &&
        m_meta->load(QUrl::fromLocalFile(photo.originalUrl())))
    {
        // ignore errors here
        if (m_meta->setXmpTagString(QLatin1String(XMP_SERVICE_ID), photo.urn()) &&
            m_meta->save(QUrl::fromLocalFile(photo.originalUrl())))
        {
            qCDebug(KIPIPLUGINS_LOG) << "MARK: " << photo.originalUrl();
        }
    }

    m_transferQueue.pop();
    updateNextPhoto();
}

void YandexFotkiWindow::slotUpdateAlbumDone()
{
    qCDebug(KIPIPLUGINS_LOG) << "Album created";
    m_albumsCombo->clear();
    m_talker.listAlbums();
}

} // namespace KIPIYandexFotkiPlugin
