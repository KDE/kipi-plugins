/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright (C) 2006-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_TIMEADJUST_H
#define PLUGIN_TIMEADJUST_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

using namespace KIPI;

namespace KIPITimeAdjustPlugin
{

class Plugin_TimeAdjust : public Plugin
{
    Q_OBJECT

public:

    Plugin_TimeAdjust(QObject* const parent, const QVariantList& args);
   ~Plugin_TimeAdjust();

    void setup(QWidget* const);

protected:

    void setupActions();

protected Q_SLOTS:

    void slotActivate();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPITimeAdjustPlugin

#endif // PLUGIN_TIMEADJUST_H
