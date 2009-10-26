/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-01-31
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2006-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at gmx dot net>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
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

#ifndef PLUGIN_ADVANCEDSLIDESHOW_H
#define PLUGIN_ADVANCEDSLIDESHOW_H

// KDE includes

#include <kurl.h>

// LibKIPI includes

#include <libkipi/plugin.h>
#include <libkipi/interface.h>

class KAction;

namespace KIPIAdvancedSlideshowPlugin
{
    class SharedContainer;
}

class Plugin_AdvancedSlideshow : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_AdvancedSlideshow(QObject *parent, const QVariantList& args);
    ~Plugin_AdvancedSlideshow();

    virtual KIPI::Category category(KAction* action) const;
    virtual void setup(QWidget*);

public Q_SLOTS:

    void slotActivate();

private Q_SLOTS:

    void slotAlbumChanged(bool anyAlbum);
    void slotSlideShow();

private:

    KAction*                                      m_actionSlideShow;
    KIPI::Interface*                              m_interface;
    KUrl::List                                    m_urlList;

    KIPIAdvancedSlideshowPlugin::SharedContainer* m_sharedData;
};

#endif  // PLUGIN_ADVANCEDSLIDESHOW_H
