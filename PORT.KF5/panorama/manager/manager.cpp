/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

// Qt includes

#include <QFile>

// KDE includes

#include <kdebug.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "importwizarddlg.h"
#include "actionthread.h"
#include "autooptimiserbinary.h"
#include "cpcleanbinary.h"
#include "cpfindbinary.h"
#include "enblendbinary.h"
#include "makebinary.h"
#include "nonabinary.h"
#include "panomodifybinary.h"
#include "pto2mkbinary.h"
#include <ptofile.h>

namespace KIPIPanoramaPlugin
{

struct Manager::ManagerPriv
{
    ManagerPriv()
    : basePtoData(0),
      cpFindPtoData(0),
      cpCleanPtoData(0),
      autoOptimisePtoData(0),
      viewAndCropOptimisePtoData(0),
      previewPtoData(0),
      panoPtoData(0),
      iface(0),
      thread(0),
      wizard(0),
      config("kipirc"),
      group(config.group(QString("Panorama Settings")))
    {
        gPano    = group.readEntry("GPano", false);
//         hdr      = group.readEntry("HDR", false);
        fileType = (PanoramaFileType) group.readEntry("File Type", (int) JPEG);
    }


    ~ManagerPriv()
    {
//         group.writeEntry("HDR", hdr);
        group.writeEntry("GPano", gPano);
        group.writeEntry("File Type", (int) fileType);
        config.sync();
        if (basePtoData != 0)
            delete basePtoData;
        if (cpFindPtoData != 0)
            delete cpFindPtoData;
        if (cpCleanPtoData != 0)
            delete cpCleanPtoData;
        if (autoOptimisePtoData != 0)
            delete autoOptimisePtoData;
        if (viewAndCropOptimisePtoData != 0)
            delete viewAndCropOptimisePtoData;
        if (previewPtoData != 0)
            delete previewPtoData;
        if (panoPtoData != 0)
            delete panoPtoData;
    }

    KUrl::List                     inputUrls;

    KUrl                           basePtoUrl;
    PTOType*                       basePtoData;
    KUrl                           cpFindPtoUrl;
    PTOType*                       cpFindPtoData;
    KUrl                           cpCleanPtoUrl;
    PTOType*                       cpCleanPtoData;
    KUrl                           autoOptimisePtoUrl;
    PTOType*                       autoOptimisePtoData;
    KUrl                           viewAndCropOptimisePtoUrl;
    PTOType*                       viewAndCropOptimisePtoData;
    KUrl                           previewPtoUrl;
    PTOType*                       previewPtoData;
    KUrl                           panoPtoUrl;
    PTOType*                       panoPtoData;

    KUrl                           previewMkUrl;
    KUrl                           previewUrl;
    KUrl                           mkUrl;
    KUrl                           panoUrl;

    bool                           gPano;
//     bool                           hdr;

    PanoramaFileType               fileType;

    ItemUrlsMap                    preProcessedUrlsMap;

    RawDecodingSettings            rawDecodingSettings;

    Interface*                     iface;

    ActionThread*                  thread;

    AutoOptimiserBinary            autoOptimiserBinary;
    CPCleanBinary                  cpCleanBinary;
    CPFindBinary                   cpFindBinary;
    EnblendBinary                  enblendBinary;
    MakeBinary                     makeBinary;
    NonaBinary                     nonaBinary;
    PanoModifyBinary               panoModifyBinary;
    Pto2MkBinary                   pto2MkBinary;

    ImportWizardDlg*               wizard;

private:

