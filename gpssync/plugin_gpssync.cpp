/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a plugin to synchronize pictures with
 *               a GPS device.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_gpssync.h"
#include "plugin_gpssync.moc"

// Qt includes

#include <QPointer>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// Local includes

#include "gpsbabelbinary.h"
#include "gpsdatacontainer.h"
#include "gpseditdialog.h"
#include "gpssyncdialog.h"
#include "gpstracklistcontainer.h"
#include "gpstracklisteditdialog.h"
#include "kmlexport.h"
#include "kmlexportconfig.h"

K_PLUGIN_FACTORY( GPSSyncFactory, registerPlugin<Plugin_GPSSync>(); )
K_EXPORT_PLUGIN ( GPSSyncFactory("kipiplugin_gpssync") )

Plugin_GPSSync::Plugin_GPSSync(QObject *parent, const QVariantList &)
              : KIPI::Plugin( GPSSyncFactory::componentData(), parent, "GPSSync")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_GPSSync plugin loaded" ;
}

void Plugin_GPSSync::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_geolocation = new KActionMenu(KIcon("applications-internet"), i18n("Geolocation"), actionCollection());

    KAction *gpssync = actionCollection()->addAction("gpssync");
    gpssync->setText(i18n("Correlator"));
    gpssync->setIcon(KIcon("gpsimagetag"));
    connect(gpssync, SIGNAL(triggered(bool)),
            this, SLOT(slotGPSSync()));
    m_action_geolocation->addAction(gpssync);

    KAction *gpsedit = actionCollection()->addAction("gpsedit");
    gpsedit->setText(i18n("Edit Coordinates..."));
    connect(gpsedit, SIGNAL(triggered(bool)),
            this, SLOT(slotGPSEdit()));
    m_action_geolocation->addAction(gpsedit);

    KAction *gpstracklistedit = actionCollection()->addAction("gpstracklistedit");
    gpstracklistedit->setText(i18n("Track List Editor..."));
    connect(gpstracklistedit, SIGNAL(triggered(bool)),
            this, SLOT(slotGPSTrackListEdit()));
    m_action_geolocation->addAction(gpstracklistedit);

    KAction *gpsremove = actionCollection()->addAction("gpsremove");
    gpsremove->setText(i18n("Remove Coordinates..."));
    connect(gpsremove, SIGNAL(triggered(bool)),
            this, SLOT(slotGPSRemove()));
    m_action_geolocation->addAction(gpsremove);

    addAction( m_action_geolocation );

    m_actionKMLExport = actionCollection()->addAction("kmlexport");
    m_actionKMLExport->setText(i18n("Export to KML..."));
    m_actionKMLExport->setIcon(KIcon("applications-internet"));
    connect(m_actionKMLExport, SIGNAL(triggered(bool)),
            this, SLOT(slotKMLExport()));

    addAction( m_actionKMLExport );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !m_interface )
    {
        kError() << "Kipi interface is null!" ;
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
                     i18n("<p>Unable to find the gpsbabel executable:<br/> "
                          "This program is required by this plugin to support GPS data file decoding. "
                          "Please install gpsbabel as a package from your distributor "
                          "or <a href=\"%1\">download the source</a>.</p>"
                          "<p>Note: at least, gpsbabel version %2 is required by this plugin.</p>",
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
                     i18n("<p>gpsbabel executable is not up to date:<br/> "
                          "The version %1 of gpsbabel have been found on your computer. "
                          "This version is too old to run properly with this plugin. "
                          "Please update gpsbabel as a package from your distributor "
                          "or <a href=\"%2\">download the source</a>.</p>"
                          "<p>Note: at least, gpsbabel version %3 is required by this "
                          "plugin</p>",
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

    bool hasGPSInfo = false;
    double alt, lat, lng;
    KExiv2Iface::KExiv2 exiv2Iface;
    KUrl::List imageURLs = images.images();
    KUrl img             = images.images().first();
    QMap<QString, QVariant> attributes;
    KIPI::ImageInfo info = m_interface->info(img);
    attributes = info.attributes();

    exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
    exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

    if (attributes.contains("latitude") &&
        attributes.contains("longitude") &&
        attributes.contains("altitude"))
    {
        lat = attributes["latitude"].toDouble();
        lng = attributes["longitude"].toDouble();
        alt = attributes["altitude"].toDouble();
        hasGPSInfo = true;
    }
    else
    {
        exiv2Iface.load(img.path());
        hasGPSInfo = exiv2Iface.getGPSInfo(alt, lat, lng);
    }

    KIPIGPSSyncPlugin::GPSDataContainer gpsData(alt, lat, lng, false);

    QPointer<KIPIGPSSyncPlugin::GPSEditDialog> dlg = new KIPIGPSSyncPlugin::GPSEditDialog(
                                                         kapp->activeWindow(), gpsData, img.fileName(), hasGPSInfo);

    if (dlg->exec() == KDialog::Accepted)
    {
        gpsData = dlg->getGPSInfo();

        for( KUrl::List::iterator it = imageURLs.begin() ;
            it != imageURLs.end(); ++it)
        {
            KUrl url = *it;

            // Set file metadata GPS location.
            bool ret = exiv2Iface.load(url.path());
            if (ret)
            {
                ret &= exiv2Iface.setGPSInfo(gpsData.altitude(),
                                             gpsData.latitude(),
                                             gpsData.longitude());
                ret &= exiv2Iface.save(url.path());
            }

            if (!ret)
                kDebug() << "Cannot set GPS location into file metadata from "
                         << url.fileName() ;

            // Set kipi host GPS location
            attributes.clear();
            attributes.insert("latitude",  gpsData.latitude());
            attributes.insert("longitude", gpsData.longitude());
            attributes.insert("altitude",  gpsData.altitude());
            KIPI::ImageInfo info = m_interface->info(url);
            info.addAttributes(attributes);
        }
    }

    delete dlg;
}

void Plugin_GPSSync::slotGPSTrackListEdit()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    double    alt, lat, lng;
    QDateTime dt;
    QMap<QString, QVariant> attributes;
    KExiv2Iface::KExiv2 exiv2Iface;
    KIPIGPSSyncPlugin::GPSTrackList trackList;
    KUrl::List urls = images.images();

    exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
    exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

    for( KUrl::List::iterator it = urls.begin() ;
        it != urls.end() ; ++it)
    {
        bool hasGPSInfo = false;
        KIPI::ImageInfo info = m_interface->info(*it);
        attributes = info.attributes();
        if (attributes.contains("latitude") &&
            attributes.contains("longitude") &&
            attributes.contains("altitude"))
        {
            lat = attributes["latitude"].toDouble();
            lng = attributes["longitude"].toDouble();
            alt = attributes["altitude"].toDouble();
            hasGPSInfo = true;
        }
        else
        {
            exiv2Iface.load((*it).path());
            hasGPSInfo = exiv2Iface.getGPSInfo(alt, lat, lng);
        }

        if(hasGPSInfo)
        {
            QDateTime dt = info.time(KIPI::FromInfo);
            if (!dt.isValid())
                dt = exiv2Iface.getImageDateTime();

            if (dt.isValid())
            {
                KIPIGPSSyncPlugin::GPSDataContainer gpsData(alt, lat, lng, false);
                KIPIGPSSyncPlugin::GPSTrackListItem trackListItem(*it, gpsData);
                trackList.insert(dt, trackListItem);
            }
        }
    }

    if (trackList.isEmpty())
    {
        KMessageBox::sorry(0, i18n("No GPS locations have been found in your pictures."));
        return;
    }

    // Set track list id sorted by datetime.
    int id = 1;
    for( KIPIGPSSyncPlugin::GPSTrackList::iterator it = trackList.begin() ;
         it != trackList.end() ; ++it)
    {
        it.value().setId(id);
        id++;
    }

    QPointer<KIPIGPSSyncPlugin::GPSTrackListEditDialog> dlg = new KIPIGPSSyncPlugin::GPSTrackListEditDialog(
                                                                  m_interface, kapp->activeWindow(), trackList);

    if (dlg->exec() == KDialog::Accepted)
    {
        trackList = dlg->trackList();

        for( KIPIGPSSyncPlugin::GPSTrackList::iterator it = trackList.begin() ;
            it != trackList.end() ; ++it)
        {
            if ((*it).isDirty())
            {
                KUrl url = (*it).url();

                // Set file metadata GPS location.
                bool ret = exiv2Iface.load(url.path());
                ret &= exiv2Iface.setGPSInfo((*it).gpsData().altitude(),
                                             (*it).gpsData().latitude(),
                                             (*it).gpsData().longitude());
                ret &= exiv2Iface.save(url.path());

                if (!ret)
                    kDebug() << "Cannot set GPS location into file metadata from "
                             << url.fileName() ;

                // Set kipi host GPS location
                attributes.clear();
                attributes.insert("latitude",  (*it).gpsData().latitude());
                attributes.insert("longitude", (*it).gpsData().longitude());
                attributes.insert("altitude",  (*it).gpsData().altitude());
                KIPI::ImageInfo info = m_interface->info(url);
                info.addAttributes(attributes);
            }
        }
    }

    delete dlg;
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
    QStringList errorFiles;
    KExiv2Iface::KExiv2 exiv2Iface;

    exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
    exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;

        // Remove file metadata GPS location.
        bool ret = exiv2Iface.load(url.path());
        ret &= exiv2Iface.removeGPSInfo();
        ret &= exiv2Iface.save(url.path());

        if (!ret)
            kDebug() << "Cannot remove GPS location into file metadata from "
                     << url.fileName() ;

        // Remove kipi host GPS location
        QStringList list;
        list << "gpslocation";
        KIPI::ImageInfo info = m_interface->info(url);
        info.delAttributes(list);
    }
}

void Plugin_GPSSync::slotKMLExport()
{
    KIPI::ImageCollection selection = m_interface->currentSelection();

    if ( !selection.isValid() )
    {
        kDebug() << "No Selection!" ;
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
       return KIPI::ImagesPlugin;
    if ( action == m_actionKMLExport )
       return KIPI::ExportPlugin;

    kWarning() << "Unrecognized action for plugin category identification" ;
    return KIPI::ImagesPlugin; // no warning from compiler, please
}
