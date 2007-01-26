/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : a dialog to edit IPTC metadata
 * 
 * Copyright 2006-2007 by Gilles Caulier
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

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/plugin.h>

// LibKExiv2 includes. 

#include <libkexiv2/libkexiv2.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "iptccaption.h"
#include "iptccredits.h"
#include "iptcstatus.h"
#include "iptcorigin.h"
#include "iptcdatetime.h"
#include "iptckeywords.h"
#include "iptcsubjects.h"
#include "iptccategories.h"
#include "iptceditdialog.h"
#include "iptceditdialog.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCEditDialogDialogPrivate
{

public:

    IPTCEditDialogDialogPrivate()
    {
        modified      = false;
        isReadOnly    = false;
        page_caption  = 0;
        page_datetime = 0;
        page_subjects = 0;
        page_keywords = 0;
        page_credits  = 0;
        page_status   = 0;
        page_origin   = 0;

        about         = 0;

        captionPage   = 0;
        datetimePage  = 0;
        subjectsPage  = 0;
        keywordsPage  = 0;
        creditsPage   = 0;
        statusPage    = 0;
        originPage    = 0;
    }

    bool                      modified;
    bool                      isReadOnly;

    QByteArray                exifData;
    QByteArray                iptcData;

    QFrame                   *page_caption;
    QFrame                   *page_datetime;
    QFrame                   *page_subjects;
    QFrame                   *page_keywords;
    QFrame                   *page_categories;
    QFrame                   *page_credits;
    QFrame                   *page_status;
    QFrame                   *page_origin;

    KURL::List                urls;

    KURL::List::iterator      currItem;

    IPTCCaption              *captionPage;
    IPTCDateTime             *datetimePage;
    IPTCSubjects             *subjectsPage;
    IPTCKeywords             *keywordsPage;
    IPTCCategories           *categoriesPage;
    IPTCCredits              *creditsPage;
    IPTCStatus               *statusPage;
    IPTCOrigin               *originPage;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;
};

IPTCEditDialog::IPTCEditDialog(QWidget* parent, KURL::List urls, KIPI::Interface *iface)
              : KDialogBase(IconList, QString::null, 
                            urls.count() > 1 ? Help|User1|User2|Stretch|Ok|Apply|Close 
                                             : Help|Stretch|Ok|Apply|Close, 
                            Ok, parent, 0, true, true,
                            KStdGuiItem::guiItem(KStdGuiItem::Forward),
                            KStdGuiItem::guiItem(KStdGuiItem::Back) )
{
    d = new IPTCEditDialogDialogPrivate;
    d->urls      = urls;
    d->interface = iface;
    d->currItem  = d->urls.begin();

    // ---------------------------------------------------------------

    d->page_caption    = addPage(i18n("Caption"), i18n("Caption Informations"),
                                 BarIcon("editclear", KIcon::SizeMedium));
    d->captionPage     = new IPTCCaption(d->page_caption);

    d->page_datetime   = addPage(i18n("Date & Time"), i18n("Date and Time Informations"),
                                 BarIcon("today", KIcon::SizeMedium));
    d->datetimePage    = new IPTCDateTime(d->page_datetime);

    d->page_subjects   = addPage(i18n("Subjects"), i18n("Subjects Informations"),
                                 BarIcon("cookie", KIcon::SizeMedium));
    d->subjectsPage    = new IPTCSubjects(d->page_subjects);

    d->page_keywords   = addPage(i18n("Keywords"), i18n("Keywords Informations"),
                                 BarIcon("bookmark", KIcon::SizeMedium));
    d->keywordsPage    = new IPTCKeywords(d->page_keywords);

    d->page_categories = addPage(i18n("Categories"), i18n("Categories Informations"),
                                 BarIcon("bookmark_folder", KIcon::SizeMedium));
    d->categoriesPage  = new IPTCCategories(d->page_categories);

    d->page_credits    = addPage(i18n("Credits"), i18n("Credits Informations"),
                                 BarIcon("identity", KIcon::SizeMedium));
    d->creditsPage     = new IPTCCredits(d->page_credits);
  
    d->page_status     = addPage(i18n("Status"), i18n("Status Informations"),
                                 BarIcon("messagebox_info", KIcon::SizeMedium));
    d->statusPage      = new IPTCStatus(d->page_status);

    d->page_origin     = addPage(i18n("Origin"), i18n("Origin Informations"),
                                 BarIcon("www", KIcon::SizeMedium));
    d->originPage      = new IPTCOrigin(d->page_origin);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(I18N_NOOP("Edit Metadata"),
                                            NULL,
                                            KAboutData::License_GPL,
                                            I18N_NOOP("A Plugin to edit pictures metadata"),
                                            "(c) 2006, Gilles Caulier");

    d->about->addAuthor("Gilles Caulier", I18N_NOOP("Author and Maintainer"),
                        "caulier dot gilles at kdemail dot net");

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

    connect(d->subjectsPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->keywordsPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->categoriesPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->creditsPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->statusPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->originPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    // ------------------------------------------------------------

    readSettings();
    slotItemChanged();
}

IPTCEditDialog::~IPTCEditDialog()
{
    delete d->about;
    delete d;
}

void IPTCEditDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("metadataedit", "kipi-plugins");
}

void IPTCEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void IPTCEditDialog::slotClose()
{
    saveSettings();
    KDialogBase::slotClose();
}

void IPTCEditDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("Metadata Edit Settings");
    showPage(config.readNumEntry("IPTC Edit Page", 0));
    d->captionPage->setCheckedSyncJFIFComment(config.readBoolEntry("Sync JFIF Comment", true));
    d->captionPage->setCheckedSyncHOSTComment(config.readBoolEntry("Sync Host Comment", true));
    d->captionPage->setCheckedSyncEXIFComment(config.readBoolEntry("Sync EXIF Comment", true));
    d->datetimePage->setCheckedSyncHOSTDate(config.readBoolEntry("Sync Host Date", true));
    d->datetimePage->setCheckedSyncEXIFDate(config.readBoolEntry("Sync EXIF Date", true));
    resize(configDialogSize(config, QString("IPTC Edit Dialog")));
}

void IPTCEditDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("Metadata Edit Settings");
    config.writeEntry("IPTC Edit Page", activePageIndex());
    config.writeEntry("Sync JFIF Comment", d->captionPage->syncJFIFCommentIsChecked());
    config.writeEntry("Sync Host Comment", d->captionPage->syncHOSTCommentIsChecked());
    config.writeEntry("Sync EXIF Comment", d->captionPage->syncEXIFCommentIsChecked());
    config.writeEntry("Sync Host Date", d->datetimePage->syncHOSTDateIsChecked());
    config.writeEntry("Sync EXIF Date", d->datetimePage->syncEXIFDateIsChecked());
    saveDialogSize(config, QString("IPTC Edit Dialog"));
    config.sync();
}

void IPTCEditDialog::slotItemChanged()
{
    KExiv2Library::LibKExiv2 exiv2Iface;
    exiv2Iface.load((*d->currItem).path());
    d->exifData = exiv2Iface.getExif();
    d->iptcData = exiv2Iface.getIptc();
    d->captionPage->readMetadata(d->iptcData);
    d->datetimePage->readMetadata(d->iptcData);
    d->subjectsPage->readMetadata(d->iptcData);
    d->keywordsPage->readMetadata(d->iptcData);
    d->categoriesPage->readMetadata(d->iptcData);
    d->creditsPage->readMetadata(d->iptcData);
    d->statusPage->readMetadata(d->iptcData);
    d->originPage->readMetadata(d->iptcData);

    d->isReadOnly = KExiv2Library::LibKExiv2::isReadOnly((*d->currItem).path()); 
    d->page_caption->setEnabled(!d->isReadOnly);
    d->page_datetime->setEnabled(!d->isReadOnly);
    d->page_subjects->setEnabled(!d->isReadOnly);
    d->page_keywords->setEnabled(!d->isReadOnly);
    d->page_categories->setEnabled(!d->isReadOnly);
    d->page_credits->setEnabled(!d->isReadOnly);
    d->page_status->setEnabled(!d->isReadOnly);
    d->page_origin->setEnabled(!d->isReadOnly);
    enableButton(Apply, !d->isReadOnly);
    
    setCaption(QString("%1 (%2/%3) - %4")
               .arg((*d->currItem).filename())
               .arg(d->urls.findIndex(*(d->currItem))+1)
               .arg(d->urls.count())
               .arg(i18n("Edit IPTC Metadata")) + 
               (d->isReadOnly ? QString(" - ") + i18n("(read only)") : QString::null));
    enableButton(User1, *(d->currItem) != d->urls.last());
    enableButton(User2, *(d->currItem) != d->urls.first());
    enableButton(Apply, false);
}

void IPTCEditDialog::slotApply()
{
    if (d->modified && !d->isReadOnly) 
    {
        KIPI::ImageInfo info = d->interface->info(*d->currItem);

        if (d->captionPage->syncHOSTCommentIsChecked())
        {
            info.setDescription(d->captionPage->getIPTCCaption());
        }
        d->captionPage->applyMetadata(d->exifData, d->iptcData);

        if (d->datetimePage->syncHOSTDateIsChecked())
        {
            info.setTime(d->datetimePage->getIPTCCreationDate());
        }
        d->datetimePage->applyMetadata(d->exifData, d->iptcData);

        d->subjectsPage->applyMetadata(d->iptcData);
        d->keywordsPage->applyMetadata(d->iptcData);
        d->categoriesPage->applyMetadata(d->iptcData);
        d->creditsPage->applyMetadata(d->iptcData);
        d->statusPage->applyMetadata(d->iptcData);
        d->originPage->applyMetadata(d->iptcData);
        KExiv2Library::LibKExiv2 exiv2Iface;
        exiv2Iface.load((*d->currItem).path());
        exiv2Iface.setExif(d->exifData);
        exiv2Iface.setIptc(d->iptcData);
        exiv2Iface.save((*d->currItem).path());
        d->modified = false;
    }
}

void IPTCEditDialog::slotUser1()
{
    slotApply();
    d->currItem++;
    slotItemChanged();
}

void IPTCEditDialog::slotUser2()
{
    slotApply();
    d->currItem--;
    slotItemChanged();
}

void IPTCEditDialog::slotModified()
{
    if (!d->isReadOnly)
    {
        enableButton(Apply, true);
        d->modified = true;
    }
}

void IPTCEditDialog::slotOk()
{
    slotApply();
    saveSettings();
    accept();
}

bool IPTCEditDialog::eventFilter(QObject *, QEvent *e)
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
