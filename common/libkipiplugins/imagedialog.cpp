/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-05-01
 * Description : image files selector dialog.
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

#include "imagedialog.h"
#include "imagedialog.moc"

// Qt includes.

#include <QLabel>
#include <QVBoxLayout>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kglobalsettings.h>
#include <kio/previewjob.h>

// LibKExiv2 includes.

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes.

#include <libkdcraw/dcrawinfocontainer.h>
#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

namespace KIPIPlugins
{

class ImageDialogPreviewPrivate
{

public:

    ImageDialogPreviewPrivate()
    {
        imageLabel = 0;
        infoLabel  = 0;
        iface      = 0;
    }

    QLabel              *imageLabel;
    QLabel              *infoLabel;

    KUrl                 currentURL;

    KDcrawIface::KDcraw  dcrawIface;

    KExiv2Iface::KExiv2  exiv2Iface;

    KIPI::Interface     *iface;
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

    d->infoLabel = new QLabel(this);
    d->infoLabel->setAlignment(Qt::AlignCenter);

    vlay->setMargin(0);
    vlay->setSpacing(KDialog::spacingHint());
    vlay->addWidget(d->imageLabel);
    vlay->addWidget(d->infoLabel);

    setSupportedMimeTypes(KImageIO::mimeTypes());

    if (d->iface)
    {
        connect(d->iface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
                this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));
    }
}

ImageDialogPreview::~ImageDialogPreview()
{
    delete d;
}

