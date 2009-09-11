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

namespace KIPIGPSSyncPlugin
{

class GPSTrackListEditDialogPrivate
{

public:

    GPSTrackListEditDialogPrivate()
    {
        worldMap = 0;
        about    = 0;
        imagesList = 0;
    }

    KIPIPlugins::KPAboutData *about;
    KIPIPlugins::ImagesList  *imagesList;

    GPSTrackList              gpsTrackList;

    GPSTrackListWidget       *worldMap;

    KIPI::Interface          *interface;
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
                                       i18n("Changed"), true);

    // add the context menu provider to the imagesList:
    new GPSListViewContextMenu(d->imagesList);

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

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    connect(d->worldMap, SIGNAL(signalNewGPSLocationFromMap(int, double, double, double)),
            this, SLOT(slotNewGPSLocationFromMap(int, double, double, double)));

    connect(d->worldMap, SIGNAL(signalMarkerSelectedFromMap(int)),
            this, SLOT(slotMarkerSelectedFromMap(int)));

    // ---------------------------------------------------------------

    KUrl::List urls;
    for( GPSTrackList::iterator it = d->gpsTrackList.begin() ;
         it != d->gpsTrackList.end() ; ++it)
    {
        GPSTrackListViewItem *item = new GPSTrackListViewItem(d->imagesList->listView(),
                                                              it.value().url());
        item->setGPSInfo(it.key(), it.value());
        urls.append(it.value().url());
    }
    d->interface->thumbnails(urls, 64);

    readSettings();
    QTimer::singleShot(0, this, SLOT(slotUpdateWorldMap()));
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

void GPSTrackListEditDialog::slotUpdateWorldMap()
{
    d->worldMap->resized();
}

void GPSTrackListEditDialog::resizeEvent(QResizeEvent *e)
{
    if (!e) return;
    slotUpdateWorldMap();
}

void GPSTrackListEditDialog::slotCancel()
{
    saveSettings();
    done(Cancel);
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

    d->worldMap->setMapType(mapType);
    d->worldMap->setZoomLevel(group.readEntry("Track List Zoom Level", 8));
    d->worldMap->setTrackList(d->gpsTrackList);
    d->worldMap->resized();
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

void GPSTrackListEditDialog::slotOk()
{
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
            if (lvItem->id() == id)
            {
                d->imagesList->listView()->setCurrentItem(lvItem);
                d->imagesList->listView()->scrollToItem(lvItem);
                return;
            }
        }
        ++it;
    }
}

void GPSTrackListEditDialog::slotNewGPSLocationFromMap(int id, double lat, double lng, double alt)
{
    QTreeWidgetItemIterator it(d->imagesList->listView());
    while (*it)
    {
        GPSTrackListViewItem *lvItem = dynamic_cast<GPSTrackListViewItem*>(*it);
        if (lvItem)
        {
            if (lvItem->id() == id)
            {
                GPSTrackListItem info = lvItem->gpsInfo();
                GPSDataContainer data = info.gpsData();
                data.setLatitude(lat);
                data.setLongitude(lng);
                data.setAltitude(alt);
                info.setGPSData(data);
                info.setDirty(true);
                lvItem->setGPSInfo(lvItem->dateTime(), info);

                // Update track list info.
                d->gpsTrackList.remove(lvItem->dateTime());
                d->gpsTrackList.insert(lvItem->dateTime(), info);

                d->imagesList->listView()->setCurrentItem(lvItem);
                d->imagesList->listView()->scrollToItem(lvItem);
                return;
            }
        }
        ++it;
    }
}

}  // namespace KIPIGPSSyncPlugin
