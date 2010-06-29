/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a plugin to synchronize pictures with
 *               a GPS device.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <qwidget.h>
// KDE includes

#include <kdialog.h>
#include <kurl.h>

// Libkmap includes

#include <libkmap/worldmapwidget2_primitives.h>

// LibKIPI includes

#include <libkipi/interface.h>

namespace KIPIGPSSyncPlugin
{

class KipiImageModel;
class GPSUndoCommand;

class GPSSyncDialogPriv;

class GPSSyncWMWRepresentativeChooserPrivate;

class GPSSyncWMWRepresentativeChooser : public WMW2::WMWRepresentativeChooser
{
Q_OBJECT
public:
    GPSSyncWMWRepresentativeChooser(KipiImageModel* const model, QObject* const parent = 0);
    virtual ~GPSSyncWMWRepresentativeChooser();

    virtual QPixmap pixmapFromRepresentativeIndex(const QVariant& index, const QSize& size);
    virtual QVariant bestRepresentativeIndexFromList(const QList<QVariant>& list, const int sortKey);
    virtual bool indicesEqual(const QVariant& indexA, const QVariant& indexB);

private Q_SLOTS:
    void slotThumbnailFromModel(const QPersistentModelIndex& index, const QPixmap& pixmap);

private:
    GPSSyncWMWRepresentativeChooserPrivate* const d;
};


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

private Q_SLOTS:
    void slotCurrentImageChanged(const QModelIndex& current, const QModelIndex& previous);
    void slotImageActivated(const QModelIndex& index);
    void slotSetUIEnabled(const bool enabledState, QObject* const cancelObject, const QString& cancelSlot);
    void slotSetUIEnabled(const bool enabledState);
    void slotApplyClicked();
    void slotFileChangesSaved(int beginIndex, int endIndex);
    void slotProgressChanged(const int currentProgress);
    void slotProgressSetup(const int maxProgress, const QString& progressText);
    void slotMapMarkersMoved(const QList<QPersistentModelIndex>& movedMarkers, const WMW2::WMWGeoCoordinate& coordinates);
    void slotGPSUndoCommand(GPSUndoCommand* undoCommand);
    void slotSortOptionTriggered(QAction* sortAction);
    void setCurrentTab(const int index);
    void slotProgressCancelButtonClicked();
    void slotCurrentTabChanged(int);
    void slotBookmarkVisibilityToggled();

private:

    GPSSyncDialogPriv* const d;
    int splitterSize;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSSYNCDIALOG_H */
