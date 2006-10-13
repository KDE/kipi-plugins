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
#include "iptceditdialog.h"
#include "iptceditdialog.moc"

namespace KIPIMetadataEditPlugin
{

class IPTCEditDialogDialogPrivate
{

public:

    IPTCEditDialogDialogPrivate()
    {
        page_caption = 0;
        page_credits = 0;

        captionPage  = 0;
        creditsPage  = 0;
    }

    QByteArray   iptcData;

    QFrame      *page_caption;
    QFrame      *page_credits;

    IPTCCaption *captionPage;
    IPTCCredits *creditsPage;

};

IPTCEditDialog::IPTCEditDialog(QWidget* parent, QByteArray iptcData, const QString& fileName)
              : KDialogBase(IconList, i18n("%1 - Edit IPTC").arg(fileName), Help|Ok|Cancel, 
                            Ok, parent, 0, true, true )
{
    d = new IPTCEditDialogDialogPrivate;
    d->iptcData = iptcData;

    setHelp("metadataedit", "kipi-plugins");

    d->page_caption = addPage(i18n("Caption"), i18n("IPTC Caption Informations"),
                              BarIcon("editclear", KIcon::SizeMedium));
    d->captionPage  = new IPTCCaption(d->page_caption, d->iptcData);

    d->page_credits = addPage(i18n("Credits"), i18n("IPTC Credits Informations"),
                              BarIcon("identity", KIcon::SizeMedium));
    d->creditsPage  = new IPTCCredits(d->page_credits, d->iptcData);

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
    showPage(config.readNumEntry("IPTC Edit Page", Caption));
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
    d->creditsPage->applyMetadata(d->iptcData);

    saveSettings();
    accept();
}

}  // namespace KIPIMetadataEditPlugin
