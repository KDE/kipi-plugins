/* ============================================================
 * File  : plugin_rawconverter.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-31
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt Includes.

#include <qprocess.h>

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// C ANSI Includes.

extern "C"
{
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
}

// Local includes.

#include "plugin_rawconverter.h"
#include "singledialog.h"
#include "batchdialog.h"

typedef KGenericFactory<Plugin_RawConverter> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_rawconverter,
                            Factory("kipiplugin_rawconverter"));

Plugin_RawConverter::Plugin_RawConverter(QObject *parent,
                                         const char*,
                                         const QStringList&)
    : KIPI::Plugin( Factory::instance(), parent, "RawConverter")
{
    kdDebug( 51001 ) << "Loaded RawConverter" << endl;
}


void Plugin_RawConverter::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    singleAction_ = new KAction (i18n("Raw Image Converter (Single)..."),
                                 "rawconvertersingle",
                                 0,
                                 this,
                                 SLOT(slotActivateSingle()),
                                 actionCollection(),
                                 "raw_converter_single");

    batchAction_ = new KAction (i18n("Raw Images Converter (Batch)..."),
                                 "rawconverterbatch",
                                 0,
                                 this,
                                 SLOT(slotActivateBatch()),
                                 actionCollection(),
                                 "raw_converter_batch");

    addAction( singleAction_ );
    addAction( batchAction_ );

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface )
    {
           kdError( 51000 ) << "Kipi interface is null!" << endl;
           return;
    }

    connect( interface, SIGNAL( selectionChanged( bool ) ),
             singleAction_, SLOT( setEnabled( bool ) ) );

    connect( interface, SIGNAL( currentAlbumChanged( bool ) ),
             batchAction_, SLOT( setEnabled( bool ) ) );
}

Plugin_RawConverter::~Plugin_RawConverter()
{
}

bool Plugin_RawConverter::checkBinaries()
{

    QProcess process;

    process.clearArguments();
    process.addArgument("kipidcrawclient");

    if (!process.start()) {
        KMessageBox::error(0, i18n("Failed to start raw converter client.\n"
                                   "Please check your installation."));
        return false;
    }

    process.clearArguments();
    process.addArgument("dcraw");

    if (!process.start()) {
        KMessageBox::error(0, i18n("dcraw is required for raw image conversion.\n"
                                   "Please install it."));
        return false;
    }

    return true;
}

void Plugin_RawConverter::slotActivateSingle()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface )
           {
           kdError( 51000 ) << "Kipi interface is null!" << endl;
           return;
           }

    KIPI::ImageCollection images;
    images = interface->currentSelection();

    if ( !images.isValid() )
        return;

    if (!checkBinaries()) return;

    KIPIRawConverterPlugin::SingleDialog *converter =
        new KIPIRawConverterPlugin::SingleDialog(images.images()[0].path()); // PENDING(blackie) handle remote URLS

    converter->show();
}

void Plugin_RawConverter::slotActivateBatch()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface )
           {
           kdError( 51000 ) << "Kipi interface is null!" << endl;
           return;
           }

    KIPI::ImageCollection images;
    images = interface->currentSelection();

    if ( !images.isValid() )
        return;

    if (!checkBinaries()) return;

    KIPIRawConverterPlugin::BatchDialog *converter =
        new KIPIRawConverterPlugin::BatchDialog();

    KURL::List urls = images.images();
    QStringList files;
    for( KURL::List::Iterator it = urls.begin(); it != urls.end(); ++it ) {
        files.append( (*it).path() ); // PENDING(blackie) handle remote URLs
    }
    converter->addItems(files);

    converter->show();
}

KIPI::Category Plugin_RawConverter::category( KAction* action ) const
{
    if ( action == singleAction_ )
       return KIPI::TOOLSPLUGIN;
    else if ( action == batchAction_ )
       return KIPI::BATCHPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::TOOLSPLUGIN; // no warning from compiler, please
}

#include "plugin_rawconverter.moc"
