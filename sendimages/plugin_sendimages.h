/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2003-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_SENDIMAGES_H
#define PLUGIN_SENDIMAGES_H

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

class QWidget;
class QObject;

class KAction;

class Plugin_SendImagesPriv;

class Plugin_SendImages : public KIPI::Plugin
{

Q_OBJECT

public:

    Plugin_SendImages(QObject *parent, const QVariantList& args);
    ~Plugin_SendImages();

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* );

public Q_SLOTS:

    void slotActivate();

private Q_SLOTS:

    void slotPrepareEmail();

private:

    Plugin_SendImagesPriv* const d;
};

#endif // PLUGIN_SENDIMAGES_H 
