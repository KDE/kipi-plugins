/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "manager.h"

// Qt includes

#include <QFile>
#include <QSharedPointer>

// LibKIPI includes

#include <interface.h>

// KDE includes

#include <KConfig>

// Local includes

#include <kipiplugins_debug.h>
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

struct Manager::Private
{
    Private()
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
      config(QStringLiteral("kipirc")),
      group(config.group("Panorama Settings"))
    {
        gPano    = group.readEntry("GPano", false);
//         hdr      = group.readEntry("HDR", false);
        fileType = (PanoramaFileType) group.readEntry("File Type", (int) JPEG);
    }


    ~Private()
    {
//         group.writeEntry("HDR", hdr);
        group.writeEntry("GPano", gPano);
        group.writeEntry("File Type", (int) fileType);
        config.sync();
    }

    QList<QUrl>                    inputUrls;

    QUrl                           basePtoUrl;
    QSharedPointer<PTOType>        basePtoData;
    QUrl                           cpFindPtoUrl;
    QSharedPointer<PTOType>        cpFindPtoData;
    QUrl                           cpCleanPtoUrl;
    QSharedPointer<PTOType>        cpCleanPtoData;
    QUrl                           autoOptimisePtoUrl;
    QSharedPointer<PTOType>        autoOptimisePtoData;
    QUrl                           viewAndCropOptimisePtoUrl;
    QSharedPointer<PTOType>        viewAndCropOptimisePtoData;
    QUrl                           previewPtoUrl;
    QSharedPointer<PTOType>        previewPtoData;
    QUrl                           panoPtoUrl;
    QSharedPointer<PTOType>        panoPtoData;

    QUrl                           previewMkUrl;
    QUrl                           previewUrl;
    QUrl                           mkUrl;
    QUrl                           panoUrl;

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
    : QObject(parent),
      d(new Private)
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
    return(d->autoOptimiserBinary.recheckDirectories() &&
           d->cpCleanBinary.recheckDirectories()       &&
           d->cpFindBinary.recheckDirectories()        &&
           d->enblendBinary.recheckDirectories()       &&
           d->makeBinary.recheckDirectories()          &&
           d->nonaBinary.recheckDirectories()          &&
           d->pto2MkBinary.recheckDirectories());
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

QUrl& Manager::basePtoUrl() const
{
    return d->basePtoUrl;
}

QSharedPointer<PTOType> Manager::basePtoData()
{
    if (d->basePtoData.isNull())
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->basePtoUrl.toLocalFile());
        d->basePtoData = QSharedPointer<PTOType>(file.getPTO());

        if (d->basePtoData.isNull())
            d->basePtoData = QSharedPointer<PTOType>(new PTOType(cpFindBinary().version()));
    }

    return d->basePtoData;
}

void Manager::resetBasePto()
{
    d->basePtoData.clear();

    QFile pto(d->basePtoUrl.toLocalFile());

    if (pto.exists())
    {
        pto.remove();
    }

    d->basePtoUrl.clear();
}

QUrl& Manager::cpFindPtoUrl() const
{
    return d->cpFindPtoUrl;
}

QSharedPointer<PTOType> Manager::cpFindPtoData()
{
    if (d->cpFindPtoData.isNull())
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->cpFindPtoUrl.toLocalFile());
        d->cpFindPtoData = QSharedPointer<PTOType>(file.getPTO());

        if (d->cpFindPtoData.isNull())
            d->cpFindPtoData = QSharedPointer<PTOType>(new PTOType(cpFindBinary().version()));
    }

    return d->cpFindPtoData;
}

void Manager::resetCpFindPto()
{
    d->cpFindPtoData.clear();

    QFile pto(d->cpFindPtoUrl.toLocalFile());

    if (pto.exists())
    {
        pto.remove();
    }

    d->cpFindPtoUrl.clear();
}

QUrl& Manager::cpCleanPtoUrl() const
{
    return d->cpCleanPtoUrl;
}

QSharedPointer<PTOType> Manager::cpCleanPtoData()
{
    if (d->cpCleanPtoData.isNull())
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->cpCleanPtoUrl.toLocalFile());
        d->cpCleanPtoData = QSharedPointer<PTOType>(file.getPTO());

        if (d->cpCleanPtoData.isNull())
            d->cpCleanPtoData = QSharedPointer<PTOType>(new PTOType(cpFindBinary().version()));
    }

    return d->cpCleanPtoData;
}

