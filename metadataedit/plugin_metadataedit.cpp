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

#include "exiv2iface.h"
#include "exifeditdialog.h"
#include "iptceditdialog.h"
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

    m_actionMetadataEdit = new KActionMenu(i18n("Metadata"),
                               0,
                               actionCollection(),
                               "metadataedit");

    m_actionMetadataEdit->insert(new KAction (i18n("Edit EXIF..."),
                                     0,
                                     0,     
                                     this,
                                     SLOT(slotEditExif()),
                                     actionCollection(),
                                     "editexif"));

    m_actionMetadataEdit->insert(new KAction (i18n("Remove EXIF..."),
                                     0,
                                     0,     
                                     this,
                                     SLOT(slotRemoveExif()),
                                     actionCollection(),
                                     "removeexif"));

    m_actionMetadataEdit->insert(new KActionSeparator());

    m_actionMetadataEdit->insert(new KAction (i18n("Edit IPTC..."),
                                     0,
                                     0,     
                                     this,
                                     SLOT(slotEditIptc()),
                                     actionCollection(),
                                     "editiptc"));

    m_actionMetadataEdit->insert(new KAction (i18n("Remove IPTC..."),
                                     0,
                                     0,     
                                     this,
                                     SLOT(slotRemoveIptc()),
                                     actionCollection(),
                                     "removeiptc"));

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

void Plugin_MetadataEdit::slotEditExif()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KIPIMetadataEditPlugin::EXIFEditDialog dialog(kapp->activeWindow(), images.images());
    dialog.exec();
    m_interface->refreshImages(images.images());
}

void Plugin_MetadataEdit::slotRemoveExif()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("EXIF metadata will be definitivly removed from all current selected pictures.\n"
                          "Do you want to continue ?"),
                     i18n("Remove EXIF Metadata")) != KMessageBox::Yes)
        return;

    KURL::List imageURLs = images.images();
    KURL::List updatedURLs;
    KURL::List errorURLs;

    for( KURL::List::iterator it = imageURLs.begin() ; 
         it != imageURLs.end(); ++it)
    {
        KURL url = *it;
        bool ret = false;

        if (!KIPIPlugins::Exiv2Iface::isReadOnly(url.path()))
        {
            ret = true;
            KIPIPlugins::Exiv2Iface exiv2Iface;
            ret &= exiv2Iface.load(url.path());
            ret &= exiv2Iface.clearExif();
            ret &= exiv2Iface.save(url.path());
        }
        
        if (!ret)
            errorURLs.append(url);
        else 
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than 
    // metadata from pictures have changed and need to be re-readed.
    
    m_interface->refreshImages(updatedURLs);

    if (!errorURLs.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to remove EXIF metadata from:"),
                    errorURLs.toStringList(),
                    i18n("Remove EXIF Metadata"));  
    }
}

void Plugin_MetadataEdit::slotEditIptc()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KIPIMetadataEditPlugin::IPTCEditDialog dialog(kapp->activeWindow(), images.images());
    dialog.exec();
    m_interface->refreshImages(images.images());
}

void Plugin_MetadataEdit::slotRemoveIptc()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("IPTC metadata will be definitivly removed from all current selected pictures.\n"
                          "Do you want to continue ?"),
                     i18n("Remove IPTC Metadata")) != KMessageBox::Yes)
        return;

    KURL::List imageURLs = images.images();
    KURL::List updatedURLs;
    KURL::List errorURLs;

    for( KURL::List::iterator it = imageURLs.begin() ; 
         it != imageURLs.end(); ++it)
    {
        KURL url = *it;
        bool ret = false;

        if (!KIPIPlugins::Exiv2Iface::isReadOnly(url.path()))
        {
            ret = true;
            KIPIPlugins::Exiv2Iface exiv2Iface;
            ret &= exiv2Iface.load(url.path());
            ret &= exiv2Iface.clearIptc();
            ret &= exiv2Iface.save(url.path());
        }
        
        if (!ret)
            errorURLs.append(url);
        else 
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than 
    // metadata from pictures have changed and need to be re-readed.
    
    m_interface->refreshImages(updatedURLs);

    if (!errorURLs.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to remove IPTC metadata from:"),
                    errorURLs.toStringList(),
                    i18n("Remove IPTC Metadata"));  
    }
}

KIPI::Category Plugin_MetadataEdit::category( KAction* action ) const
{
    if ( action == m_actionMetadataEdit )
       return KIPI::IMAGESPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}
