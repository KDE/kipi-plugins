/* ============================================================
 *
 * Date        : 2010-04-25
 * Description : A class to hold undo/redo commands
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

#ifndef GPSUNDOCOMMAND_H
#define GPSUNDOCOMMAND_H

// Qt includes

#include <QUndoCommand>

// local includes

#include "gpsimageitem.h"

namespace KIPIGPSSyncPlugin
{

class GPSUndoCommand : public QUndoCommand
{
public:

    class UndoInfo
    {
    public:
        UndoInfo(QPersistentModelIndex pModelIndex, GPSDataContainer pDataBefore, GPSDataContainer pDataAfter)
        : modelIndex(pModelIndex),
          dataBefore(pDataBefore),
          dataAfter(pDataAfter)
        {
        }

        QPersistentModelIndex modelIndex;
        GPSDataContainer dataBefore;
        GPSDataContainer dataAfter;

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
