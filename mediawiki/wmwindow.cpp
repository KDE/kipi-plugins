/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : A kipi plugin to export images to a MediaWiki wiki
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
 * Copyright (C) 2012-2016 by Peter Potrowl <peter dot potrowl at gmail dot com>
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

#include "wmwindow.h"

// Qt includes

#include <QWindow>
#include <QPointer>
#include <QLayout>
#include <QCloseEvent>
#include <QFileInfo>
#include <QFile>
#include <QMenu>
#include <QUrl>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QDir>

// KDE includes

#include <kconfig.h>
#include <klocalizedstring.h>
#include <kwindowconfig.h>

// MediaWiki includes

#include <MediaWiki/Login>
#include <MediaWiki/MediaWiki>
#include <mediawiki_version.h>

// KIPI includes

#include <KIPI/Interface>
#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_debug.h"
#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"
#include "wmwidget.h"
#include "wmtalker.h"

using namespace KIPI;
using namespace mediawiki;

namespace KIPIWikiMediaPlugin
{

class WMWindow::Private
{
public:

    Private()
    {
        widget       = 0;
        mediawiki    = 0;
        uploadTalker = 0;
    }

    QString    tmpDir;
    QString    tmpPath;
    QString    login;
    QString    pass;
    QString    wikiName;
    QUrl       wikiUrl;

    WmWidget*  widget;
    MediaWiki* mediawiki;

    WMTalker*  uploadTalker;
};

WMWindow::WMWindow(const QString& tmpFolder, QWidget* const /*parent*/)
    : KPToolDialog(0),
      d(new Private)
{
    d->tmpPath.clear();
    d->tmpDir       = tmpFolder;
    d->widget       = new WmWidget(this);
    d->uploadTalker = 0;
    d->login        = QString();
    d->pass         = QString();

    setMainWidget(d->widget);
    setWindowIcon(QIcon::fromTheme(QLatin1String("kipi-wikimedia")));
    setModal(false);
    setWindowTitle(i18n("Export to MediaWiki"));

    startButton()->setText(i18n("Start Upload"));
    startButton()->setToolTip(i18n("Start upload to MediaWiki"));

    startButton()->setEnabled(false);

    d->widget->setMinimumSize(700, 500);
    d->widget->installEventFilter(this);

    KPAboutData* const about = new KPAboutData(ki18n("MediaWiki export"),
                                               ki18n("A tool to export image collection "
                                                     "to a MediaWiki installation.\n"
                                                     "Using libmediawiki version %1").subs(QLatin1String(MEDIAWIKI_VERSION_STRING)),
                                               ki18n("(c) 2011, Alexandre Mendes"));

    about->addAuthor(ki18n("Alexandre Mendes").toString(),
                     ki18n("Author").toString(),
                     QLatin1String("alex dot mendes1988 at gmail dot com"));

    about->addAuthor(ki18n("Guillaume Hormiere").toString(),
                     ki18n("Developer").toString(),
                     QLatin1String("hormiere dot guillaume at gmail dot com"));

    about->addAuthor(ki18n("Gilles Caulier").toString(),
                     ki18n("Developer").toString(),
                     QLatin1String("caulier dot gilles at gmail dot com"));

    about->addAuthor(ki18n("Peter Potrowl").toString(),
                     ki18n("Developer").toString(),
                     QLatin1String("peter dot potrowl at gmail dot com"));

    about->setHandbookEntry(QLatin1String("tool-wikimediaexport"));
    setAboutData(about);

    connect(startButton(), SIGNAL(clicked()),
            this, SLOT(slotStartTransfer()));

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));

    connect(d->widget, SIGNAL(signalChangeUserRequest()),
            this, SLOT(slotChangeUserClicked()));

    connect(d->widget, SIGNAL(signalLoginRequest(QString, QString, QString, QUrl)),
            this, SLOT(slotDoLogin(QString, QString, QString, QUrl)));

    connect(d->widget->progressBar(), SIGNAL(signalProgressCanceled()),
            this, SLOT(slotProgressCanceled()));

    readSettings();
    reactivate();
}

WMWindow::~WMWindow()
{
    delete d;
}

void WMWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void WMWindow::reactivate()
{
    d->widget->imagesList()->listView()->clear();
    d->widget->imagesList()->loadImagesFromCurrentSelection();
    d->widget->loadImageInfoFirstLoad();
    d->widget->clearEditFields();
    qCDebug(KIPIPLUGINS_LOG) << "imagesList items count:" << d->widget->imagesList()->listView()->topLevelItemCount();
    qCDebug(KIPIPLUGINS_LOG) << "imagesList url length:"  << d->widget->imagesList()->imageUrls(false).size();
    qCDebug(KIPIPLUGINS_LOG) << "allImagesDesc length:"   << d->widget->allImagesDesc().size();
    show();
}

void WMWindow::readSettings()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group(QLatin1String("MediaWiki export settings"));

    d->widget->readSettings(group);

    winId();
    KConfigGroup group2 = config.group(QLatin1String("MediaWiki export dialog"));
    KWindowConfig::restoreWindowSize(windowHandle(), group2);
    resize(windowHandle()->size());
}

void WMWindow::saveSettings()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group(QLatin1String("MediaWiki export settings"));

    d->widget->saveSettings(group);

    KConfigGroup group2 = config.group(QLatin1String("MediaWiki export dialog"));
    KWindowConfig::saveWindowSize(windowHandle(), group2);
    config.sync();
}

void WMWindow::slotFinished()
{
    d->widget->progressBar()->progressCompleted();
    saveSettings();
}

void WMWindow::slotProgressCanceled()
{
    slotFinished();
    reject();
}

