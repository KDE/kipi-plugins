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
#include "exiflens.h"
#include "exifexposure.h"
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
        page_caption  = 0;
        page_datetime = 0;
        page_lens     = 0;
        page_exposure = 0;
        page_light    = 0;
        page_adjust   = 0;

        captionPage   = 0;
        datetimePage  = 0;
        lensPage      = 0;
        exposurePage  = 0;
        lightPage     = 0;
        adjustPage    = 0;
    }

    QByteArray    exifData;

    QFrame       *page_caption;
    QFrame       *page_datetime;
    QFrame       *page_lens;
    QFrame       *page_exposure;
    QFrame       *page_light;
    QFrame       *page_adjust;

    EXIFCaption  *captionPage;
    EXIFDateTime *datetimePage;
    EXIFLens     *lensPage;
    EXIFExposure *exposurePage;
    EXIFLight    *lightPage;
    EXIFAdjust   *adjustPage;
};

EXIFEditDialog::EXIFEditDialog(QWidget* parent, QByteArray exifData, const QString& fileName)
              : KDialogBase(IconList, i18n("%1 - Edit EXIF Metadata").arg(fileName), Help|Ok|Cancel, 
                            Ok, parent, 0, true, true )
{
    d = new EXIFEditDialogDialogPrivate;
    d->exifData = exifData;

    setHelp("metadataedit", "kipi-plugins");

    d->page_caption    = addPage(i18n("Caption"), i18n("Caption Informations"),
                                 BarIcon("editclear", KIcon::SizeMedium));
    d->captionPage     = new EXIFCaption(d->page_caption, d->exifData);

    d->page_datetime   = addPage(i18n("Date & Time"), i18n("Date and Time Informations"),
                                 BarIcon("today", KIcon::SizeMedium));
    d->datetimePage    = new EXIFDateTime(d->page_datetime, d->exifData);

    d->page_lens       = addPage(i18n("Lens"), i18n("Camera Lens Informations"),
                                 BarIcon("camera", KIcon::SizeMedium));
    d->lensPage        = new EXIFLens(d->page_lens, d->exifData);

    d->page_exposure   = addPage(i18n("Exposure"), i18n("Camera Exposure Informations"),
                                 BarIcon("configure", KIcon::SizeMedium));
    d->exposurePage    = new EXIFExposure(d->page_exposure, d->exifData);

    d->page_light      = addPage(i18n("Light"), i18n("Light Source Informations"),
                                 BarIcon("idea", KIcon::SizeMedium));
    d->lightPage       = new EXIFLight(d->page_light, d->exifData);

    d->page_adjust     = addPage(i18n("Adjustments"), i18n("Pictures Adjustments Informations"),
                                 BarIcon("blend", KIcon::SizeMedium));
    d->adjustPage      = new EXIFAdjust(d->page_adjust, d->exifData);

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
    d->lensPage->applyMetadata(d->exifData);
    d->exposurePage->applyMetadata(d->exifData);
    d->lightPage->applyMetadata(d->exifData);
    d->adjustPage->applyMetadata(d->exifData);

    saveSettings();
    accept();
}

}  // namespace KIPIMetadataEditPlugin
