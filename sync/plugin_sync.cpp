/* ============================================================
 * File  : plugin_sync.cpp
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

// KDE includes.
#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kiconloader.h>

// libkipi includes.
#include <libkipi/interface.h>

// Local includes.
#include "plugin_sync.h"
#include "sinks.h"
//#include "sinklist.h"
//#include "gallerywindow.h"
//#include "galleryconfig.h"

typedef KGenericFactory<Plugin_Sync> Factory;

K_EXPORT_COMPONENT_FACTORY(kipiplugin_sync,
                           Factory("kipiplugin_sync"))


Plugin_Sync::Plugin_Sync(QObject *parent,
                         const char*,
                         const QStringList&)
  : KIPI::Plugin(Factory::instance(), parent, "Sync"),
      mpSinks(NULL)
{
  kdDebug(51001) << "Plugin_Sync plugin loaded"
                 << endl;
}


void Plugin_Sync::setup(QWidget* widget)
{
  // Create a collection to store the various Sinks a user can
  // define.
  mpSinks = new KIPISyncPlugin::Sinks();

  // Standard initialisation for Kipi Plugins
  KIPI::Plugin::setup(widget);
  KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

  if (!interface) 
  {
    kdError(51000) << "KIPI interface is null!" << endl;
    return;
  }

  // Add our directory in to the icon loader dirs.
  KGlobal::iconLoader()->addAppDir("kipiplugin_sync");

  mpActionSync = new KAction(i18n("Synchronize..."),
    0,
    this,
    SLOT(slotSync()),
    actionCollection(),
    "sync");
  mpActionSync->setEnabled(true);
  addAction(mpActionSync);

  mpActionConfigure = new KAction(i18n("Configure Synchronization..."),
    0,
    this,
    SLOT(slotConfigure()),
    actionCollection(),
    "sync");
  mpActionConfigure->setEnabled(true);
  addAction(mpActionConfigure);

  mpActionSettingsCollection = new KAction(i18n("Synchronization Settings..."),
    0,
    this,
    SLOT(slotSettingsCollection()),
    actionCollection(),
    "sync");
  mpActionSettingsCollection->setEnabled(true);
  addAction(mpActionSettingsCollection);

  mpActionSettingsImage = new KAction(i18n("Synchronization Settings..."),
    0,
    this,
    SLOT(slotSettingsImage()),
    actionCollection(),
    "sync");
  mpActionSettingsImage->setEnabled(true);
  addAction(mpActionSettingsImage);
}


Plugin_Sync::~Plugin_Sync()
{
  // Tidy up.
  if (mpSinks)
    delete mpSinks;
}


void Plugin_Sync::slotSync()
{
  KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
  if (!interface) 
  {
    kdError( 51000 ) << "Kipi interface is null!" << endl;
    return;
  }
/*
  KIPISyncPlugin::GalleryWindow dlg(interface, kapp->activeWindow(), mpSinks);
  dlg.exec();
*/
}


void Plugin_Sync::slotConfigure()
{
  KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
  if (!interface) 
  {
    kdError(51000) << "Kipi interface is null!" << endl;
    return;
  }
/*
  KIPISyncPlugin::SinkList dlg(kapp->activeWindow(), mpSinks, false);
  dlg.exec();
*/
}


void Plugin_Sync::slotSettingsCollection()
{
  KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
  if (!interface) 
  {
    kdError(51000) << "Kipi interface is null!" << endl;
    return;
  }

  KMessageBox::error(kapp->activeWindow(), "Not Implemented Yet!");
}


void Plugin_Sync::slotSettingsImage()
{
  KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());
  if (!interface) 
  {
    kdError(51000) << "Kipi interface is null!" << endl;
    return;
  }

  KMessageBox::error(kapp->activeWindow(), "Not Implemented Yet!");
}


KIPI::Category Plugin_Sync::category(KAction* pAction) const
{
  if (pAction == mpActionSync)
    return KIPI::EXPORTPLUGIN;
  if (pAction == mpActionConfigure)
    return KIPI::TOOLSPLUGIN;
  if (pAction == mpActionSettingsCollection)
    return KIPI::COLLECTIONSPLUGIN;
  if (pAction == mpActionSettingsImage)
    return KIPI::IMAGESPLUGIN;
     
  kdWarning(51000) << "Unrecognized action for plugin category identification"
                   << endl;
  return KIPI::EXPORTPLUGIN;
}

#include "plugin_sync.moc"
