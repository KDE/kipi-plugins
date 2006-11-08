/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : a dialog to edit EXIF metadata
 * 
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
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
#include <kaboutdata.h>

// Local includes.

#include "pluginsversion.h"
#include "exiv2iface.h"
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

    bool                  modified;
    bool                  isReadOnly;

    QByteArray            exifData;

    QFrame               *page_caption;
    QFrame               *page_datetime;
    QFrame               *page_lens;
    QFrame               *page_device;
    QFrame               *page_light;
    QFrame               *page_adjust;
    
    KURL::List            urls;

    KURL::List::iterator  currItem;

    EXIFCaption          *captionPage;
    EXIFDateTime         *datetimePage;
    EXIFLens             *lensPage;
    EXIFDevice           *devicePage;
    EXIFLight            *lightPage;
    EXIFAdjust           *adjustPage;
};

EXIFEditDialog::EXIFEditDialog(QWidget* parent, KURL::List urls)
              : KDialogBase(IconList, QString::null, 
                            urls.count() > 1 ? Help|User1|User2|Stretch|Ok|Apply|Close 
                                             : Help|Stretch|Ok|Apply|Close, 
                            Ok, parent, 0, true, true,
                            KStdGuiItem::guiItem(KStdGuiItem::Forward),
                            KStdGuiItem::guiItem(KStdGuiItem::Back) )
{
    d = new EXIFEditDialogDialogPrivate;
    d->urls     = urls;
    d->currItem = d->urls.begin();

    // ---------------------------------------------------------------

    d->page_caption  = addPage(i18n("Caption"), i18n("Caption Informations"),
                               BarIcon("editclear", KIcon::SizeMedium));
    d->captionPage   = new EXIFCaption(d->page_caption);

    d->page_datetime = addPage(i18n("Date & Time"), i18n("Date and Time Informations"),
                                 BarIcon("today", KIcon::SizeMedium));
    d->datetimePage  = new EXIFDateTime(d->page_datetime);

    d->page_lens     = addPage(i18n("Lens"), i18n("Lens Settings"),
                               BarIcon("camera", KIcon::SizeMedium));
    d->lensPage      = new EXIFLens(d->page_lens);

    d->page_device   = addPage(i18n("Device"), i18n("Capture Device Settings"),
                               BarIcon("scanner", KIcon::SizeMedium));
    d->devicePage    = new EXIFDevice(d->page_device);

    d->page_light    = addPage(i18n("Light"), i18n("Light Source Informations"),
                               BarIcon("idea", KIcon::SizeMedium));
    d->lightPage     = new EXIFLight(d->page_light);

    d->page_adjust   = addPage(i18n("Adjustments"), i18n("Pictures Adjustments Informations"),
                               BarIcon("blend", KIcon::SizeMedium));
    d->adjustPage    = new EXIFAdjust(d->page_adjust);

    // ---------------------------------------------------------------
    // About data and help button.

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Edit Metadata"),
                                       kipiplugins_version,
                                       I18N_NOOP("A Plugin to edit pictures metadata"),
                                       KAboutData::License_GPL,
                                       "(c) 2006, Gilles Caulier",
                                       0,
                                       "http://extragear.kde.org/apps/kipi");

    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and Maintainer"),
                     "caulier dot gilles at kdemail dot net");

    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
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
    resize(configDialogSize(config, QString("EXIF Edit Dialog")));
}

void EXIFEditDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("Metadata Edit Settings");
    config.writeEntry("EXIF Edit Page", activePageIndex());
    saveDialogSize(config, QString("EXIF Edit Dialog"));
    config.sync();
}

void EXIFEditDialog::slotItemChanged()
{
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.load((*d->currItem).path());
    d->exifData = exiv2Iface.getExif();
    d->captionPage->readMetadata(d->exifData);
    d->datetimePage->readMetadata(d->exifData);
    d->lensPage->readMetadata(d->exifData);
    d->devicePage->readMetadata(d->exifData);
    d->lightPage->readMetadata(d->exifData);
    d->adjustPage->readMetadata(d->exifData);

    d->isReadOnly = KIPIPlugins::Exiv2Iface::isReadOnly((*d->currItem).path()); 
    d->page_caption->setEnabled(!d->isReadOnly);
    d->page_datetime->setEnabled(!d->isReadOnly);
    d->page_lens->setEnabled(!d->isReadOnly);
    d->page_device->setEnabled(!d->isReadOnly);
    d->page_light->setEnabled(!d->isReadOnly);
    d->page_adjust->setEnabled(!d->isReadOnly);
    enableButton(Apply, !d->isReadOnly);
    
    setCaption(i18n("%1 (%2/%3) - Edit EXIF Metadata%4")
               .arg((*d->currItem).filename())
               .arg(d->urls.findIndex(*(d->currItem))+1)
               .arg(d->urls.count())
               .arg(d->isReadOnly ? i18n(" - (read only)") : QString::null));
    enableButton(User1, *(d->currItem) != d->urls.last());
    enableButton(User2, *(d->currItem) != d->urls.first());
    enableButton(Apply, false);
}

void EXIFEditDialog::slotApply()
{
    if (d->modified && !d->isReadOnly) 
    {
        d->captionPage->applyMetadata(d->exifData);
        d->datetimePage->applyMetadata(d->exifData);
        d->lensPage->applyMetadata(d->exifData);
        d->devicePage->applyMetadata(d->exifData);
        d->lightPage->applyMetadata(d->exifData);
        d->adjustPage->applyMetadata(d->exifData);
        KIPIPlugins::Exiv2Iface exiv2Iface;
        exiv2Iface.load((*d->currItem).path());
        exiv2Iface.setExif(d->exifData);
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
