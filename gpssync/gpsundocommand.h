/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-04-25
 * @brief  A class to hold undo/redo commands.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
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

#ifndef GPSUNDOCOMMAND_H
#define GPSUNDOCOMMAND_H

// Qt includes

#include <QUndoCommand>

// local includes

#include "kipiimageitem.h"

namespace KIPIGPSSyncPlugin
{

class GPSUndoCommand : public QUndoCommand
{
public:

    class UndoInfo
    {
    public:
        UndoInfo(QPersistentModelIndex pModelIndex)
        : modelIndex(pModelIndex)
        {
        }

        void readOldDataFromItem(const KipiImageItem* const imageItem);
        void readNewDataFromItem(const KipiImageItem* const imageItem);

        QPersistentModelIndex modelIndex;
        GPSDataContainer dataBefore;
        GPSDataContainer dataAfter;

        QList<QList<TagData> > oldTagList;
        QList<QList<TagData> > newTagList;

        typedef QList<UndoInfo> List;
    };

    GPSUndoCommand(QUndoCommand* const parent = 0);

    void addUndoInfo(const UndoInfo& info);

    virtual void redo();
    virtual void undo();

    void changeItemData(const bool redoIt);
    inline int affectedItemCount() const
    {
        return undoList.count();
    }

private:
    UndoInfo::List undoList;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSUNDOCOMMAND_H */
