/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-05-16
 * Description : a plugin to synchronize pictures with 
 *               a GPS device.
 *
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes.

#include <qfileinfo.h>

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>

// Local includes.

#include "exiv2iface.h"
#include "gpsbabelbinary.h"
#include "gpsdatacontainer.h"
#include "gpseditdialog.h"
#include "gpssyncdialog.h"
#include "plugin_gpssync.h"
#include "plugin_gpssync.moc"

typedef KGenericFactory<Plugin_GPSSync> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_gpssync, Factory("kipiplugin_gpssync"))

Plugin_GPSSync::Plugin_GPSSync(QObject *parent, const char*, const QStringList&)
              : KIPI::Plugin( Factory::instance(), parent, "GPSSync")
{
    kdDebug( 51001 ) << "Plugin_GPSSync plugin loaded" << endl;
}

void Plugin_GPSSync::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_geolocalization = new KActionMenu(i18n("Geolocalization"),
                               0,
                               actionCollection(),
                               "geolocalization");

    m_action_geolocalization->insert(new KAction (i18n("Correlator..."),
                                     "gpsimagetag",
                                     0,     
                                     this,
                                     SLOT(slotGPSSync()),
                                     actionCollection(),
                                     "gpssync"));

    m_action_geolocalization->insert(new KAction (i18n("Edit coordinates..."),
                                     0,
                                     0,     
                                     this,
                                     SLOT(slotGPSEdit()),
                                     actionCollection(),
                                     "gpsedit"));

    m_action_geolocalization->insert(new KAction (i18n("Remove coordinates..."),
                                     0,
                                     0,     
                                     this,
                                     SLOT(slotGPSRemove()),
                                     actionCollection(),
                                     "gpsremove"));

    addAction( m_action_geolocalization );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !m_interface )
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    KIPI::ImageCollection selection = m_interface->currentSelection();
    m_action_geolocalization->setEnabled( selection.isValid() && !selection.images().isEmpty() );

    connect( m_interface, SIGNAL(selectionChanged(bool)),
             m_action_geolocalization, SLOT(setEnabled(bool)));
}

bool Plugin_GPSSync::checkBinaries(QString &gpsBabelVersion)
{
    KIPIGPSSyncPlugin::GPSBabelBinary gpsBabelBinary;
    gpsBabelVersion = gpsBabelBinary.version();

    if (!gpsBabelBinary.isAvailable()) 
    {
        KMessageBox::information(
                     kapp->activeWindow(),
                     i18n("<qt><p>Unable to find the gpsbabel executable:<br> "
                          "This program is required by this plugin to support GPS data file decoding. "
                          "Please install gpsbabel as a package from your distributor "
                          "or <a href=\"%1\">download the source</a>.</p>"
                          "<p>Note: at least, gpsbabel version %2 is required by this plugin.</p></qt>")
                          .arg("http://www.gpsbabel.org")
                          .arg(gpsBabelBinary.minimalVersion()),
                     QString::null,
                     QString::null,
                     KMessageBox::Notify | KMessageBox::AllowLink);
        return false;
    }

    if (!gpsBabelBinary.versionIsRight()) 
    {
        KMessageBox::information(
                     kapp->activeWindow(),
                     i18n("<qt><p>gpsbabel executable isn't up to date:<br> "
                          "The version %1 of gpsbabel have been found on your computer. "
                          "This version is too old to run properly with this plugin. "
                          "Please update gpsbabel as a package from your distributor "
                          "or <a href=\"%2\">download the source</a>.</p>"
                          "<p>Note: at least, gpsbabel version %3 is required by this "
                          "plugin</p></qt>")
                          .arg(gpsBabelVersion)
                          .arg("http://www.gpsbabel.org")
                          .arg(gpsBabelBinary.minimalVersion()),
                     QString::null,
                     QString::null,
                     KMessageBox::Notify | KMessageBox::AllowLink);
        return false;
    }

    return true;
}

void Plugin_GPSSync::slotGPSSync()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

/* NOTE: this plugin do not use yet GPSBabel to convert GPS data file to GPX
    QString gpsBabelVersion;
    if (!checkBinaries(gpsBabelVersion)) 
        return;*/

    KIPIGPSSyncPlugin::GPSSyncDialog *dialog = new KIPIGPSSyncPlugin::GPSSyncDialog(
                                               m_interface, kapp->activeWindow());

    dialog->setImages( images.images() );
    dialog->show();
}

void Plugin_GPSSync::slotGPSEdit()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KURL img = images.images().first();
    KIPIPlugins::Exiv2Iface exiv2Iface;
    exiv2Iface.load(img.path());
    double alt, lat, lng;
    bool hasGPSInfo = exiv2Iface.getGPSInfo(alt, lat, lng);
    KIPIGPSSyncPlugin::GPSDataContainer gpsData(alt, lat, lng, false);

    KIPIGPSSyncPlugin::GPSEditDialog dlg(kapp->activeWindow(), 
                                         gpsData, img.fileName(), hasGPSInfo);

    if (dlg.exec() == KDialogBase::Accepted)
    {
        gpsData = dlg.getGPSInfo();
        KURL::List imageURLs = images.images();
        KURL::List errorURLs;
    
        for( KURL::List::iterator it = imageURLs.begin() ; 
            it != imageURLs.end(); ++it)
        {
            KURL url = *it;
        
            // We only add all JPEG files as R/W because Exiv2 can't yet 
            // update metadata on others file formats.
        
            QFileInfo fi(url.path());
            QString ext = fi.extension(false).upper();
            if (ext == QString("JPG") || ext == QString("JPEG") || ext == QString("JPE"))
            {
                if (exiv2Iface.load(url.path()))
                {
                    bool ret = exiv2Iface.setGPSInfo(gpsData.altitude(), 
                                                     gpsData.latitude(), 
                                                     gpsData.longitude());
                    ret &= exiv2Iface.save(url.path());
                
                    if (!ret)
                        errorURLs.append(url);
                }
            }
        }

        if (!errorURLs.isEmpty())
        {
            KMessageBox::errorList(
                        kapp->activeWindow(),
                        i18n("Unable to save geographical coordinates to:"),
                        errorURLs.toStringList(),
                        i18n("Edit Geographical Coordinates"));  
        }
    }
}

void Plugin_GPSSync::slotGPSRemove()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KURL::List imageURLs = images.images();
    KURL::List errorURLs;

    for( KURL::List::iterator it = imageURLs.begin() ; 
         it != imageURLs.end(); ++it)
    {
        KURL url = *it;
    
        // We only add all JPEG files as R/W because Exiv2 can't yet 
        // update metadata on others file formats.
    
        QFileInfo fi(url.path());
        QString ext = fi.extension(false).upper();
        if (ext == QString("JPG") || ext == QString("JPEG") || ext == QString("JPE"))
        {
            KIPIPlugins::Exiv2Iface exiv2Iface;
            exiv2Iface.load(url.path());
            bool ret = exiv2Iface.removeGPSInfo();
            ret &= exiv2Iface.save(url.path());
        
            if (!ret)
                errorURLs.append(url);
        }
    }

    if (!errorURLs.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to remove geographical coordinates from:"),
                    errorURLs.toStringList(),
                    i18n("Remove Geographical Coordinates"));  
    }
}

KIPI::Category Plugin_GPSSync::category( KAction* action ) const
{
    if ( action == m_action_geolocalization )
       return KIPI::IMAGESPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}
