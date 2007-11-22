/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to acquire images
 *
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_ACQUIREIMAGES_H
#define PLUGIN_ACQUIREIMAGES_H

// LibKIPI includes.

#include <libkipi/plugin.h>

class KScanDialog;
class KAction;

class AcquireImageDialog;
class ScreenGrabDialog;

class Plugin_AcquireImages : public KIPI::Plugin
{
    Q_OBJECT

public:

  Plugin_AcquireImages(QObject *parent, const char* name, const QStringList &args);
  virtual ~Plugin_AcquireImages();
  virtual KIPI::Category category( KAction* action ) const;
  virtual void setup( QWidget* widget );

public slots:

  void slotActivate();
  void slotAcquireImageDone(const QImage &img);

private:

  KAction                                     *m_action_scanimages;
  KAction                                     *m_action_screenshotimages;
  KScanDialog                                 *m_scanDialog;
  
  KIPIAcquireImagesPlugin::AcquireImageDialog *m_acquireImageDialog;
  KIPIAcquireImagesPlugin::ScreenGrabDialog   *m_screenshotDialog;
};

#endif /* PLUGIN_ACQUIREIMAGES_H */
