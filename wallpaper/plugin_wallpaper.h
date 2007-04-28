/* ============================================================
 * File   : plugin_wallpaper.h
 *
 * Authors: Gregory KOKANOSKY <gregory dot kokanosky at free.fr>
 *          Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Date   : 01/2004
 *
 * Description : Wall Paper plugin parts for KII
 *
 * Copyright 2004 by Gregory KOKANOSKY <gregory dot kokanosky at free.fr>
 * Copyright 2004 by Gilles CAULIER <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

  #ifndef PLUGIN_IMAGES2DESKTOP_H
  #define PLUGIN_IMAGES2DESKTOP_H

  // Include files for KDE

  #include <kprocess.h>

  // KIPI includes

  #include <libkipi/plugin.h>

  class KActionMenu;
  class KAction;

  class Plugin_WallPaper : public KIPI::Plugin
  {
  Q_OBJECT

   public:

   Plugin_WallPaper(QObject *parent,
                      const char* name,
                      const QStringList &args);
   virtual KIPI::Category category( KAction* action ) const;
   virtual void setup( QWidget* );

   private slots:

   void slotSetCenter();
   void slotSetTiled();
   void slotSetCenterTiled();
   void slotSetCenteredMaxpect();
   void slotSetTiledMaxpect();
   void slotSetScaled();
   void slotSetCenteredAutoFit();
   void slotSetScaleAndCrop();

   private:

   enum {
        CENTER = 1,
        TILED = 2,
        CENTER_TILED = 3,
        CENTER_MAXPECT = 4,
        TILED_MAXPECT = 5,
        SCALED = 6,
        CENTERED_AUTOFIT = 7,
	SCALE_AND_CROP = 8
        };

   KActionMenu    *m_action_Background;
   void setWallpaper( int layout );
  };

  #endif // PLUGIN_IMAGES2DESKTOP_H
