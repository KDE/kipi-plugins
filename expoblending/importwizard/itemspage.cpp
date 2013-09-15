/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "itemspage.moc"

// Qt includes

#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>
#include <QTimer>

// KDE includes

#include <kstandarddirs.h>
#include <kdialog.h>
#include <kvbox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kpimageslist.h"
#include "manager.h"
#include "actionthread.h"

namespace KIPIExpoBlendingPlugin
{

class ItemsPage::ItemsPagePriv
{
public:

    ItemsPagePriv()
    {
        list = 0;
        mngr = 0;
    }

    KPImagesList* list;

    Manager*      mngr;
};

ItemsPage::ItemsPage(Manager* const mngr, KAssistantDialog* const dlg)
         : KPWizardPage(dlg, i18n("<b>Set Bracketed Images</b>")),
           d(new ItemsPagePriv)
{
    d->mngr        = mngr;
    KVBox* vbox    = new KVBox(this);
    QLabel* label1 = new QLabel(vbox);
    label1->setWordWrap(true);
    label1->setText(i18n("<qt>"
                         "<p>Set here the list of your bracketed images to fuse. Please follow these conditions:</p>"
                         "<ul><li>At least 2 images from the same subject must be added to the stack.</li>"
                         "<li>Do not mix images with different color depth.</li>"
                         "<li>All images must have the same dimensions.</li></ul>"
                         "</qt>"));

    d->list = new KPImagesList(vbox);
    d->list->listView()->setColumn(KPImagesListView::User1, i18n("Exposure (EV)"), true);
    d->list->slotAddImages(d->mngr->itemsList());

    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_expoblending/pics/assistant-stack.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->mngr->thread(), SIGNAL(starting(KIPIExpoBlendingPlugin::ActionData)),
            this, SLOT(slotAction(KIPIExpoBlendingPlugin::ActionData)));

    connect(d->mngr->thread(), SIGNAL(finished(KIPIExpoBlendingPlugin::ActionData)),
            this, SLOT(slotAction(KIPIExpoBlendingPlugin::ActionData)));

    connect(d->list, SIGNAL(signalAddItems(KUrl::List)),
            this, SLOT(slotAddItems(KUrl::List)));

    connect(d->list, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    QTimer::singleShot(0, this, SLOT(slotSetupList()));
}

ItemsPage::~ItemsPage()
{
    delete d;
}

void ItemsPage::slotSetupList()
{
    slotAddItems(d->mngr->itemsList());
}

void ItemsPage::slotAddItems(const KUrl::List& urls)
{
    if (!urls.empty())
    {
        d->mngr->thread()->identifyFiles(urls);
        if (!d->mngr->thread()->isRunning())
            d->mngr->thread()->start();
    }

    slotImageListChanged();
}

KUrl::List ItemsPage::itemUrls() const
{
    return d->list->imageUrls();
}

void ItemsPage::setIdentity(const KUrl& url, const QString& identity)
{
    KPImagesListViewItem* item = d->list->listView()->findItem(url);
    if (item)
        item->setText(KPImagesListView::User1, identity);
}

void ItemsPage::slotImageListChanged()
{
    emit signalItemsPageIsValid( d->list->imageUrls().count() > 1 );
}

void ItemsPage::slotAction(const KIPIExpoBlendingPlugin::ActionData& ad)
{
    QString text;

    if (!ad.starting)           // Something is complete...
    {
        switch (ad.action)
        {
            case(IDENTIFY):
            {
                setIdentity(ad.inUrls[0], ad.message);
                break;
            }
            default:
            {
                kWarning() << "Unknown action";
                break;
            }
        }
    }
}

}   // namespace KIPIExpoBlendingPlugin
