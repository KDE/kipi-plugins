/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 20011-2013 by Veaceslav Munteanu <slavuttici at gmail dot com>
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
#include <kapplication.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "aboutdata.h"
#include "importwizarddlg.h"
#include "simpleviewer.h"

using namespace KIPIPlugins;

namespace KIPIFlashExportPlugin
{

class FlashManager::FlashManagerPriv
{
public:

    FlashManagerPriv()
    {
        iface               = 0;
        wizard              = 0;
        simple              = 0;
        containerSettings   = 0;
    }

    SimpleViewerSettingsContainer* containerSettings;

    Interface*                     iface;

    ImportWizardDlg*               wizard;

    SimpleViewer*                  simple;
};

FlashManager::FlashManager(QObject* const parent)
   : QObject(parent), d(new FlashManagerPriv)
{
}

FlashManager::~FlashManager()
{
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

void FlashManager::setIface(Interface* const iface)
{
    d->iface = iface;
}

Interface* FlashManager::iface() const
{
    return d->iface;
}

bool FlashManager::installPlugin(const KUrl& url)
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
    d->wizard = new ImportWizardDlg(this, kapp->activeWindow());
    d->wizard->show();
}

} // namespace KIPIExpoBlendingPlugin
