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
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

// KDE includes

#include <khbox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kvbox.h>

// local includes

#include "searchbackend.h"

namespace KIPIGPSSyncPlugin
{

class SearchWidgetPrivate
{
public:
    SearchWidgetPrivate()
    {
    }

    KLineEdit* searchTermLineEdit;
    QPushButton* searchButton;
    SearchBackend* searchBackend;
    QTreeView* treeView;
    SearchResultModel* searchResultsModel;
    SearchResultModelHelper* searchResultModelHelper;
};

SearchWidget::SearchWidget(QWidget* parent)
: QWidget(parent), d(new SearchWidgetPrivate())
{
    d->searchBackend = new SearchBackend(this);
    d->searchResultsModel = new SearchResultModel(this);
    d->searchResultModelHelper = new SearchResultModelHelper(d->searchResultsModel, this);

    KVBox* const mainVBox = new KVBox(this);

    KHBox* const topHBox = new KHBox(mainVBox);
    d->searchTermLineEdit = new KLineEdit(topHBox);
    d->searchButton = new QPushButton(i18n("Search"), topHBox);

    d->treeView = new QTreeView(mainVBox);
    d->treeView->setModel(d->searchResultsModel);

    QListView* const listView = new QListView(mainVBox);
    listView->setModel(d->searchResultsModel);

//     dynamic_cast<QVBoxLayout*>(mainVBox->layout())->addStretch(300);

    connect(d->searchButton, SIGNAL(clicked()),
            this, SLOT(slotTriggerSearch()));

    connect(d->searchBackend, SIGNAL(signalSearchCompleted()),
            this, SLOT(slotSearchCompleted()));
}

SearchWidget::~SearchWidget()
{
    delete d;
}

void SearchWidget::slotSearchCompleted()
{
    const SearchBackend::SearchResult::List searchResults = d->searchBackend->getResults();

    d->searchResultsModel->addResults(searchResults);
}

void SearchWidget::slotTriggerSearch()
{
    d->searchBackend->search("osm", d->searchTermLineEdit->text());
}

class SearchResultModelPrivate
{
public:
    SearchResultModelPrivate()
    {
    }

    QList<SearchResultModel::SearchResultItem> searchResults;
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
        return d->searchResults.at(rowNumber).result.name;
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
    return d->searchResults.count();
}

bool SearchResultModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    return false;
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return "Name";
}

Qt::ItemFlags SearchResultModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index);
}

void SearchResultModel::addResults(const SearchBackend::SearchResult::List& results)
{
    beginInsertRows(QModelIndex(), d->searchResults.count(), d->searchResults.count()+results.count());
    for (int i=0; i<results.count(); ++i)
    {
        SearchResultItem item;
        item.result = results.at(i);
        d->searchResults << item;
    }
    endInsertRows();
}

class SearchResultModelHelperPrivate
{
public:
    SearchResultModelHelperPrivate()
    {
    }

    SearchResultModel* model;
};

SearchResultModelHelper::SearchResultModelHelper(SearchResultModel* const resultModel, QObject* const parent)
: WMW2::WMWModelHelper(parent), d(new SearchResultModelHelperPrivate())
{
    d->model = resultModel;
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
    return 0;
}

bool SearchResultModelHelper::itemCoordinates(const QModelIndex& index, WMW2::WMWGeoCoordinate* const coordinates) const
{
    const SearchResultModel::SearchResultItem item = d->model->resultItem(index);

    *coordinates = item.result.coordinates;

    return true;
}

QPixmap SearchResultModelHelper::itemIcon(const QModelIndex& index, QPoint* const offset) const
{
    return QPixmap();
}

bool SearchResultModelHelper::visible() const
{
    return true;
}

bool SearchResultModelHelper::snaps() const
{
    return false;
}

SearchResultModel::SearchResultItem SearchResultModel::resultItem(const QModelIndex& index) const
{
    return d->searchResults.at(index.row());
}

WMW2::WMWModelHelper* SearchWidget::getModelHelper()
{
    return d->searchResultModelHelper;
}

} /* KIPIGPSSyncPlugin */
