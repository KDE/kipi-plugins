/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#include "autooptimiserbinary.h"
#include "cpcleanbinary.h"
#include "cpfindbinary.h"
#include "enblendbinary.h"
#include "makebinary.h"
#include "nonabinary.h"
#include "pto2mkbinary.h"

namespace KIPIPanoramaPlugin
{

struct Manager::ManagerPriv
{
    ManagerPriv()
    : iface(0),
      about(0),
      thread(0),
      wizard(0),
      config("kipirc"),
      group(config.group(QString("Panorama Settings")))
    {
        hdr      = group.readEntry("HDR", false);
        fileType = (ActionThread::PanoramaFileType) group.readEntry("File Type", (int) ActionThread::JPEG);
    }


    ~ManagerPriv()
    {
        group.writeEntry("HDR", hdr);
        config.sync();
    }

    KUrl::List                     inputUrls;
    KUrl                           cpFindUrl;
    KUrl                           autoOptimiseUrl;
    KUrl                           previewUrl;
    KUrl                           panoUrl;

    bool                           hdr;

    ActionThread::PanoramaFileType fileType;

    ItemUrlsMap                    preProcessedUrlsMap;

    RawDecodingSettings            rawDecodingSettings;

    Interface*                     iface;

    PanoramaAboutData*             about;

    ActionThread*                  thread;

    AutoOptimiserBinary            autoOptimiserBinary;
    CPCleanBinary                  cpCleanBinary;
    CPFindBinary                   cpFindBinary;
    EnblendBinary                  enblendBinary;
    MakeBinary                     makeBinary;
    NonaBinary                     nonaBinary;
    Pto2MkBinary                   pto2MkBinary;

    ImportWizardDlg*               wizard;

private:

    KConfig      config;
    KConfigGroup group;
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
    delete d;
}

bool Manager::checkBinaries()
{
    return d->autoOptimiserBinary.recheckDirectories()
        && d->cpCleanBinary.recheckDirectories()
        && d->cpFindBinary.recheckDirectories()
        && d->enblendBinary.recheckDirectories()
        && d->makeBinary.recheckDirectories()
        && d->nonaBinary.recheckDirectories()
        && d->pto2MkBinary.recheckDirectories();
}

void Manager::setHDR(bool hdr)
{
    d->hdr = hdr;
}

bool Manager::hdr() const
{
    return d->hdr;
}

void Manager::setFileFormatJPEG()
{
    d->fileType = ActionThread::JPEG;
}

void Manager::setFileFormatTIFF()
{
    d->fileType = ActionThread::TIFF;
}

ActionThread::PanoramaFileType Manager::format() const
{
    return d->fileType;
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

CPCleanBinary& Manager::cpCleanBinary() const
{
    return d->cpCleanBinary;
}

CPFindBinary& Manager::cpFindBinary() const
{
    return d->cpFindBinary;
}

EnblendBinary& Manager::enblendBinary() const
{
    return d->enblendBinary;
}

MakeBinary& Manager::makeBinary() const
{
    return d->makeBinary;
}

NonaBinary& Manager::nonaBinary() const
{
    return d->nonaBinary;
}

Pto2MkBinary& Manager::pto2MkBinary() const
{
    return d->pto2MkBinary;
}

void Manager::setPreviewUrl(const KUrl& url)
{
    d->previewUrl = url;
}

KUrl Manager::previewUrl() const
{
    return d->previewUrl;
}

void Manager::setPanoUrl(const KUrl& url)
{
    d->panoUrl = url;
}

KUrl Manager::panoUrl() const
{
    return d->panoUrl;
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

void Manager::setCPFindUrl(const KUrl& url)
{
    d->cpFindUrl = url;
}

KUrl Manager::cpFindUrl() const
{
    return d->cpFindUrl;
}

void Manager::setAutoOptimiseUrl(const KUrl& url)
{
    d->autoOptimiseUrl = url;
}

KUrl Manager::autoOptimiseUrl() const
{
    return d->autoOptimiseUrl;
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
}

} // namespace KIPIPanoramaPlugin
