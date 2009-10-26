/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_GALLERYEXPORT_H
#define PLUGIN_GALLERYEXPORT_H

// Qt includes

#include <QVariantList>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPIGalleryExportPlugin
{
class Gallery;
}

class Plugin_GalleryExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_GalleryExport(QObject *parent, const QVariantList &args);
    ~Plugin_GalleryExport();

    virtual KIPI::Category category(KAction* action) const;
    virtual void setup(QWidget*);

public Q_SLOTS:

    void slotSync();

private:

    KAction                          *m_action;

    KIPIGalleryExportPlugin::Gallery *mpGallery;
};

#endif /* PLUGIN_GALLERYEXPORT_H */
