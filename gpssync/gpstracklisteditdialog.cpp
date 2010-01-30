/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-07
 * Description : a dialog to edit GPS track list.
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "gpstracklisteditdialog.h"
#include "gpstracklisteditdialog.moc"

// Qt includes

#include <QTimer>
#include <QTreeWidgetItemIterator>
#include <QVBoxLayout>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <khtmlview.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Local includes

#include "gpstracklistviewitem.h"
#include "gpstracklistwidget.h"
#include "imageslist.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "gpslistviewcontextmenu.h"
#include "gpsbookmarkowner.h"

namespace KIPIGPSSyncPlugin
{

class GPSTrackListEditDialogPrivate
{

public:

    GPSTrackListEditDialogPrivate()
    {
        about         = 0;
        imagesList    = 0;
        worldMap      = 0;
        interface     = 0;
        bookmarkOwner = 0;
    }

    KIPIPlugins::KPAboutData *about;
    KIPIPlugins::ImagesList  *imagesList;

    GPSTrackList              gpsTrackList;

    GPSTrackListWidget       *worldMap;

    KIPI::Interface          *interface;

    GPSBookmarkOwner         *bookmarkOwner;
};

GPSTrackListEditDialog::GPSTrackListEditDialog(KIPI::Interface* interface, QWidget *parent, const GPSTrackList& gpsTrackList)
                      : KDialog(parent), d(new GPSTrackListEditDialogPrivate)
{
    d->interface    = interface;
    d->gpsTrackList = gpsTrackList;

    setButtons(Help | Ok | Cancel);
    setDefaultButton(Ok);
    setCaption(i18n("Edit Track List Geographical Coordinates"));
    setModal(true);

    QWidget *page = new QWidget( this );
    setMainWidget( page );

    QVBoxLayout* vlay = new QVBoxLayout(page);
    d->imagesList     = new KIPIPlugins::ImagesList(d->interface, this);
    d->imagesList->setControlButtonsPlacement(KIPIPlugins::ImagesList::NoControlButtons);
    d->imagesList->enableDragAndDrop(false);
    d->imagesList->setAllowRAW(true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User1,
                                       i18n("Id"), true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User2,
                                       i18n("Date"), true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User3,
                                       i18n("Latitude"), true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User4,
                                       i18n("Longitude"), true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User5,
                                       i18n("Altitude"), true);
    d->imagesList->listView()->setColumn(KIPIPlugins::ImagesListView::User6,
                                       i18nc("entry changed", "Changed"), true);

    d->bookmarkOwner = new GPSBookmarkOwner(this);
    
    // add the context menu provider to the imagesList:
    GPSListViewContextMenu* const gpsListViewContextMenu =
                new GPSListViewContextMenu(d->imagesList, d->bookmarkOwner);

    d->worldMap       = new GPSTrackListWidget(page);
    d->worldMap->show();

    // ---------------------------------------------------------------

    vlay->addWidget(d->worldMap->view(), 10);
    vlay->addWidget(d->imagesList, 4);
    vlay->setSpacing(spacingHint());
    vlay->setMargin(0);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("GPS Sync"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A plugin to edit GPS track lists."),
                   ki18n("(c) 2008-2009, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"),
                        ki18n("Developer and maintainer"),
                              "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ---------------------------------------------------------------

    connect(d->worldMap, SIGNAL(signalNewGPSLocationFromMap(int, double, double, double)),
            this, SLOT(slotNewGPSLocationFromMap(int, double, double, double)));

    connect(d->worldMap, SIGNAL(signalMarkerSelectedFromMap(int)),
            this, SLOT(slotMarkerSelectedFromMap(int)));

    connect(gpsListViewContextMenu, SIGNAL(signalItemsChanged(const QList<QTreeWidgetItem*>&)),
            this, SLOT(slotListItemsChanged(const QList<QTreeWidgetItem*>&)));

    // ---------------------------------------------------------------

    KUrl::List urls;
    for( GPSTrackList::iterator it = d->gpsTrackList.begin() ;
         it != d->gpsTrackList.end() ; ++it)
    {
        GPSTrackListViewItem *item = new GPSTrackListViewItem(d->imagesList->listView(),
                                                              it->url());
        item->setGPSInfo(*it);
        urls.append(it->url());
    }
    d->interface->thumbnails(urls, 64);

    // this loads the settings and forwards the gpsTrackList to the map:
    readSettings();
}

GPSTrackListEditDialog::~GPSTrackListEditDialog()
{
    delete d->about;
    delete d;
}

void GPSTrackListEditDialog::slotHelp()
{
    KToolInvocation::invokeHelp("gpssync", "kipi-plugins");
}

GPSTrackList GPSTrackListEditDialog::trackList() const
{
    return d->gpsTrackList;
}

void GPSTrackListEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void GPSTrackListEditDialog::updateWorldMap()
{
    QTimer::singleShot(0, d->worldMap, SLOT(slotResized()));
}

void GPSTrackListEditDialog::resizeEvent(QResizeEvent *e)
{
    if (!e) return;
    updateWorldMap();
}

void GPSTrackListEditDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync Settings"));

    KConfigGroup group2 = config.group(QString("GPS Track List Edit Dialog"));
    restoreDialogSize(group2);

    QString mapType = group.readEntry("Track List Map Type", QString("G_NORMAL_MAP"));

    if (mapType == QString("G_SATELLITE_TYPE"))   mapType = QString("G_SATELLITE_MAP");
    else if (mapType == QString("G_MAP_TYPE"))    mapType = QString("G_NORMAL_MAP");
    else if (mapType == QString("G_HYBRID_TYPE")) mapType = QString("G_HYBRID_MAP");

    // This parameter is reserved for future use in case we need to quickly add options
    // between releases. They can be added relatively easily by the users without recompiling.
    d->worldMap->setExtraOptions(group.readEntry("Extra Options", ""));

    d->worldMap->setMapType(mapType);
    d->worldMap->setZoomLevel(group.readEntry("Track List Zoom Level", 8));
    d->worldMap->setTrackList(d->gpsTrackList);
    updateWorldMap();
}

void GPSTrackListEditDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync Settings"));

