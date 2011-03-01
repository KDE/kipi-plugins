/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2009-05-07
 * @brief  Context menu for GPS list view.
 *
 * @author Copyright (C) 2009,2010 by Michael G. Hansen
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
#ifndef GPSLISTVIEWCONTEXTMENU_H
#define GPSLISTVIEWCONTEXTMENU_H

// Qt includes:

#include <QObject>

// local includes:

#include "gpsdatacontainer.h"
#include "kipiimagelist.h"

class KUrl;

namespace KIPIGPSSyncPlugin
{

class GPSListViewContextMenuPriv;
class GPSBookmarkOwner;
class GPSUndoCommand;

class GPSListViewContextMenu : public QObject
{
    Q_OBJECT

public:

    explicit GPSListViewContextMenu(KipiImageList *imagesList, GPSBookmarkOwner* const bookmarkOwner = 0);
    ~GPSListViewContextMenu();

    void setEnabled(const bool state);

protected:

    virtual bool eventFilter(QObject *watched, QEvent *event);
    void setGPSDataForSelectedItems(const GPSDataContainer gpsData, const QString& undoDescription);
    static bool getCurrentPosition(GPSDataContainer* position, void* mydata);
    bool getCurrentItemPositionAndUrl(GPSDataContainer* const gpsInfo, KUrl* const itemUrl);
    void removeInformationFromSelectedImages(const GPSDataContainer::HasFlags flagsToClear, const QString& undoDescription);

private Q_SLOTS:

    void copyActionTriggered();
    void pasteActionTriggered();
    void slotBookmarkSelected(GPSDataContainer bookmarkPosition);
    void slotRemoveCoordinates();
    void slotRemoveAltitude();
    void slotRemoveUncertainty();
    void slotRemoveSpeed();

Q_SIGNALS:
    void signalUndoCommand(GPSUndoCommand* undoCommand);

private:

    GPSListViewContextMenuPriv* const d;
};

} // namespace KIPIGPSSyncPlugin

#endif /* GPSLISTVIEWCONTEXTMENU_H */
