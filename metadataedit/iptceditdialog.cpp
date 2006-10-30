/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-12
 * Description : a dialog to edit IPTC metadata
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
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvalidator.h>

// KDE includes.

#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <klineedit.h>
#include <kmessagebox.h>

// Local includes.

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
        page_caption  = 0;
        page_subjects = 0;
        page_keywords = 0;
        page_credits  = 0;
        page_status   = 0;
        page_origin   = 0;
        page_datetime = 0;

        captionPage   = 0;
        subjectsPage  = 0;
        keywordsPage  = 0;
        creditsPage   = 0;
        statusPage    = 0;
        originPage    = 0;
        datetimePage  = 0;
    }

    QByteArray   iptcData;

    QFrame         *page_caption;
    QFrame         *page_subjects;
    QFrame         *page_keywords;
    QFrame         *page_categories;
    QFrame         *page_credits;
    QFrame         *page_status;
    QFrame         *page_origin;
    QFrame         *page_datetime;

    IPTCCaption    *captionPage;
    IPTCSubjects   *subjectsPage;
    IPTCKeywords   *keywordsPage;
    IPTCCategories *categoriesPage;
    IPTCCredits    *creditsPage;
    IPTCStatus     *statusPage;
    IPTCOrigin     *originPage;
    IPTCDateTime   *datetimePage;
};

IPTCEditDialog::IPTCEditDialog(QWidget* parent, QByteArray iptcData, const QString& fileName)
              : KDialogBase(IconList, i18n("%1 - Edit IPTC Metadata").arg(fileName), Help|Ok|Cancel, 
                            Ok, parent, 0, true, true )
{
    d = new IPTCEditDialogDialogPrivate;
    d->iptcData = iptcData;

    setHelp("metadataedit", "kipi-plugins");

    d->page_caption    = addPage(i18n("Caption"), i18n("Caption Informations"),
                                 BarIcon("editclear", KIcon::SizeMedium));
    d->captionPage     = new IPTCCaption(d->page_caption, d->iptcData);

    d->page_subjects   = addPage(i18n("Subjects"), i18n("Subjects Informations"),
                                 BarIcon("cookie", KIcon::SizeMedium));
    d->subjectsPage    = new IPTCSubjects(d->page_subjects, d->iptcData);

    d->page_keywords   = addPage(i18n("Keywords"), i18n("Keywords Informations"),
                                 BarIcon("bookmark", KIcon::SizeMedium));
    d->keywordsPage    = new IPTCKeywords(d->page_keywords, d->iptcData);

    d->page_categories = addPage(i18n("Categories"), i18n("Categories Informations"),
                                 BarIcon("bookmark_folder", KIcon::SizeMedium));
    d->categoriesPage  = new IPTCCategories(d->page_categories, d->iptcData);

    d->page_credits    = addPage(i18n("Credits"), i18n("Credits Informations"),
                                 BarIcon("identity", KIcon::SizeMedium));
    d->creditsPage     = new IPTCCredits(d->page_credits, d->iptcData);
  
    d->page_status     = addPage(i18n("Status"), i18n("Status Informations"),
                                 BarIcon("messagebox_info", KIcon::SizeMedium));
    d->statusPage      = new IPTCStatus(d->page_status, d->iptcData);

    d->page_origin     = addPage(i18n("Origin"), i18n("Origin Informations"),
                                 BarIcon("www", KIcon::SizeMedium));
    d->originPage      = new IPTCOrigin(d->page_origin, d->iptcData);

    d->page_datetime   = addPage(i18n("Date & Time"), i18n("Date and Time Informations"),
                                 BarIcon("today", KIcon::SizeMedium));
    d->datetimePage    = new IPTCDateTime(d->page_datetime, d->iptcData);

    readSettings();
    show();
}

IPTCEditDialog::~IPTCEditDialog()
{
    delete d;
}

void IPTCEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void IPTCEditDialog::slotCancel()
{
    saveSettings();
    KDialogBase::slotCancel();
}

void IPTCEditDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("Metadata Edit Settings");
    showPage(config.readNumEntry("IPTC Edit Page", 0));
    resize(configDialogSize(config, QString("IPTC Edit Dialog")));
}

void IPTCEditDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("Metadata Edit Settings");
    config.writeEntry("IPTC Edit Page", activePageIndex());
    saveDialogSize(config, QString("IPTC Edit Dialog"));
    config.sync();
}

QByteArray IPTCEditDialog::getIPTCInfo()
{
    return d->iptcData;
}

void IPTCEditDialog::slotOk()
{
    d->captionPage->applyMetadata(d->iptcData);
    d->subjectsPage->applyMetadata(d->iptcData);
    d->keywordsPage->applyMetadata(d->iptcData);
    d->categoriesPage->applyMetadata(d->iptcData);
    d->creditsPage->applyMetadata(d->iptcData);
    d->statusPage->applyMetadata(d->iptcData);
    d->originPage->applyMetadata(d->iptcData);
    d->datetimePage->applyMetadata(d->iptcData);
    saveSettings();
    accept();
}

}  // namespace KIPIMetadataEditPlugin
