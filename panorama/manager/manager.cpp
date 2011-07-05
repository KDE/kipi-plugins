/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#include "manager.moc"

// KDE includes

#include <kdebug.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "aboutdata.h"
#include "importwizarddlg.h"

#include "actionthread.h"
#include "cpfindbinary.h"
#include "autooptimiserbinary.h"

namespace KIPIPanoramaPlugin
{

struct Manager::ManagerPriv
{
    ManagerPriv() : iface(0), about(0), thread(0), wizard(0)/*, dlg(0)*/ {};

    KUrl::List              inputUrls;

    ItemUrlsMap             preProcessedUrlsMap;

    RawDecodingSettings     rawDecodingSettings;

    Interface*              iface;

    PanoramaAboutData*      about;

    ActionThread*           thread;

    AutoOptimiserBinary     autoOptimiserBinary;
    CPFindBinary            cpFindBinary;

    ImportWizardDlg*        wizard;
    //PanoramaDlg*            dlg;
};

Manager::Manager(QObject* parent)
       : QObject(parent), d(new ManagerPriv)
{
    d->thread                               = new ActionThread(this);
    d->rawDecodingSettings.sixteenBitsImage = true;
}

Manager::~Manager()
{
    if (d->about)
        delete d->about;
    delete d->thread;
    delete d->wizard;
    //delete d->dlg;
    delete d;
}

bool Manager::checkBinaries()
{
    if (!d->autoOptimiserBinary.showResults())
        return false;

    if (!d->cpFindBinary.showResults())
        return false;

    return true;
}

void Manager::setAbout(PanoramaAboutData* about)
{
    d->about = about;
}

PanoramaAboutData* Manager::about() const
{
    return d->about;
}

AutoOptimiserBinary& Manager::autoOptimiserBinary() const
{
    return d->autoOptimiserBinary;
}

CPFindBinary& Manager::cpFindBinary() const
{
    return d->cpFindBinary;
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

    connect(d->wizard, SIGNAL(accepted()),
            this, SLOT(slotStartDialog()));

    d->wizard->show();
}

void Manager::slotStartDialog()
{
    d->inputUrls = d->wizard->itemUrls();
    //d->dlg = new PanoramaDlg(this);
    //d->dlg->show();
}

} // namespace KIPIPanoramaPlugin
