/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-17-06
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_FLICKREXPORT_H
#define PLUGIN_FLICKREXPORT_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPIFlickrExportPlugin
{
    class FlickrWindow;
}

class Plugin_FlickrExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_FlickrExport( QObject *parent, const QVariantList &args);
    ~Plugin_FlickrExport();

    KIPI::Category category(KAction* action) const;
    void setup(QWidget*);

public Q_SLOTS:

    void slotActivateFlickr();
    void slotActivate23();
    void slotActivateZooomr();

private:

    KAction                              *m_actionFlickr;
    KAction                              *m_action23;
    KAction                              *m_actionZooomr;

    KIPIFlickrExportPlugin::FlickrWindow *m_dlgFlickr;
    KIPIFlickrExportPlugin::FlickrWindow *m_dlg23;
    KIPIFlickrExportPlugin::FlickrWindow *m_dlgZooomr;
};

#endif // PLUGIN_FLICKREXPORT_H
