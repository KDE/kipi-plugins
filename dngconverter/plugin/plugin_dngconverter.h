/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-24
 * Description : a kipi plugin to convert Raw file to DNG
 *
 * Copyright (C) 2008-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_DNGCONVERTER_H
#define PLUGIN_DNGCONVERTER_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPIDNGConverterPlugin
{
    class BatchDialog;
}

class Plugin_DNGConverter : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_DNGConverter(QObject* parent, const QVariantList& args);
    ~Plugin_DNGConverter();

    KIPI::Category category(KAction* action) const;
    void setup(QWidget* widget);

private:

    bool checkBinaries();

private Q_SLOTS:

    void slotActivate();

private:

    KAction*                             m_action;

    KIPIDNGConverterPlugin::BatchDialog* m_batchDlg;
};

#endif /* PLUGIN_DNGCONVERTER_H */
