/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-03-14
 * Description : a KPageWidget to edit IPTC metadata
 *
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011 by Victor Dodon <dodon dot victor at gmail dot com>
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

#include "iptceditwidget.moc"

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
#include <kurl.h>

// Local includes

#include "metadataedit.h"
#include "iptccategories.h"
#include "iptccontent.h"
#include "iptccredits.h"
#include "iptcenvelope.h"
#include "iptckeywords.h"
#include "iptcorigin.h"
#include "iptcproperties.h"
#include "iptcstatus.h"
#include "iptcsubjects.h"
#include "kpimageinfo.h"
#include "kphostsettings.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIMetadataEditPlugin
{

class IPTCEditWidget::IPTCEditWidgetPrivate
{

public:

    IPTCEditWidgetPrivate()
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
        page_envelope   = 0;
        contentPage     = 0;
        propertiesPage  = 0;
        subjectsPage    = 0;
        keywordsPage    = 0;
        categoriesPage  = 0;
        creditsPage     = 0;
        statusPage      = 0;
        originPage      = 0;
        envelopePage    = 0;
        dlg             = 0;
    }

    bool                  modified;
    bool                  isReadOnly;

    QByteArray            exifData;
    QByteArray            iptcData;

    KPageWidgetItem*     page_content;
    KPageWidgetItem*     page_properties;
    KPageWidgetItem*     page_subjects;
    KPageWidgetItem*     page_keywords;
    KPageWidgetItem*     page_categories;
    KPageWidgetItem*     page_credits;
    KPageWidgetItem*     page_status;
    KPageWidgetItem*     page_origin;
    KPageWidgetItem*     page_envelope;

    IPTCContent*         contentPage;
    IPTCProperties*      propertiesPage;
    IPTCSubjects*        subjectsPage;
    IPTCKeywords*        keywordsPage;
    IPTCCategories*      categoriesPage;
    IPTCCredits*         creditsPage;
    IPTCStatus*          statusPage;
    IPTCOrigin*          originPage;
    IPTCEnvelope*        envelopePage;

    MetadataEditDialog*  dlg;
};

IPTCEditWidget::IPTCEditWidget(MetadataEditDialog* const parent)
    : KPageWidget(parent), d(new IPTCEditWidgetPrivate)
{
    d->dlg           = parent;

    d->contentPage   = new IPTCContent(this);
    d->page_content  = addPage(d->contentPage, i18n("Content"));
    d->page_content->setHeader(i18n("<qt>Content Information<br/>"
                     "<i>Use this panel to describe the visual content of the image</i></qt>"));
    d->page_content->setIcon(KIcon("help-contents"));

    d->originPage  = new IPTCOrigin(this);
    d->page_origin = addPage(d->originPage, i18n("Origin"));
    d->page_origin->setHeader(i18n("<qt>Origin Information<br/>"
                    "<i>Use this panel for formal descriptive information about the image</i></qt>"));
    d->page_origin->setIcon(KIcon("applications-internet"));

    d->creditsPage  = new IPTCCredits(this);
    d->page_credits = addPage(d->creditsPage, i18n("Credits"));
    d->page_credits->setHeader(i18n("<qt>Credit Information<br/>"
                     "<i>Use this panel to record copyright information about the image</i></qt>"));
    d->page_credits->setIcon(KIcon("view-pim-contacts"));

    d->subjectsPage  = new IPTCSubjects(this);
    d->page_subjects = addPage(d->subjectsPage, i18n("Subjects"));
    d->page_subjects->setHeader(i18n("<qt>Subject Information<br/>"
                      "<i>Use this panel to record subject information about the image</i></qt>"));
    d->page_subjects->setIcon(KIcon("feed-subscribe"));

    d->keywordsPage  = new IPTCKeywords(this);
    d->page_keywords = addPage(d->keywordsPage, i18n("Keywords"));
    d->page_keywords->setHeader(i18n("<qt>Keyword Information<br/>"
                      "<i>Use this panel to record keywords relevant to the image</i></qt>"));
    d->page_keywords->setIcon(KIcon("bookmarks"));

    d->categoriesPage  = new IPTCCategories(this);
    d->page_categories = addPage(d->categoriesPage, i18n("Categories"));
    d->page_categories->setHeader(i18n("<qt>Category Information<br/>"
                        "<i>Use this panel to record categories relevant to the image</i></qt>"));
    d->page_categories->setIcon(KIcon("folder-image"));

    d->statusPage  = new IPTCStatus(this);
    d->page_status = addPage(d->statusPage, i18n("Status"));
    d->page_status->setHeader(i18n("<qt>Status Information<br/>"
                    "<i>Use this panel to record workflow information</i></qt>"));
    d->page_status->setIcon(KIcon("view-pim-tasks"));

    d->propertiesPage  = new IPTCProperties(this);
    d->page_properties = addPage(d->propertiesPage, i18n("Properties"));
    d->page_properties->setHeader(i18n("<qt>Status Properties<br/>"
                      "<i>Use this panel to record workflow properties</i></qt>"));
    d->page_properties->setIcon(KIcon("draw-freehand"));

    d->envelopePage  = new IPTCEnvelope(this);
    d->page_envelope = addPage(d->envelopePage, i18n("Envelope"));
    d->page_envelope->setHeader(i18n("<qt>Envelope Information<br/>"
                      "<i>Use this panel to record editorial details</i></qt>"));
    d->page_envelope->setIcon(KIcon("view-pim-mail"));

    // ---------------------------------------------------------------

    connect(d->contentPage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->propertiesPage, SIGNAL(signalModified()),
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

    connect(d->envelopePage, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    // ------------------------------------------------------------

    readSettings();
    slotItemChanged();
}

IPTCEditWidget::~IPTCEditWidget()
{
    delete d;
}

void IPTCEditWidget::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("All Metadata Edit Settings");
    showPage(group.readEntry("All IPTC Edit Page", 0));
    d->contentPage->setCheckedSyncJFIFComment(group.readEntry("All Sync JFIF Comment", true));
    d->contentPage->setCheckedSyncHOSTComment(group.readEntry("All Sync Host Comment", true));
    d->contentPage->setCheckedSyncEXIFComment(group.readEntry("All Sync EXIF Comment", true));
    d->originPage->setCheckedSyncHOSTDate(group.readEntry("All Sync Host Date", true));
    d->originPage->setCheckedSyncEXIFDate(group.readEntry("All Sync EXIF Date", true));
}

void IPTCEditWidget::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("All Metadata Edit Settings");
    group.writeEntry("All IPTC Edit Page", activePageIndex());
    group.writeEntry("All Sync JFIF Comment", d->contentPage->syncJFIFCommentIsChecked());
    group.writeEntry("All Sync Host Comment", d->contentPage->syncHOSTCommentIsChecked());
    group.writeEntry("All Sync EXIF Comment", d->contentPage->syncEXIFCommentIsChecked());
    group.writeEntry("All Sync Host Date", d->originPage->syncHOSTDateIsChecked());
    group.writeEntry("All Sync EXIF Date", d->originPage->syncEXIFDateIsChecked());
    config.sync();
}

