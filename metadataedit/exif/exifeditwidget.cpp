/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-03-14
 * Description : a KPageWidget to edit EXIF metadata
 *
 * Copyright (C) 2006-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011 by Victor Dodon <dodonvictor at gmail dot com>

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

#include "exifeditwidget.moc"

// Qt includes

#include <QCloseEvent>
#include <QKeyEvent>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kguiitem.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <KConfigGroup>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>
#include <libkipi/plugin.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// Local includes

#include "exifadjust.h"
#include "exifcaption.h"
#include "exifdatetime.h"
#include "exifdevice.h"
#include "exiflens.h"
#include "exiflight.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"

using namespace KExiv2Iface;
using namespace KIPIPlugins;

namespace KIPIMetadataEditPlugin
{

class allEXIFEditWidgetPrivate
{

public:

    allEXIFEditWidgetPrivate()
    {
        modified      = false;
        isReadOnly    = false;
        interface     = 0;

        about         = 0;

        page_caption  = 0;
        page_datetime = 0;
        page_lens     = 0;
        page_device   = 0;
        page_light    = 0;
        page_adjust   = 0;

        captionPage   = 0;
        datetimePage  = 0;
        lensPage      = 0;
        devicePage    = 0;
        lightPage     = 0;
        adjustPage    = 0;
    }

    bool                      modified;
    bool                      isReadOnly;

    QByteArray                exifData;
    QByteArray                iptcData;
    QByteArray                xmpData;

    KPageWidgetItem          *page_caption;
    KPageWidgetItem          *page_datetime;
    KPageWidgetItem          *page_lens;
    KPageWidgetItem          *page_device;
    KPageWidgetItem          *page_light;
    KPageWidgetItem          *page_adjust;

    KUrl::List                urls;

    KUrl::List::iterator      currItem;

    EXIFCaption              *captionPage;
    EXIFDateTime             *datetimePage;
    EXIFLens                 *lensPage;
    EXIFDevice               *devicePage;
    EXIFLight                *lightPage;
    EXIFAdjust               *adjustPage;

    Interface                *interface;

    KPAboutData              *about;
};

allEXIFEditWidget::allEXIFEditWidget(QWidget* parent, KUrl::List urls, Interface *iface)
              : KPageWidget(parent), d(new allEXIFEditWidgetPrivate)
{
    d->urls      = urls;
    d->interface = iface;
    d->currItem  = d->urls.begin();

    // ---------------------------------------------------------------

    d->captionPage   = new EXIFCaption(this);
    d->page_caption  = addPage(d->captionPage, i18nc("image caption", "Caption"));
    d->page_caption->setHeader(i18n("Caption Information"));
    d->page_caption->setIcon(KIcon("edit-rename"));

    d->datetimePage  = new EXIFDateTime(this);
    d->page_datetime = addPage(d->datetimePage, i18n("Date & Time"));
    d->page_datetime->setHeader(i18n("Date and Time Information"));
    d->page_datetime->setIcon(KIcon("view-calendar-day"));

    d->lensPage      = new EXIFLens(this);
    d->page_lens     = addPage(d->lensPage, i18n("Lens"));
    d->page_lens->setHeader(i18n("Lens Settings"));
    d->page_lens->setIcon(KIcon("camera-photo"));

    d->devicePage    = new EXIFDevice(this);
    d->page_device   = addPage(d->devicePage, i18n("Device"));
    d->page_device->setHeader(i18n("Capture Device Settings"));
    d->page_device->setIcon(KIcon("scanner"));

    d->lightPage     = new EXIFLight(this);
    d->page_light    = addPage(d->lightPage, i18n("Light"));
    d->page_light->setHeader(i18n("Light Source Information"));
    d->page_light->setIcon(KIcon("image-x-generic"));

    d->adjustPage    = new EXIFAdjust(this);
    d->page_adjust   = addPage(d->adjustPage, i18nc("Picture adjustments", "Adjustments"));
    d->page_adjust->setHeader(i18n("Pictures Adjustments"));
    d->page_adjust->setIcon(KIcon("fill-color"));

    // ------------------------------------------------------------

    connect(d->captionPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->datetimePage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->lensPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->devicePage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->lightPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->adjustPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    // ------------------------------------------------------------

    readSettings();
    slotItemChanged();
}

allEXIFEditWidget::~allEXIFEditWidget()
{
    delete d->about;
    delete d;
}

void allEXIFEditWidget::slotClose()
{
    saveSettings();
}

void allEXIFEditWidget::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("All Metadata Edit Settings");
    showPage(group.readEntry("All EXIF Edit Page", 0));
    d->captionPage->setCheckedSyncJFIFComment(group.readEntry("All Sync JFIF Comment", true));
    d->captionPage->setCheckedSyncHOSTComment(group.readEntry("All Sync Host Comment", true));
    d->captionPage->setCheckedSyncXMPCaption(group.readEntry("All Sync XMP Caption", true));
    d->captionPage->setCheckedSyncIPTCCaption(group.readEntry("All Sync IPTC Caption", true));
    d->datetimePage->setCheckedSyncHOSTDate(group.readEntry("All Sync Host Date", true));
    d->datetimePage->setCheckedSyncXMPDate(group.readEntry("All Sync XMP Date", true));
    d->datetimePage->setCheckedSyncIPTCDate(group.readEntry("All Sync IPTC Date", true));
}

void allEXIFEditWidget::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("All Metadata Edit Settings");
    group.writeEntry("All EXIF Edit Page", activePageIndex());
    group.writeEntry("All Sync JFIF Comment", d->captionPage->syncJFIFCommentIsChecked());
    group.writeEntry("All Sync Host Comment", d->captionPage->syncHOSTCommentIsChecked());
    group.writeEntry("All Sync XMP Caption", d->captionPage->syncXMPCaptionIsChecked());
    group.writeEntry("All Sync IPTC Caption", d->captionPage->syncIPTCCaptionIsChecked());
    group.writeEntry("All Sync Host Date", d->datetimePage->syncHOSTDateIsChecked());
    group.writeEntry("All Sync XMP Date", d->datetimePage->syncXMPDateIsChecked());
    group.writeEntry("All Sync IPTC Date", d->datetimePage->syncIPTCDateIsChecked());
    config.sync();
}

