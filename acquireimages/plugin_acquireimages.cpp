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
#include <kapplication.h>

// Local includes

#include "screenshotdialog.h"
#include "acquireimagedialog.h"
#include "plugin_acquireimages.h"
#include "plugin_acquireimages.moc"

typedef KGenericFactory<Plugin_AcquireImages> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_acquireimages,
                            Factory("kipiplugin_acquireimages"))

Plugin_AcquireImages::Plugin_AcquireImages(QObject *parent, const char*, const QStringList&)
            : KIPI::Plugin( Factory::instance(), parent, "AcquireImages")
{
    kdDebug( 51001 ) << "Plugin_AcquireImages plugin loaded" << endl;
}

void Plugin_AcquireImages::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_scanimages = new KAction (i18n("Scan Images..."),         // Menu message.
                                            "scanner",                 // Menu icon.
                                            0,                         // default shortcut.
                                            this,
                                            SLOT(slotActivate()),
                                            actionCollection(),
                                            "scan_images");

    m_action_screenshotimages =  new KAction (i18n("Screenshot..."),   // Menu message.
                                            "ksnapshot",               // Menu icon.
                                            0,                         // default shortcut.
                                            this,
                                            SLOT(slotActivate()),
                                            actionCollection(),
                                            "screenshot_images");

    addAction( m_action_scanimages );
    addAction( m_action_screenshotimages );
}

Plugin_AcquireImages::~Plugin_AcquireImages()
{
}

void Plugin_AcquireImages::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
        
    if ( !interface ) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }
        
    QString from(sender()->name());
    
    if (from == "scan_images")
    {
        m_scanDialog = KScanDialog::getScanDialog(kapp->activeWindow(), "KIPI Scan Images Plugin");
    
        if ( m_scanDialog )
        {
            m_scanDialog->setMinimumSize(400, 300);
        
            connect(m_scanDialog, SIGNAL(finalImage(const QImage &, int)),
                    this, SLOT(slotAcquireImageDone(const QImage &)));
        }
        else
        {
            KMessageBox::sorry(kapp->activeWindow(), i18n("No KDE scan-service available; check your system."),
                                i18n("KIPI's 'Scan Images' Plugin"));
            kdDebug ( 51000 ) << "No Scan-service available, aborting!" << endl;
            return;
        }
    
        if ( m_scanDialog->setup() )
            m_scanDialog->show();
    }
    else if (from == "screenshot_images")
    {
        m_screenshotDialog = new KIPIAcquireImagesPlugin::ScreenGrabDialog(interface,
            kapp->activeWindow(), "KIPI ScreenshotImagesDialog");
        m_screenshotDialog->show();
    }
    else
    {
        kdWarning( 51000 ) << "The impossible happened... unknown flip specified" << endl;
        return;
    }
}

void Plugin_AcquireImages::slotAcquireImageDone(const QImage &img)
{
    //FIXME: this is not a cleaned way to test if scan has been interrupted
    //       anyway it prevents a crash
    QImage * pImg = (QImage*)&img;
    if (!pImg )
    {
        kdError(51000) << "Acquired image is null!" << endl;
        return;
    }
    
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
        
    if ( !interface ) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }
            
    m_acquireImageDialog = new KIPIAcquireImagesPlugin::AcquireImageDialog( interface, kapp->activeWindow(), img);
    m_acquireImageDialog->setMinimumWidth(400);
    m_acquireImageDialog->exec();
}

KIPI::Category Plugin_AcquireImages::category( KAction* action ) const
{
    if ( action == m_action_scanimages )
       return KIPI::IMPORTPLUGIN;
    else if ( action == m_action_screenshotimages )
       return KIPI::IMPORTPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMPORTPLUGIN; // no warning from compiler, please   
}
