/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-11
 * Description : a dialog to edit XMP metadata
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "xmpeditdialog.h"
#include "xmpeditdialog.moc"

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

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "xmpcategories.h"
#include "xmpcontent.h"
#include "xmpcredits.h"
#include "xmpkeywords.h"
#include "xmporigin.h"
#include "xmpproperties.h"
#include "xmpstatus.h"
#include "xmpsubjects.h"

namespace KIPIMetadataEditPlugin
{

class XMPEditDialogPrivate
{

public:

    XMPEditDialogPrivate()
    {
        modified        = false;
        isReadOnly      = false;
        page_content    = 0;
        page_properties = 0;
        page_subjects   = 0;
        page_keywords   = 0;
        page_categories = 0;
        page_credits    = 0;
        page_status     = 0;
        page_origin     = 0;
        about           = 0;
        keywordsPage    = 0;
        categoriesPage  = 0;
        contentPage     = 0;
        subjectsPage    = 0;
        originPage      = 0;
        creditsPage     = 0;
        statusPage      = 0;
        propertiesPage  = 0;
    }

    bool                      modified;
    bool                      isReadOnly;

    QByteArray                exifData;
    QByteArray                iptcData;
    QByteArray                xmpData;

    KPageWidgetItem          *page_content;
    KPageWidgetItem          *page_origin;
    KPageWidgetItem          *page_subjects;
    KPageWidgetItem          *page_keywords;
    KPageWidgetItem          *page_categories;
    KPageWidgetItem          *page_credits;
    KPageWidgetItem          *page_status;
    KPageWidgetItem          *page_properties;

    KUrl::List                urls;

    KUrl::List::iterator      currItem;

    XMPContent               *contentPage;
    XMPKeywords              *keywordsPage;
    XMPCategories            *categoriesPage;
    XMPSubjects              *subjectsPage;
    XMPOrigin                *originPage;
    XMPCredits               *creditsPage;
    XMPStatus                *statusPage;
    XMPProperties            *propertiesPage;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;
};

XMPEditDialog::XMPEditDialog(QWidget* parent, KUrl::List urls, KIPI::Interface *iface)
             : KPageDialog(parent), d(new XMPEditDialogPrivate)
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

    d->contentPage   = new XMPContent(this);
    d->page_content  = addPage(d->contentPage, i18n("Content"));
    d->page_content->setHeader(i18n("<qt>Content Information<br/>"
                     "<i>Use this panel to describe the visual content of the image</i></qt>"));
    d->page_content->setIcon(KIcon("draw-text"));

    d->originPage  = new XMPOrigin(this);
    d->page_origin = addPage(d->originPage, i18n("Origin"));
    d->page_origin->setHeader(i18n("<qt>Origin Information<br/>"
                    "<i>Use this panel for formal descriptive information about the image</i></qt>"));
    d->page_origin->setIcon(KIcon("document-properties"));

    d->creditsPage  = new XMPCredits(this);
    d->page_credits = addPage(d->creditsPage, i18n("Credits"));
    d->page_credits->setHeader(i18n("<qt>Credit Information<br/>"
                     "<i>Use this panel to record copyright information about the image</i></qt>"));
    d->page_credits->setIcon(KIcon("view-pim-contacts"));

    d->subjectsPage  = new XMPSubjects(this);
    d->page_subjects = addPage(d->subjectsPage, i18n("Subjects"));
    d->page_subjects->setHeader(i18n("<qt>Subject Information<br/>"
                      "<i>Use this panel to record subject information about the image</i></qt>"));
    d->page_subjects->setIcon(KIcon("feed-subscribe"));

    d->keywordsPage  = new XMPKeywords(this);
    d->page_keywords = addPage(d->keywordsPage, i18n("Keywords"));
    d->page_keywords->setHeader(i18n("<qt>Keyword Information<br/>"
                      "<i>Use this panel to record keywords relevant to the image</i></qt>"));
    d->page_keywords->setIcon(KIcon("bookmarks"));

