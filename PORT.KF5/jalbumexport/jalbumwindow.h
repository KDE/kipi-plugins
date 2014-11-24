/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : a plugin to launch jAlbum using selected images.
 *
 * Copyright (C) 2013 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

#ifndef JALBUMWINDOW_H
#define JALBUMWINDOW_H

// Qt includes

#include <QList>
#include <QProgressDialog>

// Local includes

#include "kptooldialog.h"

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{
    class KPAboutData;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIJAlbumExportPlugin
{
class JAlbum;

class JAlbumWindow : public KPToolDialog
{
    Q_OBJECT

public:

    JAlbumWindow(QWidget* const parent, JAlbum* const pJAlbum);
    ~JAlbumWindow();

private:

    void connectSignals();
    void readSettings();

private Q_SLOTS:

    void slotSettings();
    void slotError(const QString& msg);
    void slotNewAlbum();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIJAlbumExportPlugin

#endif /* JALBUMWINDOW_H */
