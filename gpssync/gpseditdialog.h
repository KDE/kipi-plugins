/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-22
 * Description : a dialog to edit GPS positions
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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GPSEDITDIALOG_H
#define GPSEDITDIALOG_H

// KDE includes

#include <kdialog.h>

// Local includes

#include "gpsdatacontainer.h"

namespace KIPIGPSSyncPlugin
{

class GPSEditDialogPrivate;

class GPSEditDialog : public KDialog
{
    Q_OBJECT

public:

    GPSEditDialog(QWidget* parent, const GPSDataContainer& gpsData,
                  const QString& fileName, bool hasGPSInfo);
    ~GPSEditDialog();

    GPSDataContainer getGPSInfo();

protected Q_SLOTS:

    void slotOk();
    void slotCancel();
    void slotNewGPSLocationFromMap(const QString& lat, const QString& lon, const QString& alt);
    void slotUpdateWorldMap();
    void slotGotoLocation();
    void slotGPSPositionChanged();
    void slotHelp();

protected:

    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

private:

    void readSettings();
    void saveSettings();
    bool checkGPSLocation();

private:

    GPSEditDialogPrivate* const d;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSEDITDIALOG_H */
