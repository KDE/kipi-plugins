/* ============================================================
 * File  : plugin_helloworld.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-03-10
 * Description :
 *
 * Copyright 2003 by Renchi Raju
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
 * ============================================================ */

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

#include <digikam/albummanager.h>
#include <digikam/albuminfo.h>

#include "plugin_helloworld.h"
#include <libkipi/imagecollection.h>

// A macro from KDE KParts to export the symbols for this plugin and
// create the factory for it. The first argument is the name of the
// plugin library and the second is the genericfactory templated from
// the class for your plugin
K_EXPORT_COMPONENT_FACTORY( kipiplugin_helloworld,
                            KGenericFactory<Plugin_HelloWorld>("kipiplugin_helloworld"));

Plugin_HelloWorld::Plugin_HelloWorld(QObject *parent,
                                     const char*,
                                     const QStringList&)
    : KIPI::Plugin(parent, "HelloWorld")
{
#ifdef TEMPORARILY_REMOVED
    // Set the instance of this to generic factory instance
    // created using the macro above
    setInstance(KGenericFactory<Plugin_HelloWorld>::instance());

    // Set the xml file which will be merged with the mainwindow's gui
    setXMLFile("plugins/digikamplugin_helloworld.rc");
#endif

    // Insert our translations into the global catalogue
    KGlobal::locale()->insertCatalogue("digikamplugin_helloworld");

    kdDebug() << "Plugin_HelloWorld plugin loaded" << endl;

    // this is our action shown in the menubar/toolbar of the mainwindow
    (void) new KAction (i18n("Hello World..."),
                        "misc",
                        0,	// or a shortcut like CTRL+SHIFT+Key_S,
                        this,
                        SLOT(slotActivate()),
                        actionCollection(),
                        "helloworld");

    m_interface = dynamic_cast< KIPI::Interface* >( parent );
}

void Plugin_HelloWorld::slotActivate()
{
    kdDebug() << "Plugin_HelloWorld slot activated" << endl;

    // Get the current/selected album
    KIPI::ImageCollection album = m_interface->currentAlbum();


    // Now how do we check if there was an album selected?
#ifdef TEMPORARILY_REMOVED
    // Make sure to check that we have a selected album
    if (!album) return;
#endif

    // Now get some properties of the album
    kdDebug() << "The current album title is " << album.name() << endl;
#ifdef TEMPORARILY_REMOVED
    kdDebug() << "The current album collection is " << album.getCollection() << endl;
    kdDebug() << "The current album date is " << album.date().toString() << endl;
#endif

    // see the comments in the album

#ifdef TEMPORARILY_REMOVED
    // First open the album database
    album->openDB();

    // get the comments for this particular item
    kdDebug() << album->getItemComments("IMG_100.JPG") << endl;

    // Close the album database once you are done
    album->closeDB();

    // Get all the Albums in the current library path
    for (Digikam::AlbumInfo *a = Digikam::AlbumManager::instance()->firstAlbum();
         a; a = a->nextAlbum()) {
        kdDebug() << "Album title: " << a->getTitle() << endl;
    }
#endif
}

KIPI::Category Plugin_HelloWorld::category() const
{
    return KIPI::IMAGESPLUGIN;
}

