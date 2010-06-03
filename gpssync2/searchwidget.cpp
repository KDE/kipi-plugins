/* ============================================================
 *
 * Date        : 2010-06-01
 * Description : A widget to search for places
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "searchwidget.moc"

// Qt includes

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
#include <khbox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kvbox.h>

// local includes

#include "searchbackend.h"
#include <worldmapwidget2/worldmapwidget2.h>

namespace KIPIGPSSyncPlugin
{

class SearchWidgetPrivate
{
public:
    SearchWidgetPrivate()
    : searchInProgress(false),
      actionToggleAllResultsVisibilityIconUnchecked(SmallIcon("layer-visible-off")),
      actionToggleAllResultsVisibilityIconChecked(SmallIcon("layer-visible-on"))
    {
    }

    WMW2::WorldMapWidget2* mapWidget;
    KLineEdit* searchTermLineEdit;
    QPushButton* searchButton;
    SearchBackend* searchBackend;
    QTreeView* treeView;
    SearchResultModel* searchResultsModel;
    QItemSelectionModel* searchResultsSelectionModel;
    SearchResultModelHelper* searchResultModelHelper;
    QVBoxLayout* mainVBox;
    KComboBox* backendSelectionBox;

    KAction* actionClearResultsList;
    KAction* actionKeepOldResults;
    KAction* actionToggleAllResultsVisibility;
    bool searchInProgress;
    KIcon actionToggleAllResultsVisibilityIconUnchecked;
    KIcon actionToggleAllResultsVisibilityIconChecked;
};

SearchWidget::SearchWidget(WMW2::WorldMapWidget2* const mapWidget, QWidget* parent)
: QWidget(parent), d(new SearchWidgetPrivate())
{
    d->mapWidget = mapWidget;
    d->searchBackend = new SearchBackend(this);
    d->searchResultsModel = new SearchResultModel(this);
    d->searchResultsSelectionModel = new QItemSelectionModel(d->searchResultsModel);
    d->searchResultsModel->setSelectionModel(d->searchResultsSelectionModel);
    d->searchResultModelHelper = new SearchResultModelHelper(d->searchResultsModel, d->searchResultsSelectionModel, this);

    d->mainVBox = new QVBoxLayout(this);
    setLayout(d->mainVBox);

    KHBox* const topHBox = new KHBox(this);
    d->mainVBox->addWidget(topHBox);
    d->searchTermLineEdit = new KLineEdit(topHBox);
    d->searchTermLineEdit->setClearButtonShown(true);
    d->searchButton = new QPushButton(i18n("Search"), topHBox);

    KHBox* const actionHBox = new KHBox(this);
    d->mainVBox->addWidget(actionHBox);

    d->actionClearResultsList = new KAction(this);
    d->actionClearResultsList->setIcon(SmallIcon("edit-clear-list"));
    QToolButton* const tbClearResultsList = new QToolButton(actionHBox);
    tbClearResultsList->setDefaultAction(d->actionClearResultsList);

    d->actionKeepOldResults = new KAction("S", this);
    d->actionKeepOldResults->setCheckable(true);
    d->actionKeepOldResults->setChecked(false);
    QToolButton* const tbKeepOldResults = new QToolButton(actionHBox);
    tbKeepOldResults->setDefaultAction(d->actionKeepOldResults);

    d->actionToggleAllResultsVisibility = new KAction(this);
    d->actionToggleAllResultsVisibility->setCheckable(true);
    d->actionToggleAllResultsVisibility->setChecked(true);
    QToolButton* const tbToggleAllVisibility = new QToolButton(actionHBox);
    tbToggleAllVisibility->setDefaultAction(d->actionToggleAllResultsVisibility);

    d->backendSelectionBox = new KComboBox(actionHBox);
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
        return;

    if (!d->actionKeepOldResults->isChecked())
        slotClearSearchResults();

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
        const KUrl markerUrl = KStandardDirs::locate("data", "gpssync2/searchmarker-normal.png");
        markerNormal = QPixmap(markerUrl.toLocalFile());

        const KUrl markerSelectedUrl = KStandardDirs::locate("data", "gpssync2/searchmarker-selected.png");
        markerSelected = QPixmap(markerSelectedUrl.toLocalFile());
    }

    QList<SearchResultModel::SearchResultItem> searchResults;
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
    return 1;
}

bool SearchResultModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
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
            return getMarkerIcon(index, 0);

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

    if ( (column>=1)
         || (row>=d->searchResults.count()) )
        return QModelIndex();

    return createIndex(row, column, 0);
}

QModelIndex SearchResultModel::parent(const QModelIndex& index) const
{
    // we have only top level items
    return QModelIndex();
}

int SearchResultModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return d->searchResults.count();
}

bool SearchResultModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    return false;
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((section>=1)||(orientation!=Qt::Horizontal))
        return false;

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
        return;

    beginInsertRows(QModelIndex(), d->searchResults.count(), d->searchResults.count()+nonDuplicates.count());
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
    : visible(true)
    {
    }

    SearchResultModel* model;
    QItemSelectionModel* selectionModel;
    bool visible;
};

SearchResultModelHelper::SearchResultModelHelper(SearchResultModel* const resultModel, QItemSelectionModel* const selectionModel, QObject* const parent)
: WMW2::WMWModelHelper(parent), d(new SearchResultModelHelperPrivate())
{
    d->model = resultModel;
    d->selectionModel = selectionModel;
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

bool SearchResultModelHelper::itemCoordinates(const QModelIndex& index, WMW2::WMWGeoCoordinate* const coordinates) const
{
    const SearchResultModel::SearchResultItem item = d->model->resultItem(index);

    *coordinates = item.result.coordinates;

    return true;
}

QPixmap SearchResultModelHelper::itemIcon(const QModelIndex& index, QPoint* const offset) const
{
    return d->model->getMarkerIcon(index, offset);
}

bool SearchResultModelHelper::visible() const
{
    return d->visible;
}

bool SearchResultModelHelper::snaps() const
{
    return true;
}

SearchResultModel::SearchResultItem SearchResultModel::resultItem(const QModelIndex& index) const
{
    return d->searchResults.at(index.row());
}

WMW2::WMWModelHelper* SearchWidget::getModelHelper()
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

QPixmap SearchResultModel::getMarkerIcon(const QModelIndex& index, QPoint* const offset) const
{
    const bool itemIsSelected = d->selectionModel ? d->selectionModel->isSelected(index) : false;

    QPixmap markerPixmap = itemIsSelected ? d->markerSelected : d->markerNormal;

    // determine the id of the marker
    const int markerNumber = index.row();
    if (markerNumber<26)
    {
        const QString markerId = QChar('A'+markerNumber);

        QPainter painter(&markerPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::black);
        QRect textRect(0,2,markerPixmap.width(),markerPixmap.height());
        painter.drawText(textRect, Qt::AlignHCenter, markerId);
    }

    if (offset)
    {
        *offset = QPoint(markerPixmap.width()/2, 0);
    }

    return markerPixmap;
}

void SearchResultModel::setSelectionModel(QItemSelectionModel* const selectionModel)
{
    d->selectionModel = selectionModel;
}

void SearchWidget::slotCurrentlySelectedResultChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if (!current.isValid())
        return;

    SearchResultModel::SearchResultItem currentItem = d->searchResultsModel->resultItem(current);

    d->mapWidget->setCenter(currentItem.result.coordinates);
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

} /* KIPIGPSSyncPlugin */
