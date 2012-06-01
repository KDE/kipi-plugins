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

/** Standard C++ header wrapper
 */
#ifndef PLUGIN_HELLOWORLD_H
#define PLUGIN_HELLOWORLD_H

// NOTE: Always limit include in header file to speed-up compilation.

// Qt includes

#include <QVariant>

// LibKIPI includes

#include <libkipi/plugin.h>

/** To make source code more readable, we will declare KIPI namespace as well.
 */
using namespace KIPI;

/** Each plugin must be fully wrapped into a dedicated C++ namespace to prevent conflicts with symbols and identifiers.
 */
namespace KIPIHelloWorldPlugin
{

/** We will use KIPI::Plugin class as parent from libkipi
 *  See this API for details : http://api.kde.org/4.x-api/kdegraphics-apidocs/libs/libkipi/libkipi/html/index.html
 */
class Plugin_HelloWorld : public Plugin
{
    Q_OBJECT

public:

    /** Notice the constructor
        takes two arguments QObject* const parent (the parent of this object),
        and const QStringList& args (the arguments passed).
    */
    Plugin_HelloWorld(QObject* const parent, const QVariantList& args);
    ~Plugin_HelloWorld();

    /** This method assign the plugin category for each plugin actions. This plug action to
     *  right menu in KIPI host application.
     */
    Category category(KAction* const action) const;

    /** This method setup the plugin actions and connect internal signals and slots to handle plugin actions.
     */
    void setup(QWidget* const widget);

private Q_SLOTS:

    /** There are slots to handle action events.
     */
    void slotActivateActionImages();
    void slotActivateActionTools();
    void slotActivateActionExport();
    void slotActivateActionImport();

private:

    /** We use d private internal container. See implementation for details.
     */
    class Private;
    Private* const d;
};

}  // namespace KIPIHelloWorldPlugin

#endif // PLUGIN_HELLOWORLD_H
