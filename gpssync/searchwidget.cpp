/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-06-01
 * @brief  A widget to search for places.
 *
 * @author Copyright (C) 2010, 2011 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

#include "searchwidget.moc"

// Qt includes

#include <QContextMenuEvent>
#include <QListView>
#include <QPainter>
#include <QPushButton>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

// KDE includes

#include <kaction.h>
#include <kcombobox.h>
#include <kconfiggroup.h>
#include <khbox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kvbox.h>

// Libkmap includes

#include <libkmap/kmap_widget.h>

// local includes

#include "searchbackend.h"
#include "gpssync_common.h"
#include "gpsbookmarkowner.h"
#include "gpsundocommand.h"
#include "kipiimagemodel.h"

#ifdef GPSSYNC_MODELTEST
#include <modeltest.h>
#endif /* GPSSYNC_MODELTEST */

namespace KIPIGPSSyncPlugin
{

class SearchWidgetPrivate
{
public:
    SearchWidgetPrivate()
    {
        // TODO: initialize in initializer list
        searchInProgress = false;
        actionToggleAllResultsVisibilityIconUnchecked=KIcon(SmallIcon("layer-visible-off"));
        actionToggleAllResultsVisibilityIconChecked=KIcon(SmallIcon("layer-visible-on"));
    }

    // Map
    KMap::KMapWidget                          *mapWidget;
    GPSBookmarkOwner                          *gpsBookmarkOwner;
    KipiImageModel                            *kipiImageModel;
    QItemSelectionModel                       *kipiImageSelectionModel;
    KLineEdit                                 *searchTermLineEdit;
    QPushButton                               *searchButton;

    // Search: backend
    SearchBackend                             *searchBackend;
    SearchResultModel                         *searchResultsModel;
    QItemSelectionModel                       *searchResultsSelectionModel;
    SearchResultModelHelper                   *searchResultModelHelper;

