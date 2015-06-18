/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-10-12
 * Description : a dialog to edit EXIF metadata
 *
 * Copyright (C) 2006-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006      by Tom Albers <tomalbers at kde dot nl>
 * Copyright (C) 2006      by Michael Hoechstetter <michael dot hoechstetter at gmx dot de>
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

#include "sendimagesdialog.h"

// Qt includes

#include <QCloseEvent>
#include <QGridLayout>
#include <QMenu>

// KDE includes

#include <klocalizedstring.h>
#include <kwindowconfig.h>
#include <kconfig.h>

// Libkipi includes

#include <imagecollection.h>
#include <interface.h>

// Local includes

#include "kpaboutdata.h"
#include "kpversion.h"
#include "myimagelist.h"
#include "settingswidget.h"

namespace KIPISendimagesPlugin
{

class SendImagesDialog::Private
{

public:

    Private()
    {
        imagesList     = 0;
        settingsWidget = 0;
    }

    QList<QUrl>      urls;

    MyImageList*    imagesList;
    SettingsWidget* settingsWidget;
};

SendImagesDialog::SendImagesDialog(QWidget* const /*parent*/, const QList<QUrl>& urls)
    : KPToolDialog(0), d(new Private)
{
    d->urls = urls;

    setCaption(i18n("Email Images Options"));
    setButtons(Help | User1 | Close);
    setDefaultButton(Close);
    setModal(false);

    button(User1)->setText(i18nc("@action:button", "&Send"));
    button(User1)->setIcon(KIcon("mail-send"));

    // ---------------------------------------------------------------

    setMainWidget(new QWidget(this));
    QGridLayout* mainLayout = new QGridLayout(mainWidget());
    d->imagesList           = new MyImageList(mainWidget());
    d->settingsWidget       = new SettingsWidget(mainWidget());
    d->imagesList->slotAddImages(urls);

    mainLayout->addWidget(d->imagesList,     0, 0, 1, 1);
    mainLayout->addWidget(d->settingsWidget, 0, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());

    // ---------------------------------------------------------------
    // About data and help button.

    KPAboutData* const about = new KPAboutData(ki18n("Send Images"),
                                   0,
                                   KAboutLicense::GPL,
                                   ki18n("A plugin to e-mail items"),
                                   ki18n("(c) 2003-2014, Gilles Caulier"));

    about->addAuthor(ki18n("Gilles Caulier").toString(), ki18n("Author and Maintainer").toString(),
                     "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Michael Hoechstetter").toString(), ki18n("Developer").toString(),
                     "michael dot hoechstetter at gmx dot de");

    about->addAuthor(ki18n("Tom Albers").toString(), ki18n("Developer").toString(),
                     "tomalbers at kde dot nl");

    about->setHandbookEntry("sendimages");
    setAboutData(about);

    // ------------------------------------------------------------

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotSubmit()));

    connect(d->imagesList, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImagesCountChanged()));

    // ------------------------------------------------------------

    readSettings();
}

SendImagesDialog::~SendImagesDialog()
{
    delete d;
}

void SendImagesDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    d->imagesList->listView()->clear();
    e->accept();
}

void SendImagesDialog::slotClose()
{
    saveSettings();
    d->imagesList->listView()->clear();
    reject();
}

void SendImagesDialog::slotSubmit()
{
    saveSettings();
    accept();
}

EmailSettings SendImagesDialog::emailSettings() const
{
    EmailSettings settings = d->settingsWidget->emailSettings(); 
    settings.itemsList     = d->imagesList->imagesList();
    return settings;
}

void SendImagesDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("SendImages Settings");

    EmailSettings settings;
    settings.emailProgram            = (EmailSettings::EmailClient)group.readEntry("EmailProgram", (int)EmailSettings::KMAIL);
    settings.imageSize               = (EmailSettings::ImageSize)group.readEntry("ImageResize",    (int)EmailSettings::MEDIUM);
    settings.imageFormat             = (EmailSettings::ImageFormat)group.readEntry("ImageFormat",  (int)EmailSettings::JPEG);
    settings.imagesChangeProp        = group.readEntry("ImagesChangeProp", false);
    settings.addCommentsAndTags      = group.readEntry("AddCommentsAndTags", false);
    settings.imageCompression        = group.readEntry("ImageCompression", 75);
    settings.attachmentLimitInMbytes = group.readEntry("AttachmentLimit", 17);
    d->settingsWidget->setEmailSettings(settings);

    KConfigGroup group2 = config.group(QString("SendImages Dialog"));
    KWindowConfig::restoreWindowSize(windowHandle(), group2);
}

void SendImagesDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("SendImages Settings");

    EmailSettings settings = d->settingsWidget->emailSettings();
    group.writeEntry("EmailProgram",       (int)settings.emailProgram);
    group.writeEntry("ImageResize",        (int)settings.imageSize);
    group.writeEntry("ImageFormat",        (int)settings.imageFormat);
    group.writeEntry("ImagesChangeProp",   settings.imagesChangeProp);
    group.writeEntry("AddCommentsAndTags", settings.addCommentsAndTags);
    group.writeEntry("ImageCompression",   settings.imageCompression);
    group.writeEntry("AttachmentLimit",    settings.attachmentLimitInMbytes);

    KConfigGroup group2 = config.group(QString("SendImages Dialog"));
    KWindowConfig::saveWindowSize(windowHandle(), group2);
    config.sync();
}

void SendImagesDialog::slotImagesCountChanged()
{
   enableButton(User1, !d->imagesList->imagesList().isEmpty());
}

}  // namespace KIPISendimagesPlugin