    d->categoriesPage  = new XMPCategories(this);
    d->page_categories = addPage(d->categoriesPage, i18n("Categories"));
    d->page_categories->setHeader(i18n("<qt>Category Information<br/>"
                        "<i>Use this panel to record categories relevant to the image</i></qt>"));
    d->page_categories->setIcon(KIcon("folder"));

    d->statusPage  = new XMPStatus(this);
    d->page_status = addPage(d->statusPage, i18n("Status"));
    d->page_status->setHeader(i18n("<qt>Status Information<br/>"
                    "<i>Use this panel to record workflow information</i></qt>"));
    d->page_status->setIcon(KIcon("view-pim-tasks"));

    d->propertiesPage  = new XMPProperties(this);
    d->page_properties = addPage(d->propertiesPage, i18n("Properties"));
    d->page_properties->setHeader(i18n("<qt>Status Properties<br/>"
                      "<i>Use this panel to record workflow properties</i></qt>"));
    d->page_properties->setIcon(KIcon("draw-freehand"));

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("Edit Metadata"),
                                            0,
                                            KAboutData::License_GPL,
                                            ki18n("A Plugin to edit pictures' metadata."),
                                            ki18n("(c) 2007-2009, Gilles Caulier"));

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

    connect(d->contentPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->propertiesPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->originPage, SIGNAL(signalModified()),
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

    // ------------------------------------------------------------

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

XMPEditDialog::~XMPEditDialog()
{
    delete d->about;
    delete d;
}

void XMPEditDialog::slotHelp()
{
    KToolInvocation::invokeHelp("metadataedit", "kipi-plugins");
}

void XMPEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void XMPEditDialog::slotClose()
{
    saveSettings();
    close();
}

void XMPEditDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Metadata Edit Settings");

    showPage(group.readEntry("XMP Edit Page", 0));
    d->contentPage->setCheckedSyncJFIFComment(group.readEntry("Sync JFIF Comment", true));
    d->contentPage->setCheckedSyncHOSTComment(group.readEntry("Sync Host Comment", true));
    d->contentPage->setCheckedSyncEXIFComment(group.readEntry("Sync EXIF Comment", true));
    d->originPage->setCheckedSyncHOSTDate(group.readEntry("Sync Host Date", true));
    d->originPage->setCheckedSyncEXIFDate(group.readEntry("Sync EXIF Date", true));

    KConfigGroup group2 = config.group(QString("XMP Edit Dialog"));
    restoreDialogSize(group2);
}

void XMPEditDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Metadata Edit Settings");

    group.writeEntry("XMP Edit Page", activePageIndex());
    group.writeEntry("Sync JFIF Comment", d->contentPage->syncJFIFCommentIsChecked());
    group.writeEntry("Sync Host Comment", d->contentPage->syncHOSTCommentIsChecked());
    group.writeEntry("Sync EXIF Comment", d->contentPage->syncEXIFCommentIsChecked());
    group.writeEntry("Sync Host Date", d->originPage->syncHOSTDateIsChecked());
    group.writeEntry("Sync EXIF Date", d->originPage->syncEXIFDateIsChecked());

    KConfigGroup group2 = config.group(QString("XMP Edit Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void XMPEditDialog::slotItemChanged()
{
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.load((*d->currItem).path());
    d->exifData = exiv2Iface.getExif();
    d->iptcData = exiv2Iface.getIptc();
    d->xmpData  = exiv2Iface.getXmp();

    d->contentPage->readMetadata(d->xmpData);
    d->originPage->readMetadata(d->xmpData);
    d->subjectsPage->readMetadata(d->xmpData);
    d->keywordsPage->readMetadata(d->xmpData);
    d->categoriesPage->readMetadata(d->xmpData);
    d->creditsPage->readMetadata(d->xmpData);
    d->statusPage->readMetadata(d->xmpData);
    d->propertiesPage->readMetadata(d->xmpData);
    d->isReadOnly = !KExiv2Iface::KExiv2::canWriteXmp((*d->currItem).path());

    d->page_content->setEnabled(!d->isReadOnly);
    d->page_origin->setEnabled(!d->isReadOnly);
    d->page_subjects->setEnabled(!d->isReadOnly);
    d->page_keywords->setEnabled(!d->isReadOnly);
    d->page_categories->setEnabled(!d->isReadOnly);
    d->page_credits->setEnabled(!d->isReadOnly);
    d->page_status->setEnabled(!d->isReadOnly);
    d->page_properties->setEnabled(!d->isReadOnly);

    enableButton(Apply, !d->isReadOnly);
    setCaption(QString("%1 (%2/%3) - %4")
               .arg((*d->currItem).fileName())
               .arg(d->urls.indexOf(*(d->currItem))+1)
               .arg(d->urls.count())
               .arg(i18n("Edit XMP Metadata")) +
               (d->isReadOnly ? QString(" - ") + i18n("(read only)") : QString::null));
    enableButton(User1, *(d->currItem) != d->urls.last());
    enableButton(User2, *(d->currItem) != d->urls.first());
    enableButton(Apply, false);
}

void XMPEditDialog::slotApply()
{
    if (d->modified && !d->isReadOnly)
    {
        KIPI::ImageInfo info = d->interface->info(*d->currItem);

        if (d->contentPage->syncHOSTCommentIsChecked())
        {
            info.setDescription(d->contentPage->getXMPCaption());
        }
        d->contentPage->applyMetadata(d->exifData, d->xmpData);

        if (d->originPage->syncHOSTDateIsChecked())
        {
            info.setTime(d->originPage->getXMPCreationDate());
        }
        d->originPage->applyMetadata(d->exifData, d->xmpData);

        d->subjectsPage->applyMetadata(d->xmpData);
        d->keywordsPage->applyMetadata(d->xmpData);
        d->categoriesPage->applyMetadata(d->xmpData);
        d->creditsPage->applyMetadata(d->xmpData);
        d->statusPage->applyMetadata(d->xmpData);
        d->propertiesPage->applyMetadata(d->xmpData);

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

void XMPEditDialog::slotUser1()
{
    slotApply();
    d->currItem++;
    slotItemChanged();
}

void XMPEditDialog::slotUser2()
{
    slotApply();
    d->currItem--;
    slotItemChanged();
}

void XMPEditDialog::slotModified()
{
    if (!d->isReadOnly)
    {
        enableButton(Apply, true);
        d->modified = true;
    }
}

void XMPEditDialog::slotOk()
{
    slotApply();
    saveSettings();
    accept();
}

bool XMPEditDialog::eventFilter(QObject *, QEvent *e)
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

void XMPEditDialog::showPage(int page)
{
    switch(page)
    {
        case 0:
            setCurrentPage(d->page_content);
            break;
        case 1:
            setCurrentPage(d->page_origin);
            break;
        case 2:
            setCurrentPage(d->page_credits);
            break;
        case 3:
            setCurrentPage(d->page_subjects);
            break;
        case 4:
            setCurrentPage(d->page_keywords);
            break;
        case 5:
            setCurrentPage(d->page_categories);
            break;
        case 6:
            setCurrentPage(d->page_status);
            break;
        case 7:
            setCurrentPage(d->page_properties);
            break;
        default:
            setCurrentPage(d->page_content);
            break;
    }
}

int XMPEditDialog::activePageIndex()
{
    KPageWidgetItem *cur = currentPage();

    if (cur == d->page_content)    return 0;
    if (cur == d->page_origin)     return 1;
    if (cur == d->page_credits)    return 2;
    if (cur == d->page_subjects)   return 3;
    if (cur == d->page_keywords)   return 4;
    if (cur == d->page_categories) return 5;
    if (cur == d->page_status)     return 6;
    if (cur == d->page_properties) return 7;

    return 0;
}

}  // namespace KIPIMetadataEditPlugin
