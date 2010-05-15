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
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
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

#include <qtconcurrentmap.h>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QFuture>
#include <QFutureWatcher>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QPointer>
#include <QProgressBar>
#include <QRadioButton>
#include <QSplitter>
#include <QTreeView>
#include <QUndoView>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QHBoxLayout>

// KDE includes

#include <kaction.h>
#include <kapplication.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialogbuttonbox.h>
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
#include <kundostack.h>
#include <kvbox.h>

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
#include "gpsundocommand.h"

namespace KIPIGPSSyncPlugin
{

struct SaveChangedImagesHelper
{
public:
    SaveChangedImagesHelper(KipiImageModel* const model)
    : imageModel(model)
    {
    }

    typedef QPair<KUrl, QString> result_type;
    KipiImageModel* const imageModel;

    QPair<KUrl, QString> operator()(const QPersistentModelIndex& itemIndex)
    {
        GPSImageItem* const item = dynamic_cast<GPSImageItem*>(imageModel->itemFromIndex(itemIndex));
        if (!item)
            return QPair<KUrl, QString>(KUrl(), QString());

        return QPair<KUrl, QString>(item->url(), item->saveChanges());
    }
};

class GPSSyncDialogPriv
{
public:

    GPSSyncDialogPriv()
    : interface(0),
      about(0),
      mapWidget(0),
      uiEnabled(true),
      splitterSize(0)
    {
    }

    KIPI::Interface          *interface;
    KIPIPlugins::KPAboutData *about;
    KipiImageModel           *imageModel;
    QItemSelectionModel      *selectionModel;
    MapDragDropHandler       *mapDragDropHandler;

    KDialogButtonBox         *buttonBox;
    KTabWidget               *tabWidget;
    QSplitter                *VSplitter;
    QSplitter                *HSplitter;
    WMW2::WorldMapWidget2    *mapWidget;
    KipiImageList            *treeView;
    KIPIPlugins::PreviewManager *previewManager;
    GPSSettingsWidget        *settingsWidget;
    GPSCorrelatorWidget      *correlatorWidget;
    GPSSyncWMWRepresentativeChooser *representativeChooser;
    bool uiEnabled;
    QFuture<QPair<KUrl,QString> > changedFilesSaveFuture;
    QFutureWatcher<QPair<KUrl,QString> > *changedFilesSaveFutureWatcher;
    int changedFilesCountDone;
    int changedFilesCountTotal;
    bool changedFilesCloseAfterwards;
    QProgressBar *progressBar;
    KUndoStack *undoStack;
    QUndoView *undoView;
    QAction *sortActionOldestFirst;
    QAction *sortActionYoungestFirst;

