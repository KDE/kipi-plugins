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

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <qprocess.h>

extern "C"
{
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
}

#include "plugin_rawconverter.h"
#include "singledialog.h"
#include "batchdialog.h"

K_EXPORT_COMPONENT_FACTORY( digikamplugin_rawconverter,
                            KGenericFactory<Plugin_RawConverter>("digikam"));


Plugin_RawConverter::Plugin_RawConverter(QObject *parent,
                                         const char*,
                                         const QStringList&)
    : Digikam::Plugin(parent, "RawConverter")
{
    setInstance(KGenericFactory<Plugin_RawConverter>::instance());
    setXMLFile("plugins/digikamplugin_rawconverter.rc");
    KGlobal::locale()->insertCatalogue("digikamplugin_rawconverter");

    singleAction_ = new KAction (i18n("Raw Image Converter ..."),
                                 "rawconverter",
                                 0,
                                 this,
                                 SLOT(slotActivateSingle()),
                                 actionCollection(),
                                 "raw_converter");

    batchAction_ = new KAction (i18n("Raw Images Converter..."),
                                 "rawconverter",
                                 0,
                                 this,
                                 SLOT(slotActivateBatch()),
                                 actionCollection(),
                                 "raw_converter_batch");

    connect(Digikam::AlbumManager::instance(),
            SIGNAL(signalAlbumItemsSelected(bool)),
            SLOT(slotItemsSelected(bool)));

    slotItemsSelected(false);
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
        KMessageBox::error(0, i18n("Failed to start Raw Converter Client\n"
                                   "Please check your installation"));
        return false;
    }

    process.clearArguments();
    process.addArgument("dcraw");
    if (!process.start()) {
        KMessageBox::error(0, i18n("dcraw is required for Raw Image Conversion\n"
                                   "Please install it"));
        return false;
    }

    return true;
}

void Plugin_RawConverter::slotActivateSingle()
{
    Digikam::AlbumInfo *album =
        Digikam::AlbumManager::instance()->currentAlbum();

    if (!album) return;

    if (!checkBinaries()) return;

    RawConverter::SingleDialog *converter =
        new RawConverter::SingleDialog(album->getSelectedItemsPath().first());
    converter->show();
}

void Plugin_RawConverter::slotActivateBatch()
{
    Digikam::AlbumInfo *album =
        Digikam::AlbumManager::instance()->currentAlbum();

    if (!album) return;

    if (!checkBinaries()) return;

    RawConverter::BatchDialog *converter =
        new RawConverter::BatchDialog();

    converter->addItems(album->getSelectedItemsPath());

    converter->show();
}


void Plugin_RawConverter::slotItemsSelected(bool val)
{
    singleAction_->setEnabled(val);
    batchAction_->setEnabled(val);
}

KIPI::Category Plugin_RawConverter::category() const
{
    return KIPI::TOOLSPLUGIN;
}

#include "plugin_rawconverter.moc"