    KConfig      config;
    KConfigGroup group;
};

Manager::Manager(QObject* const parent)
    : QObject(parent), d(new ManagerPriv)
{
    d->thread                               = new ActionThread(this);
    d->rawDecodingSettings.sixteenBitsImage = true;
}

Manager::~Manager()
{
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

void Manager::setGPano(bool gPano)
{
    d->gPano = gPano;
}

bool Manager::gPano() const
{
    return d->gPano;
}

// void Manager::setHDR(bool hdr)
// {
//     d->hdr = hdr;
// }
// 
// bool Manager::hdr() const
// {
//     return d->hdr;
// }

void Manager::setFileFormatJPEG()
{
    d->fileType = JPEG;
}

void Manager::setFileFormatTIFF()
{
    d->fileType = TIFF;
}

void Manager::setFileFormatHDR()
{
    d->fileType = HDR;
}

PanoramaFileType Manager::format() const
{
    return d->fileType;
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

PanoModifyBinary& Manager::panoModifyBinary() const
{
    return d->panoModifyBinary;
}

Pto2MkBinary& Manager::pto2MkBinary() const
{
    return d->pto2MkBinary;
}

KUrl& Manager::basePtoUrl() const
{
    return d->basePtoUrl;
}

const PTOType& Manager::basePtoData()
{
    if (d->basePtoData == 0)
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->basePtoUrl.toLocalFile());
        d->basePtoData = file.getPTO();

        if (d->basePtoData == 0)
            d->basePtoData = new PTOType(cpFindBinary().version());
    }

    return *(d->basePtoData);
}

void Manager::resetBasePto()
{
    if (d->basePtoData != 0)
    {
        delete d->basePtoData;
        d->basePtoData = 0;
    }
    QFile pto(d->basePtoUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->basePtoUrl = KUrl();
}

KUrl& Manager::cpFindPtoUrl() const
{
    return d->cpFindPtoUrl;
}

const PTOType& Manager::cpFindPtoData()
{
    if (d->cpFindPtoData == 0)
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->cpFindPtoUrl.toLocalFile());
        d->cpFindPtoData = file.getPTO();

        if (d->cpFindPtoData == 0)
            d->cpFindPtoData = new PTOType(cpFindBinary().version());
    }

    return *(d->cpFindPtoData);
}

void Manager::resetCpFindPto()
{
    if (d->cpFindPtoData != 0)
    {
        delete d->cpFindPtoData;
        d->cpFindPtoData = 0;
    }
    QFile pto(d->cpFindPtoUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->cpFindPtoUrl = KUrl();
}

KUrl& Manager::cpCleanPtoUrl() const
{
    return d->cpCleanPtoUrl;
}

const PTOType& Manager::cpCleanPtoData()
{
    if (d->cpCleanPtoData == 0)
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->cpCleanPtoUrl.toLocalFile());
        d->cpCleanPtoData = file.getPTO();

        if (d->cpCleanPtoData == 0)
            d->cpCleanPtoData = new PTOType(cpFindBinary().version());
    }

    return *(d->cpCleanPtoData);
}

void Manager::resetCpCleanPto()
{
    if (d->cpCleanPtoData != 0)
    {
        delete d->cpCleanPtoData;
        d->cpCleanPtoData = 0;
    }
    QFile pto(d->cpCleanPtoUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->cpCleanPtoUrl = KUrl();
}

KUrl& Manager::autoOptimisePtoUrl() const
{
    return d->autoOptimisePtoUrl;
}

const PTOType& Manager::autoOptimisePtoData()
{
    if (d->autoOptimisePtoData == 0)
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->autoOptimisePtoUrl.toLocalFile());
        d->autoOptimisePtoData = file.getPTO();

        if (d->autoOptimisePtoData == 0)
            d->autoOptimisePtoData = new PTOType(cpFindBinary().version());
    }

    return *(d->autoOptimisePtoData);
}

