/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "wizard.moc"

// Qt includes

#include <QFileInfo>
#include <QPalette>
#include <QtGlobal>

// KDE includes

#include <kdebug.h>
#include <kurl.h>

// libHUpnp includes

#include <HUpnpCore/HUpnpInfo>
#include <HUpnpAv/HUpnpAvInfo>

// libkipi includes

#include <libkipi/imagecollection.h>
#include <libkipi/imagecollectionselector.h>
#include <libkipi/interface.h>

// Local includes

#include "kpimageslist.h"
#include "kpaboutdata.h"
#include "dlnawidget.h"
#include "welcomepage.h"

using namespace Herqq;

namespace KIPIDLNAExportPlugin
{

class Wizard::Private
{
public:

    Private()
    {
        selectionPage              = 0;
        collectionSelector         = 0;
        welcomePage                = 0;
        selectionPageItem          = 0;
        collectionSelectorPageItem = 0;
        welcomePageItem            = 0;
        imageDialogOptionSelected  = true;
    }

    DLNAWidget*              selectionPage;
    ImageCollectionSelector* collectionSelector;
    WelcomePage*             welcomePage;
    KPageWidgetItem*         selectionPageItem;
    KPageWidgetItem*         collectionSelectorPageItem;
    KPageWidgetItem*         welcomePageItem;
    KUrl::List               imageList;
    bool                     imageDialogOptionSelected;
};

Wizard::Wizard(QWidget* const parent)
    : KPWizardDialog(parent), d(new Private)
{
    // Caption
    setCaption(i18n("DLNA Export"));
    setMinimumSize(300, 500);

    //--------------------------------------------------------------------

    KPAboutData* about = new KPAboutData(ki18n("DLNA Export"),
                             0,
                             KAboutData::License_GPL,
                             ki18n("A Kipi plugin to export image collections via DLNA\n"
                                   "Using LibHUpnp %1\n"
                                   "Using LibHUpnpAv %2\n")
                                   .subs(Upnp::hupnpCoreVersion())
                                   .subs(Upnp::Av::hupnpAvVersion()),
                             ki18n("(c) 2012, Smit Mehta"));

    about->addAuthor(ki18n( "Smit Mehta" ),
                     ki18n("Author and maintainer"),
                     "smit dot meh at gmail dot com");

    about->addAuthor(ki18n("Marcel Wiesweg"),
                     ki18n("Developer"),
                     "marcel dot wiesweg at gmx dot de");

    about->addAuthor(ki18n("Gilles Caulier"),
                     ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->setHandbookEntry("dlnaexport");
    setAboutData(about);

    //-----------------------------------------------------------------------

    d->welcomePage     = new WelcomePage(this);
    d->welcomePageItem = addPage(d->welcomePage, "Welcome to DLNA Export");

    d->collectionSelector         = iface()->imageCollectionSelector(this);
    d->collectionSelectorPageItem = addPage(d->collectionSelector, i18n("Select the required collections"));

    setValid(d->collectionSelectorPageItem, false);

    connect(d->collectionSelector, SIGNAL(selectionChanged()),
            this, SLOT(updateCollectionSelectorPageValidity()));

    connect(d->collectionSelector, SIGNAL(selectionChanged()),
            this, SLOT(getImagesFromCollection()));

    d->selectionPage     = new DLNAWidget(this);
    d->selectionPageItem = addPage(d->selectionPage, "Images to be exported");
}

Wizard::~Wizard()
{
    delete d;
}

void Wizard::next()
{
    if (currentPage() == d->welcomePageItem)
    {
        d->imageDialogOptionSelected = d->welcomePage->getImageDialogOptionSelected();

        if (d->imageDialogOptionSelected)
        {
            KAssistantDialog::next();
            KAssistantDialog::next();
            d->selectionPage->setControlButtons(true);
        }
        else
        {
            KAssistantDialog::next();
            d->selectionPage->setControlButtons(false);
        }
    }
    else if (currentPage() == d->collectionSelectorPageItem)
    {
        d->selectionPage->setImages(d->imageList);
        KAssistantDialog::next();
    }
    else
    {
        KAssistantDialog::next();
    }
}

void Wizard::back()
{
    if (currentPage() == d->selectionPageItem)
    {
        if (d->imageDialogOptionSelected)
        {
            KAssistantDialog::back();
            KAssistantDialog::back();
        }
        else
        {
            KAssistantDialog::back();
        }
    }
    else
    {
        KAssistantDialog::back();
    }
}

void Wizard::updateCollectionSelectorPageValidity()
{
    setValid(d->collectionSelectorPageItem, !d->collectionSelector->selectedImageCollections().empty());
}

void Wizard::getImagesFromCollection()
{
    d->imageList.clear();

    foreach(ImageCollection images, d->collectionSelector->selectedImageCollections())
    {
        d->imageList.append(images.images());
    }
}

void Wizard::accept()
{
    kDebug() << "you clicked finish";
    d->selectionPage->slotSelectDirectory();
    KAssistantDialog::accept();
}

} // namespace KIPIDLNAExportPlugin
