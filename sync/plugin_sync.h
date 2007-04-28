/* ============================================================
 * File  : plugin_sync.h
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2007-01-14
 *
 * Copyright 2007 by Colin Guthrie <kde@colin.guthr.ie>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_SYNC_H
#define PLUGIN_SYNC_H

// libKIPI includes.

#include <libkipi/plugin.h>

class KAction;

// Let this header know this exists (no need to load full definition)
namespace KIPISyncPlugin
{
class Sinks;
}

class Plugin_Sync : public KIPI::Plugin
{
  Q_OBJECT

public:

  Plugin_Sync(QObject* pParent,
              const char* pName,
              const QStringList &rArgs);
  ~Plugin_Sync();
    
  virtual KIPI::Category category(KAction* pAction) const;
  virtual void setup(QWidget* pWidget);

public slots:

  void slotSync();
  void slotConfigure();

  void slotSettingsCollection();
  void slotSettingsImage();

private:

  KIPISyncPlugin::Sinks* mpSinks;

  KAction* mpActionSync;
  KAction* mpActionConfigure;

  KAction* mpActionSettingsCollection;
  KAction* mpActionSettingsImage;
};

#endif
