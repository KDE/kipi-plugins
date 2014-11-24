/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-03-14
 * Description : a dialog to edit EXIF,IPTC and XMP metadata
 *
 * Copyright (C) 2011      by Victor Dodon <dodon dot victor at gmail dot com>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "metadataedit.moc"

// Qt includes

#include <QCloseEvent>
#include <QKeyEvent>
#include <QPointer>
#include <QObject>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kguiitem.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktabwidget.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>
#include <libkipi/plugin.h>

// Local includes

#include "kpaboutdata.h"
#include "kpversion.h"
#include "kpimageinfo.h"
#include "exifeditwidget.h"
#include "iptceditwidget.h"
#include "xmpeditwidget.h"

using namespace KIPIPlugins;

namespace KIPIMetadataEditPlugin
{

class MetadataEditDialog::Private
{
public:

    Private()
    {
        isReadOnly = false;
        tabWidget  = 0;
        tabExif    = 0;
        tabIptc    = 0;
        tabXmp     = 0;
    }

    bool                 isReadOnly;

    KUrl::List           urls;
    KUrl::List::iterator currItem;

    KTabWidget*          tabWidget;

    EXIFEditWidget*      tabExif;
    IPTCEditWidget*      tabIptc;
    XMPEditWidget*       tabXmp;
};

MetadataEditDialog::MetadataEditDialog(QWidget* const parent, const KUrl::List& urls)
    : KPToolDialog(parent), d(new Private)
{
    d->urls     = urls;
    d->currItem = d->urls.begin();

    setCaption(i18n("Metadata edit dialog"));
    d->tabWidget = new KTabWidget(this);
    d->tabExif   = new EXIFEditWidget(this);
    d->tabIptc   = new IPTCEditWidget(this);
    d->tabXmp    = new XMPEditWidget(this);
    d->tabWidget->addTab(d->tabExif, i18n("Edit EXIF"));
    d->tabWidget->addTab(d->tabIptc, i18n("Edit IPTC"));
    d->tabWidget->addTab(d->tabXmp,  i18n("Edit XMP"));

    setMainWidget(d->tabWidget);
    setButtons(urls.count() > 1 ? Help|User1|User2|Ok|Apply|Close
                                : Help|Ok|Apply|Close);
    setDefaultButton(Ok);
    setButtonIcon(User1, KIcon("go-next"));
    setButtonIcon(User2, KIcon("go-previous"));
    setButtonText(User1, i18n("Next"));
    setButtonText(User2, i18n("Previous"));
    setModal(true);

    //----------------------------------------------------------

    connect(d->tabExif, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->tabIptc, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(d->tabXmp, SIGNAL(signalModified()),
            this, SLOT(slotModified()));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotApply()));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotNext()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotPrevious()));

    connect(d->tabExif, SIGNAL(signalSetReadOnly(bool)),
            this, SLOT(slotSetReadOnly(bool)));

    connect(d->tabIptc, SIGNAL(signalSetReadOnly(bool)),
            this, SLOT(slotSetReadOnly(bool)));

    connect(d->tabXmp, SIGNAL(signalSetReadOnly(bool)),
            this, SLOT(slotSetReadOnly(bool)));

    //----------------------------------------------------------

    enableButton(Apply, false);

    KPAboutData* const about = new KPAboutData(ki18n("Edit Metadata"),
                                   0,
                                   KAboutData::License_GPL,
                                   ki18n("A Plugin to edit pictures' metadata."),
                                   ki18n("(c) 2006-2013, Gilles Caulier"));

    about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author and Maintainer"),
                     "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Victor Dodon"), ki18n("Developer"),
                     "victor dot dodon at cti dot pub dot ro");

    about->setHandbookEntry("metadataeditor");
    setAboutData(about);

    readSettings();
    slotItemChanged();
}

MetadataEditDialog::~MetadataEditDialog()
{
    delete d;
}

KUrl::List::iterator MetadataEditDialog::currentItem() const
{
    return d->currItem;
}

void MetadataEditDialog::slotModified()
{
    bool modified = false;

    switch (d->tabWidget->currentIndex())
    {
        case 0:
            modified = d->tabExif->isModified();
            break;

        case 1:
            modified = d->tabIptc->isModified();
            break;

        case 2:
            modified = d->tabXmp->isModified();
            break;
    }

    enableButton(Apply, modified);
}

void MetadataEditDialog::slotOk()
{
    slotApply();
    saveSettings();
    accept();
}

void MetadataEditDialog::slotClose()
{
    saveSettings();
    close();
}

void MetadataEditDialog::slotApply()
{
    d->tabExif->apply();
    d->tabIptc->apply();
    d->tabXmp->apply();
    slotItemChanged();
    iface()->refreshImages(*d->currItem);
}

void MetadataEditDialog::slotNext()
{
    slotApply();
    d->currItem++;
    slotItemChanged();
}

void MetadataEditDialog::slotPrevious()
{
    slotApply();
    d->currItem--;
    slotItemChanged();
}

void MetadataEditDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Metadata Edit Dialog"));
    d->tabWidget->setCurrentIndex(group.readEntry("Tab Index", 0));
    restoreDialogSize(group);
}

void MetadataEditDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Metadata Edit Dialog"));
    group.writeEntry("Tab Index", d->tabWidget->currentIndex());
    saveDialogSize(group);

    d->tabExif->saveSettings();
    d->tabIptc->saveSettings();
    d->tabXmp->saveSettings();
}

void MetadataEditDialog::slotItemChanged()
{
    d->tabExif->slotItemChanged();
    d->tabIptc->slotItemChanged();
    d->tabXmp->slotItemChanged();
    enableButton(Apply, !d->isReadOnly);
    setCaption(i18n("%1 (%2/%3) - Edit Metadata")
        .arg((*d->currItem).fileName())
        .arg(d->urls.indexOf(*(d->currItem))+1)
        .arg(d->urls.count()));
    enableButton(User1, *(d->currItem) != d->urls.last());
    enableButton(User2, *(d->currItem) != d->urls.first());
    enableButton(Apply, false);
}

bool MetadataEditDialog::eventFilter(QObject*, QEvent* e)
{
    if ( e->type() == QEvent::KeyPress )
    {
        QKeyEvent* k = (QKeyEvent*)e;

        if (k->modifiers() == Qt::ControlModifier &&
            (k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return))
        {
            slotApply();

            if (isButtonEnabled(User1))
                slotNext();

            return true;
        }
        else if (k->modifiers() == Qt::ShiftModifier &&
                 (k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return))
        {
            slotApply();

            if (isButtonEnabled(User2))
                slotPrevious();

            return true;
        }

        return false;
    }

    return false;
}

void MetadataEditDialog::closeEvent(QCloseEvent* e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void MetadataEditDialog::slotSetReadOnly(bool state)
{
    d->isReadOnly = state;
}

}  // namespace KIPIMetadataEditPlugin
