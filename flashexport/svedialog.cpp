/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-12-19
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
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

#include "svedialog.h"
#include "svedialog.moc"

// Qt includes

#include <QFrame>
#include <QCloseEvent>

// KDE includes

#include <kconfig.h>
#include <kglobalsettings.h>
#include <kguiitem.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Local includes

#include "generalpage.h"
#include "kpaboutdata.h"
#include "lookpage.h"
#include "pluginsversion.h"
#include "selectionpage.h"

namespace KIPIFlashExportPlugin
{

class SVEDialogPriv
{
public:

    SVEDialogPriv()
    {
        page_selection = 0;
        page_general   = 0;
        page_look      = 0;
        selectionPage  = 0;
        generalPage    = 0;
        lookPage       = 0;
        about          = 0;
        interface      = 0;
    }

    KPageWidgetItem               *page_selection;
    KPageWidgetItem               *page_general;
    KPageWidgetItem               *page_look;

    SelectionPage                 *selectionPage;
    GeneralPage                   *generalPage;
    LookPage                      *lookPage;
    SimpleViewerSettingsContainer  settings;

    KIPIPlugins::KPAboutData       *about;

    KIPI::Interface                *interface;
};

SVEDialog::SVEDialog(KIPI::Interface *interface, QWidget *parent)
         : KPageDialog(parent), d(new SVEDialogPriv)
{
    d->interface = interface;

    setCaption(i18n("Flash Export"));
    setWindowIcon(KIcon("flash"));
    setButtons(Help|Ok|Cancel);
    setDefaultButton(Ok);
    setFaceType(List);
    setModal(true);

    // ---------------------------------------------------------------

    d->selectionPage  = new SelectionPage(d->interface, this);
    d->page_selection = addPage(d->selectionPage, i18nc("Settings tab for selections",
                                                        "Selection"));
    d->page_selection->setHeader(i18n("Collections List"));
    d->page_selection->setIcon(KIcon("view-list-tree"));

    d->generalPage  = new GeneralPage(this);
    d->page_general = addPage(d->generalPage, i18nc("General settings tab", "General"));
    d->page_general->setHeader(i18n("General Settings"));
    d->page_general->setIcon(KIcon("text-html"));

    d->lookPage  = new LookPage(this);
    d->page_look = addPage(d->lookPage, i18nc("Settings tab for the look of the flash export",
                                              "Look"));
    d->page_look->setHeader(i18n("Look Settings"));
    d->page_look->setIcon(KIcon("view-media-visualization"));

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("Flash Export"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to export images to Flash using the Simple Viewer component."),
                   ki18n("(c) 2005-2006, Joern Ahrens\n"
                         "(c) 2008-2009, Gilles Caulier"));

    d->about->addAuthor(ki18n("Joern Ahrens"),
                       ki18n("Author"),
                             "joern dot ahrens at kdemail dot net");

    d->about->addAuthor(ki18n("Gilles Caulier"),
                       ki18n("Developer and maintainer"),
                             "caulier dot gilles at gmail dot com");

    d->about->addCredit(ki18n("Felix Turner"),
                       ki18n("Author of the Simple Viewer Flash component"),
                       0,
                       "http://www.airtightinteractive.com/simpleviewer");

    d->about->addCredit(ki18n("Mikkel B. Stegmann"),
                       ki18n("Basis for the index.html template"),
                       0,
                       "http://www.stegmann.dk/mikkel/porta");

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

SVEDialog::~SVEDialog()
{
    delete d->about;
    delete d;
}

void SVEDialog::showPage(int page)
{
    switch(page)
    {
        case 0:
            setCurrentPage(d->page_selection);
            break;
        case 1:
            setCurrentPage(d->page_general);
            break;
        case 2:
            setCurrentPage(d->page_look);
            break;
        default:
            setCurrentPage(d->page_selection);
            break;
    }
}

int SVEDialog::activePageIndex()
{
    KPageWidgetItem *cur = currentPage();

    if (cur == d->page_selection) return 0;
    if (cur == d->page_general)   return 1;
    if (cur == d->page_look)      return 2;

    return 0;
}

void SVEDialog::slotHelp()
{
    KToolInvocation::invokeHelp("flashexport", "kipi-plugins");
}

void SVEDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void SVEDialog::slotCancel()
{
    saveSettings();
    reject();
}

void SVEDialog::slotOk()
{
    if (d->selectionPage->selection().isEmpty())
    {
        KMessageBox::sorry(this, i18n("You must select at least one collection to export."));
        return;
    }

    saveSettings();
    accept();
}

void SVEDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("FlashExport Settings");

    d->settings.thumbnailRows        = group.readEntry("ThumbnailRows", 3);
    d->settings.thumbnailColumns     = group.readEntry("ThumbnailColumns", 3);
    d->settings.thumbnailPosition    = (SimpleViewerSettingsContainer::ThumbPosition)group.readEntry("ThumbnailPosition", (int)SimpleViewerSettingsContainer::RIGHT);
    d->settings.navDirection         = (SimpleViewerSettingsContainer::NavDir)group.readEntry("NavDirection", (int)SimpleViewerSettingsContainer::LEFT2RIGHT);
    d->settings.textColor            = group.readEntry("TextColor", QColor("#ffffff"));
    d->settings.backgroundColor      = group.readEntry("BackgroundColor", QColor("#181818"));
    d->settings.frameColor           = group.readEntry("FrameColor", QColor("#ffffff"));
    d->settings.frameWidth           = group.readEntry("FrameWidth", 1);
    d->settings.stagePadding         = group.readEntry("StagePadding", 20);
    d->settings.title                = group.readEntry("Title", QString());
    d->settings.exportUrl            = group.readEntry("ExportUrl", KUrl(KGlobalSettings::documentPath() + "/simpleviewer"));
    d->settings.resizeExportImages   = group.readEntry("ResizeExportImages", true);
    d->settings.imagesExportSize     = group.readEntry("ImagesExportSize", 640);
    d->settings.maxImageDimension    = group.readEntry("MaxImageDimension", 640);
    d->settings.showComments         = group.readEntry("ShowComments", true);
    d->settings.enableRightClickOpen = group.readEntry("EnableRightClickOpen", false);
    d->settings.fixOrientation       = group.readEntry("FixOrientation", true);
    d->settings.openInKonqueror      = group.readEntry("OpenInKonqueror", true);

    d->generalPage->setSettings(d->settings);
    d->lookPage->setSettings(d->settings);

    KConfigGroup group2 = config.group(QString("FlashExport Dialog"));
    showPage(group2.readEntry("FlashExport Page", 0));
    restoreDialogSize(group2);
}

void SVEDialog::saveSettings()
{
    d->settings.collections = d->selectionPage->selection();
    d->generalPage->settings(d->settings);
    d->lookPage->settings(d->settings);

    KConfig config("kipirc");
    KConfigGroup group = config.group("FlashExport Settings");

    group.writeEntry("ThumbnailRows", d->settings.thumbnailRows);
    group.writeEntry("ThumbnailColumns", d->settings.thumbnailColumns);
    group.writeEntry("ThumbnailPosition", (int)d->settings.thumbnailPosition);
    group.writeEntry("NavDirection", (int)d->settings.navDirection);
    group.writeEntry("TextColor", d->settings.textColor);
    group.writeEntry("BackgroundColor", d->settings.backgroundColor);
    group.writeEntry("FrameColor", d->settings.frameColor);
    group.writeEntry("FrameWidth", d->settings.frameWidth);
    group.writeEntry("StagePadding", d->settings.stagePadding);
    group.writeEntry("Title", d->settings.title);
    group.writeEntry("ExportUrl",  d->settings.exportUrl);
    group.writeEntry("ResizeExportImages", d->settings.resizeExportImages);
    group.writeEntry("ImagesExportSize", d->settings.imagesExportSize);
    group.writeEntry("MaxImageDimension", d->settings.maxImageDimension);
    group.writeEntry("ShowComments", d->settings.showComments);
    group.writeEntry("EnableRightClickOpen", d->settings.enableRightClickOpen);
    group.writeEntry("FixOrientation", d->settings.fixOrientation);
    group.writeEntry("OpenInKonqueror", d->settings.openInKonqueror);

    KConfigGroup group2 = config.group(QString("FlashExport Dialog"));
    group2.writeEntry("FlashExport Page", activePageIndex());
    saveDialogSize(group2);
    config.sync();
}

SimpleViewerSettingsContainer SVEDialog::settings() const
{
    return d->settings;
}

} //  namespace KIPIFlashExportPlugin
