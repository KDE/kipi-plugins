/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

namespace KIPIPhotoLayoutsEditor
{
    class PhotoLayoutsEditor;
}

class Plugin_PhotoLayoutsEditor : public KIPI::Plugin
{
        Q_OBJECT

    public:

        Plugin_PhotoLayoutsEditor(QObject* parent, const QVariantList& args);
        virtual ~Plugin_PhotoLayoutsEditor();

        KIPI::Category category( KAction* action ) const;
        void setup( QWidget* );

        static Plugin_PhotoLayoutsEditor * self();

    public Q_SLOTS:

        void slotActivate();

    private:

        QWidget*                                    m_parentWidget;

        KAction*                                    m_action;

        KIPIPhotoLayoutsEditor::PhotoLayoutsEditor*   m_manager;

        KIPI::Interface*                            m_interface;
};

#endif // PLUGIN_PHOTOLAYOUTSEDITOR_H
