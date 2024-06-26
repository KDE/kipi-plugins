/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

#ifndef PLUGIN_FACEBOOK_H
#define PLUGIN_FACEBOOK_H

// Qt includes

#include <QVariant>

// Libkipi includes

#include <KIPI/Plugin>

class QAction;

namespace KIPIFacebookPlugin
{
    class FbWindow;
}

class KJob;

using namespace KIPI;

namespace KIPIFacebookPlugin
{

class Plugin_Facebook : public Plugin
{
    Q_OBJECT

public:

    Plugin_Facebook(QObject* const parent, const QVariantList& args);
    ~Plugin_Facebook();

    void setup(QWidget* const) override;

public Q_SLOTS:

    void slotExport();

private:

    void setupActions();

private:

    QAction*   m_actionExport;

    FbWindow*  m_dlgExport;
};

} // namespace KIPIFacebookPlugin

#endif // PLUGIN_FACEBOOK_H
