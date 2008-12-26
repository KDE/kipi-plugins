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
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_rawconverter.h"
#include "plugin_rawconverter.moc"

// C ANSI includes.

extern "C"
{
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
}

// C++ includes.

#include <cstdlib>

// Qt includes.

#include <QFileInfo>

// KDE includes.

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>

// LibKDcraw includes.

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// LibKIPI includes.

#include <libkipi/interface.h>

// Local includes.

#include "singledialog.h"
#include "batchdialog.h"

K_PLUGIN_FACTORY( RawConverterFactory, registerPlugin<Plugin_RawConverter>(); )
K_EXPORT_PLUGIN ( RawConverterFactory("kipiplugin_rawconverter") )

Plugin_RawConverter::Plugin_RawConverter(QObject *parent, const QVariantList &)
                   : KIPI::Plugin( RawConverterFactory::componentData(), parent, "RawConverter")
{
    kDebug( 51001 ) << "Plugin_RawConverter plugin loaded";
}

void Plugin_RawConverter::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    KGlobal::locale()->insertCatalog("libkdcraw");

    m_singleAction = actionCollection()->addAction("raw_converter_single");
    m_singleAction->setText(i18n("Raw Image Converter..."));
    m_singleAction->setIcon(KIcon("rawconvertersingle"));

    connect(m_singleAction, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateSingle()));

    addAction(m_singleAction);

    m_batchAction = actionCollection()->addAction("raw_converter_batch");
    m_batchAction->setText(i18n("Batch Raw Converter..."));
    m_batchAction->setIcon(KIcon("rawconverterbatch"));

    connect(m_batchAction, SIGNAL(triggered(bool)),
            this, SLOT(slotActivateBatch()));

    addAction(m_batchAction);

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
#if KDCRAW_VERSION < 0x000400
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    QFileInfo fileInfo(filePath);
    if (rawFilesExt.toUpper().contains( fileInfo.suffix().toUpper() ))
        return true;

    return false;
}

bool Plugin_RawConverter::checkBinaries()
{
#if KDCRAW_VERSION < 0x000400
    KDcrawIface::DcrawBinary::instance()->checkSystem();
    KDcrawIface::DcrawBinary::instance()->checkReport();
    return KDcrawIface::DcrawBinary::instance()->isAvailable();
#else
    return true;
#endif
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
    if ( images.images().isEmpty() )
        return;

    if (!isRAWFile(images.images()[0].path()))
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("\"%1\" is not a Raw file.", images.images()[0].fileName()));
        return;
    }

    KIPIRawConverterPlugin::SingleDialog *converter =
        new KIPIRawConverterPlugin::SingleDialog(images.images()[0].path(), interface);

    converter->show();
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

    KIPIRawConverterPlugin::BatchDialog *converter =
        new KIPIRawConverterPlugin::BatchDialog(interface);

    KUrl::List urls = images.images();
    KUrl::List items;

    for( KUrl::List::Iterator it = urls.begin(); it != urls.end(); ++it )
    {
        if (isRAWFile((*it).path()))
            items.append((*it));
    }

    converter->addItems(items);
    converter->show();
}

KIPI::Category Plugin_RawConverter::category( KAction* action ) const
{
    if ( action == m_singleAction )
       return KIPI::ToolsPlugin;
    else if ( action == m_batchAction )
       return KIPI::BatchPlugin;

    kWarning( 51000 ) << "Unrecognized action for plugin category identification";
    return KIPI::ToolsPlugin; // no warning from compiler, please
}