    // Search: UI
    QTreeView                                 *treeView;
    QVBoxLayout                               *mainVBox;
    KComboBox                                 *backendSelectionBox;
    KAction                                   *actionClearResultsList;
    KAction                                   *actionKeepOldResults;
    KAction                                   *actionToggleAllResultsVisibility;
    bool                                       searchInProgress;
    KIcon                                      actionToggleAllResultsVisibilityIconUnchecked;
    KIcon                                      actionToggleAllResultsVisibilityIconChecked;
    KAction                                   *actionCopyCoordinates;
    KAction                                   *actionBookmark;
    KAction                                   *actionMoveImagesToThisResult;
};

SearchWidget::SearchWidget(GPSBookmarkOwner* const gpsBookmarkOwner,
                           KipiImageModel* const kipiImageModel,
                           QItemSelectionModel* const kipiImageSelectionModel,
                           QWidget* const parent)
            : QWidget(parent), d(new SearchWidgetPrivate())
{
    d->gpsBookmarkOwner = gpsBookmarkOwner;
    d->kipiImageModel = kipiImageModel;
    d->kipiImageSelectionModel = kipiImageSelectionModel;
    d->searchBackend = new SearchBackend(this);
    d->searchResultsModel = new SearchResultModel(this);
#ifdef GPSSYNC_MODELTEST
    new ModelTest(d->searchResultsModel, this);
#endif /* GPSSYNC_MODELTEST */
    d->searchResultsSelectionModel = new QItemSelectionModel(d->searchResultsModel);
    d->searchResultsModel->setSelectionModel(d->searchResultsSelectionModel);
    d->searchResultModelHelper = new SearchResultModelHelper(d->searchResultsModel, d->searchResultsSelectionModel, d->kipiImageModel, this);

    d->mainVBox = new QVBoxLayout(this);
    setLayout(d->mainVBox);

    KHBox* const topHBox = new KHBox(this);
    d->mainVBox->addWidget(topHBox);
    d->searchTermLineEdit = new KLineEdit(topHBox);
    d->searchTermLineEdit->setClearButtonShown(true);
    d->searchButton = new QPushButton(i18nc("Start the search", "Search"), topHBox);

    KHBox* const actionHBox = new KHBox(this);
    d->mainVBox->addWidget(actionHBox);

    d->actionClearResultsList = new KAction(this);
    d->actionClearResultsList->setIcon(SmallIcon("edit-clear-list"));
    d->actionClearResultsList->setToolTip(i18n("Clear the search results."));
    QToolButton* const tbClearResultsList = new QToolButton(actionHBox);
    tbClearResultsList->setDefaultAction(d->actionClearResultsList);

    d->actionKeepOldResults = new KAction("S", this);
    d->actionKeepOldResults->setCheckable(true);
    d->actionKeepOldResults->setChecked(false);
    d->actionKeepOldResults->setToolTip(i18n("Keep the results of old searches when doing a new search."));
    QToolButton* const tbKeepOldResults = new QToolButton(actionHBox);
    tbKeepOldResults->setDefaultAction(d->actionKeepOldResults);

    d->actionToggleAllResultsVisibility = new KAction(this);
    d->actionToggleAllResultsVisibility->setCheckable(true);
    d->actionToggleAllResultsVisibility->setChecked(true);
    d->actionToggleAllResultsVisibility->setToolTip(i18n("Toggle the visibility of the search results on the map."));
    QToolButton* const tbToggleAllVisibility = new QToolButton(actionHBox);
    tbToggleAllVisibility->setDefaultAction(d->actionToggleAllResultsVisibility);

    d->actionCopyCoordinates = new KAction(i18n("Copy coordinates"), this);
    d->actionCopyCoordinates->setIcon(SmallIcon("edit-copy"));

    d->actionMoveImagesToThisResult = new KAction(i18n("Move selected images to this position"), this);

    d->backendSelectionBox = new KComboBox(actionHBox);
    d->backendSelectionBox->setToolTip(i18n("Select which service you would like to use."));
    const QList<QPair<QString, QString> > backendList = d->searchBackend->getBackends();
    for (int i=0; i<backendList.count(); ++i)
    {
        d->backendSelectionBox->addItem(backendList.at(i).first, backendList.at(i).second);
    }
    
    // add stretch after the controls:
    QHBoxLayout* const hBoxLayout = reinterpret_cast<QHBoxLayout*>(actionHBox->layout());
    if (hBoxLayout)
    {
        hBoxLayout->addStretch();
    }

    d->treeView = new QTreeView(this);
    d->mainVBox->addWidget(d->treeView);
    d->treeView->setRootIsDecorated(false);
    d->treeView->setModel(d->searchResultsModel);
    d->treeView->setSelectionModel(d->searchResultsSelectionModel);

    d->actionBookmark = new KAction(i18n("Bookmarks"), this);
    d->actionBookmark->setMenu(d->gpsBookmarkOwner->getMenu());

    connect(d->actionMoveImagesToThisResult, SIGNAL(triggered(bool)),
            this, SLOT(slotMoveSelectedImagesToThisResult()));

    connect(d->searchButton, SIGNAL(clicked()),
            this, SLOT(slotTriggerSearch()));

    connect(d->searchBackend, SIGNAL(signalSearchCompleted()),
            this, SLOT(slotSearchCompleted()));

    connect(d->searchTermLineEdit, SIGNAL(returnPressed()),
            this, SLOT(slotTriggerSearch()));

    connect(d->searchTermLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotUpdateUIState()));

    connect(d->searchResultsSelectionModel, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(slotCurrentlySelectedResultChanged(const QModelIndex&, const QModelIndex&)));

    connect(d->actionClearResultsList, SIGNAL(triggered(bool)),
            this, SLOT(slotClearSearchResults()));

