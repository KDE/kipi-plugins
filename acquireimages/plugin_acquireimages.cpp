//////////////////////////////////////////////////////////////////////////////
//
//    plugin_acquireimages.cpp
//
//    Copyright (C) 2003-2004 Gilles CAULIER <caulier dot gilles at free.fr>
//
//    Description : Digikam Acquire Images Plugin.
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

// Include files for Qt

#include <qimage.h>

// Include files for KDE

#include <klocale.h>
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

// Local includes

#include "plugin_acquireimages.h"
#include "screenshotdialog.h"
#include "acquireimagedialog.h"


K_EXPORT_COMPONENT_FACTORY( kipiplugin_acquireimages,
                            KGenericFactory<Plugin_AcquireImages>("kipiplugin_acquireimages"));

// -----------------------------------------------------------
Plugin_AcquireImages::Plugin_AcquireImages(QObject *parent, const char*, const QStringList&)
            : KIPI::Plugin(parent, "AcquireImages")
{
    KGlobal::locale()->insertCatalogue("kipiplugin_acquireimages");
    kdDebug() << "Plugin_AcquireImages plugin loaded" << endl;


    m_action_acquire = new KActionMenu(i18n("&Acquire images"),
                                             actionCollection(),
                                             "acquire");

    m_action_scanimages = new KAction (i18n("Scan Images..."),         // Menu message.
                                            "scanner",                 // Menu icon.
                                            0,                         // default shortcut.
                                            this,
                                            SLOT(slotActivate()),
                                            m_action_acquire,
                                            "scan_images");

    m_action_acquire->insert(m_action_scanimages);

    m_action_screenshotimages =  new KAction (i18n("Screenshot..."),   // Menu message.
                                            "ksnapshot",               // Menu icon.
                                            0,                         // default shortcut.
                                            this,
                                            SLOT(slotActivate()),
                                            m_action_acquire,
                                            "screenshot_images");

    m_action_acquire->insert(m_action_screenshotimages);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_AcquireImages::~Plugin_AcquireImages()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_AcquireImages::slotActivate()
{
    QString from(sender()->name());

    if (from == "scan_images")
       {
       m_scanDialog = KScanDialog::getScanDialog(0, i18n("Digikam Scan Images Plugin"));

       if ( m_scanDialog )
           {
           m_scanDialog->setMinimumSize(400, 300);
           connect(m_scanDialog, SIGNAL(finalImage(const QImage &, int)),
                   this, SLOT(slotAcquireImageDone(const QImage &)));
           }
       else
           {
           KMessageBox::sorry(0, i18n("No KDE scan-service available. Check your system!"),
                              i18n("Digikam Scan Images Plugin"));
           qDebug("No Scan-service available, aborting!");
           return;
           }

       if ( m_scanDialog->setup() )
           m_scanDialog->show();
       }
    else if (from == "screenshot_images")
       {
       m_screenshotDialog = new ScreenGrabDialog(0, "Digikam ScreenshotImagesDialog");
       m_screenshotDialog->show();
       }
    else
       {
       kdWarning() << "The impossible happened... unknown flip specified" << endl;
       return;
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_AcquireImages::slotAcquireImageDone(const QImage &img)
{
    m_acquireImageDialog = new AcquireImageDialog(0, img);
    m_acquireImageDialog->setMinimumWidth(400);
    m_acquireImageDialog->exec();
}

KIPI::Category Plugin_AcquireImages::category() const
{
    return KIPI::TOOLSPLUGIN;
}
