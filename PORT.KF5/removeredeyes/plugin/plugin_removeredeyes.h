/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-06-08
 * Description : a kipi plugin to automatically detect
 *               and remove red eyes from images
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

#ifndef PLUGIN_REMOVEREDEYES_H
#define PLUGIN_REMOVEREDEYES_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

using namespace KIPI;

namespace KIPIRemoveRedEyesPlugin
{

class Plugin_RemoveRedEyes: public Plugin
{
    Q_OBJECT

public:

    Plugin_RemoveRedEyes(QObject* const parent, const QVariantList& args);
    ~Plugin_RemoveRedEyes();

    void setup(QWidget* const widget);

private Q_SLOTS:

    void activate();

private:

    void setupActions();

private:

    KAction* m_action;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif // PLUGIN_REMOVEREDEYES_H
