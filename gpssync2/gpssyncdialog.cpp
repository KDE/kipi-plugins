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
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "gpssyncdialog.h"
#include "gpssyncdialog.moc"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QPointer>
#include <QRadioButton>
#include <QSplitter>
#include <QTreeView>

// KDE includes

#include <kapplication.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobalsettings.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kseparator.h>
#include <ksqueezedtextlabel.h>
#include <kstandarddirs.h>
#include <ktabwidget.h>
#include <ktoolinvocation.h>

// WorldMapWidget2 includes

#include <worldmapwidget2/worldmapwidget2.h>

// Local includes

#include "kipiimagemodel.h"
#include "gpsimageitem.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "mapdragdrophandler.h"
#include "kipiimagelist.h"
#include "gpsimagelistdragdrophandler.h"
#include "previewmanager.h"
#include "gpssettingswidget.h"
#include "gpscorrelatorwidget.h"

namespace KIPIGPSSyncPlugin
{

class GPSSyncDialogPriv
{
public:

    GPSSyncDialogPriv()
    : interface(0),
      about(0),
      mapWidget(0)
    {
    }

    KIPI::Interface          *interface;
    KIPIPlugins::KPAboutData *about;
    KipiImageModel           *imageModel;
    QItemSelectionModel      *selectionModel;
    MapDragDropHandler       *mapDragDropHandler;

    KTabWidget               *tabWidget;
    QSplitter                *splitter1;
    QSplitter                *splitter2;
    WMW2::WorldMapWidget2    *mapWidget;
    KipiImageList            *treeView;
    KIPIPlugins::PreviewManager *previewManager;
    GPSSettingsWidget        *settingsWidget;
    GPSCorrelatorWidget      *correlatorWidget;
    GPSSyncWMWRepresentativeChooser *representativeChooser;
};

GPSSyncDialog::GPSSyncDialog(KIPI::Interface* interface, QWidget* parent)
             : KDialog(parent), d(new GPSSyncDialogPriv)
{
    d->interface = interface;

    setButtons(Apply|Close);
    setDefaultButton(Close);
    setCaption(i18n("Geolocation"));
    setModal(true);
    d->imageModel = new KipiImageModel(this);
    d->imageModel->setKipiInterface(d->interface);
    GPSImageItem::setHeaderData(d->imageModel);
    d->imageModel->setSupportedDragActions(Qt::CopyAction);
    d->selectionModel = new QItemSelectionModel(d->imageModel);
    d->mapDragDropHandler = new MapDragDropHandler(d->imageModel, this);
    d->representativeChooser = new GPSSyncWMWRepresentativeChooser(d->imageModel, this);

    d->splitter2 = new QSplitter(Qt::Horizontal, this);
    setMainWidget(d->splitter2);

    d->splitter1 = new QSplitter(Qt::Vertical, d->splitter2);
    d->splitter2->addWidget(d->splitter1);
    d->splitter2->setStretchFactor(0, 10);

    d->mapWidget = new WMW2::WorldMapWidget2(this);
    d->mapWidget->setRepresentativeChooser(d->representativeChooser);
    d->mapWidget->setEditModeAvailable(true);
    d->mapWidget->setDisplayMarkersModel(d->imageModel, GPSImageItem::RoleCoordinates, d->selectionModel);
    d->mapWidget->setDragDropHandler(d->mapDragDropHandler);

    QWidget* const dummyWidget = new QWidget(this);
    QVBoxLayout* const vbox = new QVBoxLayout(dummyWidget);
    vbox->addWidget(d->mapWidget);
    vbox->addWidget(d->mapWidget->getControlWidget());
    d->splitter1->addWidget(dummyWidget);

    d->treeView = new KipiImageList(d->interface, this);
    d->treeView->setModel(d->imageModel, d->selectionModel);
    d->treeView->setDragDropHandler(new GPSImageListDragDropHandler(this));
    // TODO: save and restore the state of the header
    // TODO: add a context menu to the header to select which columns should be visible
    // TODO: add sorting by column
    d->treeView->view()->setDragEnabled(true);
    d->treeView->view()->setDragDropMode(QAbstractItemView::DragOnly);
    d->treeView->view()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->splitter1->addWidget(d->treeView);

    d->tabWidget = new KTabWidget(d->splitter2);
    d->splitter2->setCollapsible(1, true);

    d->previewManager = new KIPIPlugins::PreviewManager(d->tabWidget);
    // TODO: why is the minimum size hardcoded to 400x300???
    d->previewManager->setMinimumSize(QSize(200, 200));
    d->tabWidget->addTab(d->previewManager, i18n("Image viewer"));

    d->correlatorWidget = new GPSCorrelatorWidget(d->tabWidget, d->imageModel, marginHint(), spacingHint());
    d->tabWidget->addTab(d->correlatorWidget, i18n("GPS Correlator"));

    d->settingsWidget = new GPSSettingsWidget(d->tabWidget);
    d->tabWidget->addTab(d->settingsWidget, i18n("Settings"));

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("GPS Sync"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Plugin to synchronize pictures' metadata with a GPS device"),
                   ki18n("(c) 2006-2009, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"),
                        ki18n("Developer and maintainer"),
                              "caulier dot gilles at gmail dot com");

    d->about->addAuthor(ki18n("Michael G. Hansen"),
                        ki18n("Developer and maintainer"),
                              "mike at mghansen dot de");

    connect(d->selectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(slotCurrentImageChanged(const QModelIndex&, const QModelIndex&)));

    connect(d->treeView->view(), SIGNAL(activated(const QModelIndex&)),
            this, SLOT(slotImageActivated(const QModelIndex&)));

    connect(d->correlatorWidget, SIGNAL(signalSetUIEnabled(const bool)),
            this, SLOT(slotSetUIEnabled(const bool)));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotApplyClicked()));

    readSettings();
}