void Manager::resetAutoOptimisePto()
{
    if (d->autoOptimisePtoData != 0) {
        delete d->autoOptimisePtoData;
        d->autoOptimisePtoData = 0;
    }
    QFile pto(d->autoOptimisePtoUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->autoOptimisePtoUrl = KUrl();
}

KUrl& Manager::viewAndCropOptimisePtoUrl() const
{
    return d->viewAndCropOptimisePtoUrl;
}

const PTOType& Manager::viewAndCropOptimisePtoData()
{
    if (d->viewAndCropOptimisePtoData == 0)
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->viewAndCropOptimisePtoUrl.toLocalFile());
        d->viewAndCropOptimisePtoData = file.getPTO();

        if (d->viewAndCropOptimisePtoData == 0)
            d->viewAndCropOptimisePtoData = new PTOType(cpFindBinary().version());
    }

    return *(d->viewAndCropOptimisePtoData);
}

void Manager::resetViewAndCropOptimisePto()
{
    if (d->viewAndCropOptimisePtoData != 0)
    {
        delete d->viewAndCropOptimisePtoData;
        d->viewAndCropOptimisePtoData = 0;
    }
    QFile pto(d->viewAndCropOptimisePtoUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->viewAndCropOptimisePtoUrl = KUrl();
}

KUrl& Manager::previewPtoUrl() const
{
    return d->previewPtoUrl;
}

const PTOType& Manager::previewPtoData()
{
    if (d->previewPtoData == 0)
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->previewPtoUrl.toLocalFile());
        d->previewPtoData = file.getPTO();

        if (d->previewPtoData == 0)
            d->previewPtoData = new PTOType(cpFindBinary().version());
    }

    return *(d->previewPtoData);
}

void Manager::resetPreviewPto()
{
    if (d->previewPtoData != 0) {
        delete d->previewPtoData;
        d->previewPtoData = 0;
    }
    QFile pto(d->previewPtoUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->previewPtoUrl = KUrl();
}

KUrl& Manager::panoPtoUrl() const
{
    return d->panoPtoUrl;
}

const PTOType& Manager::panoPtoData()
{
    if (d->panoPtoData == 0)
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->panoPtoUrl.toLocalFile());
        d->panoPtoData = file.getPTO();

        if (d->panoPtoData == 0)
            d->panoPtoData = new PTOType(cpFindBinary().version());
    }

    return *(d->panoPtoData);
}

void Manager::resetPanoPto()
{
    if (d->panoPtoData != 0) {
        delete d->panoPtoData;
        d->panoPtoData = 0;
    }
    QFile pto(d->panoPtoUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->panoPtoUrl = KUrl();
}

KUrl& Manager::previewMkUrl() const
{
    return d->previewMkUrl;
}

void Manager::resetPreviewMkUrl()
{
    QFile pto(d->previewMkUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->previewMkUrl = KUrl();
}

KUrl& Manager::previewUrl() const
{
    return d->previewUrl;
}

void Manager::resetPreviewUrl()
{
    QFile pto(d->previewUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->previewUrl = KUrl();
}

KUrl& Manager::mkUrl() const
{
    return d->mkUrl;
}

void Manager::resetMkUrl()
{
    QFile pto(d->mkUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->mkUrl = KUrl();
}

KUrl& Manager::panoUrl() const
{
    return d->panoUrl;
}

void Manager::resetPanoUrl()
{
    QFile pto(d->panoUrl.toLocalFile());
    if (pto.exists())
    {
        pto.remove();
    }
    d->panoUrl = KUrl();
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

KUrl::List& Manager::itemsList() const
{
    return d->inputUrls;
}

void Manager::setRawDecodingSettings(const RawDecodingSettings& settings)
{
    d->rawDecodingSettings = settings;
}

RawDecodingSettings& Manager::rawDecodingSettings() const
{
    return d->rawDecodingSettings;
}

void Manager::setPreProcessedMap(const ItemUrlsMap& urls)
{
    d->preProcessedUrlsMap = urls;
}

ItemUrlsMap& Manager::preProcessedMap() const
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

void Manager::startWizard()
{
    d->wizard = new ImportWizardDlg(this);
    d->wizard->show();
}

} // namespace KIPIPanoramaPlugin
