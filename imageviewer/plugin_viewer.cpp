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

#include "plugin_viewer.h"
#include "plugin_viewer.moc"

// Qt includes

#include <QMessageBox>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kurl.h>

// LibKIPI includes

#include <libkipi/imageinfo.h>
#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "viewerwidget.h"

K_PLUGIN_FACTORY( viewerFactory, registerPlugin<Plugin_viewer>(); )
K_EXPORT_PLUGIN( viewerFactory("kipiplugin_imageviewer") )

Plugin_viewer::Plugin_viewer( QObject *parent, const QVariantList & )
             : KIPI::Plugin( viewerFactory::componentData(), parent, "kipiplugin_imageviewer" )
{
    kDebug(AREA_CODE_LOADING) << "OpenGL viewer plugin loaded";
}

void Plugin_viewer::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    KIPI::Interface* pv_interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !pv_interface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    actionViewer = actionCollection()->addAction("oglimageviewer");
    actionViewer->setText(i18n("OpenGL Image Viewer..."));
    actionViewer->setIcon(KIcon("ogl"));

    connect(actionViewer, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(actionViewer);

    widget=0;
}

/*!
    \fn Plugin_viewer::slotActivate()
 */
void  Plugin_viewer::slotActivate()
{
    KIPI::Interface* pv_interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !pv_interface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    widget = new KIPIviewer::ViewerWidget(pv_interface);
    if ( widget->listOfFilesIsEmpty() )
    {
        delete widget;
        return;
    }
    switch(widget->getOGLstate())
    {
        case KIPIviewer::oglOK:
            widget->show();
            break;

        case KIPIviewer::oglNoRectangularTexture:
            kError() << "GL_ARB_texture_rectangle not supported";
            delete widget;
            QMessageBox::critical(new QWidget(),i18n("OpenGL error"),i18n("GL_ARB_texture_rectangle not supported"));
            break;

        case KIPIviewer::oglNoContext:
            kError() << "no OpenGL context found";
            delete widget;
            QMessageBox::critical(new QWidget(),i18n("OpenGL error"),i18n("no OpenGL context found"));
    }
}

KIPI::Category Plugin_viewer::category( KAction* action ) const
{
    if ( action == actionViewer )
    {
        return KIPI::ToolsPlugin;
    }
    else
    {
        kWarning() << "Unrecognized action for plugin category identification";
        return KIPI::ToolsPlugin; // no warning from compiler, please
    }
}
