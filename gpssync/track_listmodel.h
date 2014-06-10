/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2014-06-09
 * @brief  A model to list the tracks
 *
 * @author Copyright (C) 2014 by Michael G. Hansen
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

#ifndef TRACK_LISTMODEL_H
#define TRACK_LISTMODEL_H

// Qt includes

#include <QAbstractItemModel>

namespace KGeoMap
{
    class TrackManager;
}

namespace KIPIGPSSyncPlugin
{

class TrackListModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    TrackListModel(KGeoMap::TrackManager* const trackManager, QObject* const parent);
    ~TrackListModel();

    // QAbstractItemModel:
    virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role);
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex() ) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

private Q_SLOTS:

    void slotTrackManagerUpdated();

private:

    class Private;
    const QScopedPointer<Private> d;
};

} /* namespace KIPIGPSSyncPlugin */

#endif //  TRACK_LISTMODEL_H
