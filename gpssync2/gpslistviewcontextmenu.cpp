/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-05-07
 * Description : Context menu for GPS list view
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

#include "gpsbookmarkowner.h"
#include "gpsimageitem.h"
#include "gpsundocommand.h"
#include "gpssync2_common.h"

namespace KIPIGPSSyncPlugin
{

class GPSListViewContextMenuPriv
{
public:

    GPSListViewContextMenuPriv()
    {
        imagesList     = 0;
        actionCopy     = 0;
        actionPaste    = 0;
        actionBookmark = 0;
    }

    KAction          *actionCopy;
    KAction          *actionPaste;
    KAction          *actionBookmark;

    GPSBookmarkOwner *bookmarkOwner;

    KipiImageList       *imagesList;
};

GPSListViewContextMenu::GPSListViewContextMenu(KipiImageList *imagesList, GPSBookmarkOwner* const bookmarkOwner)
                      : QObject(imagesList), d(new GPSListViewContextMenuPriv)
{
    d->imagesList  = imagesList;

    d->actionCopy  = new KAction(i18n("Copy coordinates"), this);
    d->actionCopy->setIcon(SmallIcon("edit-copy"));
    d->actionPaste = new KAction(i18n("Paste coordinates"), this);
    d->actionPaste->setIcon(SmallIcon("edit-paste"));

    connect(d->actionCopy, SIGNAL(triggered()),
            this, SLOT(copyActionTriggered()));

    connect(d->actionPaste, SIGNAL(triggered()),
            this, SLOT(pasteActionTriggered()));

    if (bookmarkOwner)
    {
        d->bookmarkOwner = bookmarkOwner;
        d->actionBookmark = new KAction(i18n("Bookmarks"), this);
        d->actionBookmark->setMenu(d->bookmarkOwner->getMenu());

        connect(d->bookmarkOwner, SIGNAL(positionSelected(GPSDataContainer)),
                this, SLOT(slotBookmarkSelected(GPSDataContainer)));
    }


    d->imagesList->view()->installEventFilter(this);
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
        KipiImageModel* const imageModel = d->imagesList->getModel();
        QItemSelectionModel* const selectionModel = d->imagesList->getSelectionModel();
        const QList<QModelIndex> selectedIndices = selectionModel->selectedRows();
        const int nSelected = selectedIndices.size();

        // "copy" and "Add bookmark" are only available for one selected image with geo data:
        bool copyAvailable = (nSelected == 1);
        if (copyAvailable)
        {
            GPSImageItem* const gpsItem = static_cast<GPSImageItem*>(imageModel->itemFromIndex(selectedIndices.first()));
            if (gpsItem)
            {
                copyAvailable = gpsItem->gpsData().m_coordinates.hasCoordinates();
            }
            else
            {
                copyAvailable = false;
            }
        }
        d->actionCopy->setEnabled(copyAvailable);
        if (d->bookmarkOwner)
        {
            d->bookmarkOwner->changeAddBookmark(copyAvailable);
            GPSDataContainer position;
            getCurrentItemPositionAndUrl(&position, 0);
            d->bookmarkOwner->setPositionAndTitle(position.m_coordinates, QString());
        }

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
        if (d->actionBookmark)
        {
            menu->addSeparator();
            menu->addAction(d->actionBookmark);
            d->actionBookmark->setEnabled(nSelected>=1);
        }

        QContextMenuEvent * const e = static_cast<QContextMenuEvent*>(event);
        menu->exec(e->globalPos());

        return true;
    }
    else
    {
        return QObject::eventFilter(watched, event);
    }

}

bool GPSListViewContextMenu::getCurrentItemPositionAndUrl(GPSDataContainer* const gpsInfo, KUrl* const itemUrl)
{
    KipiImageModel* const imageModel = d->imagesList->getModel();
    QItemSelectionModel* const selectionModel = d->imagesList->getSelectionModel();
    const QModelIndex currentIndex = selectionModel->currentIndex();
    if (!currentIndex.isValid())
        return false;

    GPSImageItem* const gpsItem = static_cast<GPSImageItem*>(imageModel->itemFromIndex(currentIndex));
    if (gpsItem)
    {
        if (gpsInfo)
            *gpsInfo = gpsItem->gpsData();

        if (itemUrl)
            *itemUrl = gpsItem->url();

        return true;
    }

    return false;
}

