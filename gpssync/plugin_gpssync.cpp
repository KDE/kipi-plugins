/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a plugin to synchronize pictures with 
 *               a GPS device.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QFileInfo>

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kactionmenu.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "gpsbabelbinary.h"
#include "gpsdatacontainer.h"
#include "gpseditdialog.h"
#include "gpssyncdialog.h"
#include "kmlexport.h"
#include "kmlexportconfig.h"
#include "plugin_gpssync.h"
#include "plugin_gpssync.moc"

K_PLUGIN_FACTORY( GPSSyncFactory, registerPlugin<Plugin_GPSSync>(); )
K_EXPORT_PLUGIN ( GPSSyncFactory("kipiplugin_gpssync") )

Plugin_GPSSync::Plugin_GPSSync(QObject *parent, const QVariantList &)
              : KIPI::Plugin( GPSSyncFactory::componentData(), parent, "GPSSync")
{
    kDebug( 51001 ) << "Plugin_GPSSync plugin loaded" << endl;
}

void Plugin_GPSSync::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_geolocation = new KActionMenu(KIcon("applications-internet"), i18n("Geolocation"), actionCollection());
    m_action_geolocation->setObjectName("geolocation");
    addAction(m_action_geolocation);

    KAction *gpssync = new KAction(KIcon("gpsimagetag"), i18n("Correlator"), actionCollection());
    gpssync->setObjectName("gpssync");
    connect(gpssync, SIGNAL(triggered(bool)), 
            this, SLOT(slotGPSSync()));
    m_action_geolocation->addAction(gpssync);

    KAction *gpsedit = new KAction(i18n("Edit Coordinates..."), actionCollection());
    gpsedit->setObjectName("gpsedit");
    connect(gpsedit, SIGNAL(triggered(bool)), 
            this, SLOT(slotGPSEdit()));
    m_action_geolocation->addAction(gpsedit);

    KAction *gpsremove = new KAction(i18n("Remove Coordinates..."), actionCollection());
    gpsremove->setObjectName("gpsremove");
    connect(gpsremove, SIGNAL(triggered(bool)), 
            this, SLOT(slotGPSRemove()));
    m_action_geolocation->addAction(gpsremove);

    addAction( m_action_geolocation );

    m_actionKMLExport = new KAction(KIcon("applications-internet"), i18n("KML Export..."), actionCollection());
    m_actionKMLExport->setObjectName("kmlexport");
    connect(m_actionKMLExport, SIGNAL(triggered(bool)), 
            this, SLOT(slotKMLExport()));

    addAction( m_actionKMLExport );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !m_interface )
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    KIPI::ImageCollection selection = m_interface->currentSelection();
    m_action_geolocation->setEnabled( selection.isValid() && !selection.images().isEmpty() );

    connect( m_interface, SIGNAL(selectionChanged(bool)),
             m_action_geolocation, SLOT(setEnabled(bool)));
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
                          "<p>Note: at least, gpsbabel version %2 is required by this plugin.</p></qt>",
                          QString("http://www.gpsbabel.org"),
                          gpsBabelBinary.minimalVersion()),
                     QString::null,
                     QString::null,
                     KMessageBox::Notify | KMessageBox::AllowLink);
        return false;
    }

    if (!gpsBabelBinary.versionIsRight()) 
    {
        KMessageBox::information(
                     kapp->activeWindow(),
                     i18n("<qt><p>gpsbabel executable is not up to date:<br> "
                          "The version %1 of gpsbabel have been found on your computer. "
                          "This version is too old to run properly with this plugin. "
                          "Please update gpsbabel as a package from your distributor "
                          "or <a href=\"%2\">download the source</a>.</p>"
                          "<p>Note: at least, gpsbabel version %3 is required by this "
                          "plugin</p></qt>",
                          gpsBabelVersion,
                          QString("http://www.gpsbabel.org"),
                          gpsBabelBinary.minimalVersion()),
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
        return;
    */

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

    KUrl img = images.images().first();
    KExiv2Iface::KExiv2 exiv2Iface;
    exiv2Iface.load(img.path());
    double alt, lat, lng;
    bool hasGPSInfo = exiv2Iface.getGPSInfo(alt, lat, lng);
    KIPIGPSSyncPlugin::GPSDataContainer gpsData(alt, lat, lng, false);

    KIPIGPSSyncPlugin::GPSEditDialog dlg(kapp->activeWindow(), 
                                         gpsData, img.fileName(), hasGPSInfo);

    if (dlg.exec() == KDialog::Accepted)
    {
        gpsData = dlg.getGPSInfo();
        KUrl::List  imageURLs = images.images();
        KUrl::List  updatedURLs;
        QStringList errorFiles;

        for( KUrl::List::iterator it = imageURLs.begin() ; 
            it != imageURLs.end(); ++it)
        {
            KUrl url = *it;

            // We only add all JPEG files as R/W because Exiv2 can't yet 
            // update metadata on others file formats.

            QFileInfo fi(url.path());
            QString ext = fi.suffix().toUpper();
            bool ret = false;
            if (ext == QString("JPG") || ext == QString("JPEG") || ext == QString("JPE"))
            {
                ret = true;
                ret &= exiv2Iface.load(url.path());
                if (ret)
                {
                    ret &= exiv2Iface.setGPSInfo(gpsData.altitude(), 
                                                 gpsData.latitude(), 
                                                 gpsData.longitude());
                    ret &= exiv2Iface.save(url.path());
                }
            }

            if (!ret)
                errorFiles.append(url.fileName());
            else 
                updatedURLs.append(url);
        }

        // We use kipi interface refreshImages() method to tell to host than 
        // metadata from pictures have changed and need to be re-readed.

        m_interface->refreshImages(updatedURLs);

        if (!errorFiles.isEmpty())
        {
            KMessageBox::errorList(
                        kapp->activeWindow(),
                        i18n("Unable to save geographical coordinates into:"),
                        errorFiles,
                        i18n("Edit Geographical Coordinates"));  
        }
    }
}

void Plugin_GPSSync::slotGPSRemove()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("Geographical coordinates will be definitively removed from all selected images.\n"
                          "Do you want to continue ?"),
                     i18n("Remove Geographical Coordinates")) != KMessageBox::Yes)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ; 
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;

        // We only add all JPEG files as R/W because Exiv2 can't yet 
        // update metadata on others file formats.

        QFileInfo fi(url.path());
        QString ext = fi.suffix().toUpper();
        bool ret = false;
        if (ext == QString("JPG") || ext == QString("JPEG") || ext == QString("JPE"))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            ret &= exiv2Iface.load(url.path());
            ret &= exiv2Iface.removeGPSInfo();
            ret &= exiv2Iface.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else 
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than 
    // metadata from pictures have changed and need to be re-readed.

    m_interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to remove geographical coordinates from:"),
                    errorFiles,
                    i18n("Remove Geographical Coordinates"));  
    }
}

void Plugin_GPSSync::slotKMLExport()
{
    KIPI::ImageCollection selection = m_interface->currentSelection();

    if ( !selection.isValid() ) 
    {
        kDebug( 51000) << "No Selection!" << endl;
    }
    else 
    {
        KIPIGPSSyncPlugin::KMLExportConfig *kmlExportConfigGui = new KIPIGPSSyncPlugin::KMLExportConfig(kapp->activeWindow());
        connect(kmlExportConfigGui, SIGNAL(okButtonClicked()), 
                this, SLOT(slotKMLGenerate()));
        kmlExportConfigGui->show();
    }
}

void Plugin_GPSSync::slotKMLGenerate()
{
    KIPI::ImageCollection selection = m_interface->currentSelection();
    KIPIGPSSyncPlugin::kmlExport myExport(m_interface);
    if(!myExport.getConfig())
        return;
    myExport.generate();
}

KIPI::Category Plugin_GPSSync::category( KAction* action ) const
{
    if ( action == m_action_geolocation )
       return KIPI::IMAGESPLUGIN;
    if ( action == m_actionKMLExport )
       return KIPI::EXPORTPLUGIN;

    kWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}
