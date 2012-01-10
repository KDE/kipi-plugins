/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-11-21
 * @brief  kipi host test application
 *
 * @author Copyright (C) 2009-2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

// C++ include

#include <memory>

// Qt includes

#include <QTextStream>
#include <QMenu>

// KDE includes

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kaction.h>
#include <kactioncollection.h>

// LibKIPI includes

#include <libkipi/plugin.h>
#include <libkipi/pluginloader.h>

// local includes:

#include "kipiinterface.h"
#include "kipitest-debug.h"
#include "pluginsversion.h"

/**
* \brief Returns the name of a KIPI::Category
* \param category Category which should be returned as a string
* \returns String version of the category
*/
QString PluginCategoriesToString(const KIPI::Category& category)
{
    QString categorystring;
    switch(category)
    {
        case KIPI::ImagesPlugin:
            categorystring = i18n("Images");
            break;

        case KIPI::EffectsPlugin:
            categorystring = i18n("Effect");
            break;

        case KIPI::ToolsPlugin:
            categorystring = i18n("Tool");
            break;

        case KIPI::ImportPlugin:
            categorystring = i18n("Import");
            break;

        case KIPI::ExportPlugin:
            categorystring = i18n("Export");
            break;

        case KIPI::BatchPlugin:
            categorystring = i18n("Batch");
            break;

        case KIPI::CollectionsPlugin:
            categorystring = i18n("Collections");
            break;

        default:
            categorystring = i18n("Unknown");
            break;
    };

    return categorystring;
}

/**
* \brief Takes a list of QActions with submenus and converts them to a flat list
* \param actions List of QActions with submenus
* \param level Initial level of indentation (used internally for recursion)
* \returns A list of QPairs containing the indentation level and the QAction*
*/
QList<QPair<int, QAction*> > FlattenActionList(const QList<QAction*>& actions, const int level = 0)
{
    QList<QPair<int, QAction*> > results;

    for (QList<QAction*>::ConstIterator it = actions.constBegin(); it!=actions.constEnd(); ++it)
    {
        const QString text = (*it)->text();
        if (!text.isEmpty())
        {
            results.append( QPair<int, QAction*>(level, *it) );
        }
        QMenu* const menu = (*it)->menu();
        if (menu)
        {
            results.append( FlattenActionList(menu->actions(), level+1) );
        }
    }
    return results;
}

/**
* \brief Converts a QList<KAction*> to a QList<QAction*>
* \param list List to convert
* \returns The converted list
*/
QList<QAction*> QListKAction2QListQAction(const QList<KAction*>& list)
{
    QList<QAction*> result;
    for (QList<KAction*>::ConstIterator it = list.constBegin(); it!=list.constEnd(); ++it)
    {
        result.append(*it);
    }
    return result;
}

/**
* \brief Loads the kipi-plugins
* \param libraryName If not empty, only the plugin in library libraryName is loaded and setup
* \returns False if plugin library libraryName could not be loaded
*/
bool LoadPlugins(const QString& libraryName = "")
{
    if (!libraryName.isEmpty())
    {
        qerr << QString("Will only load library \"%1\"").arg(libraryName) << "\n";
    }
    else
    {
        qerr << "Will load all libraries, if possible." << "\n";
    }

    KIPI::PluginLoader* const kipiPluginLoader = KIPI::PluginLoader::instance();

    if (libraryName.isEmpty())
    {
        kipiPluginLoader->loadPlugins();
        return true;
    }

    const KIPI::PluginLoader::PluginList pluginList = kipiPluginLoader->pluginList();
    bool foundPlugin                                = false;

    for (KIPI::PluginLoader::PluginList::ConstIterator it = pluginList.constBegin();
         it!=pluginList.constEnd(); ++it)
    {
        if ( (*it)->library() == libraryName )
        {
            if ( !(*it)->shouldLoad() )
            {
                qerr << i18n("Can not load plugin \"%1\": Loader says it should not load.", libraryName) << endl;
                return false;
            }

            foundPlugin = true;
            (*it)->shouldLoad();

            if ( !(*it)->plugin() )
            {
                qerr << i18n("Plugin \"%1\" failed to load.", libraryName) << endl;
                return false;
            }

            qerr << i18n("Plugin \"%1\" loaded.", libraryName) << endl;
            return true;
        }
    }

    qerr << i18n("Plugin \"%1\" not found.", libraryName) << endl;

    return false;
}