void allEXIFEditWidget::slotItemChanged()
{
    KExiv2 exiv2Iface;
    exiv2Iface.load((*d->currItem).path());

#if KEXIV2_VERSION >= 0x010000
    d->exifData = exiv2Iface.getExifEncoded();
#else
    d->exifData = exiv2Iface.getExif();
#endif

    d->iptcData = exiv2Iface.getIptc();
    d->xmpData  = exiv2Iface.getXmp();
    d->captionPage->readMetadata(d->exifData);
    d->datetimePage->readMetadata(d->exifData);
    d->lensPage->readMetadata(d->exifData);
    d->devicePage->readMetadata(d->exifData);
    d->lightPage->readMetadata(d->exifData);
    d->adjustPage->readMetadata(d->exifData);

    d->isReadOnly = !KExiv2::canWriteExif((*d->currItem).path());
    emit signalSetReadOnly(d->isReadOnly);
    d->page_caption->setEnabled(!d->isReadOnly);
    d->page_datetime->setEnabled(!d->isReadOnly);
    d->page_lens->setEnabled(!d->isReadOnly);
    d->page_device->setEnabled(!d->isReadOnly);
    d->page_light->setEnabled(!d->isReadOnly);
    d->page_adjust->setEnabled(!d->isReadOnly);
}

void allEXIFEditWidget::slotApply()
{
    if (d->modified && !d->isReadOnly)
    {
        ImageInfo info = d->interface->info(*d->currItem);

        if (d->captionPage->syncHOSTCommentIsChecked())
        {
            info.setDescription(d->captionPage->getEXIFUserComments());
        }
        d->captionPage->applyMetadata(d->exifData, d->iptcData, d->xmpData);

        if (d->datetimePage->syncHOSTDateIsChecked())
        {
            info.setTime(d->datetimePage->getEXIFCreationDate());
        }
        d->datetimePage->applyMetadata(d->exifData, d->iptcData, d->xmpData);

        d->lensPage->applyMetadata(d->exifData);
        d->devicePage->applyMetadata(d->exifData);
        d->lightPage->applyMetadata(d->exifData);
        d->adjustPage->applyMetadata(d->exifData);

        KExiv2 exiv2Iface;
        exiv2Iface.setWriteRawFiles(d->interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
        exiv2Iface.setUpdateFileTimeStamp(d->interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

        exiv2Iface.load((*d->currItem).path());
        exiv2Iface.setExif(d->exifData);
        exiv2Iface.setIptc(d->iptcData);
        exiv2Iface.setXmp(d->xmpData);
        exiv2Iface.save((*d->currItem).path());

        d->modified = false;
    }
}

void allEXIFEditWidget::slotUser1()
{
    slotApply();
    d->currItem++;
    slotItemChanged();
}

void allEXIFEditWidget::slotUser2()
{
    slotApply();
    d->currItem--;
    slotItemChanged();
}

void allEXIFEditWidget::slotModified()
{
    if (!d->isReadOnly)
    {
        d->modified = true;
        emit signalModified();
    }
}

void allEXIFEditWidget::slotOk()
{
    slotApply();
    saveSettings();
}

void allEXIFEditWidget::showPage(int page)
{
    switch(page)
    {
        case 0:
            setCurrentPage(d->page_caption);
            break;
        case 1:
            setCurrentPage(d->page_datetime);
            break;
        case 2:
            setCurrentPage(d->page_lens);
            break;
        case 3:
            setCurrentPage(d->page_device);
            break;
        case 4:
            setCurrentPage(d->page_light);
            break;
        case 5:
            setCurrentPage(d->page_adjust);
            break;
        default:
            setCurrentPage(d->page_caption);
            break;
    }
}

int allEXIFEditWidget::activePageIndex()
{
    KPageWidgetItem *cur = currentPage();

    if (cur == d->page_caption)  return 0;
    if (cur == d->page_datetime) return 1;
    if (cur == d->page_lens)     return 2;
    if (cur == d->page_device)   return 3;
    if (cur == d->page_light)    return 4;
    if (cur == d->page_adjust)   return 5;

    return 0;
}

bool allEXIFEditWidget::isModified()
{
    return d->modified;
}

}  // namespace KIPIMetadataEditPlugin
