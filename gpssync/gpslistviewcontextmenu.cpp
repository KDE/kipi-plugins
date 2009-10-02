/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-05-07
 * Description : Context menu for GPS list view which can be used
 *               in the track list editor and the sync dialog
 *
 * Copyright (C) 2009 by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "gpslistviewcontextmenu.h"
#include "gpslistviewcontextmenu.moc"

// Qt includes:

#include <QEvent>
#include <QContextMenuEvent>
#include <QClipboard>
#include <QApplication>
#include <QDomDocument>

// KDE includes.

#include <kaction.h>
#include <kmenu.h>
#include <klocale.h>
#include <kmessagebox.h>

// LibKIPI includes.

#include <libkipi/interface.h>

// Local includes.

#include <gpslistviewitem.h>
#include <gpstracklistviewitem.h>

using namespace KIPIPlugins;

namespace KIPIGPSSyncPlugin
{

class GPSListViewContextMenuPriv
{
public:

    GPSListViewContextMenuPriv()
    {
        imagesList  = 0;
        actionCopy  = 0;
        actionPaste = 0;
    }

    KAction    *actionCopy;
    KAction    *actionPaste;

    ImagesList *imagesList;
};

GPSListViewContextMenu::GPSListViewContextMenu(KIPIPlugins::ImagesList *imagesList)
                      : QObject(imagesList), d(new GPSListViewContextMenuPriv)
{
    d->imagesList  = imagesList;

    d->actionCopy  = new KAction(i18n("Copy coordinates"), this);
    d->actionPaste = new KAction(i18n("Paste coordinates"), this);

    connect(d->actionCopy, SIGNAL(triggered()),
            this, SLOT(copyActionTriggered()));

    connect(d->actionPaste, SIGNAL(triggered()),
            this, SLOT(pasteActionTriggered()));

    d->imagesList->installEventFilter(this);
}

GPSListViewContextMenu::~GPSListViewContextMenu()
{
    delete d;
}

bool GPSListViewContextMenu::eventFilter(QObject *watched, QEvent *event)
{
    // we are only interested in context-menu events:
    if (event->type()==QEvent::ContextMenu)
    {
        // enable or disable the actions:
        const QList<QTreeWidgetItem*> selectedItems = d->imagesList->listView()->selectedItems();
        const int nSelected = selectedItems.size();

        // "copy" is only available for one selected image with geo data:
        bool copyAvailable = (nSelected == 1);
        if (copyAvailable)
        {
            QTreeWidgetItem * treeItem = d->imagesList->listView()->currentItem();
            GPSListViewItem * lvItem = dynamic_cast<GPSListViewItem*>(treeItem);
            if (lvItem)
            {
                copyAvailable = lvItem->hasGPSInfo();
            }
            else
            {
                GPSTrackListViewItem *lvItem = dynamic_cast<GPSTrackListViewItem*>(treeItem);
            if (lvItem)
            {
                // "GPSTrackListViewItem"s only exist if they have GPSInfo
                copyAvailable = true;
            }
            else
                copyAvailable = false;
            }
        }
        d->actionCopy->setEnabled(copyAvailable);

        // "paste" is only available if there is geo data in the clipboard
        // and at least one photo is selected:
        bool pasteAvailable = (nSelected >= 1);
        if (pasteAvailable)
        {
            QClipboard * const clipboard = QApplication::clipboard();
            const QMimeData * mimedata = clipboard->mimeData();
            pasteAvailable = mimedata->hasFormat("application/gpx+xml") || mimedata->hasText();
        }
        d->actionPaste->setEnabled(pasteAvailable);

        // construct the context-menu:
        KMenu * const menu = new KMenu(d->imagesList);
        menu->addAction(d->actionCopy);
        menu->addAction(d->actionPaste);

        QContextMenuEvent * const e = static_cast<QContextMenuEvent*>(event);
        menu->exec(e->globalPos());

        return true;
    }
    else
    {
        return QObject::eventFilter(watched, event);
    }

}

void GPSListViewContextMenu::copyActionTriggered()
{
    GPSDataContainer gpsInfo;
    KUrl itemUrl;

    QTreeWidgetItem * treeItem     = d->imagesList->listView()->currentItem();
    const GPSListViewItem * lvItem = dynamic_cast<GPSListViewItem*>(treeItem);
    if (lvItem)
    {
        gpsInfo = lvItem->GPSInfo();
        itemUrl = lvItem->url();
    }
    else
    {
        const GPSTrackListViewItem * lvItem = dynamic_cast<GPSTrackListViewItem*>(treeItem);
        if (!lvItem)
            return;

        gpsInfo = lvItem->gpsInfo().gpsData();
        itemUrl = lvItem->gpsInfo().url();
    }

    const double lat = gpsInfo.latitude();
    const double lon = gpsInfo.longitude();
    const double altitude = gpsInfo.altitude();

    const QString coordinates = QString::fromLatin1("%1,%2,%3")
                                .arg(lon, 0, 'f', 10).arg(lat, 0, 'f', 10).arg(altitude, 0, 'f', 10);

    // importing this representation into Marble does not show anything,
    // but Merkaartor shows the point
    const QString kmlRepresentation = QString::fromLatin1(
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
      "<Document>\n"
      " <Placemark>\n"
      "   <name>%1</name>\n"
      "   <Point>\n"
      "     <coordinates>%2</coordinates>\n"
      "   </Point>\n"
      " </Placemark>\n"
      "</Document>\n"
      "</kml>\n"
      ).arg(itemUrl.toLocalFile()).arg(coordinates);

    // importing this data into Marble and Merkaartor works
    const QString gpxRepresentation = QString::fromLatin1(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n"
      "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" creator=\"trippy\" version=\"0.1\"\n"
      " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
      " xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">\n"
      "  <wpt lat=\"%1\" lon=\"%2\">\n"
      "   <ele>%3</ele>\n"
//      "   <time></time>\n"
      "   <name>%4</name>\n"
      "  </wpt>\n"
      "</gpx>\n"
      ).arg(lat, 0, 'f', 10).arg(lon, 0, 'f', 10).arg(altitude, 0, 'f', 10).arg(itemUrl.toLocalFile());

    QMimeData * const myMimeData = new QMimeData();
    myMimeData->setText(coordinates);
    myMimeData->setData(QLatin1String("application/vnd.google-earth.kml+xml"), kmlRepresentation.toUtf8());
    myMimeData->setData(QLatin1String("application/gpx+xml"), gpxRepresentation.toUtf8());

    QClipboard * const clipboard = QApplication::clipboard();
    clipboard->setMimeData(myMimeData);
}

void GPSListViewContextMenu::pasteActionTriggered()
{
    // extract the coordinates from the clipboard:
    QClipboard * const clipboard = QApplication::clipboard();
    const QMimeData * mimedata   = clipboard->mimeData();

    GPSDataContainer gpsData;
    bool foundData       = false;
    bool foundDoubleData = false;

    if (mimedata->hasFormat("application/gpx+xml"))
    {
        const QByteArray data = mimedata->data("application/gpx+xml");

        bool xmlOkay = true;

        // code adapted from gpsdataparser.cpp
        QDomDocument gpxDoc("gpx");
        if (!gpxDoc.setContent(data))
            xmlOkay = false;

        if (xmlOkay)
        {
            QDomElement gpxDocElem = gpxDoc.documentElement();
            if (gpxDocElem.tagName()!="gpx")
            xmlOkay = false;

            if (xmlOkay)
            {
                for (QDomNode nWpt = gpxDocElem.firstChild();
                    !nWpt.isNull(); nWpt = nWpt.nextSibling())
                {
                    QDomElement wptElem = nWpt.toElement();
                    if (wptElem.isNull()) continue;
                    if (wptElem.tagName() != "wpt") continue;

                    double    ptAltitude  = 0.0;
                    double    ptLatitude  = 0.0;
                    double    ptLongitude = 0.0;

                    // Get GPS position. If not available continue to next point.
                    QString lat = wptElem.attribute("lat");
                    QString lon = wptElem.attribute("lon");
                    if (lat.isEmpty() || lon.isEmpty()) continue;

                    ptLatitude  = lat.toDouble();
                    ptLongitude = lon.toDouble();

                    if (foundData)
                    {
                        foundDoubleData = true;
                        break;
                    }

                    // Get metadata of way point (altitude and time stamp)
                    for (QDomNode nWptMeta = wptElem.firstChild();
                        !nWptMeta.isNull(); nWptMeta = nWptMeta.nextSibling())
                    {
                        QDomElement wptMetaElem = nWptMeta.toElement();
                        if (wptMetaElem.isNull()) continue;

                        if (wptMetaElem.tagName() == QString("ele"))
                        {
                            // Get GPS point altitude. If not available continue to next point.
                            QString ele = wptMetaElem.text();
                            if (!ele.isEmpty())
                            {
                                ptAltitude  = ele.toDouble();
                                break;
                            }
                        }
                    }

                    foundData = true;
                    gpsData   = GPSDataContainer(ptAltitude, ptLatitude, ptLongitude, false);
                }
            }
        }

        if (foundDoubleData)
        {
            KMessageBox::sorry(d->imagesList,
              i18n("Found more than one point on the clipboard - can only assign one point at a time.")
              , i18n("GPS Sync"));
        }
    }

    if ((!foundData)&&(mimedata->hasText()))
    {
        const QString textdata  = mimedata->text();
        const QStringList parts = textdata.split(',');

        if ((parts.size()==3)||(parts.size()==2))
        {
            bool okay = true;
            double    ptLongitude = 0.0;
            double    ptLatitude  = 0.0;
            double    ptAltitude  = 0.0;

            ptLongitude = parts[0].toDouble(&okay);
            if (okay)
                ptLatitude = parts[1].toDouble(&okay);

            if (okay&&(parts.size()==3))
                ptAltitude = parts[2].toDouble(&okay);

            foundData = okay;

            gpsData = GPSDataContainer(ptAltitude, ptLatitude, ptLongitude, false);
        }
    }

    if (!foundData)
    {
        KMessageBox::sorry(d->imagesList,
                     i18n("Could not find any coordinates on the clipboard."), i18n("GPS Sync"));
        return;
    }

    const QList<QTreeWidgetItem*> selectedItems = d->imagesList->listView()->selectedItems();
    for (QList<QTreeWidgetItem*>::const_iterator it = selectedItems.begin(); it!=selectedItems.end(); ++it)
    {
        GPSListViewItem * lvItem = dynamic_cast<GPSListViewItem*>(*it);
        if (lvItem)
        {
            lvItem->setGPSInfo(gpsData, true, true);
        }
        else
        {
            GPSTrackListViewItem * lvItem2 = dynamic_cast<GPSTrackListViewItem*>(*it);
            if (lvItem2)
            {
                GPSTrackListItem data = lvItem2->gpsInfo();
                data.setGPSData(gpsData);
                data.setDirty(true);
                lvItem2->setGPSInfo(lvItem2->dateTime(), data);
            }
        }
    }
}

} // namespace KIPIGPSSyncPlugin
