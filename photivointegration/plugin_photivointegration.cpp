/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-06-14
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
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

#include "plugin_photivointegration.moc"

// Qt includes

#include <qdir.h>

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

// local includes

#include "xmpinfo.h"
#include "xmpidmap.h"

/// You must wrap all your plugin code to a dedicated namespace
namespace KIPIPhotivoIntegrationPlugin
{

/**
 * Macros from KDE KParts to export the symbols for this plugin and
 * create the factory for it. The first argument is the name of the
 * plugin library and the second is the genericfactory templated from
 * the class for your plugin
 */
K_PLUGIN_FACTORY(PhotivoIntegrationFactory, registerPlugin<Plugin_PhotivoIntegration>();)
K_EXPORT_PLUGIN(PhotivoIntegrationFactory("kipiplugin_photivointegration") )

//Contructor is called by the factory that is set up by the macros K_PLUGIN_FACTORY and K_EXPORT_PLUGIN
Plugin_PhotivoIntegration::Plugin_PhotivoIntegration(QObject* const parent, const QVariantList&)
    : Plugin(PhotivoIntegrationFactory::componentData(), parent, "PhotivoIntegration")
{
    /// There is a debug space for plugin loading area.
    kDebug(AREA_CODE_LOADING) << "Plugin_PhotivoIntegration plugin loaded";

    /// Merge the gui of the plugin into the host application
    setUiBaseName("kipiplugin_photivointegrationui.rc");
    setupXML();
}

Plugin_PhotivoIntegration::~Plugin_PhotivoIntegration()
{
}

void Plugin_PhotivoIntegration::setup(QWidget* const widget)
{
    /** We pass the widget which host plugin in KIPI host application
     */
    Plugin::setup(widget);

    setupActions();

    m_iface = interface();
    if (!m_iface)
    {
       /// No need special debug space outside load plugin area, it will be selected automatically.
       kError() << "Kipi interface is null!";
       return;
    }

    /** This will get items selection from KIPI host application
     */
    KIPI::ImageCollection selection = m_iface->currentSelection();
    m_action->setEnabled(selection.isValid() && !selection.images().isEmpty());

    /** If selection change in KIPI host application, this signal will be fired, and plugin action enabled accordingly.
     */
    connect(m_iface, SIGNAL(selectionChanged(bool)),
            m_action, SLOT(setEnabled(bool)));
}

void Plugin_PhotivoIntegration::setupActions()
{
    /** We define plugin action which will be plug in KIPI host application.
     */
    m_action = actionCollection()->addAction("photivointegration");
    m_action->setText(i18n("Photivo Integration"));
    m_action->setIcon(KIcon("photivo"));
    m_action->setEnabled(false);

    /** Connect action signal to dedicated slot.
     */
    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    /** Action is registered in plugin instance.
     */
    addAction(m_action);
}

void Plugin_PhotivoIntegration::slotActivate()
{
    /** When plugin action is actived, we display list of item selected in a message box
     */
    KIPI::ImageCollection images = m_iface->currentSelection();

    if (!images.isValid() || images.images().isEmpty())
        return;

    // Currently just a provisional arrangement to examine 
    // Photivo (using Exiv2) and this implementation (using KExiv2)
    XmpInfo     xmpInfo;
    KUrl::List  imageList = images.images();
    QStringList infoList;
    XmpIDMap    &idmap    = XmpIDMap::getMap();

    for (int i = 0, end = imageList.size();  i < end;  i++)
    {
        const QString file       = imageList[i].toLocalFile();//KPMetadata seems to be picky (doesn't accept file://)
        const XmpMM   mm         = xmpInfo.getXmpMM(file);
        const QString pureOrigID = mm.pureID(mm.originalDocumentID);//remove prefix

        // simple data
        infoList.append("-- " + file + " --");
        infoList.append("    isDerivate(): "       + xmpInfo.isDerivate(file));
        infoList.append("    originalDocumentID: " + mm.originalDocumentID);
        infoList.append("    documentID: "         + mm.documentID);
        infoList.append("    instanceID: "         + mm.instanceID);

        // history
        int hsize = static_cast<int>(mm.history.size());
        for (int h = 0; h < hsize; h++)
        {
            infoList.append(QString("    history %1: action: ").arg(h)     + mm.history[h].action);
            infoList.append(QString("    history %1: instanceID: ").arg(h) + mm.history[h].instanceID);
            infoList.append(QString("    history %1: when: ").arg(h)       + mm.history[h].when);
        }

// TODO: enable when XmpMM::loadDerivedFrom() is implemented
//         // derived from
//         int dsize = static_cast<int>(mm.derivedFrom.size());
//         for (int d = 0; d < dsize; d++)
//         {
//             infoList.append(QString("    derivedFrom %1: documentID: ").arg(d) + mm.derivedFrom[d].documentID);
//             infoList.append(QString("    derivedFrom %1: instanceID: ").arg(d) + mm.derivedFrom[d].instanceID);
//         }

        // map originalDocumentID to file
        if (!pureOrigID.isEmpty()) 
            infoList.append("   Origin file: "     + idmap.value("xmp.did/" + pureOrigID));
    }

    KMessageBox::informationList(0,
                                 i18n("This is the list of selected items"),
                                 infoList
                                );
}

Category Plugin_PhotivoIntegration::category(KAction* const action) const
{
    if (action == m_action)
       return ToolsPlugin;//Plugin will be listed as an item of the "Tools" menu

    /// No need special debug space outside load plugin area, it will be selected automatically.
    kWarning() << "Unrecognized action for plugin category identification";
    return ToolsPlugin; // no warning from compiler, please (return an object)
}

}  // namespace KIPIPhotivoIntegrationPlugin
