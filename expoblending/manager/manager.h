/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
class ManagerPriv;

class Manager : public QObject
{
    Q_OBJECT

public:

    Manager(QObject* parent=0);
    ~Manager();

    bool checkBinaries();

    void setAbout(ExpoBlendingAboutData* about);
    ExpoBlendingAboutData* about() const;

    void setIface(Interface* iface);
    Interface* iface() const;

    void setItemsList(const KUrl::List& urls);
    KUrl::List itemsList() const;

    void setRawDecodingSettings(const RawDecodingSettings& settings);
    RawDecodingSettings rawDecodingSettings() const;

    void setAlignedMap(const ItemUrlsMap& urls);
    ItemUrlsMap alignedMap() const;

    ActionThread* thread() const;

    void run();

private:

    void startImportWizard();
    void startExpoBlendingDlg();

private:

    ManagerPriv* const d;
};

} // namespace KIPIExpoBlendingPlugin

#endif /* MANAGER_H */
