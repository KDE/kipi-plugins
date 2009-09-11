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

// KDE includes

#include <kdialog.h>
#include <kurl.h>

// LibKIPI includes

#include <libkipi/interface.h>

namespace KIPIGPSSyncPlugin
{

class GPSSyncDialogPriv;

class GPSSyncDialog :public KDialog
{
    Q_OBJECT

public:

    GPSSyncDialog(KIPI::Interface* interface, QWidget* parent);
    ~GPSSyncDialog();

    void setImages(const KUrl::List& images);

protected:

    void closeEvent(QCloseEvent *);

protected Q_SLOTS:

    void slotApply();
    void slotHelp();
    void slotUser1Correlate();
    void slotUser2EditCoordinates();
    void slotUser3RemoveCoordinates();

private Q_SLOTS:

    void slotLoadGPXFile();

private:

    bool promptUserClose();
    void readSettings();
    void saveSettings();

private:

    GPSSyncDialogPriv* const d;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSSYNCDIALOG_H */
