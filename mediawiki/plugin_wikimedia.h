/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : A kipi plugin to export images to a MediaWiki wiki
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_WIKIMEDIA_H
#define PLUGIN_WIKIMEDIA_H

// Qt includes

#include <QVariant>

// KIPI includes

#include <libkipi/plugin.h>

// MediaWiki includes

#include <libmediawiki/login.h>
#include <libmediawiki/mediawiki.h>

class KAction;

using namespace KIPI;

namespace KIPIWikiMediaPlugin
{

class WMWindow;

class Plugin_WikiMedia : public Plugin
{
    Q_OBJECT

public:

    Plugin_WikiMedia(QObject* const parent, const QVariantList& args);
    ~Plugin_WikiMedia();

    void setup(QWidget* const);
    void runMWindow();

public Q_SLOTS:

    void slotExport();

private:

    void setupActions();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIWikiMediaPlugin

#endif // PLUGIN_WIKIMEDIA_H