    connect(d->actionToggleAllResultsVisibility, SIGNAL(triggered(bool)),
            this, SLOT(slotVisibilityChanged(bool)));

    connect(d->actionCopyCoordinates, SIGNAL(triggered(bool)),
            this, SLOT(slotCopyCoordinates()));

    connect(d->searchResultModelHelper, SIGNAL(signalUndoCommand(GPSUndoCommand*)),
            this, SIGNAL(signalUndoCommand(GPSUndoCommand*)));

    d->treeView->installEventFilter(this);

    slotUpdateUIState();
}

SearchWidget::~SearchWidget()
{
    delete d;
}

void SearchWidget::slotSearchCompleted()
{
    d->searchInProgress = false;

    const QString errorString = d->searchBackend->getErrorMessage();
    if (!errorString.isEmpty())
    {
        KMessageBox::error(this, i18n("Your search failed:\n%1", errorString), i18n("Search failed"));
        slotUpdateUIState();
        return;
    }

    const SearchBackend::SearchResult::List searchResults = d->searchBackend->getResults();
    d->searchResultsModel->addResults(searchResults);

    slotUpdateUIState();
}

void SearchWidget::slotTriggerSearch()
{
    // this is necessary since this slot is also connected to QLineEdit::returnPressed
    if (d->searchTermLineEdit->text().isEmpty() || d->searchInProgress)
    {
        return;
    }

    if (!d->actionKeepOldResults->isChecked())
    {
        slotClearSearchResults();
    }

    d->searchInProgress = true;

    const QString searchBackendName = d->backendSelectionBox->itemData(d->backendSelectionBox->currentIndex()).toString();
    d->searchBackend->search(searchBackendName, d->searchTermLineEdit->text());

    slotUpdateUIState();
}

class SearchResultModelPrivate
{
public:
    SearchResultModelPrivate()
    {
        markerNormalUrl = KStandardDirs::locate("data", "gpssync/searchmarker-normal.png");
        markerNormal = QPixmap(markerNormalUrl.toLocalFile());

        markerSelectedUrl = KStandardDirs::locate("data", "gpssync/searchmarker-selected.png");
        markerSelected = QPixmap(markerSelectedUrl.toLocalFile());
    }

    QList<SearchResultModel::SearchResultItem> searchResults;
    KUrl markerNormalUrl;
    KUrl markerSelectedUrl;
    QPixmap markerNormal;
    QPixmap markerSelected;
    QItemSelectionModel* selectionModel;
};

SearchResultModel::SearchResultModel(QObject* const parent)
: QAbstractItemModel(parent), d(new SearchResultModelPrivate())
{
}

SearchResultModel::~SearchResultModel()
{
    delete d;
}

int SearchResultModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return 1;
}

bool SearchResultModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)

    return false;
}

QVariant SearchResultModel::data(const QModelIndex& index, int role) const
{
    const int rowNumber = index.row();
    if ((rowNumber<0)||(rowNumber>=d->searchResults.count()))
    {
        return QVariant();
    }

    const int columnNumber = index.column();

    if (columnNumber==0)
    {
        switch (role)
        {
        case Qt::DisplayRole:
            return d->searchResults.at(rowNumber).result.name;

        case Qt::DecorationRole:
        {
            QPixmap markerIcon;
            getMarkerIcon(index, 0, 0, &markerIcon, 0);
            return markerIcon;
        }

        default:
            return QVariant();
        }
    }

    return QVariant();
}

QModelIndex SearchResultModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        // there are no child items, only top level items
        return QModelIndex();
    }

    if ( (column<0) || (column>=1)
         || (row<0) || (row>=d->searchResults.count()) )
    {
        return QModelIndex();
    }

    return createIndex(row, column, 0);
}

QModelIndex SearchResultModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index)

    // we have only top level items
    return QModelIndex();
}

int SearchResultModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return d->searchResults.count();
}

