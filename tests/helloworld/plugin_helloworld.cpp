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

/// No need to include plugin_helloworld.h, it will be done through Qt moc file.
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

/** Using private container everywhere is clear to speed up compilation and reduce source code depencies through header files.
 *  See this url for details : http://techbase.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B#Using_a_d-Pointer
 */
class Plugin_HelloWorld::Private
{
public:

    Private()
    {
        /// It's always clean to init pointers to zero. If crash appear, 
        /// debugger will show a null pointer instead a non initialized one.
        action1 = 0;
        iface  = 0;
    }

    /** These plugin actions will pluged into menu KIPI host application.
     */
    KAction*   action1;

    /** This is the interface instance to plugin host application. Note that you can get it everywhere in your plugin using
     *  instance of KIPI::PluginLoader singleton which provide a method for that.
     *  Since libkipi 2.0.0, KIPI host interface is also available from KIPI::Plugin::interface().
     */
    Interface* iface;
};

/** Macro from KDE KParts to create the factory for this plugin.
 *  The first argument is the name of the plugin library 
 *  and the second is the generic factory templated from
 *  the class for your plugin.
 */
K_PLUGIN_FACTORY(HelloWorldFactory, registerPlugin<Plugin_HelloWorld>();)

/** Macro from KDE KParts to export the symbols for this plugin
 *  NOTE: The plugin library is the name used in CMakeList.txt to link bin file, 
 *  and with X-KDE-Library value from .desktop file.
 */
K_EXPORT_PLUGIN(HelloWorldFactory("kipiplugin_helloworld") )

/** The plugin constructor. Note that plugin name passed as string in 3rd arguement of KIPI::Plugin parent class
 *  is the same than Name value from .desktop file.
 */
Plugin_HelloWorld::Plugin_HelloWorld(QObject* const parent, const QVariantList&)
    : Plugin(HelloWorldFactory::componentData(), parent, "HelloWorld"),
      /// Private container is allocated here.
      d(new Private)
{
    /// There is a debug space for plugin loading area. Please do not use qDebug and qWarning in plugin.
    kDebug(AREA_CODE_LOADING) << "Plugin_HelloWorld plugin loaded";
}

Plugin_HelloWorld::~Plugin_HelloWorld()
{
    /// Don't forget to clear d private container allocation in destructor to prevent memory leak.
    delete d;
}

void Plugin_HelloWorld::setup(QWidget* const widget)
{
    /** We pass the widget which host plugin in KIPI host application
     */
    Plugin::setup(widget);

    /** We will check KIPI host application interface instance validity
     */
    d->iface = dynamic_cast<Interface*>(parent());
    if (!d->iface)
    {
       /// No need special debug space outside load plugin area, it will be selected automatically.
       kError() << "Kipi interface is null!";
       return;
    }

    /** We define plugin action which will be plug in KIPI host application.
     *  Note that if you set keyboard shortcut to an action you must take a care
     *  about already existing one from other tool to prevent conflict.
     *  Don't forget to define an unique string name to your action, to be able to disable it 
     *  in KIPI host application if necessary. You must check of course name already used in 
     *  others tool before to prevent redondancy.
     */

    d->action1 = actionCollection()->addAction("helloworld");
    d->action1->setText(i18n("Hello World..."));
    d->action1->setIcon(KIcon("misc"));
    d->action1->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_H));

    /** Connect plugin action signal to dedicated slot.
     */
    connect(d->action1, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateAction1()));

    /** Action is registered in plugin instance.
     */
    addAction(d->action1);

    /** This will get items selection from KIPI host application.
     */
    ImageCollection selection = d->iface->currentSelection();
    d->action1->setEnabled(selection.isValid() && !selection.images().isEmpty());

    /** If selection change in KIPI host application, this signal will be fired, and plugin action enabled accordingly.
     */
    connect(d->iface, SIGNAL(selectionChanged(bool)),
            d->action1, SLOT(setEnabled(bool)));
}

void Plugin_HelloWorld::slotActivateAction1()
{
    /** When plugin action is actived, we display list of items selected in a message box.
     *  This example show a simple dialog with current items selected in KIPI host application.
     *  You can branch here your dedicated dialog to process items as you want. 
     */
    ImageCollection images = d->iface->currentSelection();

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
    if (action == d->action1)
       return ImagesPlugin;

    /// No need special debug space outside load plugin area, it will be selected automatically.
    kWarning() << "Unrecognized action for plugin category identification";
    return ImagesPlugin; // no warning from compiler, please
}

}  // namespace KIPIHelloWorldPlugin