    QStackedWidget	     *stackedWidget;
    QTabBar                  *tabBar;
    int splitterSize;
};

GPSSyncDialog::GPSSyncDialog(KIPI::Interface* interface, QWidget* parent)
             : KDialog(parent), d(new GPSSyncDialogPriv)
{
    d->interface = interface;

    setButtons(0);
    setCaption(i18n("Geolocation"));
    setModal(true);
    d->imageModel = new KipiImageModel(this);
    d->imageModel->setKipiInterface(d->interface);
    GPSImageItem::setHeaderData(d->imageModel);
    d->imageModel->setSupportedDragActions(Qt::CopyAction);
    d->selectionModel = new QItemSelectionModel(d->imageModel);
    d->mapDragDropHandler = new MapDragDropHandler(d->imageModel, this);
    d->representativeChooser = new GPSSyncWMWRepresentativeChooser(d->imageModel, this);

    d->undoStack = new KUndoStack(this);

    KVBox* const vboxMain = new KVBox(this);
    setMainWidget(vboxMain);

    KHBox* const hboxMain = new KHBox(vboxMain);
    hboxMain->layout()->setSpacing(spacingHint());
    hboxMain->layout()->setMargin(marginHint());

    d->HSplitter = new QSplitter(Qt::Horizontal, hboxMain);
    d->HSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    KHBox* const hboxBottom = new KHBox(vboxMain);
    hboxBottom->layout()->setSpacing(spacingHint());
    hboxBottom->layout()->setMargin(marginHint());

    d->progressBar = new QProgressBar(hboxBottom);
    d->progressBar->setVisible(false);
    d->progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    // we need a really large stretch factor here because the QDialogButtonBox also stretches a lot...
    dynamic_cast<QHBoxLayout*>(hboxBottom->layout())->setStretch(200, 0);

    d->buttonBox = new KDialogButtonBox(hboxBottom);
    d->buttonBox->addButton(KStandardGuiItem::apply(), QDialogButtonBox::AcceptRole, this, SLOT(slotApplyClicked()));
    d->buttonBox->addButton(KStandardGuiItem::close(), QDialogButtonBox::RejectRole, this, SLOT(close()));

    d->VSplitter = new QSplitter(Qt::Vertical, d->HSplitter);
    d->HSplitter->addWidget(d->VSplitter);
    d->HSplitter->setStretchFactor(0, 10);

    d->mapWidget = new WMW2::WorldMapWidget2(this);
    d->mapWidget->setRepresentativeChooser(d->representativeChooser);
    d->mapWidget->setEditModeAvailable(true);
    d->mapWidget->setDisplayMarkersModel(d->imageModel, GPSImageItem::RoleCoordinates, d->selectionModel);
    d->mapWidget->setDragDropHandler(d->mapDragDropHandler);
    d->mapWidget->setDoUpdateMarkerCoordinatesInModel(false);

    QMenu* const sortMenu = new QMenu(this);
    sortMenu->setTitle(i18n("Sorting"));
    QActionGroup* const sortOrderExclusive = new QActionGroup(sortMenu);
    sortOrderExclusive->setExclusive(true);
    connect(sortOrderExclusive, SIGNAL(triggered(QAction*)),
            this, SLOT(slotSortOptionTriggered(QAction*)));

    d->sortActionOldestFirst = new KAction(i18n("Show oldest first"), sortOrderExclusive);
    d->sortActionOldestFirst->setCheckable(true);
    sortMenu->addAction(d->sortActionOldestFirst);

    d->sortActionYoungestFirst = new KAction(i18n("Show youngest first"), sortOrderExclusive);
    sortMenu->addAction(d->sortActionYoungestFirst);
    d->sortActionYoungestFirst->setCheckable(true);

    d->mapWidget->setSortOptionsMenu(sortMenu);

    QWidget* const dummyWidget = new QWidget(this);
    QVBoxLayout* const vbox = new QVBoxLayout(dummyWidget);
    vbox->addWidget(d->mapWidget);
    vbox->addWidget(d->mapWidget->getControlWidget());
    d->VSplitter->addWidget(dummyWidget);

    d->treeView = new KipiImageList(d->interface, this);
    d->treeView->setModel(d->imageModel, d->selectionModel);
    d->treeView->setDragDropHandler(new GPSImageListDragDropHandler(this));
    // TODO: save and restore the state of the header
    // TODO: add a context menu to the header to select which columns should be visible
    // TODO: add sorting by column
    d->treeView->view()->setDragEnabled(true);
    d->treeView->view()->setDragDropMode(QAbstractItemView::DragOnly);
    d->treeView->view()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->VSplitter->addWidget(d->treeView);

    d->HSplitter->setCollapsible(1, true);

    d->stackedWidget = new QStackedWidget(d->HSplitter);
    d->HSplitter->addWidget(d->stackedWidget);          
    d->splitterSize = 0;

    KVBox* vboxTabBar = new KVBox(hboxMain);
    vboxTabBar->layout()->setSpacing(0);
    vboxTabBar->layout()->setMargin(0);
    d->tabBar = new QTabBar(vboxTabBar);
    d->tabBar->setShape(QTabBar::RoundedEast);

    dynamic_cast<QVBoxLayout*>(vboxTabBar->layout())->addStretch(200);


    d->tabBar->addTab("Image viewer");                
    d->tabBar->addTab("GPS Correlator");
    d->tabBar->addTab("Settings");
    d->tabBar->addTab("Undo/Redo");                  
    
    d->tabBar->installEventFilter(this);       

    d->previewManager = new KIPIPlugins::PreviewManager(d->stackedWidget);
    // TODO: why is the minimum size hardcoded to 400x300???
    
    d->previewManager->setMinimumSize(QSize(200, 200));
    d->stackedWidget->addWidget(d->previewManager);           

    d->correlatorWidget = new GPSCorrelatorWidget(d->stackedWidget, d->imageModel, marginHint(), spacingHint());
    d->stackedWidget->addWidget(d->correlatorWidget);             

    d->settingsWidget = new GPSSettingsWidget(d->stackedWidget);
    d->stackedWidget->addWidget(d->settingsWidget);              
    
    d->undoView = new QUndoView(d->undoStack, d->stackedWidget);
    d->stackedWidget->addWidget(d->undoView);                     


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

    connect(d->mapWidget, SIGNAL(signalDisplayMarkersMoved(const QList<QPersistentModelIndex>&, const WMW2::WMWGeoCoordinate&)),
            this, SLOT(slotMapMarkersMoved(const QList<QPersistentModelIndex>&, const WMW2::WMWGeoCoordinate&)));

    connect(d->selectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(slotCurrentImageChanged(const QModelIndex&, const QModelIndex&)));

    connect(d->treeView->view(), SIGNAL(activated(const QModelIndex&)),
            this, SLOT(slotImageActivated(const QModelIndex&)));

    connect(d->correlatorWidget, SIGNAL(signalSetUIEnabled(const bool)),
            this, SLOT(slotSetUIEnabled(const bool)));

    connect(d->correlatorWidget, SIGNAL(signalProgressSetup(const int, const QString&)),
            this, SLOT(slotProgressSetup(const int, const QString&)));

    connect(d->correlatorWidget, SIGNAL(signalProgressChanged(const int)),
            this, SLOT(slotProgressChanged(const int)));

    connect(d->correlatorWidget, SIGNAL(signalUndoCommand(GPSUndoCommand*)),
            this, SLOT(slotGPSUndoCommand(GPSUndoCommand*)));

     connect(this, SIGNAL(applyClicked()),
             this, SLOT(slotApplyClicked()));

    readSettings();

}

GPSSyncDialog::~GPSSyncDialog()
{
    delete d->about;
    delete d;
}


bool GPSSyncDialog::eventFilter( QObject* o, QEvent* e)
{
    if ( ( o == d->tabBar ) && ( e->type() == QEvent::MouseButtonPress ) )
    {
        QMouseEvent const *m = static_cast<QMouseEvent *>(e);           

        QPoint p (m->x(), m->y());
        const int var = d->tabBar->tabAt(p);

        QList<int> sizes = d->HSplitter->sizes();
        if (d->splitterSize == 0)
        {
            if (d->tabBar->currentIndex() == var)
            {
                d->splitterSize = sizes[1];
                sizes[1] = 0;
            }
        }
        else
        {
            sizes[1] = d->splitterSize;
            d->splitterSize = 0;
        }

        d->tabBar->setCurrentIndex(var);
        d->stackedWidget->setCurrentIndex(var);
        d->HSplitter->setSizes(sizes);

        return true;                        
    }

    return QWidget::eventFilter(o,e);
}

void GPSSyncDialog::setCurrentTab(int index)
{

    d->tabBar->setCurrentIndex(index);
    d->stackedWidget->setCurrentIndex(index);

    QList<int> sizes = d->HSplitter->sizes();
    if (d->splitterSize != 0)
    {
        sizes[1] = d->splitterSize;
        d->splitterSize = 0;
    }
    d->HSplitter->setSizes(sizes);
}

void GPSSyncDialog::setImages( const KUrl::List& images )
{
    for( KUrl::List::ConstIterator it = images.begin(); it != images.end(); ++it )
    {
        GPSImageItem* const newItem = new GPSImageItem(d->interface, *it);
        d->imageModel->addItem(newItem);
    }
}

void GPSSyncDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync 2 Settings"));

