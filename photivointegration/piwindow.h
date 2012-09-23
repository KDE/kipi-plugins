/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-08-04
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
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

#ifndef KIPI_PIWINDOW_H
#define KIPI_PIWINDOW_H

// Qt includes

#include <QObject>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kptooldialog.h"

// forward declarations

namespace KIPIPlugins
{
    class KPImagesList;
}

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIPhotivoIntegrationPlugin
{

// ----------------------------------------------------------------------------

class PIWindow : public KPToolDialog
{
    Q_OBJECT

public:

    PIWindow(QWidget* const parent = 0);
    ~PIWindow();

    void reactivate();

 private:

    // pImpl idiom
    class Private;
    Private* const d;
};

// ----------------------------------------------------------------------------

} // namespace KIPIPhotivoIntegrationPlugin

#endif // KIPI_PIWINDOW_H */
