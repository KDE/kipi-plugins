/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-01
 * Description : image files selector dialog.
 *
 * Copyright (C) 2004-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpimagedialog.h"

// Qt includes

#include <QApplication>
#include <QStyle>
#include <QLabel>
#include <QVBoxLayout>
#include <QPointer>
#include <QDesktopServices>
#include <QImageReader>
#include <QFileDialog>
#include <QLocale>
#include <QStandardPaths>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/ImageCollection>
#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"

using namespace KIPI;

namespace KIPIPlugins
{

class KPImageDialogPreview::Private
{

public:

    Private()
    {
        imageLabel   = 0;
        infoLabel    = 0;
        iface        = 0;

        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
    }

public:

    QLabel*           imageLabel;
    QLabel*           infoLabel;

    QUrl              currentUrl;

    KPMetadata        metaIface;

    Interface*        iface;
};

KPImageDialogPreview::KPImageDialogPreview(QWidget* const parent)
    : QScrollArea(parent),
      d(new Private)
{
    QVBoxLayout* const vlay = new QVBoxLayout(this);
    d->imageLabel           = new QLabel(this);
    d->imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->imageLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    d->infoLabel            = new QLabel(this);
    d->infoLabel->setAlignment(Qt::AlignCenter);

    vlay->setMargin(0);
    vlay->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    vlay->addWidget(d->imageLabel);
    vlay->addWidget(d->infoLabel);
    vlay->addStretch();

    if (d->iface)
    {
        connect(d->iface, &Interface::gotThumbnail,
                this, &KPImageDialogPreview::slotThumbnail);
    }
}

KPImageDialogPreview::~KPImageDialogPreview()
{
    delete d;
}

QSize KPImageDialogPreview::sizeHint() const
{
    return QSize(256, 256);
}

void KPImageDialogPreview::resizeEvent(QResizeEvent*)
{
    QMetaObject::invokeMethod(this, "showPreview", Qt::QueuedConnection);
}

void KPImageDialogPreview::showPreview()
{
    QUrl url(d->currentUrl);
    clearPreview();
    showPreview(url);
}

void KPImageDialogPreview::showPreview(const QUrl& url)
{
    if (!url.isValid())
    {
        clearPreview();
        return;
    }

    if (url != d->currentUrl)
    {
        QString make, model, dateTime, aperture, focalLength, exposureTime, sensitivity;
        QString unavailable(i18n("<i>unavailable</i>"));
        clearPreview();
        d->currentUrl = url;

        if (d->iface)
        {
            d->iface->thumbnail(d->currentUrl, 256);
        }
        else
        {
            qCDebug(KIPIPLUGINS_LOG) << "No KIPI interface available : thumbnails will not generated.";
        }

        // Try to use libkexiv2 to identify image.

        if (d->metaIface.load(d->currentUrl.path()) &&
            (d->metaIface.hasExif() || d->metaIface.hasXmp()))
        {
            make = d->metaIface.getExifTagString("Exif.Image.Make");
            if (make.isEmpty())
                make = d->metaIface.getXmpTagString("Xmp.tiff.Make");

            model = d->metaIface.getExifTagString("Exif.Image.Model");
            if (model.isEmpty())
                model = d->metaIface.getXmpTagString("Xmp.tiff.Model");

            if (d->metaIface.getImageDateTime().isValid())
                dateTime = QLocale().toString(d->metaIface.getImageDateTime(), QLocale::ShortFormat);

            aperture = d->metaIface.getExifTagString("Exif.Photo.FNumber");
            if (aperture.isEmpty())
            {
                aperture = d->metaIface.getExifTagString("Exif.Photo.ApertureValue");
                if (aperture.isEmpty())
                {
                    aperture = d->metaIface.getXmpTagString("Xmp.exif.FNumber");
                    if (aperture.isEmpty())
                        aperture = d->metaIface.getXmpTagString("Xmp.exif.ApertureValue");
                }
            }

            focalLength = d->metaIface.getExifTagString("Exif.Photo.FocalLength");
            if (focalLength.isEmpty())
                focalLength = d->metaIface.getXmpTagString("Xmp.exif.FocalLength");

            exposureTime = d->metaIface.getExifTagString("Exif.Photo.ExposureTime");
            if (exposureTime.isEmpty())
            {
                exposureTime = d->metaIface.getExifTagString("Exif.Photo.ShutterSpeedValue");
                if (exposureTime.isEmpty())
                {
                    exposureTime = d->metaIface.getXmpTagString("Xmp.exif.ExposureTime");
                    if (exposureTime.isEmpty())
                        exposureTime = d->metaIface.getXmpTagString("Xmp.exif.ShutterSpeedValue");
                }
            }

            sensitivity = d->metaIface.getExifTagString("Exif.Photo.ISOSpeedRatings");
            if (sensitivity.isEmpty())
            {
                sensitivity = d->metaIface.getExifTagString("Exif.Photo.ExposureIndex");
                if (sensitivity.isEmpty())
                {
                    sensitivity = d->metaIface.getXmpTagString("Xmp.exif.ISOSpeedRatings");
                    if (sensitivity.isEmpty())
                        sensitivity = d->metaIface.getXmpTagString("Xmp.exif.ExposureIndex");
                }
            }
        }

        if (make.isEmpty())         make         = unavailable;
        if (model.isEmpty())        model        = unavailable;
        if (dateTime.isEmpty())     dateTime     = unavailable;
        if (aperture.isEmpty())     aperture     = unavailable;
        if (focalLength.isEmpty())  focalLength  = unavailable;
        if (exposureTime.isEmpty()) exposureTime = unavailable;

        if (sensitivity.isEmpty()) sensitivity = unavailable;
        else sensitivity = i18n("%1 ISO", sensitivity);

        QString identify(QStringLiteral("<qt><center>"));
        QString cellBeg(QStringLiteral("<tr><td><nobr><font size=-1>"));
        QString cellMid(QStringLiteral("</font></nobr></td><td><nobr><font size=-1>"));
        QString cellEnd(QStringLiteral("</font></nobr></td></tr>"));

        identify += QStringLiteral("<table cellspacing=0 cellpadding=0>");
        identify += cellBeg + i18n("<i>Make:</i>")        + cellMid + make         + cellEnd;
        identify += cellBeg + i18n("<i>Model:</i>")       + cellMid + model        + cellEnd;
        identify += cellBeg + i18n("<i>Created:</i>")     + cellMid + dateTime     + cellEnd;
        identify += cellBeg + i18n("<i>Aperture:</i>")    + cellMid + aperture     + cellEnd;
        identify += cellBeg + i18n("<i>Focal:</i>")       + cellMid + focalLength  + cellEnd;
        identify += cellBeg + i18n("<i>Exposure:</i>")    + cellMid + exposureTime + cellEnd;
        identify += cellBeg + i18n("<i>Sensitivity:</i>") + cellMid + sensitivity  + cellEnd;
        identify += QStringLiteral("</table></center></qt>");

        d->infoLabel->setText(identify);
    }
}

void KPImageDialogPreview::slotThumbnail(const QUrl& url, const QPixmap& pix)
{
    if (url == d->currentUrl)
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

void KPImageDialogPreview::clearPreview()
{
    d->imageLabel->clear();
    d->infoLabel->clear();
    d->currentUrl = QUrl();
}

// ------------------------------------------------------------------------

class KPImageDialog::Private
{

public:

    Private()
    {
        onlyRaw      = false;
        singleSelect = false;
        iface        = 0;

        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
    }

    bool             onlyRaw;
    bool             singleSelect;

    QString          fileFormats;

    QUrl             url;
    QList<QUrl>      urls;

    Interface*       iface;
};

KPImageDialog::KPImageDialog(QWidget* const parent, bool singleSelect, bool onlyRaw)
    : d(new Private)
{
    d->singleSelect = singleSelect;
    d->onlyRaw      = onlyRaw;

    QStringList patternList;
    QString     allPictures;
    QString     rawFiles;

    if (d->iface)
    {
        QPointer<RawProcessor> rawdec = d->iface->createRawProcessor();

        if (rawdec)
        {
            rawFiles = rawdec->rawFiles();
        }
    }
    
    if (!d->onlyRaw)
    {
        patternList = d->iface->supportedImageMimeTypes();

        // All Images from list must been always the first entry given by KDE API
        allPictures = patternList[0];

        allPictures.insert(allPictures.indexOf(QStringLiteral("|")), rawFiles + QStringLiteral(" *.JPE *.TIF"));
        patternList.removeAll(patternList[0]);
        patternList.prepend(allPictures);
    }
    else
    {
        allPictures.insert(allPictures.indexOf(QStringLiteral("|")), rawFiles + QStringLiteral(" *.JPE *.TIF"));
        patternList.prepend(allPictures);
    }

    // Added RAW file formats supported by dcraw program like a type mime.
    // Nota: we cannot use here "image/x-raw" type mime from KDE because it uncomplete
    // or unavailable(see file #121242 in bug).
    patternList.append(i18n("\n%1|Camera RAW files", rawFiles));

    d->fileFormats = patternList.join(QStringLiteral("\n"));

    QString alternatePath         = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QPointer<QFileDialog> dlg     = new QFileDialog(parent, QString(),
                                                    d->iface ? d->iface->currentAlbum().url().path()
                                                             : alternatePath,
                                                    d->fileFormats);
/*
    NOTE: KF5 do not provide a way to pass preview widget.
    KPImageDialogPreview* const preview = new KPImageDialogPreview(dlg);
    dlg->setPreviewWidget(preview);
*/
    dlg->setAcceptMode(QFileDialog::AcceptOpen);

    if (singleSelect)
    {
        dlg->setFileMode( QFileDialog::ExistingFile );
        dlg->setWindowTitle(i18n("Select an Image"));
        dlg->exec();
        d->url = dlg->selectedUrls().first();
    }
    else
    {
        dlg->setFileMode( QFileDialog::ExistingFiles );
        dlg->setWindowTitle(i18n("Select Images"));
        dlg->exec();
        d->urls = dlg->selectedUrls();
    }

    delete dlg;
}

KPImageDialog::~KPImageDialog()
{
    delete d;
}

bool KPImageDialog::onlyRaw() const
{
    return d->onlyRaw;
}

bool KPImageDialog::singleSelect() const
{
    return d->singleSelect;
}

QString KPImageDialog::fileFormats() const
{
    return d->fileFormats;
}

QUrl KPImageDialog::url() const
{
    return d->url;
}

QList<QUrl> KPImageDialog::urls() const
{
    return d->urls;
}

QUrl KPImageDialog::getImageUrl(QWidget* const parent, bool onlyRaw)
{
    KPImageDialog dlg(parent, true, onlyRaw);

    if (dlg.url().isValid())
    {
        return dlg.url();
    }
    else
    {
        return QUrl();
    }
}

QList<QUrl> KPImageDialog::getImageUrls(QWidget* const parent, bool onlyRaw)
{
    KPImageDialog dlg(parent, false, onlyRaw);

    if (!dlg.urls().isEmpty())
    {
        return dlg.urls();
    }
    else
    {
        return QList<QUrl>();
    }
}

} // namespace KIPIPlugins