/**
* \brief Lists the available plugins and their actions
* \param libraryName Load only plugin found in library libraryName
* \returns False if plugin in library \a libraryName could not be loaded
*/
bool ListPlugins(const QString& libraryName = "")
{
    KIPI::PluginLoader* const kipiPluginLoader = KIPI::PluginLoader::instance();
    if ( !LoadPlugins( libraryName ) )
        return false;

    const KIPI::PluginLoader::PluginList pluginList = kipiPluginLoader->pluginList();
    int pluginNumber                                = 1;
    const int nPlugins                              = pluginList.size();
    const int nDigits                               = QString::number(nPlugins).size();
    const QString preSpace                          = QString(nDigits+1+1, ' ');

    std::auto_ptr<QWidget> dummyWidget( new QWidget() );

    qerr << i18np("Found 1 plugin:", "Found %1 plugins:", nPlugins) << endl;

    for (KIPI::PluginLoader::PluginList::ConstIterator it = pluginList.constBegin();
         it!= pluginList.constEnd(); ++ it)
    {
        const QString pluginNumberString = QString("%1").arg(pluginNumber, nDigits); ++pluginNumber;

        qerr << QString("%1: %2 - %3").arg(pluginNumberString).arg((*it)->name()).arg((*it)->comment()) << endl;
        //      qout<<preSpace<< i18n("Should load: ")<< ( (*it)->shouldLoad() ? i18n("Yes") : i18n("No") ) << endl;
        qerr << preSpace<< i18n("Library: ")<< (*it)->library() << endl;

        KIPI::Plugin* const plugin = (*it)->plugin();

        if (plugin == 0)
        {
            qerr << preSpace << i18n( "Plugin not loaded." ) << endl;
            continue;
        }

        plugin->setup(dummyWidget.get());
        const QList<QPair<int, QAction*> > actionsList = FlattenActionList(QListKAction2QListQAction(plugin->actions()));
        qerr << preSpace<<i18n("Actions:") << endl;
        const QString preSpaceActions = preSpace + "  ";

        for (QList<QPair<int, QAction*> >::ConstIterator it = actionsList.constBegin();
             it!=actionsList.constEnd(); ++it)
        {
            const int level             = (*it).first;
            const QAction* const action = (*it).second;
            qerr << preSpaceActions << QString(level*2, ' ') << '"' << action->text() << '"' << endl;
        }
    }
    return true;
}

/**
* \brief Calls an action of a plugin
* \param actionText Text of the action to call
* \param libraryName Load only the plugin in this library
* \returns False if the action could not be called
*/
bool CallAction(const QString& actionText, const QString& libraryName = "")
{
    kDebug() << QString("Looking for action \"%1\"...").arg(actionText);

    KIPI::PluginLoader* const kipiPluginLoader = KIPI::PluginLoader::instance();
    if ( !LoadPlugins( libraryName ) )
        return false;

    KIPI::PluginLoader::PluginList pluginList = kipiPluginLoader->pluginList();

    /*std::auto_ptr<*/QWidget* dummyWidget( new QWidget() );

    bool foundAction = false;

    for (KIPI::PluginLoader::PluginList::ConstIterator info = pluginList.constBegin();
         (info!=pluginList.constEnd()) && !foundAction; ++info)
    {
        if ( !libraryName.isEmpty() && ( (*info)->library() != libraryName ) )
            continue;

        // scan for the desired action:
        KIPI::Plugin* const plugin = (*info)->plugin();

        if (plugin == 0)
        {
            qerr << i18n("Plugin \"%1\" failed to load.", (*info)->library()) << endl;
            continue;
        }

        plugin->setup(dummyWidget/*.get()*/);
        const QList<QPair<int, QAction*> > actionsList = FlattenActionList(QListKAction2QListQAction(plugin->actions()));

        for (QList<QPair<int, QAction*> >::ConstIterator it = actionsList.constBegin();
             it!=actionsList.constEnd(); ++it)
        {
            QAction* const pluginAction = (*it).second;
            qDebug()<<pluginAction->text();

            if ( pluginAction->text() != actionText )
                continue;

            qerr << i18n("Found action \"%1\" in library \"%2\", will now call it.", actionText, (*info)->library() )
                 << endl;

            // call the action:
            pluginAction->trigger();
            qerr << i18n("Plugin is done.") << endl;
            foundAction = true;

            break;
        }
    }

    if (!foundAction)
        qerr << i18n("Could not find action \"%1\".", actionText) << endl;

    return foundAction;
}

