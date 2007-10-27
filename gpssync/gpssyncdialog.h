/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a plugin to synchronize pictures with 
 *               a GPS device.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes.

#include <kdialogbase.h>
#include <kurl.h>

// LibKipi includes.

#include <libkipi/interface.h>

class QListViewItem;

namespace KIPIGPSSyncPlugin
{

class GPSSyncDialogPriv;

class GPSSyncDialog :public KDialogBase 
{
    Q_OBJECT

public:

    GPSSyncDialog(KIPI::Interface* interface, QWidget* parent);
    ~GPSSyncDialog();

    void setImages(const KURL::List& images);

protected:

    void closeEvent(QCloseEvent *);

protected slots:

    void slotApply();
    void slotHelp();
    void slotClose();
    void slotUser1();
    void slotUser2();
    void slotUser3();

private slots:

    void slotLoadGPXFile(); 

private:

    bool promptUserClose();
    void readSettings();
    void saveSettings();

private:

    GPSSyncDialogPriv *d;
};

}  // NameSpace KIPIGPSSyncPlugin

#endif /* GPSSYNCDIALOG_H */
