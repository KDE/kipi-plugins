/* ============================================================
 *
 * Date        : 2010-03-26
 * Description : A widget to configure the GPS correlation
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

#ifndef GPSCORRELATORWIDGET_H
#define GPSCORRELATORWIDGET_H

// KDE includes

#include <kurl.h>

// Qt includes

#include <QWidget>

// local includes

#include "gpsdataparser.h"

class KConfigGroup;

namespace KIPIGPSSyncPlugin
{

class KipiImageModel;
class GPSUndoCommand;

class GPSCorrelatorWidgetPrivate;

class GPSCorrelatorWidget : public QWidget
{
Q_OBJECT

public:

    GPSCorrelatorWidget(QWidget* const parent, KipiImageModel* const imageModel, const int marginHint, const int spacingHint);
    ~GPSCorrelatorWidget();

    void setUIEnabledExternal(const bool state);
    void saveSettingsToGroup(KConfigGroup* const group);
    void readSettingsFromGroup(KConfigGroup* const group);

protected:
    void setUIEnabledInternal(const bool state);

Q_SIGNALS:
    void signalSetUIEnabled(const bool enabledState);
    void signalSetUIEnabled(const bool enabledState, QObject* const cancelObject, const QString& cancelSlot);
    void signalProgressSetup(const int maxProgress, const QString& progressText);
    void signalProgressChanged(const int currentProgress);
    void signalUndoCommand(GPSUndoCommand* undoCommand);

public Q_SLOTS:
    void slotCancelCorrelation();

private Q_SLOTS:
    void updateUIState();
    void slotLoadGPXFiles();
    void slotGPXFilesReadyAt(int beginIndex, int endIndex);
    void slotAllGPXFilesReady();
    void slotCorrelate();
    void slotItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List& correlatedItems);
    void slotAllItemsCorrelated();
    void slotCorrelationCanceled();

private:
    GPSCorrelatorWidgetPrivate* const d;
};

} /* KIPIGPSSyncPlugin */

#endif /* GPSCORRELATORWIDGET_H */

