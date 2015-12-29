/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-10-12
 * Description : a dialog to edit EXIF metadata
 *
 * Copyright (C) 2006-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>
#include <kwindowconfig.h>
#include <kconfig.h>

// Libkipi includes

#include <KIPI/ImageCollection>
#include <KIPI/Interface>

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

    EmailSettings acceptedEmailSettings;
};

SendImagesDialog::SendImagesDialog(QWidget* const /*parent*/, const QList<QUrl>& urls)
    : KPToolDialog(0),
      d(new Private)
{
    d->urls = urls;

    setWindowTitle(i18n("Email Images Options"));
    setModal(false);

    startButton()->setText(i18nc("@action:button", "&Send"));
    startButton()->setIcon(QIcon::fromTheme(QLatin1String("mail-send")));

    // ---------------------------------------------------------------

    QWidget* mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    QGridLayout* mainLayout = new QGridLayout(mainWidget);
    d->imagesList           = new MyImageList(mainWidget);
    d->settingsWidget       = new SettingsWidget(mainWidget);
    d->imagesList->slotAddImages(urls);

    mainLayout->addWidget(d->imagesList,     0, 0, 1, 1);
    mainLayout->addWidget(d->settingsWidget, 0, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    KPAboutData* const about = new KPAboutData(ki18n("Send Images"),
                                   0,
                                   KAboutLicense::GPL,
                                   ki18n("A plugin to e-mail items"),
                                   ki18n("(c) 2003-2014, Gilles Caulier"));

    about->addAuthor(ki18n("Gilles Caulier").toString(), ki18n("Author and Maintainer").toString(),
                     QLatin1String("caulier dot gilles at gmail dot com"));

    about->addAuthor(ki18n("Michael Hoechstetter").toString(), ki18n("Developer").toString(),
                     QLatin1String("michael dot hoechstetter at gmx dot de"));

    about->addAuthor(ki18n("Tom Albers").toString(), ki18n("Developer").toString(),
                     QLatin1String("tomalbers at kde dot nl"));

    about->setHandbookEntry(QLatin1String("sendimages"));
    setAboutData(about);

    // ------------------------------------------------------------

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));

    connect(startButton(), &QPushButton::clicked,
            this, &SendImagesDialog::slotSubmit);

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
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void SendImagesDialog::slotFinished()
{
    saveSettings();
    d->imagesList->listView()->clear();
}

void SendImagesDialog::slotSubmit()
{
    // Prepare acceptedEmailSettings here because the image list
    // will be cleared by slotFinished() on accepting the dialog.
    d->acceptedEmailSettings = d->settingsWidget->emailSettings();
    d->acceptedEmailSettings.itemsList = d->imagesList->imagesList();

    accept();
}

EmailSettings SendImagesDialog::emailSettings() const
{
    return d->acceptedEmailSettings;
}

void SendImagesDialog::readSettings()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group(QLatin1String("SendImages Settings"));

    EmailSettings settings;
    settings.emailProgram            = (EmailSettings::EmailClient)group.readEntry(QLatin1String("EmailProgram"), (int)EmailSettings::KMAIL);
    settings.imageSize               = (EmailSettings::ImageSize)group.readEntry(QLatin1String("ImageResize"),    (int)EmailSettings::MEDIUM);
    settings.imageFormat             = (EmailSettings::ImageFormat)group.readEntry(QLatin1String("ImageFormat"),  (int)EmailSettings::JPEG);
    settings.imagesChangeProp        = group.readEntry(QLatin1String("ImagesChangeProp"), false);
    settings.addCommentsAndTags      = group.readEntry(QLatin1String("AddCommentsAndTags"), false);
    settings.imageCompression        = group.readEntry(QLatin1String("ImageCompression"), 75);
    settings.attachmentLimitInMbytes = group.readEntry(QLatin1String("AttachmentLimit"), 17);
    d->settingsWidget->setEmailSettings(settings);

    KConfigGroup group2 = config.group(QLatin1String("SendImages Dialog"));
    KWindowConfig::restoreWindowSize(windowHandle(), group2);
}

void SendImagesDialog::saveSettings()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group(QLatin1String("SendImages Settings"));

    EmailSettings settings = d->settingsWidget->emailSettings();
    group.writeEntry(QLatin1String("EmailProgram"),       (int)settings.emailProgram);
    group.writeEntry(QLatin1String("ImageResize"),        (int)settings.imageSize);
    group.writeEntry(QLatin1String("ImageFormat"),        (int)settings.imageFormat);
    group.writeEntry(QLatin1String("ImagesChangeProp"),   settings.imagesChangeProp);
    group.writeEntry(QLatin1String("AddCommentsAndTags"), settings.addCommentsAndTags);
    group.writeEntry(QLatin1String("ImageCompression"),   settings.imageCompression);
    group.writeEntry(QLatin1String("AttachmentLimit"),    settings.attachmentLimitInMbytes);

    KConfigGroup group2 = config.group(QLatin1String("SendImages Dialog"));
    KWindowConfig::saveWindowSize(windowHandle(), group2);
    config.sync();
}

void SendImagesDialog::slotImagesCountChanged()
{
    startButton()->setEnabled(!d->imagesList->imagesList().isEmpty());
}

}  // namespace KIPISendimagesPlugin
