/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : a dialog to edit EXIF metadata
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exifeditdialog.h"
#include "exifeditdialog.moc"

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

namespace KIPIMetadataEditPlugin
{

class EXIFEditDialogPrivate
{

public:

    EXIFEditDialogPrivate()
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

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;
};

EXIFEditDialog::EXIFEditDialog(QWidget* parent, KUrl::List urls, KIPI::Interface *iface)
              : KPageDialog(parent), d(new EXIFEditDialogPrivate)
{
    d->urls      = urls;
    d->interface = iface;
    d->currItem  = d->urls.begin();

    setButtons(d->urls.count() > 1 ? Help|User1|User2|Ok|Apply|Close
                                   : Help|Ok|Apply|Close);
    setDefaultButton(Ok);
    setButtonIcon(User1, KIcon("go-next"));
    setButtonIcon(User2, KIcon("go-previous"));
    setButtonText(User1, i18n("Next"));
    setButtonText(User2, i18n("Previous"));
    setFaceType(List);
    setModal(true);

    // ---------------------------------------------------------------

    d->captionPage   = new EXIFCaption(this);
    d->page_caption  = addPage(d->captionPage, i18n("Caption"));
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

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("Edit Metadata"),
                                            0,
                                            KAboutData::License_GPL,
                                            ki18n("A Plugin to edit pictures' metadata."),
                                            ki18n("(c) 2006-2009, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author and Maintainer"),
                        "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

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

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotUser1()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotUser2()));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotApply()));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));

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
    KToolInvocation::invokeHelp("metadataedit", "kipi-plugins");
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
    close();
}

void EXIFEditDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Metadata Edit Settings");
    showPage(group.readEntry("EXIF Edit Page", 0));
    d->captionPage->setCheckedSyncJFIFComment(group.readEntry("Sync JFIF Comment", true));
    d->captionPage->setCheckedSyncHOSTComment(group.readEntry("Sync Host Comment", true));
    d->captionPage->setCheckedSyncXMPCaption(group.readEntry("Sync XMP Caption", true));
    d->captionPage->setCheckedSyncIPTCCaption(group.readEntry("Sync IPTC Caption", true));
    d->datetimePage->setCheckedSyncHOSTDate(group.readEntry("Sync Host Date", true));
    d->datetimePage->setCheckedSyncXMPDate(group.readEntry("Sync XMP Date", true));
    d->datetimePage->setCheckedSyncIPTCDate(group.readEntry("Sync IPTC Date", true));
    KConfigGroup group2 = config.group(QString("EXIF Edit Dialog"));
    restoreDialogSize(group2);
}

void EXIFEditDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Metadata Edit Settings");
    group.writeEntry("EXIF Edit Page", activePageIndex());
    group.writeEntry("Sync JFIF Comment", d->captionPage->syncJFIFCommentIsChecked());
    group.writeEntry("Sync Host Comment", d->captionPage->syncHOSTCommentIsChecked());
    group.writeEntry("Sync XMP Caption", d->captionPage->syncXMPCaptionIsChecked());
    group.writeEntry("Sync IPTC Caption", d->captionPage->syncIPTCCaptionIsChecked());
    group.writeEntry("Sync Host Date", d->datetimePage->syncHOSTDateIsChecked());
    group.writeEntry("Sync XMP Date", d->datetimePage->syncXMPDateIsChecked());
    group.writeEntry("Sync IPTC Date", d->datetimePage->syncIPTCDateIsChecked());
    KConfigGroup group2 = config.group(QString("EXIF Edit Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void EXIFEditDialog::slotItemChanged()
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.load((*d->currItem).path());
    d->exifData = exiv2Iface.getExif();
    d->iptcData = exiv2Iface.getIptc();
    d->xmpData  = exiv2Iface.getXmp();
    d->captionPage->readMetadata(d->exifData);
    d->datetimePage->readMetadata(d->exifData);
    d->lensPage->readMetadata(d->exifData);
    d->devicePage->readMetadata(d->exifData);
    d->lightPage->readMetadata(d->exifData);
    d->adjustPage->readMetadata(d->exifData);

    d->isReadOnly = !KExiv2Iface::KExiv2::canWriteExif((*d->currItem).path());
    d->page_caption->setEnabled(!d->isReadOnly);
    d->page_datetime->setEnabled(!d->isReadOnly);
    d->page_lens->setEnabled(!d->isReadOnly);
    d->page_device->setEnabled(!d->isReadOnly);
    d->page_light->setEnabled(!d->isReadOnly);
    d->page_adjust->setEnabled(!d->isReadOnly);
    enableButton(Apply, !d->isReadOnly);

    setCaption(QString("%1 (%2/%3) - %4")
               .arg((*d->currItem).fileName())
               .arg(d->urls.indexOf(*(d->currItem))+1)
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

        KExiv2Iface::KExiv2 exiv2Iface;
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

        if (k->modifiers() == Qt::ControlModifier &&
            (k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return))
        {
            slotApply();

            if (isButtonEnabled(User1))
                slotUser1();

            return true;
        }
        else if (k->modifiers() == Qt::ShiftModifier &&
                 (k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return))
        {
            slotApply();

            if (isButtonEnabled(User2))
                slotUser2();

            return true;
        }

        return false;
    }

    return false;
}

void EXIFEditDialog::showPage(int page)
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

int EXIFEditDialog::activePageIndex()
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

}  // namespace KIPIMetadataEditPlugin
