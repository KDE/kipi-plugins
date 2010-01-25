/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-22
 * Description : a dialog to edit GPS positions
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#include "gpseditdialog.h"
#include "gpseditdialog.moc"

// Qt includes

#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTimer>
#include <QValidator>
#include <QToolButton>

// KDE includes

#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <khtmlview.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Local includes

#include "gpsmapwidget.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "gpsbookmarkowner.h"

namespace KIPIGPSSyncPlugin
{

typedef QPair<GPSDataContainer, QString>    RecentPair;
typedef QList<RecentPair>                   RecentPairList;

class GPSEditDialogPrivate
{

public:

    GPSEditDialogPrivate()
    {
        altitudeInput  = 0;
        latitudeInput  = 0;
        longitudeInput = 0;
        worldMap       = 0;
        about          = 0;
        goButton       = 0;
        bookmarkButton = 0;
        bookmarkOwner  = 0;
        recentButton   = 0;
        recentMenu     = 0;
        hasGPSInfo     = false;
    }

    bool                      hasGPSInfo;

    QPushButton              *goButton;
    QToolButton              *bookmarkButton;
    GPSBookmarkOwner         *bookmarkOwner;
    QToolButton              *recentButton;
    KMenu                    *recentMenu;
    QToolButton              *configButton;
    QActionGroup             *configApiGroup;
    QActionGroup             *configAltitudeGroup;

    KLineEdit                *altitudeInput;
    KLineEdit                *latitudeInput;
    KLineEdit                *longitudeInput;

    KIPIPlugins::KPAboutData *about;

    GPSDataContainer          gpsData;
    QString                   fileName;
    RecentPairList            recentLocations;

