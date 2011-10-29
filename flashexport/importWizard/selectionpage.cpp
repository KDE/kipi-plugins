/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#include "selectionpage.moc"

// Qt includes

#include <QVBoxLayout>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <kvbox.h>
#include <kstandarddirs.h>
#include <kiconloader.h>

// LibKIPI includes

#include <libkipi/imagecollectionselector.h>

//Local includes

#include "flashmanager.h"

namespace KIPIFlashExportPlugin
{
    
class SelectionPage::SelectionPagePriv
{
public:

    SelectionPagePriv()
    {
        imageCollectionSelector = 0;
    }

    KIPI::ImageCollectionSelector* imageCollectionSelector;
};

SelectionPage::SelectionPage(FlashManager *mngr, KAssistantDialog* dlg)
    : KIPIPlugins::WizardPage(dlg, i18n("Select Image Collections")), d(new SelectionPagePriv)
{
    KVBox *vbox   = new KVBox(this);
    KIPI::Interface* interface = mngr->iface();
    d->imageCollectionSelector = interface->imageCollectionSelector(vbox);

    setPageWidget(vbox);
    setLeftBottomPix(DesktopIcon("flash", 128));
}

SelectionPage::~SelectionPage()
{
    delete d;
}

QList<KIPI::ImageCollection> SelectionPage::selection() const
{
    return d->imageCollectionSelector->selectedImageCollections();
}

}   // namespace KIPIFlashExportPlugin
