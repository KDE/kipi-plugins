/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2006-05-16
 * @brief  A plugin to synchronize pictures with a GPS device.
 *
 * @author Copyright (C) 2006-2010 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2010, 2011 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 * @author Copyright (C) 2010 by Gabriel Voicu
 *         <a href="mailto:ping dot gabi at gmail dot com">ping dot gabi at gmail dot com</a>
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

#include "gpssync_common.h"
#include "gpssyncdialog.moc"

// Qt includes

#include <qtconcurrentmap.h>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QFuture>
#include <QFutureWatcher>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QPointer>
#include <QProgressBar>
#include <QRadioButton>
#include <QSplitter>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QTimer>
#include <QToolButton>
#include <QTreeView>
#include <QUndoView>

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

// Libkmap includes

#include <libkmap/kmap_widget.h>
#include <libkmap/itemmarkertiler.h>

// Local includes

#include "kipiimagemodel.h"
#include "kipiimageitem.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "mapdragdrophandler.h"
#include "kipiimagelist.h"
#include "gpsimagelistdragdrophandler.h"
#include "gpscorrelatorwidget.h"
#include "gpsundocommand.h"
#include "gpsreversegeocodingwidget.h"
#include "gpsbookmarkowner.h"
#include "gpslistviewcontextmenu.h"
#include "searchwidget.h"
#include "backend-rg.h"
#include "gpsimagedetails.h"
#include "setup.h"

#ifdef GPSSYNC_MODELTEST
#include <modeltest.h>
#endif /* GPSSYNC_MODELTEST */

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
        KipiImageItem* const item = imageModel->itemFromIndex(itemIndex);
        if (!item)
            return QPair<KUrl, QString>(KUrl(), QString());

        return QPair<KUrl, QString>(item->url(), item->saveChanges(true, true));
    }
};

struct LoadFileMetadataHelper
{
public:
    LoadFileMetadataHelper(KipiImageModel* const model)
    : imageModel(model)
    {
    }

    typedef QPair<KUrl, QString> result_type;
    KipiImageModel* const imageModel;

    QPair<KUrl, QString> operator()(const QPersistentModelIndex& itemIndex)
    {
        KipiImageItem* const item = imageModel->itemFromIndex(itemIndex);
        if (!item)
            return QPair<KUrl, QString>(KUrl(), QString());

        item->loadImageData(false, true);

        return QPair<KUrl, QString>(item->url(), QString());
    }
};

class GPSSyncDialogPriv
{
public:

    GPSSyncDialogPriv()
    {
        // TODO: initialize in the initializer list
        interface = 0;
        mapWidget = 0;
        uiEnabled = true;
        splitterSize = 0;
        mapWidget2 = 0;
        setupGlobalObject = SetupGlobalObject::instance();
    }

    // General things
    KIPI::Interface                         *interface;
    KIPIPlugins::KPAboutData                *about;
    KipiImageModel                          *imageModel;
    QItemSelectionModel                     *selectionModel;
    bool                                     uiEnabled;
    SetupGlobalObject                       *setupGlobalObject;
    GPSBookmarkOwner                        *bookmarkOwner;
    KAction                                 *actionBookmarkVisibility;
    GPSListViewContextMenu                  *listViewContextMenu;

    // Loading and saving
    QFuture<QPair<KUrl,QString> >            fileIOFuture;
    QFutureWatcher<QPair<KUrl,QString> >    *fileIOFutureWatcher;
    int                                      fileIOCountDone;
    int                                      fileIOCountTotal;
    bool                                     fileIOCloseAfterSaving;

    // UI
    KDialogButtonBox                        *buttonBox;
    KTabWidget                              *tabWidget;
    QSplitter                               *VSplitter;
    QSplitter                               *HSplitter;
    KipiImageList                           *treeView;
    QStackedWidget                          *stackedWidget;
    QTabBar                                 *tabBar;
    int                                      splitterSize;
    KUndoStack                              *undoStack;
    QUndoView                               *undoView;

    // UI: progress
    QProgressBar                            *progressBar;
    QPushButton                             *progressCancelButton;
    QObject                                 *progressCancelObject;
    QString                                  progressCancelSlot;

    // UI: tab widgets
    GPSImageDetails                         *detailsWidget;
    GPSCorrelatorWidget                     *correlatorWidget;
    GPSReverseGeocodingWidget               *rgWidget;
    SearchWidget                            *searchWidget;

    // map: UI
    MapLayout                                mapLayout;
    QSplitter                               *mapSplitter;
    KMap::KMapWidget                        *mapWidget;
    KMap::KMapWidget                        *mapWidget2;

