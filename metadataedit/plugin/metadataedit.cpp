/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-03-14
 * Description : a dialog to edit EXIF,IPTC and XMP metadata
 *
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
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <ktabwidget.h>

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
#include "exifeditwidget.h"
#include "iptceditwidget.h"
#include "xmpeditwidget.h"

using namespace KExiv2Iface;
using namespace KIPIPlugins;

namespace KIPIMetadataEditPlugin
{

class MetadataEditDialog::MetadataEditDialogPrivate
{
public:

    MetadataEditDialogPrivate()
    {
        isReadOnly = false;
        about      = 0;
        tabWidget  = 0;
        tabExif    = 0;
        tabIptc    = 0;
        tabXmp     = 0;
    }

    bool                 isReadOnly;

    KUrl::List           urls;
    KUrl::List::iterator currItem;

    Interface*           interface;

    KTabWidget*          tabWidget;
    allEXIFEditWidget*   tabExif;
    allIPTCEditWidget*   tabIptc;
    allXMPEditWidget*    tabXmp;
    KPAboutData*         about;
};

MetadataEditDialog::MetadataEditDialog(QWidget* parent,KUrl::List urls, Interface* iface)
    : KDialog(parent), d(new MetadataEditDialogPrivate)
{
    d->urls      = urls;
    d->interface = iface;
    d->currItem  = d->urls.begin();

    setCaption(i18n("Metadata edit dialog"));
    d->tabWidget = new KTabWidget(this);
    d->tabExif   = new allEXIFEditWidget(this, urls, iface);
    d->tabIptc   = new allIPTCEditWidget(this, urls, iface);
    d->tabXmp    = new allXMPEditWidget(this, urls, iface);
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

    connect(d->tabExif,SIGNAL(signalModified()),
        this,SLOT(slotModified()));

    connect(d->tabIptc,SIGNAL(signalModified()),
        this,SLOT(slotModified()));

    connect(d->tabXmp,SIGNAL(signalModified()),
        this,SLOT(slotModified()));

    connect(this,SIGNAL(applyClicked()),
        this,SLOT(slotApply()));

    connect(this,SIGNAL(signalApply()),
        d->tabExif,SLOT(slotApply()));

    connect(this,SIGNAL(signalApply()),
        d->tabIptc,SLOT(slotApply()));

    connect(this,SIGNAL(signalApply()),
        d->tabXmp,SLOT(slotApply()));

    connect(this,SIGNAL(closeClicked()),
        this,SLOT(slotClose()));

    connect(this,SIGNAL(signalClose()),
        d->tabExif,SLOT(slotClose()));

    connect(this,SIGNAL(signalClose()),
        d->tabIptc,SLOT(slotClose()));

    connect(this,SIGNAL(signalClose()),
        d->tabXmp,SLOT(slotClose()));

    connect(this,SIGNAL(user1Clicked()),
        this,SLOT(slotUser1()));

    connect(this,SIGNAL(signalUser1()),
        d->tabExif,SLOT(slotUser1()));

    connect(this,SIGNAL(signalUser1()),
        d->tabIptc,SLOT(slotUser1()));

    connect(this,SIGNAL(signalUser1()),
        d->tabXmp,SLOT(slotUser1()));

    connect(this,SIGNAL(user2Clicked()),
        this,SLOT(slotUser2()));

    connect(this,SIGNAL(signalUser2()),
        d->tabExif,SLOT(slotUser2()));

    connect(this,SIGNAL(signalUser2()),
        d->tabIptc,SLOT(slotUser2()));

    connect(this,SIGNAL(signalUser2()),
        d->tabXmp,SLOT(slotUser2()));

    connect(this,SIGNAL(okClicked()),
        this,SLOT(slotOk()));

    connect(this,SIGNAL(signalOk()),
        d->tabExif,SLOT(slotOk()));

    connect(this,SIGNAL(signalOk()),
        d->tabIptc,SLOT(slotOk()));

    connect(this,SIGNAL(signalOk()),
        d->tabXmp,SLOT(slotOk()));

    connect(d->tabExif,SIGNAL(signalSetReadOnly(bool)),
        this,SLOT(slotSetReadOnly(bool)));

    connect(d->tabIptc,SIGNAL(signalSetReadOnly(bool)),
        this,SLOT(slotSetReadOnly(bool)));

    connect(d->tabXmp,SIGNAL(signalSetReadOnly(bool)),
        this,SLOT(slotSetReadOnly(bool)));

    connect(d->tabWidget,SIGNAL(currentChanged(int)),
        this,SLOT(setWindowTitle(int)));

    //----------------------------------------------------------

    enableButton(Apply, false);

    d->about = new KPAboutData(ki18n("Edit Metadata"),
                               0,
                               KAboutData::License_GPL,
                               ki18n("A Plugin to edit pictures' metadata."),
                               ki18n("(c) 2006-2011, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author and Maintainer"),
                        "caulier dot gilles at gmail dot com");

    d->about->addAuthor(ki18n("Victor Dodon"), ki18n("Developer"),
                        "victor dot dodon at cti dot pub dot ro");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction* handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    readSettings();
    slotItemChanged();
}

MetadataEditDialog::~MetadataEditDialog()
{
    delete d->about;
    delete d;
};

void MetadataEditDialog::slotHelp()
{
    KToolInvocation::invokeHelp("metadataeditor", "kipi-plugins");
}

void MetadataEditDialog::slotClose()
{
    emit signalClose();
    saveSettings();
    close();
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

void MetadataEditDialog::slotApply()
{
    emit signalApply();
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
}

void MetadataEditDialog::slotUser1()
{
    slotApply();
    d->currItem++;
    emit signalUser1();
    slotItemChanged();
}

void MetadataEditDialog::slotUser2()
{
    slotApply();
    d->currItem--;
    emit signalUser2();
    slotItemChanged();
}

void MetadataEditDialog::slotItemChanged()
{
    enableButton(Apply, !d->isReadOnly);
    setWindowTitle(d->tabWidget->currentIndex());
    enableButton(User1, *(d->currItem) != d->urls.last());
    enableButton(User2, *(d->currItem) != d->urls.first());
    enableButton(Apply, false);
}

void MetadataEditDialog::slotOk()
{
    slotApply();
    saveSettings();
    emit signalOk();
    accept();
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

void MetadataEditDialog::setWindowTitle(int tabIndex)
{
    QString tabName;
    switch (tabIndex)
    {
        case 0:
            tabName = "EXIF";
            break;

        case 1:
            tabName = "IPTC";
            break;

        case 2:
            tabName = "XMP";
            break;
    }

    setCaption(QString("%1 (%2/%3) - %4 %5 %6 ")
        .arg((*d->currItem).fileName())
        .arg(d->urls.indexOf(*(d->currItem))+1)
        .arg(d->urls.count())
        .arg(i18n("Edit"))
        .arg(tabName)
        .arg(i18n("Metadata")));
}

}  // namespace KIPIMetadataEditPlugin
