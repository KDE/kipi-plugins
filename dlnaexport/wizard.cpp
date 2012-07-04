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

// C++ includes

#include <memory>

// Qt includes

#include <QFileInfo>
#include <QPainter>
#include <QPalette>
#include <QtGlobal>

// KDE includes

#include <kapplication.h>
#include <kconfigdialogmanager.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <kfile.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdeprintdialog.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kdesktopfile.h>

// libHUpnp includes

#include <HUpnpCore/HUpnpInfo>
#include <HUpnpAv/HUpnpAvInfo>

// libkipi includes

#include <libkipi/imagecollectionselector.h>
#include <libkipi/interface.h>

// Local includes

#include "kpimageslist.h"
#include "kpaboutdata.h"
#include "kpmetadata.h"
#include "dlnawidget.h"

using namespace Herqq;

namespace KIPIDLNAExportPlugin
{

struct Wizard::Private
{
    DLNAWidget*              m_selectionPage;
    ImageCollectionSelector* m_collectionSelector;
    KPageWidgetItem*         m_selectionPageItem;
};

Wizard::Wizard(QWidget* const parent)
    : KPWizardDialog(parent), d(new Private)
{
    // Caption
    setCaption(i18n("DLNA Export"));

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

    d->m_selectionPage  = new DLNAWidget(iface(), QString(), this);
    addPage(d->m_selectionPage, "Select the collection to upload");
}

Wizard::~Wizard()
{
    delete d;
}

} // namespace KIPIDLNAExportPlugin
