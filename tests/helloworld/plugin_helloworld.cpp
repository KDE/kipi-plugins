/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-16
 * Description : an Hello World plugin.
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_helloworld.moc"

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

/// You must wrap all your plugin code to a dedicated namespace
namespace KIPIHelloWorldPlugin
{

/**
 * Macros from KDE KParts to export the symbols for this plugin and
 * create the factory for it. The first argument is the name of the
 * plugin library and the second is the genericfactory templated from
 * the class for your plugin
 */
K_PLUGIN_FACTORY(HelloWorldFactory, registerPlugin<Plugin_HelloWorld>();)
K_EXPORT_PLUGIN(HelloWorldFactory("kipiplugin_helloworld") )

Plugin_HelloWorld::Plugin_HelloWorld(QObject* const parent, const QVariantList&)
    : Plugin(HelloWorldFactory::componentData(), parent, "HelloWorld")
{
    /// There is a debug space for plugin loading area. Please do not use qDebug and qWarning in plugin.
    kDebug(AREA_CODE_LOADING) << "Plugin_HelloWorld plugin loaded";
}

Plugin_HelloWorld::~Plugin_HelloWorld()
{
}

void Plugin_HelloWorld::setup(QWidget* const widget)
{
    /** We pass the widget which host plugin in KIPI host application
     */
    Plugin::setup(widget);

    /** We define plugin action which will be plug in KIPI host application.
     */
    m_action = actionCollection()->addAction("helloworld");
    m_action->setText(i18n("Hello World..."));
    m_action->setIcon(KIcon("misc"));

    /** Connect action signal to dedicated slot.
     */
    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    /** Action is registered in plugin instance.
     */
    addAction(m_action);

    m_iface = dynamic_cast<Interface*>(parent());
    if (!m_iface)
    {
       /// No need special debug space outside load plugin area, it will be selected automatically.
       kError() << "Kipi interface is null!";
       return;
    }

    /** This will get items selection from KIPI host application
     */
    ImageCollection selection = m_iface->currentSelection();
    m_action->setEnabled(selection.isValid() && !selection.images().isEmpty());

    /** If selection change in KIPI host application, this signal will be fired, and plugin action enabled accordingly.
     */
    connect(m_iface, SIGNAL(selectionChanged(bool)),
            m_action, SLOT(setEnabled(bool)));
}

void Plugin_HelloWorld::slotActivate()
{
    /** When plugin action is actived, we display list of item selected in a message box.
     *  This example show a simple dialog with current items selected in KIPI host application.
     *  You can branch here your dedicated dialog to process items as you want. 
     */
    ImageCollection images = m_iface->currentSelection();

    if (!images.isValid() || images.images().isEmpty())
        return;

    KMessageBox::informationList(0,
                                 i18n("This is the list of selected items"),
                                 images.images().toStringList()
                                );
}

Category Plugin_HelloWorld::category(KAction* const action) const
{
    /// For each plugin actions defined, you can attribute a category which will plug it on right KIPI host application menu.
    if (action == m_action)
       return ImagesPlugin;

    /// No need special debug space outside load plugin area, it will be selected automatically.
    kWarning() << "Unrecognized action for plugin category identification";
    return ImagesPlugin; // no warning from compiler, please
}

}  // namespace KIPIHelloWorldPlugin