    // TODO: sanely determine a default backend
    d->mapWidget->readSettingsFromGroup(&group);
    d->correlatorWidget->readSettingsFromGroup(&group);
    d->treeView->readSettingsFromGroup(&group);
    setCurrentTab(group.readEntry("Current Tab", 0));
    const bool showOldestFirst = group.readEntry("Show oldest images first", false);

    if (showOldestFirst)
    {
        d->sortActionOldestFirst->setChecked(true);
    }
    else
    {
        d->sortActionYoungestFirst->setChecked(true);
    }

    if (group.hasKey("SplitterState V1"))
    {
        const QByteArray splitterState = QByteArray::fromBase64(group.readEntry(QString("SplitterState V1"), QByteArray()));
        if (!splitterState.isEmpty())
        {
            d->VSplitter->restoreState(splitterState);
        }
    }
    if (group.hasKey("SplitterState H1"))
    {
        const QByteArray splitterState = QByteArray::fromBase64(group.readEntry(QString("SplitterState H1"), QByteArray()));
        if (!splitterState.isEmpty())
        {
            d->HSplitter->restoreState(splitterState);
        }
    }
    d->splitterSize = group.readEntry("Splitter H1 CollapsedSize", 0);

    KConfigGroup group2 = config.group(QString("GPS Sync 2 Dialog"));
    restoreDialogSize(group2);
}

void GPSSyncDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync 2 Settings"));
    group.writeEntry(QString("SplitterState V1"), d->VSplitter->saveState().toBase64());
    group.writeEntry(QString("SplitterState H1"), d->HSplitter->saveState().toBase64());
    group.writeEntry("Splitter H1 CollapsedSize", d->splitterSize);

    d->mapWidget->saveSettingsToGroup(&group);
    d->correlatorWidget->saveSettingsToGroup(&group);
    d->treeView->saveSettingsToGroup(&group);
    group.writeEntry("Current Tab", d->tabBar->currentIndex());
    group.writeEntry("Show oldest images first", d->sortActionOldestFirst->isChecked());
    KConfigGroup group2 = config.group(QString("GPS Sync 2 Dialog"));
    saveDialogSize(group2);

    config.sync();
}

void GPSSyncDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;

    // is the UI locked?
    if (!d->uiEnabled)
    {
        // please wait until we are done ...
        return;
    }

    // are there any modified images?
    int dirtyImagesCount = 0;
    for (int i=0; i<d->imageModel->rowCount(); ++i)
    {
        const QModelIndex itemIndex = d->imageModel->index(i, 0);
        GPSImageItem* const item = dynamic_cast<GPSImageItem*>(d->imageModel->itemFromIndex(itemIndex));

        if (item->isDirty())
        {
            dirtyImagesCount++;
        }
    }

    if (dirtyImagesCount>0)
    {
        const QString message = i18np(
                    "You have 1 modified image.",
                    "You have %1 modified images.",
                    dirtyImagesCount
                );

        const int chosenAction = KMessageBox::warningYesNoCancel(this,
            i18n("%1 Would you like to save the changes you made to them?", message),
            i18n("Unsaved changes"),
            KGuiItem(i18n("Save changes")),
            KGuiItem(i18n("Close and discard changes"))
            );

        if (chosenAction==KMessageBox::No)
        {
            saveSettings();
            e->accept();
            return;
        }
        if (chosenAction==KMessageBox::Yes)
        {
            // the user wants to save his changes.
            // this will initiate the saving process and then close the dialog.
            saveChanges(true);
        }

        // do not close the dialog for now
        e->ignore();
        return;
    }

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
    if (enabledState)
    {
        // hide the progress bar
        d->progressBar->setVisible(false);
    }

    // TODO: disable the worldmapwidget and the images list (at least disable editing operations)
    d->uiEnabled = enabledState;
    d->buttonBox->setEnabled(enabledState);
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
    const bool oldestFirst = sortKey & 1;

    QPersistentModelIndex bestIndex;
    QDateTime bestTime;
    for (int i=0; i<list.count(); ++i)
    {
        const QPersistentModelIndex currentIndex = list.at(i).value<QPersistentModelIndex>();
        const GPSImageItem* const currentItem = static_cast<GPSImageItem*>(d->model->itemFromIndex(currentIndex));
        const QDateTime currentTime = currentItem->dateTime();

        bool takeThisIndex = bestTime.isNull();
        if (!takeThisIndex)
        {
            if (oldestFirst)
            {
                takeThisIndex = currentTime < bestTime;
            }
            else
            {
                takeThisIndex = bestTime < currentTime;
            }
        }
        if (takeThisIndex)
        {
            bestIndex = currentIndex;
            bestTime = currentTime;
        }
    }

    return QVariant::fromValue(bestIndex);
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