    // map: helpers
    MapDragDropHandler                      *mapDragDropHandler;
    GPSSyncKMapModelHelper                  *mapModelHelper;
    KMap::ItemMarkerTiler                   *kmapMarkerModel;

    // map: actions
    QAction                                 *sortActionOldestFirst;
    QAction                                 *sortActionYoungestFirst;
    QMenu                                   *sortMenu;
};

GPSSyncDialog::GPSSyncDialog(KIPI::Interface* interface, QWidget* parent)
             : KDialog(parent), d(new GPSSyncDialogPriv)
{
    d->interface = interface;

    setAttribute(Qt::WA_DeleteOnClose, true);

    setButtons(0);
    setCaption(i18n("Geolocation"));
//     setModal(true);
    setMinimumSize(300,400);
    d->imageModel = new KipiImageModel(this);
    d->selectionModel = new QItemSelectionModel(d->imageModel);

#ifdef GPSSYNC_MODELTEST
    new ModelTest(d->imageModel, this);
#endif /* GPSSYNC_MODELTEST */

    
    d->undoStack = new KUndoStack(this);
    d->bookmarkOwner = new GPSBookmarkOwner(d->imageModel, this);
    d->stackedWidget = new QStackedWidget();
    d->searchWidget = new SearchWidget(d->bookmarkOwner, d->imageModel, d->selectionModel, d->stackedWidget);

    d->imageModel->setKipiInterface(d->interface);
    KipiImageItem::setHeaderData(d->imageModel);
    d->imageModel->setSupportedDragActions(Qt::CopyAction);
    d->mapModelHelper = new GPSSyncKMapModelHelper(d->imageModel, d->selectionModel, this);
    d->mapModelHelper->addUngroupedModelHelper(d->bookmarkOwner->bookmarkModelHelper());
    d->mapModelHelper->addUngroupedModelHelper(d->searchWidget->getModelHelper());
    d->mapDragDropHandler = new MapDragDropHandler(d->imageModel, d->mapModelHelper);
    d->kmapMarkerModel = new KMap::ItemMarkerTiler(d->mapModelHelper, this);

    d->actionBookmarkVisibility = new KAction(this);
    d->actionBookmarkVisibility->setIcon(KIcon("user-trash"));
    d->actionBookmarkVisibility->setToolTip(i18n("Display bookmarked positions on the map."));
    d->actionBookmarkVisibility->setCheckable(true);

    KVBox* const vboxMain = new KVBox(this);
    setMainWidget(vboxMain);

    KHBox* const hboxMain = new KHBox(vboxMain);

    d->HSplitter = new QSplitter(Qt::Horizontal, hboxMain);
    d->HSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    KHBox* const hboxBottom = new KHBox(vboxMain);

    d->progressBar = new QProgressBar(hboxBottom);
    d->progressBar->setVisible(false);
    d->progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    // we need a really large stretch factor here because the QDialogButtonBox also stretches a lot...
    dynamic_cast<QHBoxLayout*>(hboxBottom->layout())->setStretch(200, 0);

    d->progressCancelButton = new QPushButton(hboxBottom);
    d->progressCancelButton->setVisible(false);
    d->progressCancelButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    d->progressCancelButton->setIcon(SmallIcon("dialog-cancel"));
    connect(d->progressCancelButton, SIGNAL(clicked()),
            this, SLOT(slotProgressCancelButtonClicked()));

    d->buttonBox = new KDialogButtonBox(hboxBottom);
    d->buttonBox->addButton(KStandardGuiItem::configure(), QDialogButtonBox::ActionRole, this, SLOT(slotConfigureClicked()));
    d->buttonBox->addButton(KStandardGuiItem::apply(), QDialogButtonBox::AcceptRole, this, SLOT(slotApplyClicked()));
    d->buttonBox->addButton(KStandardGuiItem::close(), QDialogButtonBox::RejectRole, this, SLOT(close()));

    // TODO: the code below does not seem to have any effect, slotApplyClicked is still triggered
    //       when 'Enter' is pressed...
    // make sure the 'Apply' button is not triggered when enter is pressed,
    // because that causes problems with the search widget
    QAbstractButton* testButton;
    Q_FOREACH(testButton, d->buttonBox->buttons())
    {
//         if (d->buttonBox->buttonRole(testButton)==QDialogButtonBox::AcceptRole)
        {
            QPushButton* const pushButton = dynamic_cast<QPushButton*>(testButton);
            kDebug()<<pushButton<<pushButton->isDefault();
            if (pushButton)
            {
                pushButton->setDefault(false);
            }
        }
    }
    setDefaultButton(NoDefault);

    d->VSplitter = new QSplitter(Qt::Vertical, d->HSplitter);
    d->HSplitter->addWidget(d->VSplitter);
    d->HSplitter->setStretchFactor(0, 10);

    d->sortMenu = new QMenu(this);
    d->sortMenu->setTitle(i18n("Sorting"));
    QActionGroup* const sortOrderExclusive = new QActionGroup(d->sortMenu);
    sortOrderExclusive->setExclusive(true);
    connect(sortOrderExclusive, SIGNAL(triggered(QAction*)),
            this, SLOT(slotSortOptionTriggered(QAction*)));

    d->sortActionOldestFirst = new KAction(i18n("Show oldest first"), sortOrderExclusive);
    d->sortActionOldestFirst->setCheckable(true);
    d->sortMenu->addAction(d->sortActionOldestFirst);

    d->sortActionYoungestFirst = new KAction(i18n("Show youngest first"), sortOrderExclusive);
    d->sortMenu->addAction(d->sortActionYoungestFirst);
    d->sortActionYoungestFirst->setCheckable(true);

    connect(d->actionBookmarkVisibility, SIGNAL(changed()),
            this, SLOT(slotBookmarkVisibilityToggled()));


    QWidget* mapVBox;
    d->mapWidget = makeMapWidget(&mapVBox);
    d->searchWidget->setPrimaryMapWidget(d->mapWidget);
    d->mapSplitter = new QSplitter(this);
    d->mapSplitter->addWidget(mapVBox);
    d->VSplitter->addWidget(d->mapSplitter);

    d->treeView = new KipiImageList(d->interface, this);
    d->treeView->setModelAndSelectionModel(d->imageModel, d->selectionModel);
    d->treeView->setDragDropHandler(new GPSImageListDragDropHandler(this));
    d->treeView->setDragEnabled(true);
    // TODO: save and restore the state of the header
    // TODO: add a context menu to the header to select which columns should be visible
    // TODO: add sorting by column
    d->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->treeView->setSortingEnabled(true);
    d->VSplitter->addWidget(d->treeView);

    d->listViewContextMenu = new GPSListViewContextMenu(d->treeView, d->bookmarkOwner);

    d->HSplitter->setCollapsible(1, true);

    d->HSplitter->addWidget(d->stackedWidget);          
    d->splitterSize = 0;

    KVBox* vboxTabBar = new KVBox(hboxMain);
    vboxTabBar->layout()->setSpacing(0);
    vboxTabBar->layout()->setMargin(0);

    d->tabBar = new QTabBar(vboxTabBar);
    d->tabBar->setShape(QTabBar::RoundedEast);

    dynamic_cast<QVBoxLayout*>(vboxTabBar->layout())->addStretch(200);

    d->tabBar->addTab(i18n("Details"));
    d->tabBar->addTab(i18n("GPS Correlator"));
    d->tabBar->addTab(i18n("Undo/Redo"));
    d->tabBar->addTab(i18n("Reverse Geocoding"));
    d->tabBar->addTab(i18n("Search"));

    d->tabBar->installEventFilter(this);

    d->detailsWidget = new GPSImageDetails(d->stackedWidget, d->imageModel, marginHint(), spacingHint());
    d->stackedWidget->addWidget(d->detailsWidget);

    d->correlatorWidget = new GPSCorrelatorWidget(d->stackedWidget, d->imageModel, marginHint(), spacingHint());
    d->stackedWidget->addWidget(d->correlatorWidget);

    d->undoView = new QUndoView(d->undoStack, d->stackedWidget);
    d->stackedWidget->addWidget(d->undoView);

    d->rgWidget = new GPSReverseGeocodingWidget(d->interface, d->imageModel, d->selectionModel, d->stackedWidget);
    d->stackedWidget->addWidget(d->rgWidget);

    d->stackedWidget->addWidget(d->searchWidget);


    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("GPS Sync"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Plugin to synchronize pictures' metadata with a GPS device"),
                   ki18n("(c) 2006-2010, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"),
                        ki18n("Developer and maintainer"),
                              "caulier dot gilles at gmail dot com");

    d->about->addAuthor(ki18n("Michael G. Hansen"),
                        ki18n("Developer and maintainer"),
                              "mike at mghansen dot de");

    d->about->addAuthor(ki18n("Gabriel Voicu"),
                        ki18n("Developer"),
                              "ping dot gabi at gmail dot com");

    connect(d->treeView, SIGNAL(signalImageActivated(const QModelIndex&)),
            this, SLOT(slotImageActivated(const QModelIndex&)));

    connect(d->correlatorWidget, SIGNAL(signalSetUIEnabled(const bool)),
            this, SLOT(slotSetUIEnabled(const bool)));

    connect(d->correlatorWidget, SIGNAL(signalSetUIEnabled(const bool, QObject* const, const QString&)),
            this, SLOT(slotSetUIEnabled(const bool, QObject* const, const QString&)));

    connect(d->correlatorWidget, SIGNAL(signalProgressSetup(const int, const QString&)),
            this, SLOT(slotProgressSetup(const int, const QString&)));

    connect(d->correlatorWidget, SIGNAL(signalProgressChanged(const int)),
            this, SLOT(slotProgressChanged(const int)));

    connect(d->correlatorWidget, SIGNAL(signalUndoCommand(GPSUndoCommand*)),
            this, SLOT(slotGPSUndoCommand(GPSUndoCommand*)));

    connect(d->mapModelHelper, SIGNAL(signalUndoCommand(GPSUndoCommand*)),
            this, SLOT(slotGPSUndoCommand(GPSUndoCommand*)));

    connect(d->rgWidget, SIGNAL(signalSetUIEnabled(const bool)),
            this, SLOT(slotSetUIEnabled(const bool)));

    connect(d->rgWidget, SIGNAL(signalSetUIEnabled(const bool, QObject* const, const QString&)),
            this, SLOT(slotSetUIEnabled(const bool, QObject* const, const QString&)));

    connect(d->rgWidget, SIGNAL(signalProgressSetup(const int, const QString&)),
            this, SLOT(slotProgressSetup(const int, const QString&)));

    connect(d->rgWidget, SIGNAL(signalProgressChanged(const int)),
            this, SLOT(slotProgressChanged(const int)));

    connect(d->rgWidget, SIGNAL(signalUndoCommand(GPSUndoCommand*)),
            this, SLOT(slotGPSUndoCommand(GPSUndoCommand*)));

    connect(d->searchWidget, SIGNAL(signalUndoCommand(GPSUndoCommand*)),
            this, SLOT(slotGPSUndoCommand(GPSUndoCommand*)));

    connect(d->listViewContextMenu, SIGNAL(signalUndoCommand(GPSUndoCommand*)),
            this, SLOT(slotGPSUndoCommand(GPSUndoCommand*)));

    connect(this, SIGNAL(applyClicked()),
             this, SLOT(slotApplyClicked()));

    connect(d->tabBar, SIGNAL(currentChanged(int)),
            this, SLOT(slotCurrentTabChanged(int)));

    connect(d->bookmarkOwner->bookmarkModelHelper(), SIGNAL(signalUndoCommand(GPSUndoCommand*)),
            this, SLOT(slotGPSUndoCommand(GPSUndoCommand*)));

    connect(d->detailsWidget, SIGNAL(signalUndoCommand(GPSUndoCommand*)),
            this, SLOT(slotGPSUndoCommand(GPSUndoCommand*)));

    connect(d->setupGlobalObject, SIGNAL(signalSetupChanged()),
            this, SLOT(slotSetupChanged()));

    readSettings();

    d->mapWidget->setActive(true);
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

        if (var<0)
            return false;

        QList<int> sizes = d->HSplitter->sizes();
        if (d->splitterSize == 0)
        {
            if (sizes.at(1) == 0)
            {
                sizes[1] = d->stackedWidget->widget(var)->minimumSizeHint().width();
            }
            else if (d->tabBar->currentIndex() == var)
            {
                d->splitterSize = sizes.at(1);
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
        d->detailsWidget->slotSetActive( (d->stackedWidget->currentWidget()==d->detailsWidget) && (d->splitterSize==0) );

        return true;
    }

    return QWidget::eventFilter(o,e);
}

void GPSSyncDialog::slotCurrentTabChanged(int index)
{
    d->tabBar->setCurrentIndex(index);
    d->stackedWidget->setCurrentIndex(index);
    d->detailsWidget->slotSetActive(d->stackedWidget->currentWidget()==d->detailsWidget);
}


void GPSSyncDialog::setCurrentTab(int index)
{
    d->tabBar->setCurrentIndex(index);
    d->stackedWidget->setCurrentIndex(index);

    QList<int> sizes = d->HSplitter->sizes();
    if (d->splitterSize >= 0)
    {
        sizes[1] = d->splitterSize;
        d->splitterSize = 0;
    }
    d->HSplitter->setSizes(sizes);

    d->detailsWidget->slotSetActive( (d->stackedWidget->currentWidget()==d->detailsWidget) && (d->splitterSize==0) );
}

void GPSSyncDialog::setImages(const KUrl::List& images)
{
    for ( KUrl::List::ConstIterator it = images.begin(); it != images.end(); ++it )
    {
        KipiImageItem* const newItem = new KipiImageItem(d->interface, *it);
        newItem->loadImageData(true, false);
        d->imageModel->addItem(newItem);
    }

    QList<QPersistentModelIndex> imagesToLoad;
    for (int i=0; i<d->imageModel->rowCount(); ++i)
    {
        imagesToLoad << d->imageModel->index(i, 0);
    }

    slotSetUIEnabled(false);
    slotProgressSetup(imagesToLoad.count(), i18n("Loading metadata - %p%"));

    // initiate the saving
    d->fileIOCountDone = 0;
    d->fileIOCountTotal = imagesToLoad.count();
    d->fileIOFutureWatcher = new QFutureWatcher<QPair<KUrl, QString> >(this);
    connect(d->fileIOFutureWatcher, SIGNAL(resultsReadyAt(int, int)),
            this, SLOT(slotFileMetadataLoaded(int, int)));

    d->fileIOFuture = QtConcurrent::mapped(imagesToLoad, LoadFileMetadataHelper(d->imageModel));
    d->fileIOFutureWatcher->setFuture(d->fileIOFuture);
}

void GPSSyncDialog::slotFileMetadataLoaded(int beginIndex, int endIndex)
{
    kDebug()<<beginIndex<<endIndex;
    d->fileIOCountDone+=(endIndex-beginIndex);
    slotProgressChanged(d->fileIOCountDone);

    if (d->fileIOCountDone==d->fileIOCountTotal)
    {
        slotSetUIEnabled(true);
    }
}

void GPSSyncDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync 2 Settings"));

    // --------------------------

    // TODO: sanely determine a default backend
    const KConfigGroup groupMapWidget = KConfigGroup(&group, "Map Widget");
    d->mapWidget->readSettingsFromGroup(&groupMapWidget);

    const KConfigGroup groupCorrelatorWidget = KConfigGroup(&group, "Correlator Widget");
    d->correlatorWidget->readSettingsFromGroup(&groupCorrelatorWidget);

    const KConfigGroup groupTreeView = KConfigGroup(&group, "Tree View");
    d->treeView->readSettingsFromGroup(&groupTreeView);

    const KConfigGroup groupSearchWidget = KConfigGroup(&group, "Search Widget");
    d->searchWidget->readSettingsFromGroup(&groupSearchWidget);

    const KConfigGroup groupRGWidget = KConfigGroup(&group, "Reverse Geocoding Widget");
    d->rgWidget->readSettingsFromGroup(&groupRGWidget);

    const KConfigGroup groupDialog = KConfigGroup(&group, "Dialog");
    restoreDialogSize(groupDialog);

    // --------------------------

    setCurrentTab(group.readEntry("Current Tab", 0));
    const bool showOldestFirst = group.readEntry("Show oldest images first", false);

    if (showOldestFirst)
    {
        d->sortActionOldestFirst->setChecked(true);
        d->mapWidget->setSortKey(1);
    }
    else
    {
        d->sortActionYoungestFirst->setChecked(true);
        d->mapWidget->setSortKey(0);
    }

    d->actionBookmarkVisibility->setChecked(group.readEntry("Bookmarks visible", false));
    slotBookmarkVisibilityToggled();

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

    // ----------------------------------

    d->mapLayout = MapLayout(group.readEntry("Map Layout", QVariant::fromValue(int(MapLayoutOne))).value<int>());
    d->setupGlobalObject->writeEntry("Map Layout", QVariant::fromValue(d->mapLayout));
    adjustMapLayout(false);
    if (d->mapWidget2)
    {
        const KConfigGroup groupMapWidget = KConfigGroup(&group, "Map Widget 2");
        d->mapWidget2->readSettingsFromGroup(&groupMapWidget);

        d->mapWidget2->setActive(true);
    }
}

void GPSSyncDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("GPS Sync 2 Settings"));

    // --------------------------

    KConfigGroup groupMapWidget = KConfigGroup(&group, "Map Widget");
    d->mapWidget->saveSettingsToGroup(&groupMapWidget);

    if (d->mapWidget2)
    {
        KConfigGroup groupMapWidget = KConfigGroup(&group, "Map Widget 2");
        d->mapWidget2->saveSettingsToGroup(&groupMapWidget);
    }

    KConfigGroup groupCorrelatorWidget = KConfigGroup(&group, "Correlator Widget");
    d->correlatorWidget->saveSettingsToGroup(&groupCorrelatorWidget);

    KConfigGroup groupTreeView = KConfigGroup(&group, "Tree View");
    d->treeView->saveSettingsToGroup(&groupTreeView);

    KConfigGroup groupSearchWidget = KConfigGroup(&group, "Search Widget");
    d->searchWidget->saveSettingsToGroup(&groupSearchWidget);

    KConfigGroup groupRGWidget = KConfigGroup(&group, "Reverse Geocoding Widget");
    d->rgWidget->saveSettingsToGroup(&groupRGWidget);

    KConfigGroup groupDialog = KConfigGroup(&group, "Dialog");
    saveDialogSize(groupDialog);

    // --------------------------

    group.writeEntry("Current Tab", d->tabBar->currentIndex());
    group.writeEntry("Show oldest images first", d->sortActionOldestFirst->isChecked());
    group.writeEntry("Bookmarks visible", d->actionBookmarkVisibility->isChecked());
    group.writeEntry(QString("SplitterState V1"), d->VSplitter->saveState().toBase64());
    group.writeEntry(QString("SplitterState H1"), d->HSplitter->saveState().toBase64());
    group.writeEntry("Splitter H1 CollapsedSize", d->splitterSize);
    group.writeEntry("Map Layout", QVariant::fromValue(int(d->mapLayout)));

    // --------------------------

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
        KipiImageItem* const item = d->imageModel->itemFromIndex(itemIndex);

        if (item->isDirty() || item->isTagListDirty())
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

