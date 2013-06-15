/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using an OpenGL interface.
 *
 * Copyright (C) 2007-2008 by Markus Leuthold <kusi at forum dot titlis dot org>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_viewer.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kurl.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "viewerwidget.h"

namespace KIPIViewerPlugin
{

K_PLUGIN_FACTORY(viewerFactory, registerPlugin<Plugin_viewer>();)
K_EXPORT_PLUGIN(viewerFactory("kipiplugin_imageviewer"))

class Plugin_viewer::Private
{
public:

    Private()
    {
        widget       = 0;
        actionViewer = 0;
    }

    ViewerWidget* widget;
    KAction*      actionViewer;
};

Plugin_viewer::Plugin_viewer(QObject* const parent, const QVariantList&)
     : Plugin(viewerFactory::componentData(), parent, "kipiplugin_imageviewer"),
       d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "OpenGL viewer plugin loaded";

    setUiBaseName("kipiplugin_imageviewerui.rc");
    setupXML();
}

Plugin_viewer::~Plugin_viewer()
{
    delete d;
}

void Plugin_viewer::setup(QWidget* const widget)
{
    Plugin::setup(widget);
    setupActions();

    if (!interface())
    {
        kError() << "Kipi interface is null!";
        return;
    }

    d->actionViewer->setEnabled(true);
}

void Plugin_viewer::setupActions()
{
    setDefaultCategory(ToolsPlugin);

    d->actionViewer = new KAction(this);
    d->actionViewer->setText(i18n("OpenGL Image Viewer..."));
    d->actionViewer->setIcon(KIcon("kipi-ogl"));
    d->actionViewer->setEnabled(false);

    connect(d->actionViewer, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("oglimageviewer", d->actionViewer);
}

void Plugin_viewer::slotActivate()
{
    d->widget = new ViewerWidget();

    if ( d->widget->listOfFilesIsEmpty() )
    {
        delete d->widget;
        return;
    }

    switch(d->widget->getOGLstate())
    {
        case oglOK:
            d->widget->show();
            break;

        case oglNoRectangularTexture:
            kError() << "GL_ARB_texture_rectangle not supported";
            delete d->widget;
            KMessageBox::error(0, i18n("OpenGL error"), i18n("GL_ARB_texture_rectangle not supported"));
            break;

        case oglNoContext:
            kError() << "no OpenGL context found";
            delete d->widget;
            KMessageBox::error(0, i18n("OpenGL error"), i18n("No OpenGL context found"));
            break;
    }
}

} // namespace KIPIViewerPlugin
