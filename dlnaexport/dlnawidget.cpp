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

#include "dlnawidget.moc"

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

#include "mediaserver.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"

using namespace KIPIPlugins;

namespace KIPIDLNAExportPlugin
{

class DLNAWidget::Private
{
public:

    Private()
    {
        dlna         = 0;
        imgList      = 0;
        progressBar  = 0;
    }

    MediaServer*      dlna;

    KPImagesList*     imgList;

    KPProgressWidget* progressBar;
};

DLNAWidget::DLNAWidget(QWidget* const parent)
    : QWidget(parent), d(new Private)
{

    QHBoxLayout* mainLayout        = new QHBoxLayout(this);
    QWidget* settingsBox           = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    // -------------------------------------------------------------------

    d->imgList = new KPImagesList(this);
    d->imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsRight);
    d->imgList->setAllowRAW(true);
    d->imgList->listView()->setWhatsThis(i18n("This is the list of images to upload via your DLNA server"));

    KHBox* hbox = new KHBox(settingsBox);
    d->progressBar = new KIPIPlugins::KPProgressWidget(settingsBox);
    d->progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    //d->progressBar->hide();

    // ------------------------------------------------------------------------

    settingsBoxLayout->addWidget(hbox);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(0);

    // ------------------------------------------------------------------------

    mainLayout->addWidget(d->imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    // ------------------------------------------------------------------------
}

DLNAWidget::~DLNAWidget()
{
    delete d;
}

void DLNAWidget::setImages(const KUrl::List& imageList)
{
    d->imgList->slotAddImages(imageList);
}

void DLNAWidget::setControlButtons(bool select)
{
    if (select)
        d->imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsRight);
    else
        d->imgList->setControlButtonsPlacement(KPImagesList::NoControlButtons);
}

void DLNAWidget::reactivate()
{
}

void DLNAWidget::slotSelectDirectory()
{
    QString startingPath;
#if KDE_IS_VERSION(4,1,61)
    startingPath = KGlobalSettings::picturesPath();
#else
    startingPath = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#endif
    QString path = KFileDialog::getExistingDirectory(startingPath, this,
                                                     i18n("Select folder to parse"));

    if (path.isEmpty())
    {
        return;
    }

    kDebug() << path;

    // TODO : stop properly previous server instance if exist.
    d->dlna = new MediaServer();
    d->dlna->onAddContentButtonClicked(path, true);
}

} // namespace KIPIDLNAExportPlugin
