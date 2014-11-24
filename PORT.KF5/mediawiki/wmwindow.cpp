/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : A kipi plugin to export images to a MediaWiki wiki
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
 * Copyright (C) 2012-2013 by Peter Potrowl <peter dot potrowl at gmail dot com>
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

#include "wmwindow.moc"

// Qt includes

#include <QLayout>
#include <QCloseEvent>
#include <QFileInfo>
#include <QFile>

// KDE includes

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmenu.h>
#include <kurl.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kmessagebox.h>

// MediaWiki includes

#include <libmediawiki/login.h>
#include <libmediawiki/mediawiki.h>
#include <libmediawiki/version.h>

// KIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"
#include "wmwidget.h"
#include "wikimediajob.h"

using namespace KIPI;
using namespace mediawiki;

namespace KIPIWikiMediaPlugin
{

class WMWindow::Private
{
public:

    Private()
    {
        widget    = 0;
        mediawiki = 0;
        uploadJob = 0;
    }

    QString       tmpDir;
    QString       tmpPath;
    QString       login;
    QString       pass;
    QString       wikiName;
    QUrl          wikiUrl;

    WmWidget*     widget;
    MediaWiki*    mediawiki;

    WikiMediaJob* uploadJob;
};

WMWindow::WMWindow(const QString& tmpFolder, QWidget* const /*parent*/)
    : KPToolDialog(0), d(new Private)
{
    d->tmpPath.clear();
    d->tmpDir    = tmpFolder;
    d->widget    = new WmWidget(this);
    d->uploadJob = 0;
    d->login     = QString();
    d->pass      = QString();

    setMainWidget(d->widget);
    setWindowIcon(KIcon("kipi-wikimedia"));
    setButtons(Help|User1|Close);
    setDefaultButton(Close);
    setModal(false);
    setWindowTitle(i18n("Export to MediaWiki"));
    setButtonGuiItem(User1, KGuiItem(i18n("Start Upload"), "network-workgroup",
                                     i18n("Start upload to MediaWiki")));
    enableButton(User1, false);
    d->widget->setMinimumSize(700, 500);
    d->widget->installEventFilter(this);

    KPAboutData* const about = new KPAboutData(ki18n("MediaWiki export"), 0,
                                               KAboutData::License_GPL,
                                               ki18n("A Kipi plugin to export image collection "
                                                     "to a MediaWiki installation.\n"
                                                     "Using libmediawiki version %1").subs(QString(mediawiki_version)),
                                               ki18n("(c) 2011, Alexandre Mendes"));

    about->addAuthor(ki18n("Alexandre Mendes"), ki18n("Author"),
                     "alex dot mendes1988 at gmail dot com");

    about->addAuthor(ki18n("Guillaume Hormiere"), ki18n("Developer"),
                     "hormiere dot guillaume at gmail dot com");

    about->addAuthor(ki18n("Gilles Caulier"), ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Peter Potrowl"), ki18n("Developer"),
                     "peter dot potrowl at gmail dot com");

    about->setHandbookEntry("wikimedia");
    setAboutData(about);

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartTransfer()));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(d->widget, SIGNAL(signalChangeUserRequest()),
            this, SLOT(slotChangeUserClicked()));

    connect(d->widget, SIGNAL(signalLoginRequest(QString,QString,QString,QUrl)),
            this, SLOT(slotDoLogin(QString,QString,QString,QUrl)));

    connect(d->widget->progressBar(), SIGNAL(signalProgressCanceled()),
            this, SLOT(slotClose()));

    readSettings();
    reactivate();
}

WMWindow::~WMWindow()
{
    delete d;
}

void WMWindow::closeEvent(QCloseEvent* e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void WMWindow::reactivate()
{
    d->widget->imagesList()->listView()->clear();
    d->widget->imagesList()->loadImagesFromCurrentSelection();
    d->widget->loadImageInfoFirstLoad();
    d->widget->clearEditFields();
    kDebug() << "imagesList items count:" << d->widget->imagesList()->listView()->topLevelItemCount();
    kDebug() << "imagesList url length:"  << d->widget->imagesList()->imageUrls(false).size();
    kDebug() << "allImagesDesc length:"   << d->widget->allImagesDesc().size();
    show();
}

void WMWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("MediaWiki export settings"));

    d->widget->readSettings(group);

    KConfigGroup group2 = config.group(QString("MediaWiki export dialog"));
    restoreDialogSize(group2);
}

void WMWindow::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("MediaWiki export settings"));

    d->widget->saveSettings(group);

    KConfigGroup group2 = config.group(QString("MediaWiki export dialog"));
    saveDialogSize(group2);
    config.sync();
}

