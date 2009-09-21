/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using 
 *               an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
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

#ifndef PLUGIN_VIEWER_H
#define PLUGIN_VIEWER_H

// LibKIPI includes

#include <libkipi/plugin.h>

// Local includes

#include "viewerwidget.h"

class KAction;

/**
 * @short integration with KIPI
 * @author Markus Leuthold <kusi (+at) forum.titlis.org>
 * @version 0.2
 */
class Plugin_viewer :public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_viewer( QObject *parent, const QVariantList &  );
    virtual void setup( QWidget* widget );
    virtual KIPI::Category category( KAction* action ) const;

public Q_SLOTS:

    void  slotActivate();

protected:

    KIPIviewer::ViewerWidget* widget;
    KAction*                  actionViewer;
};

#endif /* PLUGIN_VIEWER_H */