GPSSyncDialog::~GPSSyncDialog()
{
    delete d->about;
    delete d;
}

void GPSSyncDialog::setImages( const KUrl::List& images )
{
    for( KUrl::List::ConstIterator it = images.begin(); it != images.end(); ++it )
    {
        GPSImageItem* const newItem = new GPSImageItem(d->interface, *it);
        d->imageModel->addItem(newItem);
    }

    d->interface->thumbnails(images, 64);
}

void GPSSyncDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync 2 Settings"));

    // TODO: sanely determine a default backend
    d->mapWidget->readSettingsFromGroup(&group);
    d->correlatorWidget->readSettingsFromGroup(&group);
    d->tabWidget->setCurrentIndex(group.readEntry("Current Tab", 0));

    if (group.hasKey("SplitterState V1"))
    {
        const QByteArray splitterState = QByteArray::fromBase64(group.readEntry(QString("SplitterState V1"), QByteArray()));
        if (!splitterState.isEmpty())
        {
            d->splitter1->restoreState(splitterState);
        }
    }

    KConfigGroup group2 = config.group(QString("GPS Sync 2 Dialog"));
    restoreDialogSize(group2);
}

void GPSSyncDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync 2 Settings"));
    group.writeEntry(QString("SplitterState V1"), d->splitter1->saveState().toBase64());

    d->mapWidget->saveSettingsToGroup(&group);
    d->correlatorWidget->saveSettingsToGroup(&group);
    group.writeEntry("Current Tab", d->tabWidget->currentIndex());

    KConfigGroup group2 = config.group(QString("GPS Sync 2 Dialog"));
    saveDialogSize(group2);

    config.sync();
}

void GPSSyncDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;

    // TODO: prompt the user before changing with unsafed data!

    saveSettings();
    e->accept();
}

void GPSSyncDialog::slotCurrentImageChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);

    // TODO: unset the image if no image is the current image
    if (!current.isValid())
        return;

    GPSImageItem* const item = dynamic_cast<GPSImageItem*>(d->imageModel->itemFromIndex(current));
    if (!item)
        return;

    d->previewManager->load(item->url().toLocalFile(), true);
}

void GPSSyncDialog::slotImageActivated(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    GPSImageItem* const item = dynamic_cast<GPSImageItem*>(d->imageModel->itemFromIndex(index));
    if (!item)
        return;

    const WMW2::WMWGeoCoordinate imageCoordinates = item->coordinates();
    if (imageCoordinates.hasCoordinates())
    {
        d->mapWidget->setCenter(imageCoordinates);
    }
}

void GPSSyncDialog::slotSetUIEnabled(const bool enabledState, QObject* const cancelObject, const QString& cancelSlot)
{
    d->correlatorWidget->setUIEnabledExternal(enabledState);
}

void GPSSyncDialog::slotSetUIEnabled(const bool enabledState)
{
    slotSetUIEnabled(enabledState, 0, QString());
}

class GPSSyncWMWRepresentativeChooserPrivate
{
public:
    GPSSyncWMWRepresentativeChooserPrivate()
    {
    }

    KipiImageModel* model;
};

GPSSyncWMWRepresentativeChooser::GPSSyncWMWRepresentativeChooser(KipiImageModel* const model, QObject* const parent)
: WMW2::WMWRepresentativeChooser(parent), d(new GPSSyncWMWRepresentativeChooserPrivate())
{
    d->model = model;

    connect(d->model, SIGNAL(signalThumbnailForIndexAvailable(const QPersistentModelIndex&, const QPixmap&)),
            this, SLOT(slotThumbnailFromModel(const QPersistentModelIndex&, const QPixmap&)));
}

GPSSyncWMWRepresentativeChooser::~GPSSyncWMWRepresentativeChooser()
{
}

QPixmap GPSSyncWMWRepresentativeChooser::pixmapFromRepresentativeIndex(const QVariant& index, const QSize& size)
{
    return d->model->getPixmapForIndex(index.value<QPersistentModelIndex>(), qMax(size.width(), size.height()));
}

QVariant GPSSyncWMWRepresentativeChooser::bestRepresentativeIndexFromList(const QList<QVariant>& list, const int sortKey)
{
    // TODO: sorting!
    return list.first();
}

void GPSSyncWMWRepresentativeChooser::slotThumbnailFromModel(const QPersistentModelIndex& index, const QPixmap& pixmap)
{
    emit(signalThumbnailAvailableForIndex(QVariant::fromValue(index), pixmap));
}

bool GPSSyncWMWRepresentativeChooser::indicesEqual(const QVariant& indexA, const QVariant& indexB)
{
    const QPersistentModelIndex a = indexA.value<QPersistentModelIndex>();
    const QPersistentModelIndex b = indexB.value<QPersistentModelIndex>();

    return a==b;
}

void GPSSyncDialog::slotApplyClicked()
{
    // TODO: save the user's changes
}

}  // namespace KIPIGPSSyncPlugin
