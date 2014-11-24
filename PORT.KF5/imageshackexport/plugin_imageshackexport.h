/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
*
* This program is free software; you can redistribute it
* and/or modify it under the terms of the GNU General
* Public License as published by the Free Software Foundation;
* either version 2, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* ============================================================ */

#ifndef PLUGIN_IMAGESHACKEXPORT_H
#define PLUGIN_IMAGESHACKEXPORT_H

// Qt includes

#include <QVariantList>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPIImageshackExportPlugin
{

class Imageshack;

class Plugin_ImageshackExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_ImageshackExport(QObject* const parent, const QVariantList& args);
    ~Plugin_ImageshackExport();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotExport();

private:

    void setupActions();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIImageshackExportPlugin

#endif /* PLUGIN_IMAGESHACKEXPORT_H */