bool SearchResultModel::setHeaderData(int section, Qt::Orientation orientation,
                                      const QVariant& value, int role)
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(value)
    Q_UNUSED(role)

    return false;
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(role)

    if ((section >= 1) || (orientation != Qt::Horizontal))
    {
        return false;
    }

    return QVariant("Name");
}

Qt::ItemFlags SearchResultModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index);
}

void SearchResultModel::addResults(const SearchBackend::SearchResult::List& results)
{
    // first check which items are not duplicates
    QList<int> nonDuplicates;
    for (int i=0; i<results.count(); ++i)
    {
        const SearchBackend::SearchResult& currentResult = results.at(i);
        bool isDuplicate = false;
        for (int j=0; j<d->searchResults.count(); ++j)
        {
            if (currentResult.internalId==d->searchResults.at(j).result.internalId)
            {
                isDuplicate = true;
                break;
            }
        }
        if (!isDuplicate)
        {
            nonDuplicates << i;
        }
    }

    if (nonDuplicates.isEmpty())
    {
        return;
    }

    beginInsertRows(QModelIndex(), d->searchResults.count(), d->searchResults.count()+nonDuplicates.count()-1);
    for (int i=0; i<nonDuplicates.count(); ++i)
    {
        SearchResultItem item;
        item.result = results.at(nonDuplicates.at(i));
        d->searchResults << item;
    }
    endInsertRows();
}

class SearchResultModelHelperPrivate
{
public:
    SearchResultModelHelperPrivate()
    : model(0),
      selectionModel(0),
      imageModel(0),
      visible(true)
    {
    }

    SearchResultModel* model;
    QItemSelectionModel* selectionModel;
    KipiImageModel* imageModel;
    bool visible;
};

SearchResultModelHelper::SearchResultModelHelper(SearchResultModel* const resultModel,
                                                 QItemSelectionModel* const selectionModel,
                                                 KipiImageModel* const imageModel,
                                                 QObject* const parent)
: KMap::ModelHelper(parent), d(new SearchResultModelHelperPrivate())
{
    d->model = resultModel;
    d->selectionModel = selectionModel;
    d->imageModel = imageModel;
}

SearchResultModelHelper::~SearchResultModelHelper()
{
    delete d;
}

QAbstractItemModel* SearchResultModelHelper::model() const
{
    return d->model;
}

QItemSelectionModel* SearchResultModelHelper::selectionModel() const
{
    return d->selectionModel;
}

bool SearchResultModelHelper::itemCoordinates(const QModelIndex& index, KMap::GeoCoordinates* const coordinates) const
{
    const SearchResultModel::SearchResultItem item = d->model->resultItem(index);

    *coordinates = item.result.coordinates;

    return true;
}

bool SearchResultModelHelper::itemIcon(const QModelIndex& index, QPoint* const offset, QSize* const size, QPixmap* const pixmap, KUrl* const url) const
{
    return d->model->getMarkerIcon(index, offset, size, pixmap, url);
}

SearchResultModel::SearchResultItem SearchResultModel::resultItem(const QModelIndex& index) const
{
    return d->searchResults.at(index.row());
}

KMap::ModelHelper* SearchWidget::getModelHelper()
{
    return d->searchResultModelHelper;
}

void SearchWidget::slotUpdateUIState()
{
    const bool haveSearchText = !d->searchTermLineEdit->text().isEmpty();

    d->searchButton->setEnabled(haveSearchText&&!d->searchInProgress);
    d->actionClearResultsList->setEnabled(d->searchResultsModel->rowCount()>0);
    d->actionToggleAllResultsVisibility->setIcon(
            d->actionToggleAllResultsVisibility->isChecked() ?
            d->actionToggleAllResultsVisibilityIconChecked :
            d->actionToggleAllResultsVisibilityIconUnchecked
        );
}

