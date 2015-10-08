/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011-2013 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#include "selectionpage.h"

// Qt includes

#include <QVBoxLayout>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <imagecollectionselector.h>

// Libkdcraw includes

#include <KDCRAW/RWidgetUtils>

// Local includes

#include "flashmanager.h"
#include "kpimageslist.h"
#include "kputil.h"

using namespace KDcrawIface;

namespace KIPIFlashExportPlugin
{

class SelectionPage::Private
{
public:

    Private()
    {
        imageCollectionSelector = 0;
        imageList               = 0;
        vbox                    = 0;
        manager                 = 0;
    }

    ImageCollectionSelector* imageCollectionSelector;
    KPImagesList*            imageList;
    FlashManager*            manager;
    KPVBox*                   vbox;
};

SelectionPage::SelectionPage(FlashManager* const mngr, KPWizardDialog* const dlg)
    : KPWizardPage(dlg, i18n("Select Image Collections")),
      d(new Private)
{
    d->manager = mngr;
}

void SelectionPage::setPageContent(int choice)
{
    if (d->vbox)
    {
        removePageWidget(d->vbox);
        delete d->vbox;
    }

    d->vbox = new KPVBox(this);

    if (choice == SimpleViewerSettingsContainer::COLLECTION)
    {
        Interface* const interface = d->manager->iface();
        d->imageCollectionSelector = interface->imageCollectionSelector(d->vbox);
    }
    else             // Image Dialog
    {
        d->imageList = new KPImagesList(d->vbox);
        d->imageList->setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    }

    setPageWidget(d->vbox);
    setLeftBottomPix(QIcon::fromTheme(QStringLiteral("kipi-flash")).pixmap(128));
}

SelectionPage::~SelectionPage()
{
    delete d;
}

void SelectionPage::settings(SimpleViewerSettingsContainer* const container)
{
    if (container->imgGetOption == 0)
        container->collections = d->imageCollectionSelector->selectedImageCollections();
    else
        container->imageDialogList = d->imageList->imageUrls();
}

bool SelectionPage::isSelectionEmpty(int imageGetOption)
{
    if (imageGetOption == 0) // Collections
        return d->imageCollectionSelector->selectedImageCollections().isEmpty();
    else                     // Image Dialog
        return d->imageList->imageUrls().isEmpty();
}

}   // namespace KIPIFlashExportPlugin