void GPSListViewContextMenu::copyActionTriggered()
{
    GPSDataContainer gpsInfo;
    KUrl itemUrl;

    if (!getCurrentItemPositionAndUrl(&gpsInfo, &itemUrl))
        return;

    CoordinatesToClipboard(gpsInfo.m_coordinates, itemUrl, QString());
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
            const QDomElement gpxDocElem = gpxDoc.documentElement();
            if (gpxDocElem.tagName()!="gpx")
            xmlOkay = false;

            if (xmlOkay)
            {
                for (QDomNode nWpt = gpxDocElem.firstChild();
                    !nWpt.isNull(); nWpt = nWpt.nextSibling())
                {
                    const QDomElement wptElem = nWpt.toElement();
                    if (wptElem.isNull()) continue;
                    if (wptElem.tagName() != "wpt") continue;

                    double    ptAltitude  = 0.0;
                    double    ptLatitude  = 0.0;
                    double    ptLongitude = 0.0;
                    bool haveAltitude = false;

                    // Get GPS position. If not available continue to next point.
                    const QString lat = wptElem.attribute("lat");
                    const QString lon = wptElem.attribute("lon");
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
                        const QDomElement wptMetaElem = nWptMeta.toElement();
                        if (wptMetaElem.isNull()) continue;

                        if (wptMetaElem.tagName() == QString("ele"))
                        {
                            // Get GPS point altitude. If not available continue to next point.
                            QString ele = wptMetaElem.text();
                            if (!ele.isEmpty())
                            {
                                ptAltitude  = ele.toDouble(&haveAltitude);
                                break;
                            }
                        }
                    }

                    foundData = true;
                    WMW2::WMWGeoCoordinate coordinates(ptLatitude, ptLongitude);
                    if (haveAltitude)
                        coordinates.setAlt(ptAltitude);
                    gpsData.setCoordinates(coordinates);
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
            bool haveAltitude = false;

            ptLongitude = parts[0].toDouble(&okay);
            if (okay)
                ptLatitude = parts[1].toDouble(&okay);

            if (okay&&(parts.size()==3))
            {
                ptAltitude = parts[2].toDouble(&okay);
                haveAltitude = okay;
            }

            foundData = okay;
            if (okay)
            {
                WMW2::WMWGeoCoordinate coordinates(ptLatitude, ptLongitude);
                if (haveAltitude)
                    coordinates.setAlt(ptAltitude);
                gpsData.setCoordinates(coordinates);
            }
        }
    }

    if (!foundData)
    {
        KMessageBox::sorry(d->imagesList,
                     i18n("Could not find any coordinates on the clipboard."), i18n("GPS Sync"));
        return;
    }

    setGPSDataForSelectedItems(gpsData, i18n("Coordinates pasted"));
}

void GPSListViewContextMenu::setGPSDataForSelectedItems(const GPSDataContainer gpsData, const QString& undoDescription)
{
    // enable or disable the actions:
    KipiImageModel* const imageModel = d->imagesList->getModel();
    QItemSelectionModel* const selectionModel = d->imagesList->getSelectionModel();
    const QList<QModelIndex> selectedIndices = selectionModel->selectedRows();
    const int nSelected = selectedIndices.size();

    GPSUndoCommand* const undoCommand = new GPSUndoCommand();
    for (int i=0; i<nSelected; ++i)
    {
        const QModelIndex itemIndex = selectedIndices.at(i);
        GPSImageItem* const gpsItem = static_cast<GPSImageItem*>(imageModel->itemFromIndex(itemIndex));
        gpsItem->setGPSData(gpsData);
        undoCommand->addUndoInfo(GPSUndoCommand::UndoInfo(itemIndex, gpsItem->gpsData(), gpsData));
    }

    undoCommand->setText(undoDescription);
    emit(signalUndoCommand(undoCommand));
}

void GPSListViewContextMenu::slotBookmarkSelected(GPSDataContainer bookmarkPosition)
{
    setGPSDataForSelectedItems(bookmarkPosition, i18n("Bookmark selected"));
}

bool GPSListViewContextMenu::getCurrentPosition(GPSDataContainer* position, void* mydata)
{
    if (!position || !mydata)
        return false;

    GPSListViewContextMenu* const me = reinterpret_cast<GPSListViewContextMenu*>(mydata);
    
    return me->getCurrentItemPositionAndUrl(position, 0);
}

} // namespace KIPIGPSSyncPlugin