    GPSMapWidget             *worldMap;
};

GPSEditDialog::GPSEditDialog(QWidget* parent, const GPSDataContainer& gpsData,
                             const QString& fileName, bool hasGPSInfo)
             : KDialog(parent), d(new GPSEditDialogPrivate)
{
    d->hasGPSInfo = hasGPSInfo;
    d->gpsData    = gpsData;
    d->fileName   = fileName;

    setButtons(Help | Ok | Cancel);
    setDefaultButton(Ok);
    setCaption(i18n("%1 - Edit Geographical Coordinates", fileName));
    setModal(true);

    QWidget *page = new QWidget( this );
    setMainWidget( page );

    QGridLayout* grid = new QGridLayout(page);

    QLabel *message   = new QLabel(i18n("<p>Use the map on the right to select the location where "
                                        "the picture was taken. Click with left mouse button or move the marker "
                                        "on the map to get the GPS coordinates.</p>"), page);
    message->setWordWrap(true);

    QLabel *altitudeLabel  = new QLabel(i18n("Altitude:"), page);
    QLabel *latitudeLabel  = new QLabel(i18n("Latitude:"), page);
    QLabel *longitudeLabel = new QLabel(i18n("Longitude:"), page);

    d->altitudeInput       = new KLineEdit(page);
    d->latitudeInput       = new KLineEdit(page);
    d->longitudeInput      = new KLineEdit(page);

    d->altitudeInput->setClearButtonShown(true);
    d->latitudeInput->setClearButtonShown(true);
    d->longitudeInput->setClearButtonShown(true);

    d->altitudeInput->setValidator(new QDoubleValidator(-20000.0, 20000.0, 1, this));
    d->latitudeInput->setValidator(new QDoubleValidator(-90.0, 90.0, 12, this));
    d->longitudeInput->setValidator(new QDoubleValidator(-180.0, 180.0, 12, this));

    d->goButton = new QPushButton(i18n("Go to Location"), page);
    d->goButton->setEnabled(false);

    d->bookmarkOwner = new GPSBookmarkOwner(this);
    d->bookmarkOwner->setPositionProvider(getCurrentPosition, this);

    d->bookmarkButton = new QToolButton(this);
    d->bookmarkButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->bookmarkButton->setIcon(SmallIcon("bookmarks"));
    d->bookmarkButton->setText(i18n("Bookmarks"));
    d->bookmarkButton->setPopupMode(QToolButton::InstantPopup);
    d->bookmarkButton->setMenu(d->bookmarkOwner->getMenu());
    d->bookmarkButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    d->recentButton = new QToolButton(this);
    d->recentButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->recentButton->setIcon(SmallIcon("document-open-recent"));
    d->recentButton->setText(i18n("Recent locations"));
    d->recentButton->setPopupMode(QToolButton::InstantPopup);
    d->recentButton->setEnabled(false);
    d->recentButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    d->recentMenu = new KMenu(this);
    d->recentButton->setMenu(d->recentMenu);

    d->configButton = new QToolButton(this);
    d->configButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    d->configButton->setIcon(SmallIcon("configuration"));
    d->configButton->setText(i18n("Configuration"));
    d->configButton->setPopupMode(QToolButton::InstantPopup);
    d->configButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    KMenu* const configMenu = new KMenu(this);
    d->configButton->setMenu(configMenu);

    d->configApiGroup = new QActionGroup(configMenu);
    d->configApiGroup->setExclusive(true);
    KAction* const configActionMapsApi2 = new KAction(i18n("Google Maps API V2"), configMenu);
    configActionMapsApi2->setCheckable(true);
    configActionMapsApi2->setData(2);
    d->configApiGroup->addAction(configActionMapsApi2);
    configMenu->addAction(configActionMapsApi2);
    KAction* const configActionMapsApi3 = new KAction(i18n("Google Maps API V3"), configMenu);
    configActionMapsApi3->setCheckable(true);
    configActionMapsApi3->setData(3);
    d->configApiGroup->addAction(configActionMapsApi3);
    configMenu->addAction(configActionMapsApi3);

    configMenu->addSeparator();

    d->configAltitudeGroup = new QActionGroup(configMenu);
    d->configAltitudeGroup->setExclusive(true);
    KAction* const configActionAltitudeNone = new KAction(i18n("No altitude lookup"), configMenu);
    configActionAltitudeNone->setCheckable(true);
    configActionAltitudeNone->setData("none");
    d->configAltitudeGroup->addAction(configActionAltitudeNone);
    configMenu->addAction(configActionAltitudeNone);
    KAction* const configActionAltitudeGeoNames = new KAction(i18n("Use geonames.org for altitude lookup"), configMenu);
    configActionAltitudeGeoNames->setCheckable(true);
    configActionAltitudeGeoNames->setData("geonames");
    d->configAltitudeGroup->addAction(configActionAltitudeGeoNames);
    configMenu->addAction(configActionAltitudeGeoNames);
    KAction* const configActionAltitudeTopocoding = new KAction(i18n("Use topocoding for altitude lookup"), configMenu);
    configActionAltitudeTopocoding->setCheckable(true);
    configActionAltitudeTopocoding->setData("topocoding");
    d->configAltitudeGroup->addAction(configActionAltitudeTopocoding);
    configMenu->addAction(configActionAltitudeTopocoding);

    d->worldMap = new GPSMapWidget(page);
    d->worldMap->setFileName(fileName);
    d->worldMap->show();

    grid->addWidget(message,             0, 0, 1, 3);
    grid->addWidget(altitudeLabel,       1, 0, 1, 3);
    grid->addWidget(d->altitudeInput,    2, 0, 1, 3);
    grid->addWidget(latitudeLabel,       3, 0, 1, 3);
    grid->addWidget(d->latitudeInput,    4, 0, 1, 3);
    grid->addWidget(longitudeLabel,      5, 0, 1, 3);
    grid->addWidget(d->longitudeInput,   6, 0, 1, 3);
    grid->addWidget(d->goButton,         7, 0, 1, 3);
    grid->addWidget(d->bookmarkButton,   8, 0, 1, 3);
    grid->addWidget(d->recentButton,     9, 0, 1, 3);
    grid->addWidget(d->configButton,    10, 0, 1, 3);
    grid->addWidget(d->worldMap->view(), 0, 3, 12, 1);
    grid->setColumnStretch(0, 3);
    grid->setColumnStretch(3, 10);
    grid->setRowStretch(11, 10);
    grid->setSpacing(spacingHint());
    grid->setMargin(0);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("GPS Sync"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Plugin to edit geographical coordinates"),
                   ki18n("(c) 2006-2009, Gilles Caulier"));

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

    connect(d->altitudeInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotGPSPositionChanged()));

