/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-01-01
 * Description : Wall Paper kipi-plugin
 *
 * Copyright (C) 2004      by Gregory Kokanosky <gregory dot kokanosky at free.fr>
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_WALLPAPER_H
#define PLUGIN_WALLPAPER_H

// LibKIPI includes

#include <libkipi/plugin.h>

class KActionMenu;
class KAction;

class Plugin_WallPaper : public KIPI::Plugin
{
Q_OBJECT

public:

    Plugin_WallPaper(QObject *parent, const QVariantList &args);
    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* );

private Q_SLOTS:

    void slotSetCenter();
    void slotSetTiled();
    void slotSetCenterTiled();
    void slotSetCenteredMaxpect();
    void slotSetTiledMaxpect();
    void slotSetScaled();
    void slotSetCenteredAutoFit();
    void slotSetScaleAndCrop();

private:

    void setWallpaper( int layout );

private:

    enum 
    {
        CENTER           = 1,
        TILED            = 2,
        CENTER_TILED     = 3,
        CENTER_MAXPECT   = 4,
        TILED_MAXPECT    = 5,
        SCALED           = 6,
        CENTERED_AUTOFIT = 7,
        SCALE_AND_CROP   = 8
    };

    KActionMenu *m_actionBackground;
};

#endif // PLUGIN_WALLPAPER_H
