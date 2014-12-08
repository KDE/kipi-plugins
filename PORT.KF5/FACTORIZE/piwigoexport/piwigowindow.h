/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2014-09-30
* Description : a plugin to export to a remote Piwigo server.
*
* Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
* Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
* Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
* Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
* Copyright (C) 2010-2014 by Frederic Coiffier <frederic dot coiffier at free dot com>
*
* This program is free software; you can redistribute it
* and/or modify it under the terms of the GNU General
* Public License as published by the Free Software Foundation;
* either version 2, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* ============================================================ */

#ifndef PIWIGOWINDOW_H
#define PIWIGOWINDOW_H

// Qt includes

#include <QList>

// local includes

#include "kptooldialog.h"

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIPiwigoExportPlugin
{

class Piwigo;
class GAlbum;

class PiwigoWindow : public KPToolDialog
{
    Q_OBJECT

public:

    PiwigoWindow(QWidget* const parent, Piwigo* const pPiwigo);
    ~PiwigoWindow();

private:

    void connectSignals();
    void readSettings();
    QString cleanName(const QString&) const;

private Q_SLOTS:

    void slotDoLogin();
    void slotLoginFailed(const QString& msg);
    void slotBusy(bool val);
    void slotProgressInfo(const QString& msg);
    void slotError(const QString& msg);
    void slotAlbums(const QList<GAlbum>& albumList);
    void slotAlbumSelected();
    void slotAddPhoto();
    void slotAddPhotoNext();
    void slotAddPhotoSucceeded();
    void slotAddPhotoFailed(const QString& msg);
    void slotAddPhotoCancel();
    void slotEnableSpinBox(int n);
    void slotSettings();
    void slotProcessUrl(const QString&);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPiwigoExportPlugin

#endif /* PIWIGOWINDOW_H */