    connect(d->latitudeInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotGPSPositionChanged()));

    connect(d->longitudeInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotGPSPositionChanged()));

    connect(d->worldMap, SIGNAL(signalNewGPSLocationFromMap(const QString&, const QString&, const QString&)),
            this, SLOT(slotNewGPSLocationFromMap(const QString&, const QString&, const QString&)));

    connect(d->goButton, SIGNAL(released()),
            this, SLOT(slotGotoLocation()));

    connect(d->bookmarkOwner, SIGNAL(positionSelected(GPSDataContainer)),
            this, SLOT(slotBookmarkSelected(GPSDataContainer)));

    connect(d->recentButton, SIGNAL(triggered(QAction*)),
            this, SLOT(slotRecentSelected(QAction*)));

    connect(configMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotConfigActionTriggered(QAction*)));

    // ---------------------------------------------------------------

    readSettings();
    QTimer::singleShot(0, this, SLOT(slotUpdateWorldMap()));
}

GPSEditDialog::~GPSEditDialog()
{
    delete d->about;
    delete d;
}

void GPSEditDialog::slotHelp()
{
    KToolInvocation::invokeHelp("gpssync", "kipi-plugins");
}

void GPSEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;

    saveSettings();
    e->accept();
}

void GPSEditDialog::slotGPSPositionChanged()
{
    d->goButton->setEnabled(true);
}

void GPSEditDialog::slotBookmarkSelected(GPSDataContainer position)
{
    d->altitudeInput->setText(position.altitudeString());
    d->latitudeInput->setText(position.latitudeString());
    d->longitudeInput->setText(position.longitudeString());

    // push the go button:
    slotGotoLocation();
}

void GPSEditDialog::slotGotoLocation()
{
    if (!checkGPSLocation()) return;
    d->worldMap->setGPSPosition(d->latitudeInput->text(), d->longitudeInput->text());
    slotUpdateWorldMap();
}

void GPSEditDialog::slotUpdateWorldMap()
{
    d->worldMap->resized();
}

void GPSEditDialog::resizeEvent(QResizeEvent *e)
{
    if (!e) return;
    slotUpdateWorldMap();
}

void GPSEditDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync Settings"));

    KConfigGroup group2 = config.group(QString("GPS Edit Dialog"));
    restoreDialogSize(group2);

    QString mapType = group.readEntry("Map Type", QString("G_NORMAL_MAP"));

    if (mapType == QString("G_SATELLITE_TYPE"))   mapType = QString("G_SATELLITE_MAP");
    else if (mapType == QString("G_MAP_TYPE"))    mapType = QString("G_NORMAL_MAP");
    else if (mapType == QString("G_HYBRID_TYPE")) mapType = QString("G_HYBRID_MAP");

    d->worldMap->setMapType(mapType);
    d->worldMap->setZoomLevel(group.readEntry("Zoom Level", 1));
    d->worldMap->setApiVersion(group.readEntry("API Version", 2));
    for (int i=0; i<d->configApiGroup->actions().size(); ++i)
    {
        if (d->configApiGroup->actions().at(i)->data().toInt()==d->worldMap->apiVersion())
        {
            d->configApiGroup->actions().at(i)->setChecked(true);
            break;
        }
    }
    d->worldMap->setAltitudeService(group.readEntry("Altitude Service", "geonames"));
    for (int i=0; i<d->configAltitudeGroup->actions().size(); ++i)
    {
        if (d->configAltitudeGroup->actions().at(i)->data().toString()==d->worldMap->altitudeService())
        {
            d->configAltitudeGroup->actions().at(i)->setChecked(true);
            break;
        }
    }

    // load recent locations:
    const int recentCount = qMin(5, group.readEntry("GPS Recent Count", 0));
    for (int i = 0; i<recentCount; ++i)
    {
        const QString recentGeoUrl = group.readEntry(QString("GPS Recent %1 GeoUrl").arg(i+1), QString());
        const QString recentFilename = group.readEntry(QString("GPS Recent %1 Filename").arg(i+1), QString());

        bool okay;
        const GPSDataContainer recentLocation = GPSDataContainer::fromGeoUrl(recentGeoUrl, &okay);

        if (okay)
        {
            d->recentLocations.append(RecentPair(recentLocation, recentFilename));

            const QString recentActionTitle = QString("%1 (from %2)").arg(recentGeoUrl).arg(recentFilename);
            KAction* const recentAction = new KAction(recentActionTitle, this);
            recentAction->setData(recentGeoUrl);
            d->recentMenu->addAction(recentAction);
        }
    }
    if (!d->recentMenu->actions().isEmpty())
    {
        d->recentButton->setEnabled(true);
    }

    d->altitudeInput->blockSignals(true);
    d->latitudeInput->blockSignals(true);
    d->longitudeInput->blockSignals(true);

    // set the initial position:
    GPSDataContainer initialPosition;
    if (d->hasGPSInfo)
    {
        initialPosition = d->gpsData;
    }
    else if (!d->recentLocations.isEmpty())
    {
            initialPosition = d->recentLocations.first().first;
    }
    d->altitudeInput->setText(initialPosition.altitudeString());
    d->latitudeInput->setText(initialPosition.latitudeString());
    d->longitudeInput->setText(initialPosition.longitudeString());

    d->altitudeInput->blockSignals(false);
    d->latitudeInput->blockSignals(false);
    d->longitudeInput->blockSignals(false);

    d->worldMap->setGPSPosition(d->latitudeInput->text(), d->longitudeInput->text());
    d->worldMap->resized();
}

void GPSEditDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync Settings"));

    KConfigGroup group2 = config.group(QString("GPS Edit Dialog"));
    saveDialogSize(group2);

    const int recentCount = qMin(5, d->recentLocations.size());
    group.writeEntry("GPS Recent Count", recentCount);
    for (int i=0; i<recentCount; ++i)
    {
        const RecentPair recentPair = d->recentLocations.at(i);
        group.writeEntry(QString("GPS Recent %1 GeoUrl").arg(i+1), recentPair.first.geoUrl());
        group.writeEntry(QString("GPS Recent %1 Filename").arg(i+1), recentPair.second);
    }

    group.writeEntry("Zoom Level", d->worldMap->zoomLevel());
    group.writeEntry("Map Type", d->worldMap->mapType());
    group.writeEntry("API Version", d->worldMap->apiVersion());
    group.writeEntry("Altitude Service", d->worldMap->altitudeService());
    config.sync();
}

GPSDataContainer GPSEditDialog::getGPSInfo()
{
    return GPSDataContainer(d->altitudeInput->text().toDouble(),
                            d->latitudeInput->text().toDouble(),
                            d->longitudeInput->text().toDouble(),
                            false);
}

bool GPSEditDialog::checkGPSLocation()
{
    bool ok;

    d->altitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Altitude value is invalid."),
                           i18n("Edit Geographical Coordinates"));
        return false;
    }

    d->latitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Latitude value is invalid."),
                           i18n("Edit Geographical Coordinates"));
        return false;
    }

    d->longitudeInput->text().toDouble(&ok);
    if (!ok)
    {
        KMessageBox::error(this, i18n("Longitude value is invalid."),
                           i18n("Edit Geographical Coordinates"));
        return false;
    }

    return true;
}

void GPSEditDialog::slotButtonClicked(int button)
{
    if (button != KDialog::Ok)
    {
        KDialog::slotButtonClicked(button);
        return;
    }
    
    if (!checkGPSLocation()) return;

    // store the current location in the recent list:
    const GPSDataContainer currentLocation = getGPSInfo();
    for (int i=0; i<d->recentLocations.size(); ++i)
    {
        if (d->recentLocations.at(i).first.sameCoordinatesAs(currentLocation))
        {
            d->recentLocations.removeAt(i);
            break;
        }
    }
    d->recentLocations.prepend(RecentPair(currentLocation, d->fileName));

    saveSettings();
    accept();
}

void GPSEditDialog::slotNewGPSLocationFromMap(const QString& lat, const QString& lon, const QString& alt)
{
    d->latitudeInput->setText(lat);
    d->longitudeInput->setText(lon);
    d->altitudeInput->setText(alt);
    d->goButton->setEnabled(false);
}

bool GPSEditDialog::getCurrentPosition(GPSDataContainer* position, void* mydata)
{
    if (!position || !mydata)
        return false;

    GPSEditDialog* me = reinterpret_cast<GPSEditDialog*>(mydata);
    *position = me->getGPSInfo();

    return true;
}

void GPSEditDialog::slotRecentSelected(QAction* action)
{
    const QString urlString = action->data().toString();

    bool okay;
    const GPSDataContainer recentLocation = GPSDataContainer::fromGeoUrl(urlString, &okay);

    if (!okay)
        return;

    slotBookmarkSelected(recentLocation);
}

void GPSEditDialog::slotConfigActionTriggered(QAction* action)
{
    if (QMetaType::Type(action->data().type())==QMetaType::QString)
    {
        const QString altitudeService = action->data().toString();
        d->worldMap->setAltitudeService(altitudeService);
        d->worldMap->resized();
    }
    else
    {
        const int apiVersion = action->data().toInt();
        d->worldMap->setApiVersion(apiVersion);
        d->worldMap->resized();
    }
}

}  // namespace KIPIGPSSyncPlugin
