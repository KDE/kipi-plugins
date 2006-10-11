/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-01-31
 * Description : a kipi plugin to convert Raw file in single 
 *               or batch mode.
 *
 * Copyright 2003-2005 by Renchi Raju
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

// C ANSI Includes.

extern "C"
{
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
}

// C++ includes.

#include <cstdlib>

// Qt Includes.

#include <qprocess.h>
#include <qfileinfo.h>

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// Local includes.

#include "rawfiles.h"
#include "dcrawbinary.h"
#include "singledialog.h"
#include "batchdialog.h"
#include "plugin_rawconverter.h"
#include "plugin_rawconverter.moc"

typedef KGenericFactory<Plugin_RawConverter> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_rawconverter,
                            Factory("kipiplugin_rawconverter"))

Plugin_RawConverter::Plugin_RawConverter(QObject *parent, const char*, const QStringList&)
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

bool Plugin_RawConverter::isRAWFile(const QString& filePath)
{
    QString rawFilesExt(kipi_raw_file_extentions);

    QFileInfo fileInfo(filePath);
    if (rawFilesExt.upper().contains( fileInfo.extension(false).upper() ))
        return true;

    return false;
}

bool Plugin_RawConverter::checkBinaries(QString &dcrawVersion)
{
    KIPIRawConverterPlugin::DcrawBinary dcrawBinary;
    dcrawVersion = dcrawBinary.version();

    if (!dcrawBinary.isAvailable()) 
    {
        KMessageBox::information(
                     kapp->activeWindow(),
                     i18n("<qt><p>Unable to find the <b>%1</b> executable:<br>"
                          "This program is required by this plugin to support "
                          "Raw files decoding. "
                          "Please check %2 installation on your computer.</p></qt>")
                          .arg(dcrawBinary.path())
                          .arg(dcrawBinary.path()),
                     QString::null,
                     QString::null,
                     KMessageBox::Notify | KMessageBox::AllowLink);
        return false;
    }

    if (!dcrawBinary.versionIsRight()) 
    {
        KMessageBox::information(
                     kapp->activeWindow(),
                     i18n("<qt><p><b>%1</b> executable isn't up to date:<br>"
                          "The version %2 of %3 have been found on your computer. "
                          "This version is too old to run properly with this plugin. "
                          "Please check %4 installation on your computer.</p>"
                          "<p>Note: at least, %5 version %6 is required by this "
                          "plugin.</p></qt>")
                          .arg(dcrawBinary.path())
                          .arg(dcrawVersion)
                          .arg(dcrawBinary.path())
                          .arg(dcrawBinary.path())
                          .arg(dcrawBinary.path())
                          .arg(dcrawBinary.minimalVersion()),
                     QString::null,
                     QString::null,
                     KMessageBox::Notify | KMessageBox::AllowLink);
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

    QString dcrawVersion;
    if (!checkBinaries(dcrawVersion)) 
        return;

    if (!isRAWFile(images.images()[0].path()))
    {
        KMessageBox::error(kapp->activeWindow(), 
                           i18n("\"%1\" is not a Raw file.").arg(images.images()[0].fileName()));
        return;
    }

    KIPIRawConverterPlugin::SingleDialog *converter = 
        new KIPIRawConverterPlugin::SingleDialog(images.images()[0].path(), 
            kapp->activeWindow(), dcrawVersion); 

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

    QString dcrawVersion;
    if (!checkBinaries(dcrawVersion)) 
        return;

    KIPIRawConverterPlugin::BatchDialog *converter =
        new KIPIRawConverterPlugin::BatchDialog(kapp->activeWindow(), dcrawVersion);

    KURL::List urls = images.images();
    QStringList files;

    for( KURL::List::Iterator it = urls.begin(); it != urls.end(); ++it ) 
    {
        if (isRAWFile((*it).path()))
            files.append( (*it).path() );
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

