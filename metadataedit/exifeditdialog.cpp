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
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

// KDE includes.

#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <klineedit.h>
#include <kmessagebox.h>

// Local includes.

#include "exifcaption.h"
#include "exifdatetime.h"
#include "exifexposure.h"
#include "exifeditdialog.h"
#include "exifeditdialog.moc"

namespace KIPIMetadataEditPlugin
{

class EXIFEditDialogDialogPrivate
{

public:

    EXIFEditDialogDialogPrivate()
    {
        page_caption  = 0;
        page_exposure = 0;
        page_credits  = 0;
        page_status   = 0;
        page_origin   = 0;
        page_datetime = 0;

        captionPage   = 0;
        datetimePage  = 0;
/*
        exposurePage  = 0;
        creditsPage   = 0;
        statusPage    = 0;
        originPage    = 0;
*/
    }

    QByteArray   exifData;

    QFrame         *page_caption;
    QFrame         *page_datetime;
    QFrame         *page_exposure;
    QFrame         *page_categories;
    QFrame         *page_credits;
    QFrame         *page_status;
    QFrame         *page_origin;

    EXIFCaption    *captionPage;
    EXIFDateTime   *datetimePage;
    EXIFExposure   *exposurePage;

/*    EXIFCategories *categoriesPage;
    EXIFCredits    *creditsPage;
    EXIFStatus     *statusPage;
    EXIFOrigin     *originPage;
*/
};

EXIFEditDialog::EXIFEditDialog(QWidget* parent, QByteArray exifData, const QString& fileName)
              : KDialogBase(IconList, i18n("%1 - Edit EXIF").arg(fileName), Help|Ok|Cancel, 
                            Ok, parent, 0, true, true )
{
    d = new EXIFEditDialogDialogPrivate;
    d->exifData = exifData;

    setHelp("metadataedit", "kipi-plugins");

    d->page_caption    = addPage(i18n("Caption"), i18n("EXIF Caption Informations"),
                                 BarIcon("editclear", KIcon::SizeMedium));
    d->captionPage     = new EXIFCaption(d->page_caption, d->exifData);

    d->page_datetime   = addPage(i18n("Date & Time"), i18n("EXIF Date and Time Informations"),
                                 BarIcon("today", KIcon::SizeMedium));
    d->datetimePage    = new EXIFDateTime(d->page_datetime, d->exifData);

    d->page_exposure   = addPage(i18n("Exposure"), i18n("EXIF Exposure Informations"),
                                 BarIcon("camera", KIcon::SizeMedium));
    d->exposurePage    = new EXIFExposure(d->page_exposure, d->exifData);

/*    d->page_categories = addPage(i18n("Categories"), i18n("EXIF Categories Informations"),
                                 BarIcon("bookmark_folder", KIcon::SizeMedium));
    d->categoriesPage  = new EXIFCategories(d->page_categories, d->exifData);

    d->page_credits    = addPage(i18n("Credits"), i18n("EXIF Credits Informations"),
                                 BarIcon("identity", KIcon::SizeMedium));
    d->creditsPage     = new EXIFCredits(d->page_credits, d->exifData);
  
    d->page_status     = addPage(i18n("Status"), i18n("EXIF Status Informations"),
                                 BarIcon("messagebox_info", KIcon::SizeMedium));
    d->statusPage      = new EXIFStatus(d->page_status, d->exifData);

    d->page_origin     = addPage(i18n("Origin"), i18n("EXIF Origin Informations"),
                                 BarIcon("www", KIcon::SizeMedium));
    d->originPage      = new EXIFOrigin(d->page_origin, d->exifData);
*/

    readSettings();
    show();
}

EXIFEditDialog::~EXIFEditDialog()
{
    delete d;
}

void EXIFEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void EXIFEditDialog::slotCancel()
{
    saveSettings();
    KDialogBase::slotCancel();
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

QByteArray EXIFEditDialog::getEXIFInfo()
{
    return d->exifData;
}

void EXIFEditDialog::slotOk()
{
    d->captionPage->applyMetadata(d->exifData);
    d->datetimePage->applyMetadata(d->exifData);
    d->exposurePage->applyMetadata(d->exifData);

/*
    d->categoriesPage->applyMetadata(d->exifData);
    d->creditsPage->applyMetadata(d->exifData);
    d->statusPage->applyMetadata(d->exifData);
    d->originPage->applyMetadata(d->exifData);
*/
    saveSettings();
    accept();
}

}  // namespace KIPIMetadataEditPlugin
