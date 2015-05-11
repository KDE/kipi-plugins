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

// KDE includes

#include <kurl.h>

// Libkdcraw includes

#include <libkdcraw/rawdecodingsettings.h>

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

    void setItemsList(const KUrl::List& urls);
    KUrl::List& itemsList() const;

    KUrl&           basePtoUrl() const;
    const PTOType&  basePtoData();
    void            resetBasePto();
    KUrl&           cpFindPtoUrl() const;
    const PTOType&  cpFindPtoData();
    void            resetCpFindPto();
    KUrl&           cpCleanPtoUrl() const;
    const PTOType&  cpCleanPtoData();
    void            resetCpCleanPto();
    KUrl&           autoOptimisePtoUrl() const;
    const PTOType&  autoOptimisePtoData();
    void            resetAutoOptimisePto();
    KUrl&           viewAndCropOptimisePtoUrl() const;
    const PTOType&  viewAndCropOptimisePtoData();
    void            resetViewAndCropOptimisePto();
    KUrl&           previewPtoUrl() const;
    const PTOType&  previewPtoData();
    void            resetPreviewPto();
    KUrl&           panoPtoUrl() const;
    const PTOType&  panoPtoData();
    void            resetPanoPto();

    KUrl&           previewMkUrl() const;
    void            resetPreviewMkUrl();
    KUrl&           previewUrl() const;
    void            resetPreviewUrl();
    KUrl&           mkUrl() const;
    void            resetMkUrl();
    KUrl&           panoUrl() const;
    void            resetPanoUrl();

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
