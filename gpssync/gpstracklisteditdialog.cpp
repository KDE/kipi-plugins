/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-07
 * Description : a dialog to edit GPS track list.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QLayout>
#include <QTimer>

// KDE includes.

#include <k3listview.h>
#include <ktoolinvocation.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <khtmlview.h>
#include <kmenu.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "gpstracklistwidget.h"
#include "gpstracklistviewitem.h"
#include "gpstracklisteditdialog.h"
#include "gpstracklisteditdialog.moc"

namespace KIPIGPSSyncPlugin
{

class GPSTrackListView : public K3ListView
{
public :

    GPSTrackListView(QWidget *parent) 
        : K3ListView(parent)
    {
        addColumn( i18n("Thumbnail") );
        addColumn( i18n("Id") );
        addColumn( i18n("File Name") );
        addColumn( i18n("Date") );
        addColumn( i18n("Latitude") );
        addColumn( i18n("Longitude") );
        addColumn( i18n("Altitude") );
        addColumn( i18n("Dirty") );
        setResizeMode(Q3ListView::AllColumns);
        setAllColumnsShowFocus(true);
        setSorting(-1);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setSelectionMode(Q3ListView::Single);
    }
    
    ~GPSTrackListView() 
    {
    }
    
protected:

    void contentsMousePressEvent(QMouseEvent*)
    {
        // Nothing todo. No user interraction are possible with googlemap when 
        // user select an item from the list. This is a limitation of googlemaps
        // interface. Only an item is selected from the list when user select a 
        // marker from a map.
    }
};

class GPSTrackListEditDialogPrivate
{

public:

    GPSTrackListEditDialogPrivate()
    {
        worldMap = 0;
        about    = 0;
        listView = 0;
    }

    K3ListView               *listView;

    KIPIPlugins::KPAboutData *about; 

    GPSTrackList              gpsTrackList;

    GPSTrackListWidget       *worldMap;

    KIPI::Interface          *interface;
};

GPSTrackListEditDialog::GPSTrackListEditDialog(KIPI::Interface* interface, QWidget *parent, const GPSTrackList& gpsTrackList)
                      : KDialog(parent)
{
    d = new GPSTrackListEditDialogPrivate;
    d->interface    = interface;
    d->gpsTrackList = gpsTrackList;

    setButtons(Help | Ok | Cancel);
    setDefaultButton(Ok);
    setCaption(i18n("Edit Track List Geographical Coordinates"));
    setModal(true);

    QWidget *page = new QWidget( this );
    setMainWidget( page );

    QVBoxLayout* vlay = new QVBoxLayout(page);
    d->listView       = new GPSTrackListView(page);
    d->worldMap       = new GPSTrackListWidget(page);
    d->worldMap->show();

    // ---------------------------------------------------------------

    vlay->addWidget(d->worldMap->view(), 10);
    vlay->addWidget(d->listView, 4);
    vlay->setSpacing(spacingHint());
    vlay->setMargin(0);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("GPS Sync"),
                   QByteArray(),
                   KAboutData::License_GPL,
                   ki18n("A Plugin to edit GPS track list"),
                   ki18n("(c) 2008, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"), 
                       ki18n("Developer and maintainer"),
                             "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KPushButton *helpButton = button( Help );
    KHelpMenu* helpMenu     = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook       = new QAction(i18n("Plugin Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    helpButton->setDelayedMenu( helpMenu->menu() );

    // ---------------------------------------------------------------

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    connect(d->worldMap, SIGNAL(signalNewGPSLocationFromMap(int, double, double)),
            this, SLOT(slotNewGPSLocationFromMap(int, double, double)));

    connect(d->interface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
            this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));

    // ---------------------------------------------------------------

    KUrl::List urls;
    for( GPSTrackList::iterator it = d->gpsTrackList.begin() ; 
         it != d->gpsTrackList.end() ; ++it)
    {
        GPSTrackListViewItem *item = new GPSTrackListViewItem(d->listView, d->listView->lastItem());
        item->setData(it.key(), it.value());
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

void GPSTrackListEditDialog::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    Q3ListViewItemIterator it(d->listView);
    while (it.current())
    {
        GPSTrackListViewItem *item = dynamic_cast<GPSTrackListViewItem*>(it.current());
        if (item->url() == url)
        {
            item->setThumbnail(pix);
            return;
        }
        ++it;
    }
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

    d->worldMap->setZoomLevel(group.readEntry("Track List Zoom Level", 8));
    d->worldMap->setMapType(group.readEntry("Track List Map Type", QString("G_MAP_TYPE")));

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

void GPSTrackListEditDialog::slotNewGPSLocationFromMap(int id, double lat, double lng)
{
    Q3ListViewItemIterator it(d->listView);
    while (it.current())
    {
        GPSTrackListViewItem *item = dynamic_cast<GPSTrackListViewItem*>(it.current());
        if (item->id() == id)
        {
            GPSTrackListItem info = item->gpsInfo();
            GPSDataContainer data = info.gpsData();
            data.setLatitude(lat);
            data.setLongitude(lng);
            info.setGPSData(data);
            info.setDirty(true);
            item->setData(item->dateTime(), info);

            // Update track list info.
            d->gpsTrackList.remove(item->dateTime());
            d->gpsTrackList.insert(item->dateTime(), info);

            // Refresh item in list view
            d->listView->repaintItem(item);
            d->listView->setCurrentItem(item);
            d->listView->ensureItemVisible(item);

            kDebug() << id << "::" << lat << "::" << lng << endl;
            return;
        }
        ++it;
    }
}

}  // namespace KIPIGPSSyncPlugin
