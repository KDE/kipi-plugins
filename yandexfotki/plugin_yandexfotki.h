/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to Yandex.Fotki web service
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
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

#ifndef PLUGIN_YANDEXFOTKI_H
#define PLUGIN_YANDEXFOTKI_H

// KDE includes

#include <kaction.h>

// Libkipi includes

#include <libkipi/plugin.h>

// Local includes

#include "yfwindow.h"

using namespace KIPI;

namespace KIPIYandexFotkiPlugin
{

class Plugin_YandexFotki : public Plugin
{
    Q_OBJECT

public:

    Plugin_YandexFotki(QObject* const parent, const QVariantList& args);
    ~Plugin_YandexFotki();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotExport();

private:

    void setupActions();

private:

    KAction*           m_actionExport;
    YandexFotkiWindow* m_dlgExport;
};

} // namespace KIPIYandexFotkiPlugin

#endif // PLUGIN_YANDEXFOTKI_H
