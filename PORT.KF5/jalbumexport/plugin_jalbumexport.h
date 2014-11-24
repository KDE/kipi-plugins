/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through jAlbum
 *
 * Copyright (C) 2013 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

#ifndef PLUGIN_JALBUMEXPORT_H
#define PLUGIN_JALBUMEXPORT_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

using namespace KIPI;

namespace KIPIJAlbumExportPlugin
{

class Plugin_JAlbumExport : public Plugin
{
    Q_OBJECT

public:

    Plugin_JAlbumExport(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_JAlbumExport();

    void setup(QWidget* const);

private Q_SLOTS:

    void slotExport();

private:

    void setupActions();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPIJAlbumExportPlugin

#endif // PLUGIN_JALBUMEXPORT_H
