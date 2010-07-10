/* ============================================================
 *
 * Date        : 2010-05-12
 * Description : A widget to apply Reverse Geocoding
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
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


#ifndef REVERSEGEOCODING_H
#define REVERSEGEOCODING_H

//KDE includes

#include <kurl.h>
#include <kconfig.h>

//Qt includes

#include <QWidget>
#include <QItemSelection>

// Libkmap includes

#include <libkmap/kmap_primitives.h>

//local includes

#include "gpsundocommand.h"
#include "backend-rg.h"

#include <libkipi/interface.h>

class QItemSelectionModel;

namespace KIPIGPSSyncPlugin
{


class GPSReverseGeocodingWidgetPrivate;
class KipiImageModel;

class GPSReverseGeocodingWidget : public QWidget
{

Q_OBJECT

public: 

    GPSReverseGeocodingWidget(KIPI::Interface*, KipiImageModel* const imageModel, QItemSelectionModel* const selectionModel, QWidget* const parent = 0);
    ~GPSReverseGeocodingWidget();

    void setUIEnabled(const bool state);
    void readSettingsFromGroup(const KConfigGroup* const group);
    void saveSettingsToGroup(KConfigGroup* const group);

private:

private Q_SLOTS:
    void slotButtonRGSelected();
    void slotRGReady(QList<RGInfo>&);
    void slotHideOptions();
    void updateUIState();
    void slotAddCountry();
    void slotAddState();
    void slotAddCounty(); 
    void slotAddCity();
    void slotAddTown();
    void slotAddVillage();
    void slotAddHamlet();
    void slotAddStreet();
    void slotAddPlace();
    void slotAddLAU2();
    void slotAddLAU1();
    void slotAddCustomizedSpacer();
    void slotRemoveTag();
    void slotRemoveAllNewTags();
    void slotReaddNewTags();
    void slotRegenerateNewTags();
    void slotAddAllAddressElementsToTag();
    void slotAddSingleSpacer(QAction*);

Q_SIGNALS:

    void signalSetUIEnabled(const bool enabledState);
    void signalSetUIEnabled(const bool enabledState, QObject* const cancelObject, const QString& cancelSlot);
    void signalProgressSetup(const int maxProgress, const QString& progressText);
    void signalProgressChanged(const int currentProgress);
    void signalUndoCommand(GPSUndoCommand* undoCommand);

protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);

private:

    GPSReverseGeocodingWidgetPrivate* const d;    

};


}  /* KIPIGPSSyncPlugin */

#endif /* REVERSEGEOCODING_H */


