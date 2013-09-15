/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "manager.moc"

// KDE includes

#include <kdebug.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "importwizarddlg.h"
#include "expoblendingdlg.h"
#include "actionthread.h"
#include "alignbinary.h"
#include "enfusebinary.h"

namespace KIPIExpoBlendingPlugin
{

class Manager::ManagerPriv
{
public:

    ManagerPriv()
    {
        iface  = 0;
        thread = 0;
        wizard = 0;
        dlg    = 0;
    }

    KUrl::List             inputUrls;

    ItemUrlsMap            preProcessedUrlsMap;

    RawDecodingSettings    rawDecodingSettings;

    Interface*             iface;

    ActionThread*          thread;

    AlignBinary            alignBinary;
    EnfuseBinary           enfuseBinary;

    ImportWizardDlg*       wizard;
    ExpoBlendingDlg*       dlg;
};

Manager::Manager(QObject* const parent)
       : QObject(parent), d(new ManagerPriv)
{
    d->thread                               = new ActionThread(this);
    d->rawDecodingSettings.sixteenBitsImage = true;

    connect(&d->enfuseBinary, SIGNAL(signalEnfuseVersion(double)),
            this, SLOT(slotSetEnfuseVersion(double)));

    if (d->enfuseBinary.isValid())
    {
        slotSetEnfuseVersion(d->enfuseBinary.getVersion());
    }
}

Manager::~Manager()
{
    delete d->thread;
    delete d->wizard;
    delete d->dlg;
    delete d;
}

bool Manager::checkBinaries()
{
    if (!d->alignBinary.recheckDirectories())
        return false;

    if (!d->enfuseBinary.recheckDirectories())
        return false;

    return true;
}

AlignBinary& Manager::alignBinary() const
{
    return d->alignBinary;
}

EnfuseBinary& Manager::enfuseBinary() const
{
    return d->enfuseBinary;
}

void Manager::setIface(Interface* const iface)
{
    d->iface = iface;
}

Interface* Manager::iface() const
{
    return d->iface;
}

void Manager::setItemsList(const KUrl::List& urls)
{
    d->inputUrls = urls;
}

KUrl::List Manager::itemsList() const
{
    return d->inputUrls;
}

void Manager::setRawDecodingSettings(const RawDecodingSettings& settings)
{
    d->rawDecodingSettings = settings;
}

RawDecodingSettings Manager::rawDecodingSettings() const
{
    return d->rawDecodingSettings;
}

void Manager::setPreProcessedMap(const ItemUrlsMap& urls)
{
    d->preProcessedUrlsMap = urls;
}

ItemUrlsMap Manager::preProcessedMap() const
{
    return d->preProcessedUrlsMap;
}

ActionThread* Manager::thread() const
{
    return d->thread;
}

void Manager::run()
{
    startWizard();
}

void Manager::cleanUp()
{
    d->thread->cleanUpResultFiles();
}

void Manager::startWizard()
{
    d->wizard = new ImportWizardDlg(this);
    d->wizard->show();

    connect(d->wizard, SIGNAL(accepted()),
            this, SLOT(slotStartDialog()));
}

void Manager::slotStartDialog()
{
    d->inputUrls = d->wizard->itemUrls();
    d->dlg = new ExpoBlendingDlg(this);
    d->dlg->show();
}

void Manager::slotSetEnfuseVersion(double version)
{
    d->thread->setEnfuseVersion(version);
}

} // namespace KIPIExpoBlendingPlugin
