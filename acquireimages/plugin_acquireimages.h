//////////////////////////////////////////////////////////////////////////////
//
//    plugin_acquireimages.h
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier.gilles at free.fr>
//
//    Description : KIPI Acquire Images Plugin.
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_ACQUIREIMAGES_H
#define PLUGIN_ACQUIREIMAGES_H

// LibKIPI includes.

#include <libkipi/plugin.h>

class KScanDialog;
class KAction;
class KActionMenu;

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
  KActionMenu        *m_action_acquire;
  KAction            *m_action_scanimages;
  KAction            *m_action_screenshotimages;
  KScanDialog        *m_scanDialog;
  AcquireImageDialog *m_acquireImageDialog;
  ScreenGrabDialog   *m_screenshotDialog;
};


#endif /* PLUGIN_ACQUIREIMAGES_H */
