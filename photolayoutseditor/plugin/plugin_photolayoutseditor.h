/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011      by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_PHOTOLAYOUTSEDITOR_H
#define PLUGIN_PHOTOLAYOUTSEDITOR_H

#include <QWidget>
#include <QVariant>

#include <libkipi/plugin.h>

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIPhotoLayoutsEditor
{

class PhotoLayoutsEditor;

class Plugin_PhotoLayoutsEditor : public Plugin
{
    Q_OBJECT

public:

    Plugin_PhotoLayoutsEditor(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_PhotoLayoutsEditor();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotActivate();

private:

    void setupActions();

private:

    QWidget*            m_parentWidget;

    KAction*            m_action;

    PhotoLayoutsEditor* m_manager;

    Interface*          m_interface;
};

} // namespace KIPIPhotoLayoutsEditor

#endif // PLUGIN_PHOTOLAYOUTSEDITOR_H
