/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "flashmanager.moc"

// KDE includes

#include <kdebug.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "aboutdata.h"
#include "importwizarddlg.h"
#include "simpleviewer.h"
#include <kapplication.h>


namespace KIPIFlashExportPlugin
{

class FlashManager::FlashManagerPriv
{
public:

	FlashManagerPriv()
    {
        iface  = 0;
        about  = 0;
        wizard = 0;
        simple = 0;
    }


    SimpleViewerSettingsContainer* containerSettings;

    Interface*                     iface;

    FlashExportAboutData*          about;

    ImportWizardDlg*               wizard;

    SimpleViewer*                  simple;

};

FlashManager::FlashManager(QObject* parent)
   : QObject(parent), d(new FlashManagerPriv)
{
}

FlashManager::~FlashManager()
{
    delete d->about;
    delete d->wizard;
    delete d->simple;
    delete d;
}

void FlashManager::initSimple()
{
// it cannot be initialized in main function because interface pointer is null.
	d->simple = new SimpleViewer(d->iface,this); 
	kDebug() << "simpleview Initialized...";
}

void FlashManager::setAbout(FlashExportAboutData* about)
{
    d->about = about;
}

FlashExportAboutData* FlashManager::about() const
{
    return d->about;
}

void FlashManager::setIface(Interface* iface)
{
    d->iface = iface;
}

Interface* FlashManager::iface() const
{
    return d->iface;
}

bool FlashManager::installPlugin(KUrl url)
{
	if(d->simple->unzip(url.path()))
		return true;
	else
		return false;
}

SimpleViewer* FlashManager::simpleView() const
{
	return d->simple;
}

void FlashManager::run()
{
    startWizard();
}

void FlashManager::startWizard()
{
    d->wizard = new ImportWizardDlg(this,kapp->activeWindow());
    d->wizard->show();
}

} // namespace KIPIExpoBlendingPlugin