    KConfigGroup group2 = config.group(QString("GPS Track List Edit Dialog"));
    saveDialogSize(group2);

    group.writeEntry("Track List Zoom Level", d->worldMap->zoomLevel());
    group.writeEntry("Track List Map Type", d->worldMap->mapType());
    config.sync();
}

void GPSTrackListEditDialog::slotButtonClicked(int button)
{
    if (button != KDialog::Ok)
    {
        KDialog::slotButtonClicked(button);
        return;
    }

    saveSettings();
    accept();
}

void GPSTrackListEditDialog::slotMarkerSelectedFromMap(int id)
{
    QTreeWidgetItemIterator it(d->imagesList->listView());
    while (*it)
    {
        GPSTrackListViewItem *lvItem = dynamic_cast<GPSTrackListViewItem*>(*it);
        if (lvItem)
        {
            if (lvItem->gpsInfo().id() == id)
            {
                d->imagesList->listView()->setCurrentItem(lvItem);
                d->imagesList->listView()->scrollToItem(lvItem);
                return;
            }
        }
        ++it;
    }
}

void GPSTrackListEditDialog::slotListItemsChanged(const QList<QTreeWidgetItem*>& changedItems)
{
    for (QList<QTreeWidgetItem*>::const_iterator it = changedItems.constBegin();
         it!=changedItems.constEnd(); ++it)
    {
        GPSTrackListViewItem *lvItem = dynamic_cast<GPSTrackListViewItem*>(*it);
        if (lvItem)
        {
            // TODO: this is only valid as long as no items are deleted!
            const int itemId = lvItem->gpsInfo().id();
            for (int i = 0; i<d->gpsTrackList.size(); ++i)
            {
                if (d->gpsTrackList.at(i).id() == itemId)
                {
                    d->gpsTrackList[i] = lvItem->gpsInfo();
                    break;
                }
            }
            break;
        }
    }

    // re-load the items into the map:
    d->worldMap->setTrackList(d->gpsTrackList);
    updateWorldMap();
}

void GPSTrackListEditDialog::slotNewGPSLocationFromMap(int id, double lat, double lng, double alt)
{
    QTreeWidgetItemIterator it(d->imagesList->listView());
    while (*it)
    {
        GPSTrackListViewItem *lvItem = dynamic_cast<GPSTrackListViewItem*>(*it);
        if (lvItem)
        {
            if (lvItem->gpsInfo().id() == id)
            {
                GPSTrackListItem info = lvItem->gpsInfo();
                GPSDataContainer data = info.gpsData();
                data.setLatitude(lat);
                data.setLongitude(lng);
                data.setAltitude(alt);
                info.setGPSData(data);
                info.setDirty(true);
                lvItem->setGPSInfo(info);

                // Update track list info.
                for (int i=0; i<d->gpsTrackList.size(); ++i)
                {
                    if (d->gpsTrackList.at(i).id()==id)
                    {
                        d->gpsTrackList[i] = info;
                        break;
                    }
                }

                d->imagesList->listView()->setCurrentItem(lvItem);
                d->imagesList->listView()->scrollToItem(lvItem);
                return;
            }
        }
        ++it;
    }
}

}  // namespace KIPIGPSSyncPlugin
