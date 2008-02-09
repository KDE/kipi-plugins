/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : a dialog to edit EXIF metadata
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <qtimer.h>
#include <qframe.h>
#include <qlayout.h>
#include <qpushbutton.h>

// KDE includes.

#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/plugin.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "exifcaption.h"
#include "exifdatetime.h"
#include "exiflens.h"
#include "exifdevice.h"
#include "exiflight.h"
#include "exifadjust.h"
#include "exifeditdialog.h"
#include "exifeditdialog.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFEditDialogDialogPrivate
{

public:

    EXIFEditDialogDialogPrivate()
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

    QFrame                   *page_caption;
    QFrame                   *page_datetime;
    QFrame                   *page_lens;
    QFrame                   *page_device;
    QFrame                   *page_light;
    QFrame                   *page_adjust;

    KURL::List                urls;

    KURL::List::iterator      currItem;

    EXIFCaption              *captionPage;
    EXIFDateTime             *datetimePage;
    EXIFLens                 *lensPage;
    EXIFDevice               *devicePage;
    EXIFLight                *lightPage;
    EXIFAdjust               *adjustPage;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;
};

EXIFEditDialog::EXIFEditDialog(QWidget* parent, KURL::List urls, KIPI::Interface *iface)
              : KDialogBase(IconList, QString::null, 
                            urls.count() > 1 ? Help|User1|User2|Stretch|Ok|Apply|Close 
                                             : Help|Stretch|Ok|Apply|Close, 
                            Ok, parent, 0, true, true,
                            KStdGuiItem::guiItem(KStdGuiItem::Forward),
                            KStdGuiItem::guiItem(KStdGuiItem::Back) )
{
    d = new EXIFEditDialogDialogPrivate;
    d->urls      = urls;
    d->interface = iface;
    d->currItem  = d->urls.begin();

    // ---------------------------------------------------------------

    d->page_caption  = addPage(i18n("Caption"), i18n("Caption Information"),
                               BarIcon("editclear", KIcon::SizeMedium));
    d->captionPage   = new EXIFCaption(d->page_caption);

    d->page_datetime = addPage(i18n("Date & Time"), i18n("Date and Time"),
                                 BarIcon("today", KIcon::SizeMedium));
    d->datetimePage  = new EXIFDateTime(d->page_datetime);

    d->page_lens     = addPage(i18n("Lens"), i18n("Lens Settings"),
                               BarIcon("camera", KIcon::SizeMedium));
    d->lensPage      = new EXIFLens(d->page_lens);

    d->page_device   = addPage(i18n("Device"), i18n("Capture Device Settings"),
                               BarIcon("scanner", KIcon::SizeMedium));
    d->devicePage    = new EXIFDevice(d->page_device);

    d->page_light    = addPage(i18n("Light"), i18n("Light Source Information"),
                               BarIcon("idea", KIcon::SizeMedium));
    d->lightPage     = new EXIFLight(d->page_light);

    d->page_adjust   = addPage(i18n("Adjustments"), i18n("Pictures Adjustments"),
                               BarIcon("blend", KIcon::SizeMedium));
    d->adjustPage    = new EXIFAdjust(d->page_adjust);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(I18N_NOOP("Edit Metadata"),
                                            kipiplugins_version,
                                            KAboutData::License_GPL,
                                            I18N_NOOP("A Plugin to edit pictures metadata"),
                                            "(c) 2006-2007, Gilles Caulier");

    d->about->addAuthor("Gilles Caulier", I18N_NOOP("Author and Maintainer"),
                        "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Edit Metadata Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );

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

EXIFEditDialog::~EXIFEditDialog()
{
    delete d->about;
    delete d;
}

void EXIFEditDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("metadataedit", "kipi-plugins");
}

void EXIFEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void EXIFEditDialog::slotClose()
{
    saveSettings();
    KDialogBase::slotClose();
}

void EXIFEditDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("Metadata Edit Settings");
    showPage(config.readNumEntry("EXIF Edit Page", 0));
    d->captionPage->setCheckedSyncJFIFComment(config.readBoolEntry("Sync JFIF Comment", true));
    d->captionPage->setCheckedSyncHOSTComment(config.readBoolEntry("Sync Host Comment", true));
    d->captionPage->setCheckedSyncIPTCCaption(config.readBoolEntry("Sync IPTC Caption", true));
    d->datetimePage->setCheckedSyncHOSTDate(config.readBoolEntry("Sync Host Date", true));
    d->datetimePage->setCheckedSyncIPTCDate(config.readBoolEntry("Sync IPTC Date", true));
    resize(configDialogSize(config, QString("EXIF Edit Dialog")));
}

void EXIFEditDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("Metadata Edit Settings");
    config.writeEntry("EXIF Edit Page", activePageIndex());
    config.writeEntry("Sync JFIF Comment", d->captionPage->syncJFIFCommentIsChecked());
    config.writeEntry("Sync Host Comment", d->captionPage->syncHOSTCommentIsChecked());
    config.writeEntry("Sync IPTC Caption", d->captionPage->syncIPTCCaptionIsChecked());
    config.writeEntry("Sync Host Date", d->datetimePage->syncHOSTDateIsChecked());
    config.writeEntry("Sync IPTC Date", d->datetimePage->syncIPTCDateIsChecked());
    saveDialogSize(config, QString("EXIF Edit Dialog"));
    config.sync();
}

void EXIFEditDialog::slotItemChanged()
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.load((*d->currItem).path());
    d->exifData = exiv2Iface.getExif();
    d->iptcData = exiv2Iface.getIptc();
    d->captionPage->readMetadata(d->exifData);
    d->datetimePage->readMetadata(d->exifData);
    d->lensPage->readMetadata(d->exifData);
    d->devicePage->readMetadata(d->exifData);
    d->lightPage->readMetadata(d->exifData);
    d->adjustPage->readMetadata(d->exifData);

    d->isReadOnly = KExiv2Iface::KExiv2::isReadOnly((*d->currItem).path()); 
    d->page_caption->setEnabled(!d->isReadOnly);
    d->page_datetime->setEnabled(!d->isReadOnly);
    d->page_lens->setEnabled(!d->isReadOnly);
    d->page_device->setEnabled(!d->isReadOnly);
    d->page_light->setEnabled(!d->isReadOnly);
    d->page_adjust->setEnabled(!d->isReadOnly);
    enableButton(Apply, !d->isReadOnly);
    
    setCaption(QString("%1 (%2/%3) - %4")
               .arg((*d->currItem).filename())
               .arg(d->urls.findIndex(*(d->currItem))+1)
               .arg(d->urls.count())
               .arg(i18n("Edit EXIF Metadata")) + 
               (d->isReadOnly ? QString(" - ") + i18n("(read only)") : QString::null));
    enableButton(User1, *(d->currItem) != d->urls.last());
    enableButton(User2, *(d->currItem) != d->urls.first());
    enableButton(Apply, false);
}

void EXIFEditDialog::slotApply()
{
    if (d->modified && !d->isReadOnly) 
    {
        KIPI::ImageInfo info = d->interface->info(*d->currItem);

        if (d->captionPage->syncHOSTCommentIsChecked())
        {
            info.setDescription(d->captionPage->getEXIFUserComments());
        }
        d->captionPage->applyMetadata(d->exifData, d->iptcData);

        if (d->datetimePage->syncHOSTDateIsChecked())
        {
            info.setTime(d->datetimePage->getEXIFCreationDate());
        }
        d->datetimePage->applyMetadata(d->exifData, d->iptcData);

        d->lensPage->applyMetadata(d->exifData);
        d->devicePage->applyMetadata(d->exifData);
        d->lightPage->applyMetadata(d->exifData);
        d->adjustPage->applyMetadata(d->exifData);
        KExiv2Iface::KExiv2 exiv2Iface;
        exiv2Iface.load((*d->currItem).path());
        exiv2Iface.setExif(d->exifData);
        exiv2Iface.setIptc(d->iptcData);
        exiv2Iface.save((*d->currItem).path());
        d->modified = false;
    }
}

void EXIFEditDialog::slotUser1()
{
    slotApply();
    d->currItem++;
    slotItemChanged();
}

void EXIFEditDialog::slotUser2()
{
    slotApply();
    d->currItem--;
    slotItemChanged();
}

void EXIFEditDialog::slotModified()
{
    if (!d->isReadOnly)
    {
        enableButton(Apply, true);
        d->modified = true;
    }
}

void EXIFEditDialog::slotOk()
{
    slotApply();
    saveSettings();
    accept();
}

bool EXIFEditDialog::eventFilter(QObject *, QEvent *e)
{
    if ( e->type() == QEvent::KeyPress )
    {
        QKeyEvent *k = (QKeyEvent *)e;

        if (k->state() == Qt::ControlButton &&
            (k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return))
        {
            slotApply();

            if (actionButton(User1)->isEnabled())
                slotUser1();

            return true;
        }
        else if (k->state() == Qt::ShiftButton &&
                 (k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return))
        {
            slotApply();

            if (actionButton(User2)->isEnabled())
                slotUser2();

            return true;
        }

        return false;
    }

    return false;
}

}  // namespace KIPIMetadataEditPlugin