void GPSSyncDialog::slotImageActivated(const QModelIndex& index)
{
    d->detailsWidget->slotSetCurrentImage(index);

    if (!index.isValid())
        return;

    KipiImageItem* const item = d->imageModel->itemFromIndex(index);
    if (!item)
        return;

    const KMap::GeoCoordinates imageCoordinates = item->coordinates();
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
        d->progressCancelButton->setVisible(false);
    }

    // TODO: disable the worldmapwidget and the images list (at least disable editing operations)
    d->progressCancelObject = cancelObject;
    d->progressCancelSlot = cancelSlot;
    d->uiEnabled = enabledState;
    d->buttonBox->setEnabled(enabledState);
    d->correlatorWidget->setUIEnabledExternal(enabledState);
    d->detailsWidget->setUIEnabledExternal(enabledState);
    d->rgWidget->setUIEnabled(enabledState);
    d->treeView->setEditEnabled(enabledState);
    d->listViewContextMenu->setEnabled(enabledState);
    d->mapWidget->setAllowModifications(enabledState);
}

void GPSSyncDialog::slotSetUIEnabled(const bool enabledState)
{
    slotSetUIEnabled(enabledState, 0, QString());
}

class GPSSyncKMapModelHelperPrivate
{
public:
    GPSSyncKMapModelHelperPrivate()
    {
    }