bool WMWindow::prepareImageForUpload(const QString& imgPath)
{
    // Create temporary directory if it does not exist

    if (!QDir(d->tmpDir).exists())
    {
        QDir().mkdir(d->tmpDir);
    }

    // Get temporary file name

    d->tmpPath = d->tmpDir + QFileInfo(imgPath).baseName().trimmed() + QLatin1String(".jpg");

    QImage image;

    // Rescale image if requested: metadata is lost

    if (d->widget->resize())
    {
        if (iface())
        {
            image = iface()->preview(QUrl::fromLocalFile(imgPath));
        }

        if (image.isNull())
        {
            image.load(imgPath);
        }

        if (image.isNull())
        {
            return false;
        }

        int maxDim = d->widget->dimension();

        if (d->widget->resize() && (image.width() > maxDim || image.height() > maxDim))
        {
            qCDebug(KIPIPLUGINS_LOG) << "Resizing to " << maxDim;
            image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        qCDebug(KIPIPLUGINS_LOG) << "Saving to temp file: " << d->tmpPath;
        image.save(d->tmpPath, "JPEG", d->widget->quality());
    }
    else
    {
        // file is copied with its embedded metadata
        QFile::copy(imgPath, d->tmpPath);
    }

    if (iface())
    {
        // NOTE : In case of metadata are saved to tmp file, we will override MetadataProcessor settings from KIPI host
        // to write metadata to image file rather than sidecar file, to be effective with remote web service.

        QPointer<MetadataProcessor> meta = iface()->createMetadataProcessor();

        if (d->widget->removeMeta())
        {
            // save empty metadata to erase them
            meta->save(QUrl::fromLocalFile(d->tmpPath), true);
        }
        else
        {
            // copy meta data from initial to temporary image

            if (meta->load(QUrl::fromLocalFile(imgPath)))
            {
                if (d->widget->resize())
                {
                    meta->setImageDimensions(image.size());
                }

                if (d->widget->removeGeo())
                {
                    meta->removeGPSInfo();
                }

                meta->save(QUrl::fromLocalFile(d->tmpPath), true);
            }
        }
    }

    return true;
}

void WMWindow::slotStartTransfer()
{
    saveSettings();
    QList<QUrl> urls                                    = d->widget->imagesList()->imageUrls(false);
    QMap <QString, QMap <QString, QString> > imagesDesc = d->widget->allImagesDesc();

    for (int i = 0; i < urls.size(); ++i)
    {
        QString url;

        if (d->widget->resize() || d->widget->removeMeta() || d->widget->removeGeo())
        {
            prepareImageForUpload(urls.at(i).toLocalFile());
            imagesDesc.insert(d->tmpPath, imagesDesc.take(urls.at(i).toLocalFile()));
        }
    }

    d->uploadTalker->setImageMap(imagesDesc);

    d->widget->progressBar()->setRange(0, 100);
    d->widget->progressBar()->setValue(0);

    connect(d->uploadTalker, SIGNAL(uploadProgress(int)),
            d->widget->progressBar(), SLOT(setValue(int)));

    connect(d->uploadTalker, SIGNAL(endUpload()),
            this, SLOT(slotEndUpload()));

    d->widget->progressBar()->show();
    d->widget->progressBar()->progressScheduled(i18n("MediaWiki export"), true, true);
    d->widget->progressBar()->progressThumbnailChanged(QIcon(QLatin1String(":/icons/kipi-icon.svg")).pixmap(22, 22));
    d->uploadTalker->begin();
}

void WMWindow::slotChangeUserClicked()
{
    startButton()->setEnabled(false);
    d->widget->invertAccountLoginBox();
}

void WMWindow::slotDoLogin(const QString& login, const QString& pass, const QString& wikiName, const QUrl& wikiUrl)
{
    d->login              = login;
    d->pass               = pass;
    d->wikiName           = wikiName;
    d->wikiUrl            = wikiUrl;
    d->mediawiki          = new MediaWiki(wikiUrl);
    Login* const loginJob = new Login(*d->mediawiki, login, pass);

    connect(loginJob, SIGNAL(result(KJob*)),
            this, SLOT(slotLoginHandle(KJob*)));

    loginJob->start();
}

int WMWindow::slotLoginHandle(KJob* loginJob)
{
    qCDebug(KIPIPLUGINS_LOG) << loginJob->error() << loginJob->errorString() << loginJob->errorText();

    if (loginJob->error())
    {
        d->login.clear();
        d->pass.clear();
        d->uploadTalker = 0;
        QMessageBox::critical(this, i18n("Login Error"), i18n("Please check your credentials and try again."));
    }
    else
    {
        d->uploadTalker = new WMTalker(iface(), d->mediawiki, this);
        startButton()->setEnabled(true);
        d->widget->invertAccountLoginBox();
        d->widget->updateLabels(d->login, d->wikiName, d->wikiUrl.toString());
    }

    return loginJob->error();
}

void WMWindow::slotEndUpload()
{
    disconnect(d->uploadTalker, SIGNAL(uploadProgress(int)),
               d->widget->progressBar(),SLOT(setValue(int)));

    disconnect(d->uploadTalker, SIGNAL(endUpload()),
               this, SLOT(slotEndUpload()));

    QMessageBox::information(this, QString(), i18n("Upload finished with no errors."));
    d->widget->progressBar()->hide();
    d->widget->progressBar()->progressCompleted();
}

bool WMWindow::eventFilter(QObject* /*obj*/, QEvent* event)
{
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* const c = dynamic_cast<QKeyEvent *>(event);

        if (c && c->key() == Qt::Key_Return)
        {
            event->ignore();
            qCDebug(KIPIPLUGINS_LOG) << "Key event pass";
            return false;

        }
    }

    return true;
}

} // namespace KIPIWikiMediaPlugin
