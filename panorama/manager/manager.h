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

#ifndef MANAGER_H
#define MANAGER_H

// Qt includes

#include <QObject>

// Libkdcraw includes

#include <rawdecodingsettings.h>

// Local includes

#include "actions.h"
#include "actionthread.h"
#include "ptotype/ptotype.h"

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

class ActionThread;
class PanoramaAboutData;
class AutoOptimiserBinary;
class CPCleanBinary;
class CPFindBinary;
class EnblendBinary;
class MakeBinary;
class NonaBinary;
class PanoModifyBinary;
class Pto2MkBinary;

class Manager : public QObject
{
    Q_OBJECT

public:

    explicit Manager(QObject* const parent = 0);
    ~Manager();

    bool checkBinaries();

    void setGPano(bool gPano);
    bool gPano() const;

//     void setHDR(bool hdr);
//     bool hdr() const;

    void setFileFormatJPEG();
    void setFileFormatTIFF();
    void setFileFormatHDR();
    PanoramaFileType format() const;

    void setIface(Interface* const iface);
    Interface* iface() const;

    void setItemsList(const QList<QUrl>& urls);
    QList<QUrl>& itemsList() const;

    QUrl&                   basePtoUrl() const;
    QSharedPointer<PTOType> basePtoData();
    void                    resetBasePto();
    QUrl&                   cpFindPtoUrl() const;
    QSharedPointer<PTOType> cpFindPtoData();
    void                    resetCpFindPto();
    QUrl&                   cpCleanPtoUrl() const;
    QSharedPointer<PTOType> cpCleanPtoData();
    void                    resetCpCleanPto();
    QUrl&                   autoOptimisePtoUrl() const;
    QSharedPointer<PTOType> autoOptimisePtoData();
    void                    resetAutoOptimisePto();
    QUrl&                   viewAndCropOptimisePtoUrl() const;
    QSharedPointer<PTOType> viewAndCropOptimisePtoData();
    void                    resetViewAndCropOptimisePto();
    QUrl&                   previewPtoUrl() const;
    QSharedPointer<PTOType> previewPtoData();
    void                    resetPreviewPto();
    QUrl&                   panoPtoUrl() const;
    QSharedPointer<PTOType> panoPtoData();
    void                    resetPanoPto();

    QUrl&                   previewMkUrl() const;
    void                    resetPreviewMkUrl();
    QUrl&                   previewUrl() const;
    void                    resetPreviewUrl();
    QUrl&                   mkUrl() const;
    void                    resetMkUrl();
    QUrl&                   panoUrl() const;
    void                    resetPanoUrl();

    void setRawDecodingSettings(const RawDecodingSettings& settings);
    RawDecodingSettings& rawDecodingSettings() const;

    ItemUrlsMap&            preProcessedMap()     const;
    ActionThread*           thread()              const;
    AutoOptimiserBinary&    autoOptimiserBinary() const;
    CPCleanBinary&          cpCleanBinary()       const;
    CPFindBinary&           cpFindBinary()        const;
    EnblendBinary&          enblendBinary()       const;
    MakeBinary&             makeBinary()          const;
    NonaBinary&             nonaBinary()          const;
    PanoModifyBinary&       panoModifyBinary()    const;
    Pto2MkBinary&           pto2MkBinary()        const;

    void run();

private Q_SLOTS:

    void setPreProcessedMap(const ItemUrlsMap& urls);

private:

    void startWizard();

private:

    struct Private;
    Private* const d;
};

} // namespace KIPIPanoramaPlugin

#endif /* MANAGER_H */
