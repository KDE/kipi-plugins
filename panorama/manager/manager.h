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
class CPFindBinary;
class CPCleanBinary;
class AutoOptimiserBinary;

class Manager : public QObject
{
    Q_OBJECT

public:

    Manager(QObject* parent=0);
    ~Manager();

    bool checkBinaries();

    void setHDR(bool hdr);
    bool hdr() const;

    void setFileFormatJPEG();
    void setFileFormatTIFF();
    ActionThread::PanoramaFileType format() const;

    void setAbout(PanoramaAboutData* about);
    PanoramaAboutData* about() const;

    void setIface(Interface* iface);
    Interface* iface() const;

    void setItemsList(const KUrl::List& urls);
    KUrl::List itemsList() const;

    void setCPFindUrl(const KUrl& url);
    KUrl cpFindUrl() const;

    void setAutoOptimiseUrl(const KUrl& url);
    KUrl autoOptimiseUrl() const;

    void setPreviewUrl(const KUrl& url);
    KUrl previewUrl() const;

    void setRawDecodingSettings(const RawDecodingSettings& settings);
    RawDecodingSettings rawDecodingSettings() const;

    void setPreProcessedMap(const ItemUrlsMap& urls);
    ItemUrlsMap preProcessedMap() const;

    ActionThread*        thread() const;
    CPFindBinary&        cpFindBinary() const;
    CPCleanBinary&       cpCleanBinary() const;
    AutoOptimiserBinary& autoOptimiserBinary() const;

    void run();

    /**
     * Clean up all temporary files produced so far.
     */
    void cleanUp();

private:

    void startWizard();

private:

    struct ManagerPriv;
    ManagerPriv* const d;
};

} // namespace KIPIPanoramaPlugin

#endif /* MANAGER_H */
