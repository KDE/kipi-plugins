/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-01-31
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2006-2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef PLUGIN_SLIDESHOW_H
#define PLUGIN_SLIDESHOW_H

// KDE includes.

#include <kurl.h>

// LibKIPI includes.

#include <libkipi/plugin.h>
#include <libkipi/interface.h>

// Local includes.

#include "common.h"

class KAction;

namespace KIPISlideShowPlugin
{
    class SharedData;
}

class Plugin_SlideShow : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_SlideShow(QObject *parent, const QVariantList &args);
    ~Plugin_SlideShow();

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* );

public slots:

    void slotActivate();

private slots:

    void slotAlbumChanged(bool anyAlbum);
    void slotSlideShow();

private:

    KAction*                         m_actionSlideShow;
    KIPI::Interface*                 m_interface;
    KUrl::List*                      m_urlList;

    KIPISlideShowPlugin::SharedData* m_sharedData;
};

#endif  // PLUGIN_SLIDESHOW_H
