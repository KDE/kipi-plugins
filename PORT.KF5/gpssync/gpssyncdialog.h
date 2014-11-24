/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-05-16
 * @brief  A plugin to synchronize pictures with a GPS device.
 *
 * @author Copyright (C) 2006-2013 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2010, 2014 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 * @author Copyright (C) 2010 by Gabriel Voicu
 *         <a href="mailto:ping dot gabi at gmail dot com">ping dot gabi at gmail dot com</a>
 * @author Copyright (C) 2014 by Justus Schwartz
 *         <a href="mailto:justus at gmx dot li">justus at gmx dot li</a>
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

#ifndef GPSSYNCDIALOG_H
#define GPSSYNCDIALOG_H

// Qt includes

#include <QModelIndex>
#include <QWidget>

// KDE includes

#include <kurl.h>

// Libkgeomap includes

#include <libkgeomap/kgeomap_primitives.h>
#include <libkgeomap/modelhelper.h>
#include <libkgeomap/tracks.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kptooldialog.h"

namespace KGeoMap
{
    class KGeoMapWidget;
}

using namespace KIPI;
using namespace KIPIPlugins;
using namespace KGeoMap;

namespace KIPIGPSSyncPlugin
{

class KipiImageModel;
class GPSUndoCommand;
class GPSCorrelatorWidget;

class GPSSyncKGeoMapModelHelper : public ModelHelper
{
    Q_OBJECT

public:

    GPSSyncKGeoMapModelHelper(KipiImageModel* const model, QItemSelectionModel* const selectionModel, QObject* const parent = 0);
    virtual ~GPSSyncKGeoMapModelHelper();

    virtual QAbstractItemModel* model() const;
    virtual QItemSelectionModel* selectionModel() const;
    virtual bool itemCoordinates(const QModelIndex& index, GeoCoordinates* const coordinates) const;
    virtual Flags modelFlags() const;

    virtual QPixmap pixmapFromRepresentativeIndex(const QPersistentModelIndex& index, const QSize& size);
    virtual QPersistentModelIndex bestRepresentativeIndexFromList(const QList<QPersistentModelIndex>& list, const int sortKey);

    virtual void onIndicesMoved(const QList<QPersistentModelIndex>& movedMarkers, 
                                const GeoCoordinates& targetCoordinates, const QPersistentModelIndex& targetSnapIndex);

    void addUngroupedModelHelper(ModelHelper* const newModelHelper);

private Q_SLOTS:

    void slotThumbnailFromModel(const QPersistentModelIndex& index, const QPixmap& pixmap);

Q_SIGNALS:

    void signalUndoCommand(GPSUndoCommand* undoCommand);

private:

    class Private;
    Private* const d;
};

// ------------------------------------------------------------------------------------------------

class GPSSyncDialog : public KPToolDialog
{
    Q_OBJECT

public:

    explicit GPSSyncDialog(QWidget* const parent);
    ~GPSSyncDialog();

    void setImages(const KUrl::List& images);

protected:

    void closeEvent(QCloseEvent* e);
    bool eventFilter(QObject*, QEvent*);

private:

    void readSettings();
    void saveSettings();
    void saveChanges(const bool closeAfterwards);
    KGeoMapWidget* makeMapWidget(QWidget** const pvbox);
    void adjustMapLayout(const bool syncSettings);

private Q_SLOTS:

    void slotImageActivated(const QModelIndex& index);
    void slotSetUIEnabled(const bool enabledState, QObject* const cancelObject, const QString& cancelSlot);
    void slotSetUIEnabled(const bool enabledState);
    void slotApplyClicked();
    void slotConfigureClicked();
    void slotFileChangesSaved(int beginIndex, int endIndex);
    void slotFileMetadataLoaded(int beginIndex, int endIndex);
    void slotProgressChanged(const int currentProgress);
    void slotProgressSetup(const int maxProgress, const QString& progressText);
    void slotGPSUndoCommand(GPSUndoCommand* undoCommand);
    void slotSortOptionTriggered(QAction* sortAction);
    void setCurrentTab(const int index);
    void slotProgressCancelButtonClicked();
    void slotCurrentTabChanged(int);
    void slotBookmarkVisibilityToggled();
    void slotSetupChanged();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSSYNCDIALOG_H */
