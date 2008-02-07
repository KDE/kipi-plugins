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

    QGridLayout* grid = new QGridLayout(page);

    // ---------------------------------------------------------------

    d->listView = new K3ListView(page);
    d->listView->addColumn( i18n("Id") );
    d->listView->addColumn( i18n("Thumbnail") );
    d->listView->addColumn( i18n("File Name") );
    d->listView->setResizeMode(Q3ListView::AllColumns);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setSorting(-1);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setSelectionMode(Q3ListView::Single);

    // ---------------------------------------------------------------

    d->worldMap = new GPSTrackListWidget(page);
    d->worldMap->show();

    // ---------------------------------------------------------------

    grid->addWidget(d->listView,         0, 0, 1, 1);
    grid->addWidget(d->worldMap->view(), 0, 1, 1, 1);
    grid->setColumnStretch(0, 3);
    grid->setColumnStretch(1, 10);
    grid->setSpacing(spacingHint());
    grid->setMargin(0);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("GPS Sync"),
                   QByteArray(),
                   KAboutData::License_GPL,
                   ki18n("A Plugin to synchronize pictures metadata with a GPS device"),
                   ki18n("(c) 2006-2008, Gilles Caulier"));

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

    connect(d->worldMap, SIGNAL(signalNewGPSLocationFromMap(const QString&, const QString&)),
            this, SLOT(slotNewGPSLocationFromMap(const QString&, const QString&)));

    connect(d->interface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
            this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));

    // ---------------------------------------------------------------

    KUrl::List urls;
    for( GPSTrackList::iterator it = d->gpsTrackList.begin() ; 
         it != d->gpsTrackList.end() ; ++it)
    {
        GPSTrackListViewItem *item = new GPSTrackListViewItem(d->listView, d->listView->lastItem());
        item->setGPSInfo(*it);
        urls.append((*it).url());
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

    d->worldMap->setGPSPosition("0", "0");
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

void GPSTrackListEditDialog::slotNewGPSLocationFromMap(const QString& lat, const QString& lon)
{
    // TODO
}

void GPSTrackListEditDialog::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    QPixmap pixmap = pix.scaled(64, 64, Qt::KeepAspectRatio);
    Q3ListViewItemIterator it(d->listView);

    while (it.current())
    {
        GPSTrackListViewItem *item = dynamic_cast<GPSTrackListViewItem*>(it.current());
        if (item->url() == url)
        {
            item->setThumbnail(pixmap);
        }
        ++it;
    }
}

}  // namespace KIPIGPSSyncPlugin