void Manager::resetCpCleanPto()
{
    d->cpCleanPtoData.clear();

    QFile pto(d->cpCleanPtoUrl.toLocalFile());

    if (pto.exists())
    {
        pto.remove();
    }

    d->cpCleanPtoUrl.clear();
}

QUrl& Manager::autoOptimisePtoUrl() const
{
    return d->autoOptimisePtoUrl;
}

QSharedPointer<PTOType> Manager::autoOptimisePtoData()
{
    if (d->autoOptimisePtoData.isNull())
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->autoOptimisePtoUrl.toLocalFile());
        d->autoOptimisePtoData = QSharedPointer<PTOType>(file.getPTO());

        if (d->autoOptimisePtoData.isNull())
            d->autoOptimisePtoData = QSharedPointer<PTOType>(new PTOType(cpFindBinary().version()));
    }

    return d->autoOptimisePtoData;
}

void Manager::resetAutoOptimisePto()
{
    d->autoOptimisePtoData.clear();

    QFile pto(d->autoOptimisePtoUrl.toLocalFile());

    if (pto.exists())
    {
        pto.remove();
    }

    d->autoOptimisePtoUrl.clear();
}

QUrl& Manager::viewAndCropOptimisePtoUrl() const
{
    return d->viewAndCropOptimisePtoUrl;
}

QSharedPointer<PTOType> Manager::viewAndCropOptimisePtoData()
{
    if (d->viewAndCropOptimisePtoData.isNull())
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->viewAndCropOptimisePtoUrl.toLocalFile());
        d->viewAndCropOptimisePtoData = QSharedPointer<PTOType>(file.getPTO());

        if (d->viewAndCropOptimisePtoData.isNull())
            d->viewAndCropOptimisePtoData = QSharedPointer<PTOType>(new PTOType(cpFindBinary().version()));
    }

    return d->viewAndCropOptimisePtoData;
}

void Manager::resetViewAndCropOptimisePto()
{
    d->viewAndCropOptimisePtoData.clear();

    QFile pto(d->viewAndCropOptimisePtoUrl.toLocalFile());

    if (pto.exists())
    {
        pto.remove();
    }

    d->viewAndCropOptimisePtoUrl.clear();
}

QUrl& Manager::previewPtoUrl() const
{
    return d->previewPtoUrl;
}

QSharedPointer<PTOType> Manager::previewPtoData()
{
    if (d->previewPtoData.isNull())
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->previewPtoUrl.toLocalFile());
        d->previewPtoData = QSharedPointer<PTOType>(file.getPTO());

        if (d->previewPtoData.isNull())
            d->previewPtoData = QSharedPointer<PTOType>(new PTOType(cpFindBinary().version()));
    }

    return d->previewPtoData;
}

void Manager::resetPreviewPto()
{
    d->previewPtoData.clear();

    QFile pto(d->previewPtoUrl.toLocalFile());

    if (pto.exists())
    {
        pto.remove();
    }

    d->previewPtoUrl.clear();
}

QUrl& Manager::panoPtoUrl() const
{
    return d->panoPtoUrl;
}

QSharedPointer<PTOType> Manager::panoPtoData()
{
    if (d->panoPtoData.isNull())
    {
        PTOFile file(cpFindBinary().version());
        file.openFile(d->panoPtoUrl.toLocalFile());
        d->panoPtoData = QSharedPointer<PTOType>(file.getPTO());

        if (d->panoPtoData.isNull())
            d->panoPtoData = QSharedPointer<PTOType>(new PTOType(cpFindBinary().version()));
    }

    return d->panoPtoData;
}

void Manager::resetPanoPto()
{
    d->panoPtoData.clear();

    QFile pto(d->panoPtoUrl.toLocalFile());

    if (pto.exists())
    {
        pto.remove();
    }

    d->panoPtoUrl.clear();
}

QUrl& Manager::previewMkUrl() const
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

    d->previewMkUrl.clear();
}

QUrl& Manager::previewUrl() const
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

    d->previewUrl.clear();
}

QUrl& Manager::mkUrl() const
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

    d->mkUrl.clear();
}

QUrl& Manager::panoUrl() const
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

    d->panoUrl.clear();
}

void Manager::setIface(Interface* const iface)
{
    d->iface = iface;
}

Interface* Manager::iface() const
{
    return d->iface;
}

void Manager::setItemsList(const QList<QUrl>& urls)
{
    d->inputUrls = urls;
}

QList<QUrl>& Manager::itemsList() const
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
