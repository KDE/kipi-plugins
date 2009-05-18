/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-07
 * Description : a dialog to edit GPS track list.
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef GPSTRACKLISTEDITDIALOG_H
#define GPSTRACKLISTEDITDIALOG_H

// KDE includes

#include <kdialog.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "gpstracklistcontainer.h"

namespace KIPIGPSSyncPlugin
{

class GPSTrackListEditDialogPrivate;

class GPSTrackListEditDialog : public KDialog
{
    Q_OBJECT

public:

    GPSTrackListEditDialog(KIPI::Interface *interface, QWidget *parent, const GPSTrackList& gpsTrackList);
    ~GPSTrackListEditDialog();

    GPSTrackList trackList() const;

protected Q_SLOTS:

    void slotOk();
    void slotCancel();
    void slotUpdateWorldMap();
    void slotHelp();
    void slotNewGPSLocationFromMap(int id, double lat, double lng, double alt);
    void slotMarkerSelectedFromMap(int id);

protected:

    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

private:

    void readSettings();
    void saveSettings();

private:

    GPSTrackListEditDialogPrivate* const d;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSTRACKLISTEDITDIALOG_H */
