/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-05-16
 * Description : a plugin to acquire image using flat scanner.
 *
 * Copyright (C) 2003-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_ACQUIREIMAGES_H
#define PLUGIN_ACQUIREIMAGES_H

// Qt includes

#include <QVariant>
#include <QPointer>

// LibKIPI includes

#include <libkipi/plugin.h>

class QWidget;

class KAction;

namespace KSaneIface
{
    class KSaneWidget;
}

using namespace KIPI;

namespace KIPIAcquireImagesPlugin
{

class ScanDialog;

class Plugin_AcquireImages : public Plugin
{
    Q_OBJECT

public:

    Plugin_AcquireImages(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_AcquireImages();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotActivate();

private:

    void setupActions();

private:

    QWidget*                 m_parentWidget;

    KAction*                 m_action_scanimages;

    KSaneIface::KSaneWidget* m_saneWidget;

    QPointer <ScanDialog>    m_scanDlg;
};

}  // namespace KIPIAcquireImagesPlugin

#endif /* PLUGIN_ACQUIREIMAGES_H */
