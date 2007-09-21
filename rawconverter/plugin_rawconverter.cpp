/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-01-31
 * Description : a kipi plugin to convert Raw file in single 
 *               or batch mode.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C ANSI includes.

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

// LibKDcraw includes.

#include <libkdcraw/rawfiles.h>
#include <libkdcraw/dcrawbinary.h>

// Local includes.

/*#include "singledialog.h"
#include "batchdialog.h"*/
#include "plugin_rawconverter.h"
#include "plugin_rawconverter.moc"

K_PLUGIN_FACTORY( RawConverterFactory, registerPlugin<Plugin_RawConverter>(); )
K_EXPORT_PLUGIN ( RawConverterFactory("kipiplugin_rawconverter") )

Plugin_RawConverter::Plugin_RawConverter(QObject *parent, const QVariantList &)
                   : KIPI::Plugin( RawConverterFactory::componentData(), parent, "RawConverter")
{
    kDebug( 51001 ) << "Plugin_RawConverter plugin loaded" << endl;
}

void Plugin_RawConverter::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_singleAction = new KAction(KIcon("rawconvertersingle"), i18n("Raw Image Converter (Single)..."),
                                 actionCollection());
    connect(m_singleAction, SIGNAL(triggered(bool)), this, SLOT(slotActivateSingle()));
    addAction(m_singleAction);

    m_batchAction = new KAction(KIcon("rawconverterbatch"), i18n("Raw Images Converter (Batch)..."),
                                 actionCollection());
    connect(m_batchAction, SIGNAL(triggered(bool)), this, SLOT(slotActivateBatch()));
    addAction(m_batchAction);

    addAction( m_singleAction );
    addAction( m_batchAction );

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface )
    {
           kError( 51000 ) << "Kipi interface is null!" << endl;
           return;
    }

    connect( interface, SIGNAL( selectionChanged( bool ) ),
             m_singleAction, SLOT( setEnabled( bool ) ) );

    connect( interface, SIGNAL( currentAlbumChanged( bool ) ),
             m_batchAction, SLOT( setEnabled( bool ) ) );
}

Plugin_RawConverter::~Plugin_RawConverter()
{
}

bool Plugin_RawConverter::isRAWFile(const QString& filePath)
{
    QString rawFilesExt(raw_file_extentions);

    QFileInfo fileInfo(filePath);
    if (rawFilesExt.toUpper().contains( fileInfo.suffix().toUpper() ))
        return true;

    return false;
}

bool Plugin_RawConverter::checkBinaries()
{
    KDcrawIface::DcrawBinary::instance()->checkSystem();
    KDcrawIface::DcrawBinary::instance()->checkReport();
    return KDcrawIface::DcrawBinary::instance()->isAvailable();
}

void Plugin_RawConverter::slotActivateSingle()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if (!interface)
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    KIPI::ImageCollection images;
    images = interface->currentSelection();

    if (!images.isValid())
        return;

    if (!checkBinaries()) 
        return;

    if (!isRAWFile(images.images()[0].path()))
    {
        KMessageBox::error(kapp->activeWindow(), 
                           i18n("\"%1\" is not a Raw file.").arg(images.images()[0].fileName()));
        return;
    }

/*/    KIPIRawConverterPlugin::SingleDialog *converter = 
        new KIPIRawConverterPlugin::SingleDialog(images.images()[0].path(), 
            kapp->activeWindow()); 

    converter->show();*/
}

void Plugin_RawConverter::slotActivateBatch()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if (!interface)
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    KIPI::ImageCollection images;
    images = interface->currentSelection();

    if (!images.isValid())
        return;

    if (!checkBinaries()) 
        return;
/*
    KIPIRawConverterPlugin::BatchDialog *converter =
        new KIPIRawConverterPlugin::BatchDialog(kapp->activeWindow());

    KURL::List urls = images.images();
    QStringList files;

    for( KURL::List::Iterator it = urls.begin(); it != urls.end(); ++it ) 
    {
        if (isRAWFile((*it).path()))
            files.append( (*it).path() );
    }

    converter->addItems(files);
    converter->show();*/
}

KIPI::Category Plugin_RawConverter::category( KAction* action ) const
{
    if ( action == m_singleAction )
       return KIPI::TOOLSPLUGIN;
    else if ( action == m_batchAction )
       return KIPI::BATCHPLUGIN;

    kWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::TOOLSPLUGIN; // no warning from compiler, please
}
