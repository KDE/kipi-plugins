/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-10-18
 * Description : e-mail settings page.
 *
 * Copyright (C) 2007-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include <emailsettings.h>

namespace KIPISendimagesPlugin
{

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:

    SettingsWidget(QWidget* const parent);
    ~SettingsWidget();

    void setEmailSettings(const EmailSettings& settings);
    EmailSettings emailSettings() const;

private Q_SLOTS:

    void slotImagesFormatChanged(int);

private:

    class Private;
    Private* const d;
};

}  // namespace KIPISendimagesPlugin

#endif // SETTINGSWIDGET_H 