bool SearchResultModel::getMarkerIcon(const QModelIndex& index, QPoint* const offset, QSize* const size, QPixmap* const pixmap, KUrl* const url) const
{
    // determine the id of the marker
    const int markerNumber = index.row();

    const bool itemIsSelected = d->selectionModel ? d->selectionModel->isSelected(index) : false;
    QPixmap markerPixmap = itemIsSelected ? d->markerSelected : d->markerNormal;

    // if the caller requests a URL and the marker will not get
    // a special label, return a URL. Otherwise, return a pixmap.
    const bool returnViaUrl = url && markerNumber>26;
    if (returnViaUrl)
    {
        *url = itemIsSelected ? d->markerSelectedUrl : d->markerNormalUrl;

        if (size)
        {
            *size = markerPixmap.size();
        }
    }
    else
    {
        if (markerNumber<=26)
        {
            const QString markerId = QChar('A'+markerNumber);

            QPainter painter(&markerPixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(Qt::black);
            QRect textRect(0,2,markerPixmap.width(),markerPixmap.height());
            painter.drawText(textRect, Qt::AlignHCenter, markerId);
        }

        *pixmap = markerPixmap;
    }

    if (offset)
    {
        *offset = QPoint(markerPixmap.width()/2, markerPixmap.height()-1);
    }

    return true;
}

void SearchResultModel::setSelectionModel(QItemSelectionModel* const selectionModel)
{
    d->selectionModel = selectionModel;
}

void SearchWidget::slotCurrentlySelectedResultChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous)

    if (!current.isValid())
    {
        return;
    }

    const SearchResultModel::SearchResultItem currentItem = d->searchResultsModel->resultItem(current);

    if (d->mapWidget)
    {
        d->mapWidget->setCenter(currentItem.result.coordinates);
    }
}

void SearchWidget::slotClearSearchResults()
{
    d->searchResultsModel->clearResults();

    slotUpdateUIState();
}

void SearchResultModel::clearResults()
{
    beginResetModel();
    d->searchResults.clear();
    endResetModel();
}

void SearchWidget::slotVisibilityChanged(bool state)
{
    d->searchResultModelHelper->setVisibility(state);
    slotUpdateUIState();
}

void SearchResultModelHelper::setVisibility(const bool state)
{
    d->visible = state;
    emit(signalVisibilityChanged());
}

bool SearchWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched==d->treeView)
    {
        // we are only interested in context-menu events:
        if ( (event->type()==QEvent::ContextMenu) && d->searchResultsSelectionModel->hasSelection() )
        {
            const QModelIndex currentIndex = d->searchResultsSelectionModel->currentIndex();
            const SearchResultModel::SearchResultItem searchResult = d->searchResultsModel->resultItem(currentIndex);
            d->gpsBookmarkOwner->setPositionAndTitle(searchResult.result.coordinates, searchResult.result.name);

            // construct the context-menu:
            KMenu* const menu = new KMenu(d->treeView);
            menu->addAction(d->actionCopyCoordinates);
            menu->addAction(d->actionMoveImagesToThisResult);
            d->actionMoveImagesToThisResult->setEnabled(!d->kipiImageSelectionModel->selectedRows().isEmpty());
//             menu->addAction(d->actionBookmark);
            d->gpsBookmarkOwner->changeAddBookmark(true);

            QContextMenuEvent* const e = static_cast<QContextMenuEvent*>(event);
            menu->exec(e->globalPos());
        }
    }

    return QObject::eventFilter(watched, event);
}

void SearchWidget::slotCopyCoordinates()
{
    const QModelIndex currentIndex = d->searchResultsSelectionModel->currentIndex();
    const SearchResultModel::SearchResultItem currentItem = d->searchResultsModel->resultItem(currentIndex);

    CoordinatesToClipboard(currentItem.result.coordinates, KUrl(), currentItem.result.name);
}

