/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a plugin to synchronize pictures with
 *               a GPS device.
 *
 * Copyright (C) 2006-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <kdialog.h>
#include <kurl.h>

// Libkmap includes

#include <libkmap/kmap_primitives.h>
#include <libkmap/kmap_modelhelper.h>

// LibKIPI includes

#include <libkipi/interface.h>

namespace KMap
{
    class KMapWidget;
}

namespace KIPIGPSSyncPlugin
{

class KipiImageModel;
class GPSUndoCommand;

class GPSSyncDialogPriv;

class GPSSyncKMapModelHelperPrivate;

class GPSSyncKMapModelHelper : public KMap::ModelHelper
{
Q_OBJECT

public:

    GPSSyncKMapModelHelper(KipiImageModel* const model, QItemSelectionModel* const selectionModel, QObject* const parent = 0);
    virtual ~GPSSyncKMapModelHelper();

    virtual QAbstractItemModel* model() const;
    virtual QItemSelectionModel* selectionModel() const;
    virtual bool itemCoordinates(const QModelIndex& index, KMap::GeoCoordinates* const coordinates) const;

    virtual QPixmap pixmapFromRepresentativeIndex(const QPersistentModelIndex& index, const QSize& size);
    virtual QPersistentModelIndex bestRepresentativeIndexFromList(const QList<QPersistentModelIndex>& list, const int sortKey);

    virtual void onIndicesMoved(const QList<QPersistentModelIndex>& movedMarkers, const KMap::GeoCoordinates& targetCoordinates, const QPersistentModelIndex& targetSnapIndex);

    void addUngroupedModelHelper(KMap::ModelHelper* const newModelHelper);

private Q_SLOTS:

    void slotThumbnailFromModel(const QPersistentModelIndex& index, const QPixmap& pixmap);

Q_SIGNALS:
    void signalUndoCommand(GPSUndoCommand* undoCommand);

private:

    GPSSyncKMapModelHelperPrivate* const d;
};

// ------------------------------------------------------------------------------------------------

class GPSSyncDialog : public KDialog
{
    Q_OBJECT

public:

    GPSSyncDialog(KIPI::Interface* interface, QWidget* parent);
    ~GPSSyncDialog();

    void setImages(const KUrl::List& images);

protected:

    void closeEvent(QCloseEvent* e);
    bool eventFilter( QObject *, QEvent *);

private:

    void readSettings();
    void saveSettings();
    void saveChanges(const bool closeAfterwards);
    KMap::KMapWidget* makeMapWidget(QWidget** const pvbox);
    void adjustMapLayout(const bool syncSettings);

private Q_SLOTS:

    void slotImageActivated(const QModelIndex& index);
    void slotSetUIEnabled(const bool enabledState, QObject* const cancelObject, const QString& cancelSlot);
    void slotSetUIEnabled(const bool enabledState);
    void slotApplyClicked();
    void slotConfigureClicked();
    void slotFileChangesSaved(int beginIndex, int endIndex);
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

    int splitterSize;
    GPSSyncDialogPriv* const d;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSSYNCDIALOG_H */
