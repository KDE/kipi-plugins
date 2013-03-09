/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#include "finalpage.moc"

// Qt includes

#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QDesktopServices>

// KDE includes

#include <khbox.h>
#include <klocale.h>
#include <kdialog.h>
#include <kpushbutton.h>
#include <kconfig.h>
#include <kcolorscheme.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobalsettings.h>
#include <kdeversion.h>
#include <kiconloader.h>

// Libkipi includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// Local includes

#include "hupnpmediaserver.h"
#include "welcomepage.h"
#include "minidlnamediaserver.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIDLNAExportPlugin
{

class FinalPage::Private
{
public:

    Private()
    {
        Hdlna          = 0;
        Mdlna          = 0;
        imgList        = 0;
        progressBar    = 0;
        startButton    = 0;
        stopButton     = 0;
        implementation = WelcomePage::HUPNP;
    }

    MediaServer*                         Hdlna;
    MinidlnaServer*                      Mdlna;
    KPImagesList*                        imgList;
    KPProgressWidget*                    progressBar;
    KPushButton*                         startButton;
    KPushButton*                         stopButton;
    QStringList                          directories;
    QMap<QString, KUrl::List>            collectionMap;
    WelcomePage::ImplementationGetOption implementation;
};

FinalPage::FinalPage(QWidget* const parent)
    : QWidget(parent), d(new Private)
{
    QVBoxLayout* const mainLayout = new QVBoxLayout(this);

    // -------------------------------------------------------------------

    d->imgList     = new KPImagesList(this);
    d->imgList->setControlButtonsPlacement(KPImagesList::NoControlButtons);
    d->imgList->setAllowRAW(true);
    d->imgList->listView()->setWhatsThis(i18n("This is the list of images to upload via your DLNA server"));

    d->startButton = new KPushButton(i18n("Start"), this);
    d->stopButton  = new KPushButton(i18n("Stop"), this);
    d->stopButton->setEnabled(false);

    connect(d->stopButton, SIGNAL(clicked()),
            this, SLOT(turnOff()));

    connect(d->startButton, SIGNAL(clicked()),
            this, SLOT(turnOn()));

    mainLayout->addWidget(d->imgList);
    mainLayout->addWidget(d->startButton);
    mainLayout->addWidget(d->stopButton);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    // ------------------------------------------------------------------------
}

void FinalPage::setOptions(WelcomePage::ImplementationGetOption implementation)
{
    d->implementation = implementation;
}

void FinalPage::turnOff()
{
    // Image sharing stopped.
    emit sharing(false);
    
    if (d->implementation == WelcomePage::HUPNP && d->Hdlna)
    {
        delete d->Hdlna;
        d->Hdlna = 0;
    }
    if (d->implementation == WelcomePage::MINIDLNA && d->Mdlna)
    {
        delete d->Mdlna;
        d->Mdlna = 0;
    }

    d->startButton->setEnabled(true);
    d->stopButton->setEnabled(false);
}

void FinalPage::turnOn()
{
    // Image sharing started.
    emit sharing(true);

    if (d->implementation == WelcomePage::HUPNP)
        startHupnpMediaServer();
    else
        startMinidlnaMediaServer();

    d->startButton->setEnabled(false);
    d->stopButton->setEnabled(true);
}

void FinalPage::setCollectionMap(const QMap<QString, KUrl::List>& collectionMap)
{
    d->collectionMap = collectionMap;
}

FinalPage::~FinalPage()
{
    delete d;
}

void FinalPage::setImages(const KUrl::List& imageList)
{
    d->imgList->slotAddImages(imageList);
}

void FinalPage::clearImages()
{
    d->imgList->listView()->selectAll();
    d->imgList->slotRemoveItems();
    d->imgList->listView()->clear();
}

void FinalPage::startHupnpMediaServer()
{
    d->Hdlna = new MediaServer();
    d->Hdlna->addImagesOnServer(d->collectionMap);
}

void FinalPage::startMinidlnaMediaServer()
{
    d->Mdlna = new MinidlnaServer(this);
    d->Mdlna->setDirectories(d->directories);
    d->Mdlna->generateConfigFile();
    d->Mdlna->startMinidlnaServer();
}

void FinalPage::setDirectories(const QStringList& directories)
{
    d->directories = directories;
}

void FinalPage::setMinidlnaBinaryPath(const QString& path)
{
    if (!d->Mdlna)
    {
        kDebug() << "d->Mdlna is null";
        return;
    }

    d->Mdlna->setBinaryPath(path);
}

} // namespace KIPIDLNAExportPlugin
