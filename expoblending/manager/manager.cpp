/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "aboutdata.h"
#include "importwizarddlg.h"
#include "expoblendingdlg.h"
#include "actionthread.h"
#include "alignbinary.h"
#include "enfusebinary.h"

namespace KIPIExpoBlendingPlugin
{

class ManagerPriv
{
public:

    ManagerPriv()
    {
        iface  = 0;
        about  = 0;
        thread = 0;
        wizard = 0;
        dlg    = 0;
    }

    KUrl::List             inputUrls;

    ItemUrlsMap            preProcessedUrlsMap;

    RawDecodingSettings    rawDecodingSettings;

    Interface*             iface;

    ExpoBlendingAboutData* about;

    ActionThread*          thread;

    AlignBinary            alignBinary;
    EnfuseBinary           enfuseBinary;

    ImportWizardDlg*       wizard;
    ExpoBlendingDlg*       dlg;
};

Manager::Manager(QObject* parent)
       : QObject(parent), d(new ManagerPriv)
{
    d->thread                               = new ActionThread(this);
    d->rawDecodingSettings.sixteenBitsImage = true;
}

Manager::~Manager()
{
    delete d->about;
    delete d->thread;
    delete d->wizard;
    delete d->dlg;

    delete d;
}

bool Manager::checkBinaries()
{
    if (!d->alignBinary.showResults())
        return false;

    if (!d->enfuseBinary.showResults())
        return false;

    d->thread->setEnfuseVersion(d->enfuseBinary.version());
    return true;
}

void Manager::setAbout(ExpoBlendingAboutData* about)
{
    d->about = about;
}

ExpoBlendingAboutData* Manager::about() const
{
    return d->about;
}

AlignBinary& Manager::alignBinary() const
{
    return d->alignBinary;
}

EnfuseBinary& Manager::enfuseBinary() const
{
    return d->enfuseBinary;
}

void Manager::setIface(Interface* iface)
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

} // namespace KIPIExpoBlendingPlugin
