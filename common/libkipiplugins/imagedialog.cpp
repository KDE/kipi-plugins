/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-05-01
 * Description : an image files selector dialog.
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// Qt includes.

#include <QLabel>
#include <QVBoxLayout>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kimageio.h>

// LibKDcraw includes.

#include <libkdcraw/rawfiles.h>
#include <libkdcraw/dcrawbinary.h>

// Local includes.

#include "imagedialog.h"
#include "imagedialog.moc"

namespace KIPIPlugins
{

class ImageDialogPreviewPrivate 
{

public:

    ImageDialogPreviewPrivate()
    {
        imageLabel = 0;
        iface      = 0;
    }

    QLabel          *imageLabel;

    KUrl             currentURL;

    KIPI::Interface *iface;
};

ImageDialogPreview::ImageDialogPreview(KIPI::Interface *iface, QWidget *parent)
                  : KPreviewWidgetBase(parent)
{
    d = new ImageDialogPreviewPrivate;
    d->iface = iface;

    QVBoxLayout *vlay = new QVBoxLayout(this);
    d->imageLabel     = new QLabel(this);
    d->imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->imageLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    vlay->setMargin(0);
    vlay->setSpacing(KDialog::spacingHint());
    vlay->addWidget(d->imageLabel);

    setSupportedMimeTypes(KImageIO::mimeTypes());
    setMinimumWidth(64);

    connect(d->iface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
            this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));
}

ImageDialogPreview::~ImageDialogPreview() 
{
    delete d;
}

QSize ImageDialogPreview::sizeHint() const
{
    return QSize(100, 200);
}

void ImageDialogPreview::resizeEvent(QResizeEvent *)
{
    QMetaObject::invokeMethod(this, "showPreview", Qt::QueuedConnection);
}

void ImageDialogPreview::showPreview()
{
    KUrl url(d->currentURL);
    clearPreview();
    showPreview(url);
}

void ImageDialogPreview::showPreview(const KUrl& url)
{
    if (!url.isValid()) 
    {
        clearPreview();
        return;
    }

    if (url != d->currentURL) 
    {
        clearPreview();
        d->currentURL = url;
        d->iface->thumbnail(d->currentURL, 256);
    }
}

void ImageDialogPreview::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    if (url == d->currentURL)
    {
        QPixmap pixmap;
        QSize s = d->imageLabel->contentsRect().size();

        if (s.width() < pix.width() || s.height() < pix.height())
            pixmap = pix.scaled(s, Qt::KeepAspectRatio);
        else 
            pixmap = pix;

        d->imageLabel->setPixmap(pixmap);
    }
}

void ImageDialogPreview::clearPreview()
{
    d->imageLabel->clear();
    d->currentURL = KUrl();
}

// ------------------------------------------------------------------------

class ImageDialogPrivate 
{

public:

    ImageDialogPrivate()
    {
        onlyRaw      = false;
        singleSelect = false;
        iface        = 0;
    }

    bool             onlyRaw;
    bool             singleSelect;

    QString          fileformats;

    KUrl             url;
    KUrl::List       urls;

    KIPI::Interface *iface;
};

ImageDialog::ImageDialog(QWidget* parent, KIPI::Interface* iface, bool singleSelect, bool onlyRaw)
{
    d = new ImageDialogPrivate;
    d->iface        = iface;
    d->singleSelect = singleSelect;
    d->onlyRaw      = onlyRaw;

    QStringList patternList;
    QString     allPictures;

    if (!d->onlyRaw)
    {
        patternList = KImageIO::pattern(KImageIO::Reading).split('\n', QString::SkipEmptyParts);

        // All Images from list must been always the first entry given by KDE API
        allPictures = patternList[0];

        // Add other files format witch are missing to All Images" type mime provided by KDE and remplace current.
        if (KDcrawIface::DcrawBinary::instance()->versionIsRight())
        {
            allPictures.insert(allPictures.indexOf("|"), QString(KDcrawIface::DcrawBinary::instance()->rawFiles()) + QString(" *.JPE *.TIF"));
            patternList.removeAll(patternList[0]);
            patternList.prepend(allPictures);
        }
    }
    else
    {
        if (KDcrawIface::DcrawBinary::instance()->versionIsRight())
        {
            allPictures.insert(allPictures.indexOf("|"), QString(KDcrawIface::DcrawBinary::instance()->rawFiles()) + QString(" *.JPE *.TIF"));
            patternList.prepend(allPictures);
        }
    }

    // Added RAW file formats supported by dcraw program like a type mime. 
    // Nota: we cannot use here "image/x-raw" type mime from KDE because it uncomplete 
    // or unavailable(see file #121242 in B.K.O).
    if (KDcrawIface::DcrawBinary::instance()->versionIsRight())
        patternList.append(i18n("\n%1|Camera RAW files",QString(KDcrawIface::DcrawBinary::instance()->rawFiles())));

    d->fileformats = patternList.join("\n");

    KFileDialog dlg(d->iface->currentAlbum().path(), d->fileformats, parent);
    ImageDialogPreview *preview = new ImageDialogPreview(d->iface, &dlg);
    dlg.setPreviewWidget(preview);
    dlg.setOperationMode(KFileDialog::Opening);

    if (singleSelect)
    {
        dlg.setMode( KFile::File );
        dlg.setWindowTitle(i18n("Select an Image"));
        dlg.exec();
        d->url = dlg.selectedUrl();

    }
    else
    {
        dlg.setMode( KFile::Files );
        dlg.setWindowTitle(i18n("Select Images"));
        dlg.exec();
        d->urls = dlg.selectedUrls();
    }
}

ImageDialog::~ImageDialog() 
{
    delete d;
}

bool ImageDialog::onlyRaw() const 
{
    return d->onlyRaw;
}

bool ImageDialog::singleSelect() const 
{
    return d->singleSelect;
}

QString ImageDialog::fileformats() const 
{
    return d->fileformats;
}

KUrl ImageDialog::url() const 
{
    return d->url;
}

KUrl::List ImageDialog::urls() const
{
    return d->urls;
}

KUrl ImageDialog::getImageURL(QWidget* parent, KIPI::Interface* iface, bool onlyRaw) 
{
    ImageDialog dlg(parent, iface, true, onlyRaw);

    if (dlg.url().isValid())
    {
        return dlg.url();
    }
    else 
    {
        return KUrl();
    }
}

KUrl::List ImageDialog::getImageURLs(QWidget* parent, KIPI::Interface* iface, bool onlyRaw)
{
    ImageDialog dlg(parent, iface, false, onlyRaw);
    if (!dlg.urls().isEmpty())
    {
        return dlg.urls();
    }
    else
    {
        return KUrl::List();
    }
}

} // namespace KIPIPlugins
