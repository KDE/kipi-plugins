/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kdeversion.h>

// LibKIPI includes

#include <libkipi/plugin.h>

#if KDE_IS_VERSION(4,3,60)
#include <libkipi/exportinterface.h>
#endif

class KAction;

namespace KIPIFacebookPlugin
{
    class FbWindow;
}

class KJob;

class Plugin_Facebook
    : public KIPI::Plugin
#if KDE_IS_VERSION(4,3,60)
    , public KIPI::ExportInterface
#endif
{
Q_OBJECT
#if KDE_IS_VERSION(4,3,60)
Q_INTERFACES( KIPI::ExportInterface )
#endif

public:

    Plugin_Facebook(QObject *parent, const QVariantList &args);
    ~Plugin_Facebook();

    KIPI::Category category(KAction* action) const;
    void setup(QWidget*);
    virtual KJob* exportFiles(const QString& album);

public Q_SLOTS:

    void slotImport();
    void slotExport();

private:

    KAction                      *m_actionImport;
    KAction                      *m_actionExport;

    KIPIFacebookPlugin::FbWindow *m_dlgImport;
    KIPIFacebookPlugin::FbWindow *m_dlgExport;
};

#endif // PLUGIN_FACEBOOK_H
