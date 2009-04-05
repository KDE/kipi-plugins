/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : a kipi plugin to automatically detect
 *               and remove red eyes from images
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

class Plugin_RemoveRedEyes: public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_RemoveRedEyes(QObject *parent, const QVariantList &args);
    ~Plugin_RemoveRedEyes();

    KIPI::Category category(KAction* action) const;
    void setup(QWidget* widget);

private Q_SLOTS:

    void activate();

private:

    KAction* m_action;
};

#endif // PLUGIN_REMOVEREDEYES_H
