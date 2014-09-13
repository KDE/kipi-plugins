/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-01
 * Description : image files selector dialog.
 *
 * Copyright (C) 2004-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpimagedialog.moc"

// Qt includes

#include <QLabel>
#include <QVBoxLayout>
#include <QPointer>
#include <QDesktopServices>

// KDE includes

#include <kdeversion.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kio/previewjob.h>

// LibKDcraw includes

#include <libkdcraw/dcrawinfocontainer.h>
#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Libkipi includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "kprawthumbthread.h"
#include "kpmetadata.h"

using namespace KIPI;
using namespace KDcrawIface;

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
        loadRawThumb = 0;

        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }
    }

    QLabel*           imageLabel;
    QLabel*           infoLabel;

    KUrl              currentUrl;

    KPMetadata        metaIface;

    Interface*        iface;

    KPRawThumbThread* loadRawThumb;
};

KPImageDialogPreview::KPImageDialogPreview(QWidget* const parent)
    : KPreviewWidgetBase(parent), d(new Private)
{
    QVBoxLayout* const vlay = new QVBoxLayout(this);
    d->imageLabel           = new QLabel(this);
    d->imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->imageLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    d->infoLabel            = new QLabel(this);
    d->infoLabel->setAlignment(Qt::AlignCenter);

    vlay->setMargin(0);
    vlay->setSpacing(KDialog::spacingHint());
    vlay->addWidget(d->imageLabel);
    vlay->addWidget(d->infoLabel);
    vlay->addStretch();

    setSupportedMimeTypes(KImageIO::mimeTypes());

    if (d->iface)
    {
        connect(d->iface, SIGNAL(gotThumbnail(KUrl,QPixmap)),
                this, SLOT(slotThumbnail(KUrl,QPixmap)));
    }

    d->loadRawThumb = new KPRawThumbThread(this);

    connect(d->loadRawThumb, SIGNAL(signalRawThumb(KUrl,QImage)),
            this, SLOT(slotRawThumb(KUrl,QImage)));
}

KPImageDialogPreview::~KPImageDialogPreview()
{
    d->loadRawThumb->cancel();
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
    KUrl url(d->currentUrl);
    clearPreview();
    showPreview(url);
}

void KPImageDialogPreview::showPreview(const KUrl& url)
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
            if ( !d->currentUrl.isValid() )
                return;

#if KDE_IS_VERSION(4,7,0)
            KFileItemList items;
            items.append(KFileItem(KFileItem::Unknown, KFileItem::Unknown, d->currentUrl, true));
            KIO::PreviewJob* const job = KIO::filePreview(items, QSize(256, 256));
#else
            KIO::PreviewJob* const job = KIO::filePreview(d->currentUrl, 256);
#endif

            connect(job, SIGNAL(gotPreview(KFileItem,QPixmap)),
                    this, SLOT(slotKDEPreview(KFileItem,QPixmap)));

            connect(job, SIGNAL(failed(KFileItem)),
                    this, SLOT(slotKDEPreviewFailed(KFileItem)));
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
                dateTime = KGlobal::locale()->formatDateTime(d->metaIface.getImageDateTime(),
                                                             KLocale::ShortDate, true);

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
        else
        {
            // Try to use libkdcraw interface to identify image.

            DcrawInfoContainer info;
            KDcraw             dcrawIface;
            dcrawIface.rawFileIdentify(info, d->currentUrl.path());
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
        identify += cellBeg + i18n("<i>Make:</i>")        + cellMid + make         + cellEnd;
        identify += cellBeg + i18n("<i>Model:</i>")       + cellMid + model        + cellEnd;
        identify += cellBeg + i18n("<i>Created:</i>")     + cellMid + dateTime     + cellEnd;
        identify += cellBeg + i18n("<i>Aperture:</i>")    + cellMid + aperture     + cellEnd;
        identify += cellBeg + i18n("<i>Focal:</i>")       + cellMid + focalLength  + cellEnd;
        identify += cellBeg + i18n("<i>Exposure:</i>")    + cellMid + exposureTime + cellEnd;
        identify += cellBeg + i18n("<i>Sensitivity:</i>") + cellMid + sensitivity  + cellEnd;
        identify += "</table></center></qt>";

        d->infoLabel->setText(identify);
    }
}

// Used only if Kipi interface is null.
void KPImageDialogPreview::slotKDEPreview(const KFileItem& item, const QPixmap& pix)
{
    if (!pix.isNull())
        slotThumbnail(item.url(), pix);
}

void KPImageDialogPreview::slotKDEPreviewFailed(const KFileItem& item)
{
    d->loadRawThumb->getRawThumb(item.url());
}

void KPImageDialogPreview::slotRawThumb(const KUrl& url, const QImage& img)
{
    slotThumbnail(url, QPixmap::fromImage(img));
}

void KPImageDialogPreview::slotThumbnail(const KUrl& url, const QPixmap& pix)
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
    d->currentUrl = KUrl();
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

    KUrl             url;
    KUrl::List       urls;

    Interface*       iface;
};

KPImageDialog::KPImageDialog(QWidget* const parent, bool singleSelect, bool onlyRaw)
           : d(new Private)
{
    d->singleSelect = singleSelect;
    d->onlyRaw      = onlyRaw;

    QStringList patternList;
    QString     allPictures;

    if (!d->onlyRaw)
    {
        patternList = KImageIO::pattern(KImageIO::Reading).split('\n', QString::SkipEmptyParts);

        // All Images from list must been always the first entry given by KDE API
        allPictures = patternList[0];

        allPictures.insert(allPictures.indexOf("|"), QString(KDcraw::rawFiles()) + QString(" *.JPE *.TIF"));
        patternList.removeAll(patternList[0]);
        patternList.prepend(allPictures);
    }
    else
    {
        allPictures.insert(allPictures.indexOf("|"), QString(KDcraw::rawFiles()) + QString(" *.JPE *.TIF"));
        patternList.prepend(allPictures);
    }

    // Added RAW file formats supported by dcraw program like a type mime.
    // Nota: we cannot use here "image/x-raw" type mime from KDE because it uncomplete
    // or unavailable(see file #121242 in bug).
    patternList.append(i18n("\n%1|Camera RAW files", QString(KDcraw::rawFiles())));

    d->fileFormats = patternList.join("\n");

    QString alternatePath         = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
    QPointer<KFileDialog> dlg     = new KFileDialog(d->iface ? d->iface->currentAlbum().path().path()
                                                             : alternatePath,
                                                    d->fileFormats, parent);
    KPImageDialogPreview* const preview = new KPImageDialogPreview(dlg);
    dlg->setPreviewWidget(preview);
    dlg->setOperationMode(KFileDialog::Opening);

    if (singleSelect)
    {
        dlg->setMode( KFile::File );
        dlg->setWindowTitle(i18n("Select an Image"));
        dlg->exec();
        d->url = dlg->selectedUrl();
    }
    else
    {
        dlg->setMode( KFile::Files );
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

KUrl KPImageDialog::url() const
{
    return d->url;
}

KUrl::List KPImageDialog::urls() const
{
    return d->urls;
}

KUrl KPImageDialog::getImageUrl(QWidget* const parent, bool onlyRaw)
{
    KPImageDialog dlg(parent, true, onlyRaw);

    if (dlg.url().isValid())
    {
        return dlg.url();
    }
    else
    {
        return KUrl();
    }
}

KUrl::List KPImageDialog::getImageUrls(QWidget* const parent, bool onlyRaw)
{
    KPImageDialog dlg(parent, false, onlyRaw);

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