int main(int argc, char* argv[])
{
    const KAboutData aboutData("kipiplugins",
                               "kipi",
                               ki18n("kipicmd"),
                               kipiplugins_version,            // version
                               ki18n("Kipi host test application"),
                               KAboutData::License_GPL,
                               ki18n("(c) 2009-2010 Michael G. Hansen"),
                               KLocalizedString(),             // optional text
                               "http://www.kipi-plugins.org",  // URI of homepage
                               "kde-imaging@kde.org"           // bugs e-mail address
                              );

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineOptions options;
    options.add( "listplugins", ki18n("List the available plugins") );
    options.add( "l" ).add("library <libraryname>", ki18n("Library name of plugin to use") );
    options.add( "a" ).add( "action <action>", ki18n("Action to call") );
    options.add( "w" ).add( "wait", ki18n("Wait until non-modal dialogs are closed.") );
    options.add( "e" ).add( "stderr", ki18n("Send additional debug output to stderr") );
    options.add( "k" ).add( "kdebug", ki18n("Send additional debug output to kdebug") );
    options.add( "!selectedalbums <album>", ki18n("Selected albums") );
    options.add( "!selectedimages <images>", ki18n("Selected images") );
    options.add( "!allalbums <albums>", ki18n("All albums") );
    options.add( "+[images]", ki18n("List of images") );
    options.add( "+[albums]", ki18n("List of albums") );
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;

    KGlobal::locale()->insertCatalog("kipiplugins");
    KGlobal::locale()->insertCatalog("libkdcraw");

    KipiInterface* const kipiInterface = new KipiInterface(&app);

    // create an instance of the plugin loader:
    new KIPI::PluginLoader(QStringList(), kipiInterface);

    KCmdLineArgs* const args           = KCmdLineArgs::parsedArgs();

    if (args->isSet("stderr"))
    {
        kipiTestDebugTarget = KipiTestDebugStdErr;
    }
    if (args->isSet("kdebug"))
    {
        kipiTestDebugTarget = KipiTestDebugKDebug;
    }

    // handling of selectedimages, selectalbums and otheralbums is complicated
    // create new lists of these parameters:
    KUrl::List listSelectedImages;
    KUrl::List listSelectedAlbums;
    KUrl::List listAllAlbums;

    // determine which with list we start:
    KUrl::List* startList = 0;

    if (args->isSet("selectedimages"))
    {
        startList = &listSelectedImages;
        startList->append(KCmdLineArgs::makeURL(args->getOption("selectedimages").toUtf8()));
    }
    else if (args->isSet("selectedalbums"))
    {
        startList = &listSelectedAlbums;
        startList->append(KCmdLineArgs::makeURL(args->getOption("selectedalbums").toUtf8()));
    }
    else if (args->isSet("allalbums"))
    {
        startList = &listAllAlbums;
        startList->append(KCmdLineArgs::makeURL(args->getOption("allalbums").toUtf8()));
    }

    // append the remaining arguments to the lists:
    for (int i = 0; i < args->count(); ++i)
    {
        const QString argValue = args->arg(i);

        if (argValue == "--selectedimages")
        {
            startList = &listSelectedImages;
        }
        else if (argValue == "--selectedalbums")
        {
            startList = &listSelectedAlbums;
        }
        else if (argValue == "--allalbums")
        {
            startList = &listAllAlbums;
        }
        else
        {
            if (startList == 0)
            {
                kError()<<"startList==0";
                args->usageError(i18n("Please specify how the filenames you provided should be used."));
            }
            else
            {
                startList->append(args->url(i));
            }
        }
    }

//     kDebug()<<"listSelectedImages:"<<listSelectedImages;
//     kDebug()<<"listSelectedAlbums:"<<listSelectedAlbums;
//     kDebug()<<"listAllAlbums:"<<listAllAlbums;

    kipiInterface->addSelectedImages(listSelectedImages);
    kipiInterface->addSelectedAlbums(listSelectedAlbums);
    kipiInterface->addAlbums(listAllAlbums);

    // determine whether only one plugin should be loaded:
    const QString nameOfOnlyOnePluginToLoad = args->getOption("library");

    // determine what to do:
    int returnValue                         = 0;
    bool startedPlugin                      = false;

    if ( args->isSet("listplugins") )
    {
        if (!ListPlugins( nameOfOnlyOnePluginToLoad ))
            returnValue = 1;
    }
    else if ( args->isSet("a") )
    {
        const QString action = args->getOption("action");

        if ( !CallAction( action, nameOfOnlyOnePluginToLoad ) )
        {
            returnValue = 1;
        }
        else
        {
            startedPlugin = true;
        }
    }
    else
    {
        KCmdLineArgs::usageError( i18n("No argument specified: either use --listplugins, "
                                       "or specify an action to be called.") );
    }

    if (startedPlugin&&args->isSet("wait"))
    {
        return app.exec();
    }
    else
    {
        return returnValue;
    }
}
