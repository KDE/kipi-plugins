/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-01-11
 * Description : a kipi plugin to print images
 *
 * Copyright 2008-2012 by Angelo Naselli <anaselli at linux dot it>
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

struct Wizard::Private
{
    
	Private()
    {
        m_imageDialogOptionSelected = true;
    }
	
	DLNAWidget*              m_selectionPage;
    ImageCollectionSelector* m_collectionSelector;
    WelcomePage*             m_welcomePage;
    KPageWidgetItem*         m_selectionPageItem;
    KPageWidgetItem*         m_collectionSelectorPageItem;
    KPageWidgetItem*         m_welcomePageItem;
    bool                     m_imageDialogOptionSelected;
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

    d->m_welcomePage  = new WelcomePage(this);
    d->m_welcomePageItem = addPage(d->m_welcomePage, "Welcome to DLNA Export");

	d->m_collectionSelector     = iface()->imageCollectionSelector(this);
	d->m_collectionSelectorPageItem = addPage(d->m_collectionSelector, i18n("Select the required collections"));

	setValid(d->m_collectionSelectorPageItem, false);
        
	connect(d->m_collectionSelector, SIGNAL(selectionChanged()),
		this, SLOT(updateCollectionSelectorPageValidity()));

	d->m_selectionPage  = new DLNAWidget(this);
	d->m_selectionPageItem = addPage(d->m_selectionPage, "Images to be exported");
}

Wizard::~Wizard()
{
    delete d;
}

void Wizard::next()
{
    if (currentPage() == d->m_welcomePageItem)
    {
		d->m_imageDialogOptionSelected = d->m_welcomePage->getImageDialogOptionSelected();

		if (d->m_imageDialogOptionSelected)
		{
			KAssistantDialog::next();
			KAssistantDialog::next();
		}
		else
		{
			KAssistantDialog::next();
		}
    }
    else
    {
        KAssistantDialog::next();
    }
    
}

void Wizard::back()
{
    if (currentPage() == d->m_selectionPageItem)
    {
		if (d->m_imageDialogOptionSelected)
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
    setValid(d->m_collectionSelectorPageItem, !d->m_collectionSelector->selectedImageCollections().empty());
}

} // namespace KIPIDLNAExportPlugin
