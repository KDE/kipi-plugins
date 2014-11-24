/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-12-05
 * Description : a tool to export image to an Ipod device.
 *
 * Copyright (C) 2006-2008 by Seb Ruiz <ruiz at kde dot org>
 * Copyright (C) 2008-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_IPODEXPORT_H
#define PLUGIN_IPODEXPORT_H

// Qt includes

#include <QVariant>

// libkipi includes

#include <libkipi/plugin.h>

class KAction;

using namespace KIPI;

namespace KIPIIpodExportPlugin
{

class UploadDialog;

class Plugin_iPodExport : public Plugin
{
    Q_OBJECT

public:

    Plugin_iPodExport(QObject* const parent, const QVariantList& args);
    ~Plugin_iPodExport();

    void setup(QWidget* const widget);

private Q_SLOTS:

    void slotImageUpload();

private:

    void setupActions();

private:

    KAction*      m_actionImageUpload;
    UploadDialog* m_dlgImageUpload;
};

} // namespace KIPIIpodExportPlugin

#endif // PLUGIN_IPODEXPORT_H
