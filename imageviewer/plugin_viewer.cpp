/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-02-11
 * Description : a kipi plugin to show image using an OpenGL interface.
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

#include "plugin_viewer.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "viewerwidget.h"

namespace KIPIViewerPlugin
{

K_PLUGIN_FACTORY(viewerFactory, registerPlugin<Plugin_viewer>();)
K_EXPORT_PLUGIN(viewerFactory("kipiplugin_imageviewer"))

Plugin_viewer::Plugin_viewer(QObject* const parent, const QVariantList&)
     : Plugin(viewerFactory::componentData(), parent, "kipiplugin_imageviewer")
{
    kDebug(AREA_CODE_LOADING) << "OpenGL viewer plugin loaded";
}

Plugin_viewer::~Plugin_viewer()
{
}

void Plugin_viewer::setup(QWidget* widget)
{
    Plugin::setup(widget);

    Interface* iface = dynamic_cast<Interface*>(parent());

    if ( !iface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_actionViewer = actionCollection()->addAction("oglimageviewer");
    m_actionViewer->setText(i18n("OpenGL Image Viewer..."));
    m_actionViewer->setIcon(KIcon("ogl"));

    connect(m_actionViewer, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_actionViewer);
}

void  Plugin_viewer::slotActivate()
{
    Interface* iface = dynamic_cast<Interface*>(parent());

    if ( !iface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_widget = new ViewerWidget(iface);

    if ( m_widget->listOfFilesIsEmpty() )
    {
        delete m_widget;
        return;
    }

    switch(m_widget->getOGLstate())
    {
        case oglOK:
            m_widget->show();
            break;

        case oglNoRectangularTexture:
            kError() << "GL_ARB_texture_rectangle not supported";
            delete m_widget;
            KMessageBox::error(0, i18n("OpenGL error"), i18n("GL_ARB_texture_rectangle not supported"));
            break;

        case oglNoContext:
            kError() << "no OpenGL context found";
            delete m_widget;
            KMessageBox::error(0, i18n("OpenGL error"), i18n("no OpenGL context found"));
            break;
    }
}

Category Plugin_viewer::category(KAction* action) const
{
    if ( action == m_actionViewer )
    {
        return ToolsPlugin;
    }
    else
    {
        kWarning() << "Unrecognized action for plugin category identification";
        return ToolsPlugin; // no warning from compiler, please
    }
}

} // namespace KIPIViewerPlugin
