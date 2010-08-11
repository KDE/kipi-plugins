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

/**
 * @class GPSReverseGeocodingWidget
 *
 * @brief The GPSReverseGeocodingWidget class represents the main widget for reverse geocoding.
 */
class GPSReverseGeocodingWidget : public QWidget
{

Q_OBJECT

public: 
    /**
     * Constructor
     * @param interface The main KIPI interface
     * @param imageModel KIPI image model
     * @param selectionModel KIPI image selection model 
     * @param parent The parent object
     */
    GPSReverseGeocodingWidget(KIPI::Interface* interface, KipiImageModel* const imageModel, QItemSelectionModel* const selectionModel, QWidget* const parent = 0);
    
    /**
     * Destructor
     */  
    ~GPSReverseGeocodingWidget();

    /**
     * Sets whether the containing widgets are enabled or disabled.
     * @param state If true, the controls are enabled.
     */ 
    void setUIEnabled(const bool state);

    /**
     * Restores the settings of widgets contained in reverse geocoding widget.
     * @param group Here are stored the settings.
     */ 
    void readSettingsFromGroup(const KConfigGroup* const group);

    /**
     * Saves the settings of widgets contained in reverse geocoding widget.
     * @param group Here are stored the settings.
     */ 
    void saveSettingsToGroup(KConfigGroup* const group);

private:

private Q_SLOTS:

    /**
     * This slot triggeres when the button that start the reverse geocoding process is pressed.
     */ 
    void slotButtonRGSelected();

    /**
     * The data has returned from backend and now it's processed here.
     * @param returnedRGList Contains the data returned by backend.
     */
    void slotRGReady(QList<RGInfo>& returnedRGList);

    /**
     * Hide or shows the extra options.
     */ 
    void slotHideOptions();

    /**
     * Enables or disables the containing widgets.
     */  
    void updateUIState();

    /**
     * Adds a new tag to the tag tree.
     */ 
    void slotAddCustomizedSpacer();

    /**
     * Removes a tag from tag tree.
     * Note: If the tag is an external, it is no more deleted.
     */ 
    void slotRemoveTag();

    /**
     * Removes all spacers.
     */
    void slotRemoveAllSpacers();

    /**
     * Re-adds all deleted tags based on Undo/Redo widget.
     */ 
    void slotReaddNewTags();

    /**
     * Deletes and re-adds all new added tags.
     */  
    void slotRegenerateNewTags();

    /**
     * Adds all address elements below the selected tag. The address ellements are order by area size.
     * For example: country > state > state district > city ...  
     */
    void slotAddAllAddressElementsToTag();

    /**
     * Adds a tag to tag tree.
     */ 
    void slotAddSingleSpacer();

Q_SIGNALS:

    /**
     * This signal emits when containing widgets need to be enabled or disabled.
     * @param enabledState If true, the containing widgets will be enabled. Else, they will be disabled.
     */ 
    void signalSetUIEnabled(const bool enabledState);

    /**
     * This signal calls the loading bar.
     */ 
    void signalSetUIEnabled(const bool enabledState, QObject* const cancelObject, const QString& cancelSlot);
    void signalProgressSetup(const int maxProgress, const QString& progressText);
 
    /** 
     * Counts how many images were processed.
     * @param currentProgress The number of processed images.
     */ 
    void signalProgressChanged(const int currentProgress);

    /**
     * Sends the needed data to Undo/Redo Widget.
     * @param undoCommand Holds the data that will be used for undo or redo actions
     */  
    void signalUndoCommand(GPSUndoCommand* undoCommand);

protected:
   
    /**
     * Here are filtered the events.
     */ 
    virtual bool eventFilter(QObject* watched, QEvent* event);

private:

    GPSReverseGeocodingWidgetPrivate* const d;    

};


}  /* KIPIGPSSyncPlugin */

#endif /* REVERSEGEOCODING_H */