void SearchWidget::saveSettingsToGroup(KConfigGroup* const group)
{
    group->writeEntry("Keep old results", d->actionKeepOldResults->isChecked());
    group->writeEntry("Search backend", d->backendSelectionBox->itemData(d->backendSelectionBox->currentIndex()).toString());

    slotUpdateUIState();
}

void SearchWidget::readSettingsFromGroup(const KConfigGroup* const group)
{
    d->actionKeepOldResults->setChecked(group->readEntry("Keep old results", false));
    const QString backendId = group->readEntry("Search backend", "osm");
    for (int i=0; i<d->backendSelectionBox->count(); ++i)
    {
        if (d->backendSelectionBox->itemData(i).toString()==backendId)
        {
            d->backendSelectionBox->setCurrentIndex(i);
            break;
        }
    }
}

KMap::ModelHelper::Flags SearchResultModelHelper::modelFlags() const
{
    return FlagSnaps|(d->visible?FlagVisible:FlagNull);
}

KMap::ModelHelper::Flags SearchResultModelHelper::itemFlags(const QModelIndex& /*index*/) const
{
    return FlagVisible|FlagSnaps;
}

void SearchResultModelHelper::snapItemsTo(const QModelIndex& targetIndex, const QList<QModelIndex>& snappedIndices)
{
    GPSUndoCommand* const undoCommand = new GPSUndoCommand();

    SearchResultModel::SearchResultItem targetItem = d->model->resultItem(targetIndex);
    const KMap::GeoCoordinates& targetCoordinates = targetItem.result.coordinates;
    for (int i=0; i<snappedIndices.count(); ++i)
    {
        const QPersistentModelIndex itemIndex = snappedIndices.at(i);
        KipiImageItem* const item = d->imageModel->itemFromIndex(itemIndex);

        GPSUndoCommand::UndoInfo undoInfo(itemIndex);
        undoInfo.readOldDataFromItem(item);

        GPSDataContainer newData;
        newData.setCoordinates(targetCoordinates);
        item->setGPSData(newData);

        undoInfo.readNewDataFromItem(item);

        undoCommand->addUndoInfo(undoInfo);
    }
    undoCommand->setText(i18np("1 image snapped to '%2'",
                               "%1 images snapped to '%2'", snappedIndices.count(), targetItem.result.name));

    emit(signalUndoCommand(undoCommand));
}

void SearchWidget::slotMoveSelectedImagesToThisResult()
{
    const QModelIndex currentIndex = d->searchResultsSelectionModel->currentIndex();
    const SearchResultModel::SearchResultItem currentItem = d->searchResultsModel->resultItem(currentIndex);
    const KMap::GeoCoordinates& targetCoordinates = currentItem.result.coordinates;

    const QModelIndexList selectedImageIndices = d->kipiImageSelectionModel->selectedRows();
    if (selectedImageIndices.isEmpty())
    {
        return;
    }

    GPSUndoCommand* const undoCommand = new GPSUndoCommand();
    for (int i=0; i<selectedImageIndices.count(); ++i)
    {
        const QPersistentModelIndex itemIndex = selectedImageIndices.at(i);
        KipiImageItem* const item = d->kipiImageModel->itemFromIndex(itemIndex);

        GPSUndoCommand::UndoInfo undoInfo(itemIndex);
        undoInfo.readOldDataFromItem(item);        

        GPSDataContainer newData;
        newData.setCoordinates(targetCoordinates);
        item->setGPSData(newData);

        undoInfo.readNewDataFromItem(item);

        undoCommand->addUndoInfo(undoInfo);
    }
    undoCommand->setText(i18np("1 image moved to '%2'",
                               "%1 images moved to '%2'", selectedImageIndices.count(), currentItem.result.name));

    emit(signalUndoCommand(undoCommand));
}

void SearchWidget::setPrimaryMapWidget(KMap::KMapWidget* const mapWidget)
{
    d->mapWidget = mapWidget;
}

} /* KIPIGPSSyncPlugin */
