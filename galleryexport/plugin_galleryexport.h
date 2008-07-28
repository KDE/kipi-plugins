/* ============================================================
 * File  : plugin_galleryexport.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-06
 * Description :
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
 *
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_GALLERYEXPORT_H
#define PLUGIN_GALLERYEXPORT_H

// libKIPI includes.

#include <libkipi/plugin.h>

class KAction;

namespace KIPIGalleryExportPlugin
{
class Galleries;
}

class Plugin_GalleryExport : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_GalleryExport(QObject *parent,
                         const QVariantList &args);
    ~Plugin_GalleryExport();

    virtual KIPI::Category category(KAction* action) const;
    virtual void setup(QWidget*);

public slots:

    void slotSync();
    void slotConfigure();
//     void slotCollectionSettings();
//     void slotImageSettings();

private:

    KAction *m_action_sync;
    KAction *m_action_configure;
//     KAction *m_action_collection_settings;
//     KAction *m_action_image_setting;

    KIPIGalleryExportPlugin::Galleries* mpGalleries;
};

#endif
