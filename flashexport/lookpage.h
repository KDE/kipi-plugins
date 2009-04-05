/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-21
 * Description : look settings page.
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Look
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef LOOK_PAGE_H
#define LOOK_PAGE_H

// Qt includes

#include <QWidget>

// Local includes

#include "simpleviewersettingscontainer.h"

namespace KIPIFlashExportPlugin
{
class LookPagePriv;

class LookPage : public QWidget
{
    Q_OBJECT

public:

    LookPage(QWidget* parent);
    ~LookPage();

    void setSettings(const SimpleViewerSettingsContainer& settings);
    void settings(SimpleViewerSettingsContainer& settings);

private:

    LookPagePriv* const d;
};

}  // namespace KIPIFlashExportPlugin

#endif // LOOK_PAGE_H 
