/* ============================================================
 * File  : plugin_sendimages.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-11-04
 * Description : Digikam Send Mail Images Plugin.
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

 // C Ansi includes
 
extern "C"
{
#include <unistd.h>
}

// Include files for Qt

#include <qimage.h>

// Include files for KDE

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
#include <kscan.h>
#include <kimageio.h>

// Digikam includes

#include <digikam/albummanager.h>
#include <digikam/albuminfo.h>

// Local includes

#include "sendimagesdialog.h"
#include "plugin_sendimages.h"


K_EXPORT_COMPONENT_FACTORY( digikamplugin_sendimages,
                            KGenericFactory<Plugin_SendImages>("digikam"));

// -----------------------------------------------------------
Plugin_SendImages::Plugin_SendImages(QObject *parent, const char*, const QStringList&)
                 : Digikam::Plugin(parent, "SendImages")
{
    setInstance(KGenericFactory<Plugin_SendImages>::instance());
    setXMLFile("plugins/digikamplugin_sendimages.rc");
    KGlobal::locale()->insertCatalogue("digikamplugin_sendimages");
    kdDebug() << "Plugin_SendImages plugin loaded" << endl;

    m_action_sendimages = new KAction (i18n("E-mail Images..."),     // Menu message.
                                        "mail_new",                  // Menu icon.
                                        0,
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "send_images");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_SendImages::~Plugin_SendImages()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_SendImages::slotActivate()
{
    QStringList imagesfileList;
    Digikam::AlbumInfo *album = Digikam::AlbumManager::instance()->currentAlbum();

    if (album)
        {
        if (album->getSelectedItemsPath().isEmpty())
             imagesfileList = album->getAllItemsPath();
        else
             imagesfileList = album->getSelectedItemsPath();
        }

    KStandardDirs dir;
    QString Tmp = dir.saveLocation("tmp", "digikam-sendimages-" + QString::number(getpid()) + "/");

    m_sendImagesDialog = new SendImagesDialog(0, Tmp, imagesfileList);
    m_sendImagesDialog->show();
}