void WMWindow::slotClose()
{
    d->widget->progressBar()->progressCompleted();
    saveSettings();
    done(Close);
}

bool WMWindow::prepareImageForUpload(const QString& imgPath)
{

    // get temporary file name
    d->tmpPath = d->tmpDir + QFileInfo(imgPath).baseName().trimmed() + ".jpg";

    QImage image;
    // rescale image if requested: metadata is lost
    if (d->widget->resize())
    {
        image.load(imgPath);

        if (image.isNull())
        {
            return false;
        }

        int maxDim = d->widget->dimension();

        if (d->widget->resize() && (image.width() > maxDim || image.height() > maxDim))
        {
            kDebug() << "Resizing to " << maxDim;
            image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        kDebug() << "Saving to temp file: " << d->tmpPath;
        image.save(d->tmpPath, "JPEG", d->widget->quality());
    }
    else
    {
        // file is copied with its embedded metadata
        QFile::copy(imgPath, d->tmpPath);
    }

    KPMetadata meta;

    // In case of metadata are saved to tmp file, we will override KPMetadata settings
    // to write metadata to image file rather than sidecar file, to be effective with remote web service.

    meta.setMetadataWritingMode((KPMetadata::MetadataWritingMode)
                                KPMetadata::WRITETOIMAGEONLY);

    if (d->widget->removeMeta())
    {
        // save empty metadata to erase them
        meta.save(d->tmpPath);
    }
    else
    {
        // copy meta data from initial to temporary image
        meta.load(imgPath);

        if (d->widget->resize())
        {
            meta.setImageDimensions(image.size());
        }

        if (d->widget->removeGeo())
        {
            meta.removeGPSInfo();
        }

        meta.save(d->tmpPath);
    }

    return true;
}

void WMWindow::slotStartTransfer()
{
    saveSettings();
    KUrl::List urls = d->widget->imagesList()->imageUrls(false);
    QMap <QString, QMap <QString, QString> > imagesDesc = d->widget->allImagesDesc();

    for (int i = 0; i < urls.size(); ++i)
    {
        QString url;

        if(d->widget->resize() || d->widget->removeMeta() || d->widget->removeGeo())
        {
            prepareImageForUpload(urls.at(i).path());
            imagesDesc.insert(d->tmpPath, imagesDesc.take(urls.at(i).path()));
        }
    }

    d->uploadJob->setImageMap(imagesDesc);

    d->widget->progressBar()->setRange(0, 100);
    d->widget->progressBar()->setValue(0);

    connect(d->uploadJob, SIGNAL(uploadProgress(int)),
            d->widget->progressBar(), SLOT(setValue(int)));

    connect(d->uploadJob, SIGNAL(endUpload()),
            this, SLOT(slotEndUpload()));

    d->widget->progressBar()->show();
    d->widget->progressBar()->progressScheduled(i18n("MediaWiki export"), true, true);
    d->widget->progressBar()->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));
    d->uploadJob->begin();
}

void WMWindow::slotChangeUserClicked()
{
    enableButton(User1, false);
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
    kDebug() << loginJob->error() << loginJob->errorString() << loginJob->errorText();

    if(loginJob->error())
    {
        d->login.clear();
        d->pass.clear();
        d->uploadJob = 0;
        //TODO Message d'erreur de login
        KMessageBox::error(this, i18n("Login error\nPlease check your credentials and try again."));
    }
    else
    {
        d->uploadJob = new WikiMediaJob(iface(), d->mediawiki, this);
        enableButton(User1, true);
        d->widget->invertAccountLoginBox();
        d->widget->updateLabels(d->login, d->wikiName, d->wikiUrl.toString());
    }

    return loginJob->error();
}

void WMWindow::slotEndUpload()
{
    disconnect(d->uploadJob, SIGNAL(uploadProgress(int)),
               d->widget->progressBar(),SLOT(setValue(int)));

    disconnect(d->uploadJob, SIGNAL(endUpload()),
               this, SLOT(slotEndUpload()));

    KMessageBox::information(this, i18n("Upload finished with no errors."));
    d->widget->progressBar()->hide();
    d->widget->progressBar()->progressCompleted();
}

bool WMWindow::eventFilter(QObject* /*obj*/, QEvent* event)
{
    if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* const c = dynamic_cast<QKeyEvent *>(event);

        if(c && c->key() == Qt::Key_Return)
        {
            event->ignore();
            kDebug() << "Key event pass";
            return false;

        }
    }

    return true;
}

} // namespace KIPIWikiMediaPlugin