QSize ImageDialogPreview::sizeHint() const
{
    return QSize(256, 256);
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
        QString make, model, dateTime, aperture, focalLength, exposureTime, sensitivity;
        QString unavailable(i18n("<i>unavailable</i>"));
        clearPreview();
        d->currentURL = url;
        if (d->iface)
        {
            d->iface->thumbnail(d->currentURL, 256);
        }
        else
        {
            KIO::PreviewJob *job = KIO::filePreview(d->currentURL, 256);

            connect(job, SIGNAL(gotPreview(const KFileItem &, const QPixmap &)),
                    this, SLOT(slotKDEPreview(const KFileItem &, const QPixmap &)));
        }

        // Try to use libkexiv2 to identify image.

        d->exiv2Iface.load(d->currentURL.path());
        if (d->exiv2Iface.hasExif() || d->exiv2Iface.hasXmp())
        {
            make = d->exiv2Iface.getExifTagString("Exif.Image.Make");
            if (make.isEmpty())
                make = d->exiv2Iface.getXmpTagString("Xmp.tiff.Make");

            model = d->exiv2Iface.getExifTagString("Exif.Image.Model");
            if (model.isEmpty())
                model = d->exiv2Iface.getXmpTagString("Xmp.tiff.Model");

            if (d->exiv2Iface.getImageDateTime().isValid())
                dateTime = KGlobal::locale()->formatDateTime(d->exiv2Iface.getImageDateTime(),
                                                             KLocale::ShortDate, true);

            aperture = d->exiv2Iface.getExifTagString("Exif.Photo.FNumber");
            if (aperture.isEmpty())
            {
                aperture = d->exiv2Iface.getExifTagString("Exif.Photo.ApertureValue");
                if (aperture.isEmpty())
                {
                    aperture = d->exiv2Iface.getXmpTagString("Xmp.exif.FNumber");
                    if (aperture.isEmpty())
                        aperture = d->exiv2Iface.getXmpTagString("Xmp.exif.ApertureValue");
                }
            }

            focalLength = d->exiv2Iface.getExifTagString("Exif.Photo.FocalLength");
            if (focalLength.isEmpty())
                focalLength = d->exiv2Iface.getXmpTagString("Xmp.exif.FocalLength");

            exposureTime = d->exiv2Iface.getExifTagString("Exif.Photo.ExposureTime");
            if (exposureTime.isEmpty())
            {
                exposureTime = d->exiv2Iface.getExifTagString("Exif.Photo.ShutterSpeedValue");
                if (exposureTime.isEmpty())
                {
                    exposureTime = d->exiv2Iface.getXmpTagString("Xmp.exif.ExposureTime");
                    if (exposureTime.isEmpty())
                        exposureTime = d->exiv2Iface.getXmpTagString("Xmp.exif.ShutterSpeedValue");
                }
            }

            sensitivity = d->exiv2Iface.getExifTagString("Exif.Photo.ISOSpeedRatings");
            if (sensitivity.isEmpty())
            {
                sensitivity = d->exiv2Iface.getExifTagString("Exif.Photo.ExposureIndex");
                if (sensitivity.isEmpty())
                {
                    sensitivity = d->exiv2Iface.getXmpTagString("Xmp.exif.ISOSpeedRatings");
                    if (sensitivity.isEmpty())
                        sensitivity = d->exiv2Iface.getXmpTagString("Xmp.exif.ExposureIndex");
                }
            }
        }
        else
        {
            // Try to use libkdcraw interface to identify image.

            KDcrawIface::DcrawInfoContainer info;
            d->dcrawIface.rawFileIdentify(info, d->currentURL.path());
            if (info.isDecodable)
            {
                if (!info.make.isEmpty())
                    make = info.make;

                if (!info.model.isEmpty())
                    model = info.model;

                if (info.dateTime.isValid())
                    dateTime = KGlobal::locale()->formatDateTime(info.dateTime, KLocale::ShortDate, true);

                if (info.aperture != -1.0)
                    aperture = QString::number(info.aperture);

                if (info.focalLength != -1.0)
                    focalLength = QString::number(info.focalLength);

                if (info.exposureTime != -1.0)
                    exposureTime = QString::number(info.exposureTime);

                if (info.sensitivity != -1)
                    sensitivity = QString::number(info.sensitivity);
            }
            else
            {
                d->infoLabel->clear();
                return;
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

        QString identify("<qt><center>");
        QString cellBeg("<tr><td><nobr><font size=-1>");
        QString cellMid("</font></nobr></td><td><nobr><font size=-1>");
        QString cellEnd("</font></nobr></td></tr>");

        identify += "<table cellspacing=0 cellpadding=0>";
        identify += cellBeg + i18n("Make:")        + cellMid + make         + cellEnd;
        identify += cellBeg + i18n("Model:")       + cellMid + model        + cellEnd;
        identify += cellBeg + i18n("Created:")     + cellMid + dateTime     + cellEnd;
        identify += cellBeg + i18n("Aperture:")    + cellMid + aperture     + cellEnd;
        identify += cellBeg + i18n("Focal:")       + cellMid + focalLength  + cellEnd;
        identify += cellBeg + i18n("Exposure:")    + cellMid + exposureTime + cellEnd;
        identify += cellBeg + i18n("Sensitivity:") + cellMid + sensitivity  + cellEnd;
        identify += "</table></center></qt>";

        d->infoLabel->setText(identify);
    }
}

// Used only if Kipi interface is null.
void ImageDialogPreview::slotKDEPreview(const KFileItem& item, const QPixmap &pix)
{
    slotThumbnail(item.url(), pix);
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
    d->infoLabel->clear();
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

    QString          fileFormats;

    KUrl             url;
    KUrl::List       urls;

    KIPI::Interface *iface;
};

ImageDialog::ImageDialog(QWidget* parent, KIPI::Interface* iface, bool singleSelect, bool onlyRaw)
{
    d = new ImageDialogPrivate;
    d->singleSelect = singleSelect;
    d->onlyRaw      = onlyRaw;
    d->iface        = iface;

    QStringList patternList;
    QString     allPictures;

    if (!d->onlyRaw)
    {
        patternList = KImageIO::pattern(KImageIO::Reading).split('\n', QString::SkipEmptyParts);

        // All Images from list must been always the first entry given by KDE API
        allPictures = patternList[0];

#if KDCRAW_VERSION < 0x000400
        // Add other files format witch are missing to All Images" type mime provided by KDE and remplace current.
        if (KDcrawIface::DcrawBinary::instance()->versionIsRight())
        {
            allPictures.insert(allPictures.indexOf("|"), QString(KDcrawIface::DcrawBinary::instance()->rawFiles()) + QString(" *.JPE *.TIF"));
            patternList.removeAll(patternList[0]);
            patternList.prepend(allPictures);
        }
#else
        allPictures.insert(allPictures.indexOf("|"), QString(KDcrawIface::KDcraw::rawFiles()) + QString(" *.JPE *.TIF"));
        patternList.removeAll(patternList[0]);
        patternList.prepend(allPictures);
#endif
    }
    else
    {
#if KDCRAW_VERSION < 0x000400
        if (KDcrawIface::DcrawBinary::instance()->versionIsRight())
        {
            allPictures.insert(allPictures.indexOf("|"), QString(KDcrawIface::DcrawBinary::instance()->rawFiles()) + QString(" *.JPE *.TIF"));
            patternList.prepend(allPictures);
        }
#else
        allPictures.insert(allPictures.indexOf("|"), QString(KDcrawIface::KDcraw::rawFiles()) + QString(" *.JPE *.TIF"));
        patternList.prepend(allPictures);
#endif
    }

#if KDCRAW_VERSION < 0x000400
    // Added RAW file formats supported by dcraw program like a type mime.
    // Nota: we cannot use here "image/x-raw" type mime from KDE because it uncomplete
    // or unavailable(see file #121242 in B.K.O).
    if (KDcrawIface::DcrawBinary::instance()->versionIsRight())
        patternList.append(i18n("\n%1|Camera RAW files", QString(KDcrawIface::DcrawBinary::instance()->rawFiles())));
#else
    // Added RAW file formats supported by dcraw program like a type mime.
    // Nota: we cannot use here "image/x-raw" type mime from KDE because it uncomplete
    // or unavailable(see file #121242 in B.K.O).
    patternList.append(i18n("\n%1|Camera RAW files", QString(KDcrawIface::KDcraw::rawFiles())));
#endif

    d->fileFormats = patternList.join("\n");

    KFileDialog dlg(d->iface ? d->iface->currentAlbum().path().path()
                             : KGlobalSettings::documentPath(),
                    d->fileFormats, parent);
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

QString ImageDialog::fileFormats() const
{
    return d->fileFormats;
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
