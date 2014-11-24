/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-10-31
 * Description : a widget to display simple settings
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#ifndef SIMPLESETTINGS_H
#define SIMPLESETTINGS_H

// Qt includes

#include <QWidget>

namespace KIPIRemoveRedEyesPlugin
{

class HaarSettings;

class SimpleSettings : public QWidget
{
    Q_OBJECT

public:

    enum SettingsType
    {
        Fast = 0,
        Standard,
        Slow
    };

public:

    SimpleSettings(QWidget* const parent = 0);
    ~SimpleSettings();

    void loadSettings(HaarSettings&);
    HaarSettings readSettings();

    int simpleMode() const;

Q_SIGNALS:

    void settingsChanged();

private Q_SLOTS:

    void simpleModeChanged(int);
    void prepareSettings();

private:

    struct Private;
    Private* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* SIMPLESETTINGS_H */