    KipiImageModel* model;
    QItemSelectionModel* selectionModel;
    QList<KMap::ModelHelper*> ungroupedModelHelpers;
};

GPSSyncKMapModelHelper::GPSSyncKMapModelHelper(KipiImageModel* const model, QItemSelectionModel* const selectionModel, QObject* const parent)
: KMap::ModelHelper(parent), d(new GPSSyncKMapModelHelperPrivate())
{
    d->model = model;
    d->selectionModel = selectionModel;

    connect(d->model, SIGNAL(signalThumbnailForIndexAvailable(const QPersistentModelIndex&, const QPixmap&)),
            this, SLOT(slotThumbnailFromModel(const QPersistentModelIndex&, const QPixmap&)));

    connect(d->model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SIGNAL(signalModelChangedDrastically()));
}

QAbstractItemModel* GPSSyncKMapModelHelper::model() const
{
    return d->model;
}

QItemSelectionModel* GPSSyncKMapModelHelper::selectionModel() const
{
    return d->selectionModel;
}

bool GPSSyncKMapModelHelper::itemCoordinates(const QModelIndex& index, KMap::GeoCoordinates* const coordinates) const
{
    KipiImageItem* const item = d->model->itemFromIndex(index);
    if (!item)
        return false;

    if (!item->gpsData().hasCoordinates())
        return false;

    if (coordinates)
        *coordinates = item->gpsData().getCoordinates();

    return true;
}

