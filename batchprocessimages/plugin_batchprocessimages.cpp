//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_BATCHPROCESSIMAGES.CPP
//
//    Copyright (C) 2003-2004 Gilles CAULIER <caulier dot gilles at free.fr>
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

// C Ansi includes

extern "C"
{
#include <unistd.h>
}

// Qt Includes

#include <qimage.h>

// KDE includes

#include <klocale.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ktempfile.h>

// Digikam includes

#include <digikam/albummanager.h>
#include <digikam/albuminfo.h>

// Local includes

#include "plugin_batchprocessimages.h"
#include "borderimagesdialog.h"
#include "colorimagesdialog.h"
#include "convertimagesdialog.h"
#include "effectimagesdialog.h"
#include "filterimagesdialog.h"
#include "renameimagesdialog.h"
#include "recompressimagesdialog.h"
#include "resizeimagesdialog.h"

K_EXPORT_COMPONENT_FACTORY( digikamplugin_batchprocessimages,
                            KGenericFactory<Plugin_BatchProcessImages>("digikam"));

// -----------------------------------------------------------
Plugin_BatchProcessImages::Plugin_BatchProcessImages(QObject *parent, const char*, const QStringList&)
            : Digikam::Plugin(parent, "BatchProcessImages")
{
    setInstance(KGenericFactory<Plugin_BatchProcessImages>::instance());
    setXMLFile("plugins/digikamplugin_batchprocessimages.rc");
    KGlobal::locale()->insertCatalogue("digikamplugin_batchprocessimages");
    kdDebug( 51001 ) << "Plugin_BatchProcessImages plugin loaded" << endl;

    m_action_borderimages = new KAction (i18n("Border Images..."),           // Menu message.
                                        "borderimages",                      // Menu icon.
                                        0,                                   // default shortcut.
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "batch_border_images");

    m_action_colorimages = new KAction (i18n("Color Images..."),             // Menu message.
                                        "colorimages",                       // Menu icon.
                                        0,                                   // default shortcut.
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "batch_color_images");

    m_action_convertimages = new KAction (i18n("Convert Images..."),         // Menu message.
                                        "convertimages",                     // Menu icon.
                                        0,                                   // default shortcut.
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "batch_convert_images");

    m_action_effectimages = new KAction (i18n("Effect Images..."),           // Menu message.
                                        "effectimages",                      // Menu icon.
                                        0,                                   // default shortcut.
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "batch_effect_images");

    m_action_filterimages = new KAction (i18n("Filter Images..."),           // Menu message.
                                        "filterimages",                      // Menu icon.
                                        0,                                   // default shortcut.
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "batch_filter_images");

    m_action_renameimages = new KAction (i18n("Rename Images..."),           // Menu message.
                                        "renameimages",                      // Menu icon.
                                        0,                                   // default shortcut.
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "batch_rename_images");

    m_action_recompressimages = new KAction (i18n("Recompress Images..."),   // Menu message.
                                        "recompressimages",                  // Menu icon.
                                        0,                                   // default shortcut.
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "batch_recompress_images");

    m_action_resizeimages = new KAction (i18n("Resize Images..."),           // Menu message.
                                        "resizeimages",                      // Menu icon.
                                        0,                                   // default shortcut.
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "batch_resize_images");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_BatchProcessImages::~Plugin_BatchProcessImages()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_BatchProcessImages::slotActivate()
{
    Digikam::AlbumInfo *album = Digikam::AlbumManager::instance()->currentAlbum();
    QStringList imagesfileList;

    if ( album )
       {
       imagesfileList = album->getSelectedItemsPath();

       if (imagesfileList.isEmpty() == true)
          imagesfileList = album->getAllItemsPath();
       }

    QString from(sender()->name());

    if (from == "batch_convert_images")
        {
        m_ConvertImagesDialog = new ConvertImagesDialog(0, imagesfileList);
        m_ConvertImagesDialog->show();
        }
    else if (from == "batch_rename_images")
        {
        m_RenameImagesDialog = new RenameImagesDialog(0, imagesfileList);
        m_RenameImagesDialog->show();
        }
    else if (from == "batch_border_images")
        {
        m_BorderImagesDialog = new BorderImagesDialog(0, imagesfileList);
        m_BorderImagesDialog->show();
        }
    else if (from == "batch_color_images")
        {
        m_ColorImagesDialog = new ColorImagesDialog(0, imagesfileList);
        m_ColorImagesDialog->show();
        }
    else if (from == "batch_filter_images")
        {
        m_FilterImagesDialog = new FilterImagesDialog(0, imagesfileList);
        m_FilterImagesDialog->show();
        }
    else if (from == "batch_effect_images")
        {
        m_EffectImagesDialog = new EffectImagesDialog(0, imagesfileList);
        m_EffectImagesDialog->show();
        }
    else if (from == "batch_recompress_images")
        {
        m_RecompressImagesDialog = new RecompressImagesDialog(0, imagesfileList);
        m_RecompressImagesDialog->show();
        }
    else if (from == "batch_resize_images")
        {
        m_ResizeImagesDialog = new ResizeImagesDialog(0, imagesfileList);
        m_ResizeImagesDialog->show();
        }
    else
        {
        kdWarning( 51000 ) << "The impossible happened... unknown batch action specified" << endl;
        return;
        }
}