void GPSSyncDialog::saveChanges(const bool closeAfterwards)
{
    // TODO: actually save the changes
    // are there any modified images?
    QList<QPersistentModelIndex> dirtyImages;
    for (int i=0; i<d->imageModel->rowCount(); ++i)
    {
        const QModelIndex itemIndex = d->imageModel->index(i, 0);
        GPSImageItem* const item = dynamic_cast<GPSImageItem*>(d->imageModel->itemFromIndex(itemIndex));

        if (item->isDirty())
        {
            dirtyImages << itemIndex;
        }
    }

    if (dirtyImages.isEmpty())
    {
        if (closeAfterwards)
        {
            close();
        }
        return;
    }

    // TODO: disable the UI and provide progress and cancel information
    slotSetUIEnabled(false);
    slotProgressSetup(dirtyImages.count(), i18n("Saving changes - %p%"));

    // initiate the saving
    d->changedFilesCountDone = 0;
    d->changedFilesCountTotal = dirtyImages.count();
    d->changedFilesCloseAfterwards = closeAfterwards;
    d->changedFilesSaveFutureWatcher = new QFutureWatcher<QPair<KUrl, QString> >(this);
    connect(d->changedFilesSaveFutureWatcher, SIGNAL(resultsReadyAt(int, int)),
            this, SLOT(slotFileChangesSaved(int, int)));

    d->changedFilesSaveFuture = QtConcurrent::mapped(dirtyImages, SaveChangedImagesHelper(d->imageModel));
    d->changedFilesSaveFutureWatcher->setFuture(d->changedFilesSaveFuture);
}

void GPSSyncDialog::slotFileChangesSaved(int beginIndex, int endIndex)
{
    kDebug()<<beginIndex<<endIndex;
    d->changedFilesCountDone+=(endIndex-beginIndex);
    slotProgressChanged(d->changedFilesCountDone);
    if (d->changedFilesCountDone==d->changedFilesCountTotal)
    {
        slotSetUIEnabled(true);

        // any errors?
        QList<QPair<KUrl, QString> > errorList;
        for (int i=0; i<d->changedFilesSaveFuture.resultCount(); ++i)
        {
            if (!d->changedFilesSaveFuture.resultAt(i).second.isEmpty())
                errorList << d->changedFilesSaveFuture.resultAt(i);
        }
        if (!errorList.isEmpty())
        {
            QStringList errorStrings;
            for (int i=0; i<errorList.count(); ++i)
            {
                // TODO: how to do kurl->qstring?
                errorStrings << QString("%1: %2").arg(errorList.at(i).first.toLocalFile()).arg(errorList.at(i).second);
            }
            KMessageBox::errorList(this, i18n("Failed to save some information:"), errorStrings, i18n("Error"));
        }

        // done saving files
        if (d->changedFilesCloseAfterwards)
        {
            close();
        }
    }
}

void GPSSyncDialog::slotApplyClicked()
{
    // save the changes, but do not close afterwards
    saveChanges(false);
}

void GPSSyncDialog::slotProgressChanged(const int currentProgress)
{
    d->progressBar->setValue(currentProgress);
}

void GPSSyncDialog::slotProgressSetup(const int maxProgress, const QString& progressText)
{
    d->progressBar->setFormat(progressText);
    d->progressBar->setMaximum(maxProgress);
    d->progressBar->setValue(0);
    d->progressBar->setVisible(true);
}

void GPSSyncDialog::slotMapMarkersMoved(const QList<QPersistentModelIndex>& movedMarkers, const WMW2::WMWGeoCoordinate& coordinates)
{
    GPSUndoCommand* const undoCommand = new GPSUndoCommand();

    for (int i=0; i<movedMarkers.count(); ++i)
    {
        const QPersistentModelIndex itemIndex = movedMarkers.at(i);
        GPSImageItem* const item = static_cast<GPSImageItem*>(d->imageModel->itemFromIndex(itemIndex));
        const GPSDataContainer oldData = item->gpsData();
        GPSDataContainer newData = oldData;
        newData.setCoordinates(coordinates);
        item->setGPSData(newData);

        kDebug()<<oldData.m_coordinates<<newData.m_coordinates;
        undoCommand->addUndoInfo(GPSUndoCommand::UndoInfo(itemIndex, oldData, newData));
    }
    undoCommand->setText(i18np("1 image moved",
                               "%1 images moved", movedMarkers.count()));

    d->undoStack->push(undoCommand);
}

void GPSSyncDialog::slotGPSUndoCommand(GPSUndoCommand* undoCommand)
{
    d->undoStack->push(undoCommand);
}

void GPSSyncDialog::slotSortOptionTriggered(QAction* sortAction)
{
    int newSortKey = 0;
    if (d->sortActionOldestFirst->isChecked())
    {
        newSortKey|=1;
    }

    d->mapWidget->setSortKey(newSortKey);
}

}  // namespace KIPIGPSSyncPlugin
