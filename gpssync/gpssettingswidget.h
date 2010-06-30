/* ============================================================
 *
 * Date        : 2010-03-26
 * Description : A widget to configure the GPS sync dialog
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef GPSSETTINGSWIDGET_H
#define GPSSETTINGSWIDGET_H

// Qt includes

#include <QWidget>

namespace KIPIGPSSyncPlugin
{

class GPSSettingsWidgetPrivate;

class GPSSettingsWidget : public QWidget
{
Q_OBJECT

public:

    GPSSettingsWidget(QWidget* const parent = 0);
    ~GPSSettingsWidget();

protected:

private:
    GPSSettingsWidgetPrivate* const d;
};

} /* KIPIGPSSyncPlugin */

#endif /* GPSSETTINGSWIDGET_H */