GPSSyncKMapModelHelper::~GPSSyncKMapModelHelper()
{
}

QPixmap GPSSyncKMapModelHelper::pixmapFromRepresentativeIndex(const QPersistentModelIndex& index, const QSize& size)
{
    return d->model->getPixmapForIndex(index, qMax(size.width(), size.height()));
}

QPersistentModelIndex GPSSyncKMapModelHelper::bestRepresentativeIndexFromList(const QList<QPersistentModelIndex>& list, const int sortKey)
{
    const bool oldestFirst = sortKey & 1;

    QPersistentModelIndex bestIndex;
    QDateTime bestTime;
    for (int i=0; i<list.count(); ++i)
    {
        const QPersistentModelIndex currentIndex = list.at(i);
        const KipiImageItem* const currentItem = static_cast<KipiImageItem*>(d->model->itemFromIndex(currentIndex));
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

    return bestIndex;
}

void GPSSyncKMapModelHelper::slotThumbnailFromModel(const QPersistentModelIndex& index, const QPixmap& pixmap)
{
    emit(signalThumbnailAvailableForIndex(index, pixmap));
}

void GPSSyncKMapModelHelper::onIndicesMoved(const QList<QPersistentModelIndex>& movedMarkers, const KMap::GeoCoordinates& targetCoordinates, const QPersistentModelIndex& targetSnapIndex)
{
    if (targetSnapIndex.isValid())
    {
        const QAbstractItemModel* const targetModel = targetSnapIndex.model();
        for (int i=0; i<d->ungroupedModelHelpers.count(); ++i)
        {
            KMap::ModelHelper* const ungroupedHelper = d->ungroupedModelHelpers.at(i);
            if (ungroupedHelper->model()==targetModel)
            {
                QList<QModelIndex> iMovedMarkers;
                for (int i=0; i<movedMarkers.count(); ++i)
                {
                    iMovedMarkers << movedMarkers.at(i);
                }

                ungroupedHelper->snapItemsTo(targetSnapIndex, iMovedMarkers);

                return;
            }
        }
    }

    GPSUndoCommand* const undoCommand = new GPSUndoCommand();

    for (int i=0; i<movedMarkers.count(); ++i)
    {
        const QPersistentModelIndex itemIndex = movedMarkers.at(i);
        KipiImageItem* const item = static_cast<KipiImageItem*>(d->model->itemFromIndex(itemIndex));

        GPSUndoCommand::UndoInfo undoInfo(itemIndex);
        undoInfo.readOldDataFromItem(item);

        GPSDataContainer newData;
        newData.setCoordinates(targetCoordinates);
        item->setGPSData(newData);

        undoInfo.readNewDataFromItem(item);

        undoCommand->addUndoInfo(undoInfo);
    }
    undoCommand->setText(i18np("1 image moved",
                               "%1 images moved", movedMarkers.count()));

    emit(signalUndoCommand(undoCommand));
}

void GPSSyncDialog::saveChanges(const bool closeAfterwards)
{
    // TODO: actually save the changes
    // are there any modified images?
    QList<QPersistentModelIndex> dirtyImages;
    for (int i=0; i<d->imageModel->rowCount(); ++i)
    {
        const QModelIndex itemIndex = d->imageModel->index(i, 0);
        KipiImageItem* const item = d->imageModel->itemFromIndex(itemIndex);

        if (item->isDirty() || item->isTagListDirty())
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
    d->fileIOCountDone = 0;
    d->fileIOCountTotal = dirtyImages.count();
    d->fileIOCloseAfterSaving = closeAfterwards;
    d->fileIOFutureWatcher = new QFutureWatcher<QPair<KUrl, QString> >(this);
    connect(d->fileIOFutureWatcher, SIGNAL(resultsReadyAt(int, int)),
            this, SLOT(slotFileChangesSaved(int, int)));

    d->fileIOFuture = QtConcurrent::mapped(dirtyImages, SaveChangedImagesHelper(d->imageModel));
    d->fileIOFutureWatcher->setFuture(d->fileIOFuture);
}

void GPSSyncDialog::slotFileChangesSaved(int beginIndex, int endIndex)
{
    kDebug()<<beginIndex<<endIndex;
    d->fileIOCountDone+=(endIndex-beginIndex);
    slotProgressChanged(d->fileIOCountDone);
    if (d->fileIOCountDone==d->fileIOCountTotal)
    {
        slotSetUIEnabled(true);

        // any errors?
        QList<QPair<KUrl, QString> > errorList;
        for (int i=0; i<d->fileIOFuture.resultCount(); ++i)
        {
            if (!d->fileIOFuture.resultAt(i).second.isEmpty())
                errorList << d->fileIOFuture.resultAt(i);
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
        if (d->fileIOCloseAfterSaving)
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
    d->progressCancelButton->setVisible(d->progressCancelObject!=0);
}

void GPSSyncDialog::slotGPSUndoCommand(GPSUndoCommand* undoCommand)
{
    d->undoStack->push(undoCommand);
}

void GPSSyncDialog::slotSortOptionTriggered(QAction* /*sortAction*/)
{
    int newSortKey = 0;
    if (d->sortActionOldestFirst->isChecked())
    {
        newSortKey|=1;
    }

    d->mapWidget->setSortKey(newSortKey);
}

void GPSSyncDialog::slotProgressCancelButtonClicked()
{
    if (d->progressCancelObject)
    {
        QTimer::singleShot(0, d->progressCancelObject, d->progressCancelSlot.toUtf8());
    }
}

void GPSSyncDialog::slotBookmarkVisibilityToggled()
{
    d->bookmarkOwner->bookmarkModelHelper()->setVisible(d->actionBookmarkVisibility->isChecked());
}

void GPSSyncKMapModelHelper::addUngroupedModelHelper(KMap::ModelHelper* const newModelHelper)
{
    d->ungroupedModelHelpers << newModelHelper;
}

void GPSSyncDialog::slotConfigureClicked()
{
    KConfig config("kipirc");
    QScopedPointer<Setup> setup(new Setup(this));

    setup->exec();
}

void GPSSyncDialog::slotSetupChanged()
{
    d->mapLayout = d->setupGlobalObject->readEntry("Map Layout").value<MapLayout>();

    adjustMapLayout(true);
}

KMap::KMapWidget* GPSSyncDialog::makeMapWidget(QWidget** const pvbox)
{
    QWidget* const dummyWidget = new QWidget(this);
    QVBoxLayout* const vbox = new QVBoxLayout(dummyWidget);

    KMap::KMapWidget* const mapWidget = new KMap::KMapWidget(dummyWidget);
    mapWidget->setAvailableMouseModes(KMap::MouseModePan|KMap::MouseModeZoomIntoGroup|KMap::MouseModeSelectThumbnail);
    mapWidget->setVisibleMouseModes(KMap::MouseModePan|KMap::MouseModeZoomIntoGroup|KMap::MouseModeSelectThumbnail);
    mapWidget->setMouseMode(KMap::MouseModeSelectThumbnail);
    mapWidget->setGroupedModel(d->kmapMarkerModel);
    mapWidget->setDragDropHandler(d->mapDragDropHandler);
    mapWidget->addUngroupedModel(d->bookmarkOwner->bookmarkModelHelper());
    mapWidget->addUngroupedModel(d->searchWidget->getModelHelper());
    mapWidget->setSortOptionsMenu(d->sortMenu);

    vbox->addWidget(mapWidget);
    vbox->addWidget(mapWidget->getControlWidget());

    QToolButton* const bookmarkVisibilityButton = new QToolButton(mapWidget);
    bookmarkVisibilityButton->setDefaultAction(d->actionBookmarkVisibility);
    mapWidget->addWidgetToControlWidget(bookmarkVisibilityButton);

    *pvbox = dummyWidget;

    return mapWidget;
}

void GPSSyncDialog::adjustMapLayout(const bool syncSettings)
{
    if (d->mapLayout==MapLayoutOne)
    {
        if (d->mapSplitter->count()>1)
        {
            delete d->mapSplitter->widget(1);
            d->mapWidget2 = 0;
        }
    }
    else
    {
        if (d->mapSplitter->count()==1)
        {
            QWidget* mapHolder;
            d->mapWidget2 = makeMapWidget(&mapHolder);
            d->mapSplitter->addWidget(mapHolder);

            if (syncSettings)
            {
                KConfig config("kipirc");
                KConfigGroup group = config.group(QString("GPS Sync 2 Settings"));

                const KConfigGroup groupMapWidget = KConfigGroup(&group, "Map Widget");
                d->mapWidget2->readSettingsFromGroup(&groupMapWidget);

                d->mapWidget2->setActive(true);
            }
        }

        if (d->mapLayout==MapLayoutHorizontal)
        {
            d->mapSplitter->setOrientation(Qt::Horizontal);
        }
        else
        {
            d->mapSplitter->setOrientation(Qt::Vertical);
        }
    }
}

KMap::ModelHelper::Flags GPSSyncKMapModelHelper::modelFlags() const
{
    return FlagMovable;
}

}  // namespace KIPIGPSSyncPlugin
