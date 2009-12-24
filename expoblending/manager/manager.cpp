/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "manager.moc"

// KDE includes

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "aboutdata.h"
#include "importwizarddlg.h"
#include "expoblendingdlg.h"
#include "actionthread.h"
#include "alignbinary.h"
#include "enfusebinary.h"

namespace KIPIExpoBlendingPlugin
{

class ManagerPriv
{
public:

    ManagerPriv()
    {
        iface  = 0;
        about  = 0;
        thread = 0;
    }

    KUrl::List             inputUrls;

    ItemUrlsMap            alignedUrlsMap;

    RawDecodingSettings    rawDecodingSettings;

    Interface*             iface;

    ExpoBlendingAboutData* about;

    ActionThread*          thread;

    AlignBinary            alignBinary;
    EnfuseBinary           enfuseBinary;
};

Manager::Manager(QObject* parent)
       : QObject(parent), d(new ManagerPriv)
{
    d->thread = new ActionThread(this);

    d->rawDecodingSettings.sixteenBitsImage = true;
}

Manager::~Manager()
{
    delete d->about;
    delete d->thread;
    delete d;
}

bool Manager::checkBinaries()
{
    if (!d->alignBinary.isAvailable() || !d->alignBinary.versionIsRight())
    {
        KMessageBox::information(
                kapp->activeWindow(),
                i18n("<p>Unable to find %1 executable:<br/> "
                    "This program is required by this plugin to align bracketed images. "
                    "Please install this program as a package from your distributor "
                    "or <a href=\"%2\">download the source</a>.</p>"
                    "<p>Note: at least, %3 version %4 is required by this plugin.</p>",
                    QString(d->alignBinary.path()),
                    d->alignBinary.url().url(),
                    QString(d->alignBinary.path()),
                    d->alignBinary.minimalVersion()),
                QString(),
                QString(),
                KMessageBox::Notify | KMessageBox::AllowLink);

        return false;
    }

    if (!d->enfuseBinary.isAvailable() || !d->enfuseBinary.versionIsRight())
    {
        KMessageBox::information(
                kapp->activeWindow(),
                i18n("<p>Unable to find %1 executable:<br/> "
                    "This program is required by this plugin to fuse bracketed images. "
                    "Please install this program as a package from your distributor "
                    "or <a href=\"%2\">download the source</a>.</p>"
                    "<p>Note: at least, %3 version %4 is required by this plugin.</p>",
                    QString(d->enfuseBinary.path()),
                    d->enfuseBinary.url().url(),
                    QString(d->enfuseBinary.path()),
                    d->enfuseBinary.minimalVersion()),
                QString(),
                QString(),
                KMessageBox::Notify | KMessageBox::AllowLink);

        return false;
    }

    return true;
}

void Manager::setAbout(ExpoBlendingAboutData* about)
{
    d->about = about;
}

ExpoBlendingAboutData* Manager::about() const
{
    return d->about;
}

AlignBinary& Manager::alignBinary() const
{
    return d->alignBinary;
}

EnfuseBinary& Manager::enfuseBinary() const
{
    return d->enfuseBinary;
}

void Manager::setIface(Interface* iface)
{
    d->iface = iface;
}

Interface* Manager::iface() const
{
    return d->iface;
}

void Manager::setItemsList(const KUrl::List& urls)
{
    d->inputUrls = urls;
}

KUrl::List Manager::itemsList() const
{
    return d->inputUrls;
}

void Manager::setRawDecodingSettings(const RawDecodingSettings& settings)
{
    d->rawDecodingSettings = settings;
}

RawDecodingSettings Manager::rawDecodingSettings() const
{
    return d->rawDecodingSettings;
}

void Manager::setAlignedMap(const ItemUrlsMap& urls)
{
    d->alignedUrlsMap = urls;
}

ItemUrlsMap Manager::alignedMap() const
{
    return d->alignedUrlsMap;
}

ActionThread* Manager::thread() const
{
    return d->thread;
}

void Manager::run()
{
    startImportWizard();
}

void Manager::startImportWizard()
{
    ImportWizardDlg *wz = new ImportWizardDlg(this, 0);

    if (wz->exec() != QDialog::Rejected)
    {
        d->inputUrls = wz->itemUrls();
        startExpoBlendingDlg();
    }
}

void Manager::startExpoBlendingDlg()
{
    ExpoBlendingDlg *dlg = new ExpoBlendingDlg(this);
    dlg->exec();
}

} // namespace KIPIExpoBlendingPlugin