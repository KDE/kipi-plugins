/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : a dialog to edit EXIF metadata
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006 by Tom Albers <tomalbers at kde dot nl>
 * Copyright (C) 2006 by Michael Hoechstetter <michael dot hoechstetter at gmx dot de>
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
#include "sendimagesdialog.moc"

// Qt includes

#include <QCloseEvent>

// KDE includes

#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kmenu.h>
#include <khelpmenu.h>
#include <ktoolinvocation.h>
#include <kguiitem.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// Local includes

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "imagespage.h"
#include "emailpage.h"

namespace KIPISendimagesPlugin
{

class SendImagesDialogPrivate
{

public:

    SendImagesDialogPrivate()
    {
        about       = 0;
        page_images = 0;
        page_email  = 0;
        imagesPage  = 0;
        emailPage   = 0;
    }

    KPageWidgetItem          *page_images;
    KPageWidgetItem          *page_email;

    KUrl::List                urls;

    ImagesPage               *imagesPage;
    EmailPage                *emailPage;

    KIPIPlugins::KPAboutData *about;
};

SendImagesDialog::SendImagesDialog(QWidget* /*parent*/, KIPI::Interface *iface, const KUrl::List& urls)
                : KPageDialog(0), d(new SendImagesDialogPrivate)
{
    d->urls = urls;

    setCaption(i18n("Email Images Options"));
    setButtons(Help|Ok|Cancel);
    setDefaultButton(Ok);
    setFaceType(List);
    setModal(false);

    // ---------------------------------------------------------------

    d->imagesPage  = new ImagesPage(this, iface);
    d->page_images = addPage(d->imagesPage, i18n("Images"));
    d->page_images->setHeader(i18n("Image List"));
    d->page_images->setIcon(KIcon("image-jp2"));
    d->imagesPage->slotAddImages(urls);

    d->emailPage  = new EmailPage(this);
    d->page_email = addPage(d->emailPage, i18n("Mail"));
    d->page_email->setHeader(i18n("Mail Options"));
    d->page_email->setIcon(KIcon("kontact"));

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("Send Images"),
                                            0,
                                            KAboutData::License_GPL,
                                            ki18n("A plugin to email pictures"),
                                            ki18n("(c) 2003-2009, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author and Maintainer"),
                        "caulier dot gilles at gmail dot com");

    d->about->addAuthor(ki18n("Michael Hoechstetter"), ki18n("Developer"),
                        "michael dot hoechstetter at gmx dot de");

    d->about->addAuthor(ki18n("Tom Albers"), ki18n("Developer"),
                        "tomalbers at kde dot nl");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ------------------------------------------------------------

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));

    // ------------------------------------------------------------

    readSettings();
}

SendImagesDialog::~SendImagesDialog()
{
    delete d->about;
    delete d;
}

void SendImagesDialog::slotHelp()
{
    KToolInvocation::invokeHelp("sendimages", "kipi-plugins");
}

void SendImagesDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    d->imagesPage->listView()->clear();
    e->accept();
}

void SendImagesDialog::slotCancel()
{
    saveSettings();
    d->imagesPage->listView()->clear();
    reject();
}

void SendImagesDialog::slotOk()
{
    saveSettings();
    accept();
}

EmailSettingsContainer SendImagesDialog::emailSettings()
{
    EmailSettingsContainer settings = d->emailPage->emailSettings(); 
    settings.itemsList              = d->imagesPage->imagesList(); 
    return settings;
}

void SendImagesDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("SendImages Settings");
    showPage(group.readEntry("SendImages Page", 0));

    EmailSettingsContainer settings;
    settings.emailProgram            = (EmailSettingsContainer::EmailClient)group.readEntry("EmailProgram", (int)EmailSettingsContainer::KMAIL);
    settings.imageSize               = (EmailSettingsContainer::ImageSize)group.readEntry("ImageResize",    (int)EmailSettingsContainer::MEDIUM);
    settings.imageFormat             = (EmailSettingsContainer::ImageFormat)group.readEntry("ImageFormat",  (int)EmailSettingsContainer::JPEG);
    settings.imagesChangeProp        = group.readEntry("ImagesChangeProp", false);
    settings.addCommentsAndTags      = group.readEntry("AddCommentsAndTags", false);
    settings.imageCompression        = group.readEntry("ImageCompression", 75);
    settings.attachmentLimitInMbytes = group.readEntry("AttachmentLimit", 17);
    d->emailPage->setEmailSettings(settings);

    KConfigGroup group2 = config.group(QString("SendImages Dialog"));
    restoreDialogSize(group2);
}

void SendImagesDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("SendImages Settings");
    group.writeEntry("SendImages Page", activePageIndex());

    EmailSettingsContainer settings = d->emailPage->emailSettings();
    group.writeEntry("EmailProgram",       (int)settings.emailProgram);
    group.writeEntry("ImageResize",        (int)settings.imageSize);
    group.writeEntry("ImageFormat",        (int)settings.imageFormat);
    group.writeEntry("ImagesChangeProp",   settings.imagesChangeProp);
    group.writeEntry("AddCommentsAndTags", settings.addCommentsAndTags);
    group.writeEntry("ImageCompression",   settings.imageCompression);
    group.writeEntry("AttachmentLimit",    settings.attachmentLimitInMbytes);

    KConfigGroup group2 = config.group(QString("SendImages Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void SendImagesDialog::showPage(int page)
{
    switch(page)
    {
        case 0:
            setCurrentPage(d->page_images); 
            break;
        case 1:
            setCurrentPage(d->page_email); 
            break;
        default: 
            setCurrentPage(d->page_images); 
            break;
    }
}

int SendImagesDialog::activePageIndex()
{
    KPageWidgetItem *cur = currentPage();

    if (cur == d->page_images)  return 0;
    if (cur == d->page_email)   return 1;

    return 0;
}

}  // namespace KIPISendimagesPlugin