void IPTCEditWidget::slotItemChanged()
{
    KPMetadata meta;
    meta.load((*d->dlg->currentItem()).path());

#if KEXIV2_VERSION >= 0x010000
    d->exifData = meta.getExifEncoded();
#else
    d->exifData = meta.getExif();
#endif

    d->iptcData = meta.getIptc();
    d->contentPage->readMetadata(d->iptcData);
    d->originPage->readMetadata(d->iptcData);
    d->creditsPage->readMetadata(d->iptcData);
    d->subjectsPage->readMetadata(d->iptcData);
    d->keywordsPage->readMetadata(d->iptcData);
    d->categoriesPage->readMetadata(d->iptcData);
    d->statusPage->readMetadata(d->iptcData);
    d->propertiesPage->readMetadata(d->iptcData);
    d->envelopePage->readMetadata(d->iptcData);

    d->isReadOnly = !KPMetadata::canWriteIptc((*d->dlg->currentItem()).path());
    emit signalSetReadOnly(d->isReadOnly);

    d->page_content->setEnabled(!d->isReadOnly);
    d->page_origin->setEnabled(!d->isReadOnly);
    d->page_credits->setEnabled(!d->isReadOnly);
    d->page_subjects->setEnabled(!d->isReadOnly);
    d->page_keywords->setEnabled(!d->isReadOnly);
    d->page_categories->setEnabled(!d->isReadOnly);
    d->page_status->setEnabled(!d->isReadOnly);
    d->page_properties->setEnabled(!d->isReadOnly);
    d->page_envelope->setEnabled(!d->isReadOnly);
}

void IPTCEditWidget::apply()
{
    if (d->modified && !d->isReadOnly)
    {
        KPImageInfo info(*d->dlg->currentItem());

        if (d->contentPage->syncHOSTCommentIsChecked())
        {
            info.setDescription(d->contentPage->getIPTCCaption());
        }
        d->contentPage->applyMetadata(d->exifData, d->iptcData);

        if (d->originPage->syncHOSTDateIsChecked())
        {
            info.setDate(d->originPage->getIPTCCreationDate());
        }
        d->originPage->applyMetadata(d->exifData, d->iptcData);

        d->creditsPage->applyMetadata(d->iptcData);
        d->subjectsPage->applyMetadata(d->iptcData);
        d->keywordsPage->applyMetadata(d->iptcData);
        d->categoriesPage->applyMetadata(d->iptcData);
        d->statusPage->applyMetadata(d->iptcData);
        d->propertiesPage->applyMetadata(d->iptcData);
        d->envelopePage->applyMetadata(d->iptcData);

        KPMetadata meta;

        meta.load((*d->dlg->currentItem()).path());
        meta.setExif(d->exifData);
        meta.setIptc(d->iptcData);
        meta.save((*d->dlg->currentItem()).path());
        d->modified = false;
    }
}

void IPTCEditWidget::slotModified()
{
    if (!d->isReadOnly)
    {
        d->modified = true;
        emit signalModified();
    }
}

void IPTCEditWidget::showPage(int page)
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
        case 8:
            setCurrentPage(d->page_envelope);
            break;
        default:
            setCurrentPage(d->page_content);
            break;
    }
}

int IPTCEditWidget::activePageIndex() const
{
    KPageWidgetItem* cur = currentPage();

    if (cur == d->page_content)    return 0;
    if (cur == d->page_origin)     return 1;
    if (cur == d->page_credits)    return 2;
    if (cur == d->page_subjects)   return 3;
    if (cur == d->page_keywords)   return 4;
    if (cur == d->page_categories) return 5;
    if (cur == d->page_status)     return 6;
    if (cur == d->page_properties) return 7;
    if (cur == d->page_envelope)   return 8;

    return 0;
}

bool IPTCEditWidget::isModified() const
{
    return d->modified;
}

}  // namespace KIPIMetadataEditPlugin
