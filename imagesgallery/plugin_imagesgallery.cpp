/* ============================================================
 * File  : plugin_imagesgallery.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-05
 * Description : Images gallery HTML export Digikam plugin
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

// Include files for KDE

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>

// Include files for Digikam

#include <digikam/albummanager.h>
#include <digikam/albuminfo.h>

// Local include files

#include "imagesgallery.h"
#include "plugin_imagesgallery.h"


K_EXPORT_COMPONENT_FACTORY( kipiplugin_imagesgallery,
                            KGenericFactory<Plugin_Imagesgallery>("kipiplugin_imagesgallery"));

// -----------------------------------------------------------
Plugin_Imagesgallery::Plugin_Imagesgallery(QObject *parent, const char*, const QStringList&)
            : KIPI::Plugin(parent, "ImagesGallery")
{
    KGlobal::locale()->insertCatalogue("kipiplugin_imagesgallery");
    kdDebug() << "Plugin_Imagesgallery plugin loaded" << endl;

    (void) new KAction (i18n("HTML export..."),        // Menu message.
                        "www",                         // Menu icon.
                        0,
                        this,
                        SLOT(slotActivate()),
                        actionCollection(),
                        "images_gallery");

}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_Imagesgallery::~Plugin_Imagesgallery()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_Imagesgallery::slotActivate()
{
    ImagesGallery Gallery( dynamic_cast< KIPI::Interface* >( parent() ) );
}

