/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-22
 * Description : a dialog to edit GPS positions
 * 
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GPSEDITDIALOG_H
#define GPSEDITDIALOG_H

// KDE includes.

#include <kdialogbase.h>

// Local includes.

#include "gpsdatacontainer.h"

namespace KIPIGPSSyncPlugin
{

class GPSEditDialogDialogPrivate;

class GPSEditDialog : public KDialogBase
{
    Q_OBJECT

public:

    GPSEditDialog(QWidget* parent, GPSDataContainer gpsData, 
                  const QString& fileName);
    ~GPSEditDialog();

    GPSDataContainer getGPSInfo();

protected slots:

    void slotOk();
    void slotUser1();
    void slotGPSLocator(int);

private:

    GPSEditDialogDialogPrivate * d;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSEDITDIALOG_H */
