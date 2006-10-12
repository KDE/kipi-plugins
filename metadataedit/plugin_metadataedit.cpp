/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-11
 * Description : a plugin to edit pictures metadata
 *
 * Copyright 2006 by Gilles Caulier
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

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// LibKIPI includes.

#include <libkipi/imagecollection.h>

// Local includes.

#include "metadataeditdialog.h"
#include "plugin_metadataedit.h"
#include "plugin_metadataedit.moc"

typedef KGenericFactory<Plugin_MetadataEdit> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_metadataedit, Factory("kipiplugin_metadataedit"))

Plugin_MetadataEdit::Plugin_MetadataEdit(QObject *parent, const char*, const QStringList&)
                   : KIPI::Plugin( Factory::instance(), parent, "MetadataEdit")
{
    kdDebug( 51001 ) << "Plugin_MetadataEdit plugin loaded" << endl;
}

void Plugin_MetadataEdit::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    // this is our action shown in the menubar/toolbar of the mainwindow

    m_actionMetadataEdit = new KAction (i18n("Edit Metadata..."),
                                        "metadataedit",
                                        0,     
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "metadataedit");

    addAction( m_actionMetadataEdit );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !m_interface )
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    KIPI::ImageCollection selection = m_interface->currentSelection();
    m_actionMetadataEdit->setEnabled( selection.isValid() && !selection.images().isEmpty() );

    connect( m_interface, SIGNAL(selectionChanged(bool)),
             m_actionMetadataEdit, SLOT(setEnabled(bool)));
}

void Plugin_MetadataEdit::slotActivate()
{
    // Get the current/selected album from host
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KIPIMetadataEditPlugin::MetadataEditDialog *dialog = new KIPIMetadataEditPlugin::MetadataEditDialog(
                                                         m_interface, kapp->activeWindow());

    dialog->setImages( images.images() );
    dialog->show();
}

KIPI::Category Plugin_MetadataEdit::category( KAction* action ) const
{
    if ( action == m_actionMetadataEdit )
       return KIPI::IMAGESPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}
