/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-01-31
 * Description : a kipi plugin to export images as video slide show
 *
 * Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
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

#ifndef PLUGIN_VIDEOSLIDESHOW_H
#define PLUGIN_VIDEOSLIDESHOW_H_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

using namespace KIPI;

namespace KIPIVideoSlideShowPlugin
{

class ExportDialog;

class Plugin_VideoSlideShow : public Plugin
{
    Q_OBJECT

public:

    Plugin_VideoSlideShow(QObject* const parent, const QVariantList& args);
    ~Plugin_VideoSlideShow();

    void setup(QWidget* const widget);

private Q_SLOTS:

    void slotExport();

private:

    void setupActions();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIVideoSlideShowPlugin

#endif /* PLUGIN_VIDEOSLIDESHOW_H */
