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
#include "gpsundocommand.h"
#include "gpssync_common.h"
#include "kipiimageitem.h"

namespace KIPIGPSSyncPlugin
{

class GPSListViewContextMenuPriv
{
public:

    GPSListViewContextMenuPriv()
    : enabled(true)
    {
        imagesList     = 0;
        actionCopy     = 0;
        actionPaste    = 0;
        actionBookmark = 0;
    }

    bool              enabled;

    KAction          *actionCopy;
    KAction          *actionPaste;
    KAction          *actionBookmark;
    KAction          *actionRemoveCoordinates;
    KAction          *actionRemoveAltitude;
    KAction          *actionRemoveUncertainty;
    KAction          *actionRemoveSpeed;

    GPSBookmarkOwner *bookmarkOwner;

    KipiImageList    *imagesList;
};

GPSListViewContextMenu::GPSListViewContextMenu(KipiImageList *imagesList, GPSBookmarkOwner* const bookmarkOwner)
                      : QObject(imagesList), d(new GPSListViewContextMenuPriv)
{
    d->imagesList  = imagesList;

    d->actionCopy  = new KAction(i18n("Copy coordinates"), this);
    d->actionCopy->setIcon(SmallIcon("edit-copy"));
    d->actionPaste = new KAction(i18n("Paste coordinates"), this);
    d->actionPaste->setIcon(SmallIcon("edit-paste"));
    d->actionRemoveCoordinates = new KAction(i18n("Remove coordinates"), this);
    d->actionRemoveAltitude = new KAction(i18n("Remove altitude"), this);
    d->actionRemoveUncertainty = new KAction(i18n("Remove uncertainty"), this);
    d->actionRemoveSpeed = new KAction(i18n("Remove speed"), this);

    connect(d->actionCopy, SIGNAL(triggered()),
            this, SLOT(copyActionTriggered()));

    connect(d->actionPaste, SIGNAL(triggered()),
            this, SLOT(pasteActionTriggered()));

    connect(d->actionRemoveCoordinates, SIGNAL(triggered()),
            this, SLOT(slotRemoveCoordinates()));

    connect(d->actionRemoveAltitude, SIGNAL(triggered()),
            this, SLOT(slotRemoveAltitude()));

    connect(d->actionRemoveUncertainty, SIGNAL(triggered()),
            this, SLOT(slotRemoveUncertainty()));

    connect(d->actionRemoveSpeed, SIGNAL(triggered()),
            this, SLOT(slotRemoveSpeed()));

    if (bookmarkOwner)
    {
        d->bookmarkOwner = bookmarkOwner;
        d->actionBookmark = new KAction(i18n("Bookmarks"), this);
        d->actionBookmark->setMenu(d->bookmarkOwner->getMenu());

        connect(d->bookmarkOwner, SIGNAL(positionSelected(GPSDataContainer)),
                this, SLOT(slotBookmarkSelected(GPSDataContainer)));
    }

    d->imagesList->installEventFilter(this);
}

GPSListViewContextMenu::~GPSListViewContextMenu()
{
    delete d;
}

bool GPSListViewContextMenu::eventFilter(QObject *watched, QEvent *event)
{
    // we are only interested in context-menu events:
    if ((event->type()==QEvent::ContextMenu)&&d->enabled)
    {
        // enable or disable the actions:
        KipiImageModel* const imageModel = d->imagesList->getModel();
        QItemSelectionModel* const selectionModel = d->imagesList->getSelectionModel();
        const QList<QModelIndex> selectedIndices = selectionModel->selectedRows();
        const int nSelected = selectedIndices.size();

        // "copy" and "Add bookmark" are only available for one selected image with geo data:
        bool copyAvailable = (nSelected==1);
        bool removeAltitudeAvailable = false;
        bool removeCoordinatesAvailable = false;
        bool removeUncertaintyAvailable = false;
        bool removeSpeedAvailable = false;
        for (int i=0; i<nSelected; ++i)
        {
            KipiImageItem* const gpsItem = imageModel->itemFromIndex(selectedIndices.at(i));
            if (gpsItem)
            {
                const bool itemHasCoordinates = gpsItem->gpsData().getCoordinates().hasCoordinates();
                copyAvailable&= itemHasCoordinates;
                removeCoordinatesAvailable|= itemHasCoordinates;
                removeAltitudeAvailable|= gpsItem->gpsData().getCoordinates().hasAltitude();
                removeUncertaintyAvailable|= gpsItem->gpsData().hasNSatellites() | gpsItem->gpsData().hasDop() | gpsItem->gpsData().hasFixType();
                removeSpeedAvailable|= gpsItem->gpsData().hasSpeed();
            }
        }

        d->actionCopy->setEnabled(copyAvailable);
        d->actionRemoveAltitude->setEnabled(removeAltitudeAvailable);
        d->actionRemoveCoordinates->setEnabled(removeCoordinatesAvailable);
        d->actionRemoveUncertainty->setEnabled(removeUncertaintyAvailable);
        d->actionRemoveSpeed->setEnabled(removeSpeedAvailable);
        if (d->bookmarkOwner)
        {
            d->bookmarkOwner->changeAddBookmark(copyAvailable);
            GPSDataContainer position;
            getCurrentItemPositionAndUrl(&position, 0);
            d->bookmarkOwner->setPositionAndTitle(position.getCoordinates(), QString());
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
        menu->addSeparator();
        menu->addAction(d->actionRemoveCoordinates);
        menu->addAction(d->actionRemoveAltitude);
        menu->addAction(d->actionRemoveUncertainty);
        menu->addAction(d->actionRemoveSpeed);
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
    // NOTE: currentIndex does not seem to work any more since we use KLinkItemSelectionModel
    KipiImageModel* const imageModel = d->imagesList->getModel();
    QItemSelectionModel* const selectionModel = d->imagesList->getSelectionModel();
    const QList<QModelIndex> selectedIndices = selectionModel->selectedRows();
    if (selectedIndices.count()!=1)
        return false;
    
    const QModelIndex currentIndex = selectedIndices.first();
    if (!currentIndex.isValid())
        return false;

    KipiImageItem* const gpsItem = imageModel->itemFromIndex(currentIndex);
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

    CoordinatesToClipboard(gpsInfo.getCoordinates(), itemUrl, QString());
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
                    KMap::GeoCoordinates coordinates(ptLatitude, ptLongitude);
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

        bool foundGeoUrl = false;
        KMap::GeoCoordinates testCoordinates = KMap::GeoCoordinates::fromGeoUrl(textdata, &foundGeoUrl);
        if (foundGeoUrl)
        {
            gpsData.setCoordinates(testCoordinates);
            foundData = true;
        }
        else
        {
            // TODO: this is legacy code from before we used geo-url
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
                    KMap::GeoCoordinates coordinates(ptLatitude, ptLongitude);
                    if (haveAltitude)
                        coordinates.setAlt(ptAltitude);
                    gpsData.setCoordinates(coordinates);
                }
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
    KipiImageModel* const imageModel = d->imagesList->getModel();
    QItemSelectionModel* const selectionModel = d->imagesList->getSelectionModel();
    const QList<QModelIndex> selectedIndices = selectionModel->selectedRows();
    const int nSelected = selectedIndices.size();

    GPSUndoCommand* const undoCommand = new GPSUndoCommand();
    for (int i=0; i<nSelected; ++i)
    {
        const QModelIndex itemIndex = selectedIndices.at(i);
        KipiImageItem* const gpsItem = imageModel->itemFromIndex(itemIndex);
        
        GPSUndoCommand::UndoInfo undoInfo(itemIndex);
        undoInfo.readOldDataFromItem(gpsItem);

        gpsItem->setGPSData(gpsData);
        undoInfo.readNewDataFromItem(gpsItem);
        

        undoCommand->addUndoInfo(undoInfo);
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

void GPSListViewContextMenu::removeInformationFromSelectedImages(const GPSDataContainer::HasFlags flagsToClear, const QString& undoDescription)
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
        KipiImageItem* const gpsItem = imageModel->itemFromIndex(itemIndex);

        GPSUndoCommand::UndoInfo undoInfo(itemIndex);
        undoInfo.readOldDataFromItem(gpsItem);

        GPSDataContainer newGPSData = gpsItem->gpsData();

        bool didSomething = false;
        if (flagsToClear.testFlag(GPSDataContainer::HasCoordinates))
        {
            if (newGPSData.hasCoordinates())
            {
                didSomething = true;
                newGPSData.clear();
            }
        }
        if (flagsToClear.testFlag(GPSDataContainer::HasAltitude))
        {
            if (newGPSData.hasAltitude())
            {
                didSomething = true;
                newGPSData.clearAltitude();
            }
        }
        if (flagsToClear.testFlag(GPSDataContainer::HasNSatellites))
        {
            if (newGPSData.hasNSatellites())
            {
                didSomething = true;
                newGPSData.clearNSatellites();
            }
        }
        if (flagsToClear.testFlag(GPSDataContainer::HasDop))
        {
            if (newGPSData.hasDop())
            {
                didSomething = true;
                newGPSData.clearDop();
            }
        }
        if (flagsToClear.testFlag(GPSDataContainer::HasFixType))
        {
            if (newGPSData.hasFixType())
            {
                didSomething = true;
                newGPSData.clearFixType();
            }
        }
        if (flagsToClear.testFlag(GPSDataContainer::HasSpeed))
        {
            if (newGPSData.hasSpeed())
            {
                didSomething = true;
                newGPSData.clearSpeed();
            }
        }
        if (didSomething)
        {
            gpsItem->setGPSData(newGPSData);
            undoInfo.readNewDataFromItem(gpsItem);
            undoCommand->addUndoInfo(undoInfo);
        }
    }

    if (undoCommand->affectedItemCount()>0)
    {
        undoCommand->setText(undoDescription);
        emit(signalUndoCommand(undoCommand));
    }
    else
    {
        delete undoCommand;
    }
}

void GPSListViewContextMenu::slotRemoveCoordinates()
{
    removeInformationFromSelectedImages(GPSDataContainer::HasCoordinates, i18n("Remove coordinates information"));
}

void GPSListViewContextMenu::slotRemoveAltitude()
{
    removeInformationFromSelectedImages(GPSDataContainer::HasAltitude, i18n("Remove altitude information"));
}

void GPSListViewContextMenu::slotRemoveUncertainty()
{
    removeInformationFromSelectedImages(
            GPSDataContainer::HasNSatellites|GPSDataContainer::HasDop|GPSDataContainer::HasFixType,
            i18n("Remove uncertainty information")
        );
}

void GPSListViewContextMenu::setEnabled(const bool state)
{
    d->enabled = state;
}

void GPSListViewContextMenu::slotRemoveSpeed()
{
    removeInformationFromSelectedImages(GPSDataContainer::HasSpeed, i18n("Remove speed"));
}

} // namespace KIPIGPSSyncPlugin
