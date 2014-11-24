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

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KDcrawIface;

namespace KIPIExpoBlendingPlugin
{

class ActionThread;
class ExpoBlendingAboutData;
class AlignBinary;
class EnfuseBinary;
class ManagerPriv;

class Manager : public QObject
{
    Q_OBJECT

public:

    explicit Manager(QObject* const parent = 0);
    ~Manager();

    bool checkBinaries();

    void setIface(Interface* const iface);
    Interface* iface() const;

    void setItemsList(const KUrl::List& urls);
    KUrl::List itemsList() const;

    void setRawDecodingSettings(const RawDecodingSettings& settings);
    RawDecodingSettings rawDecodingSettings() const;

    void setPreProcessedMap(const ItemUrlsMap& urls);
    ItemUrlsMap preProcessedMap() const;

    ActionThread* thread() const;
    AlignBinary&  alignBinary() const;
    EnfuseBinary& enfuseBinary() const;

    void run();

    /**
     * Clean up all temporary files produced so far.
     */
    void cleanUp();

private Q_SLOTS:

    void slotStartDialog();
    void slotSetEnfuseVersion(double version);

private:

    void startWizard();

private:

    class ManagerPriv;
    ManagerPriv* const d;
};

} // namespace KIPIExpoBlendingPlugin

#endif /* MANAGER_H */
