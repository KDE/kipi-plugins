/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-24
 * Description : a kipi plugin to convert Raw file to DNG
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_dngconverter.h"
#include "plugin_dngconverter.moc"

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

#include <libkdcraw/kdcraw.h>
#include <libkdcraw/version.h>

// LibKIPI includes.

#include <libkipi/interface.h>

// Local includes.

#include "batchdialog.h"

using namespace KIPIDNGConverterPlugin;

K_PLUGIN_FACTORY( RawConverterFactory, registerPlugin<Plugin_DNGConverter>(); )
K_EXPORT_PLUGIN ( RawConverterFactory("kipiplugin_dngconverter") )

Plugin_DNGConverter::Plugin_DNGConverter(QObject *parent, const QVariantList &)
                   : KIPI::Plugin( RawConverterFactory::componentData(), parent, "DNGConverter")
{
    kDebug( 51001 ) << "Plugin_DNGConverter plugin loaded" << endl;
}

void Plugin_DNGConverter::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action = actionCollection()->addAction("dngconverter");
    m_action->setText(i18n("DNG Converter..."));
    m_action->setIcon(KIcon("dngconverter"));

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_action);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );
    if ( !interface )
    {
           kError( 51000 ) << "Kipi interface is null!" << endl;
           return;
    }

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action, SLOT(setEnabled(bool)));
}

Plugin_DNGConverter::~Plugin_DNGConverter()
{
}

bool Plugin_DNGConverter::isRAWFile(const QString& filePath)
{

    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
    QFileInfo fileInfo(filePath);
    if (fileInfo.suffix().toUpper() != QString("DNG"))
    {
        if (rawFilesExt.toUpper().contains( fileInfo.suffix().toUpper() ))
            return true;
    }

    return false;
}

void Plugin_DNGConverter::slotActivate()
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

    BatchDialog *converter = new BatchDialog(interface);

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

KIPI::Category Plugin_DNGConverter::category( KAction* action ) const
{
    if (action == m_action)
       return KIPI::BatchPlugin;

    kWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::BatchPlugin; // no warning from compiler, please
}
