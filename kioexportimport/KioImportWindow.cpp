/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 04.10.2009
 * Description : A tool for importing images via KIO
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "KioImportWindow.h"
#include "KioImportWindow.moc"

// Qt includes

#include <qaction.h>

// KDE includes

#include <kdebug.h>
#include <kdialog.h>
#include <khelpmenu.h>
#include <kio/copyjob.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// Local includes

#include "KioImportWidget.h"
#include "kpaboutdata.h"
#include "imageslist.h"

namespace KIPIKioExportPlugin
{

KioImportWindow::KioImportWindow(QWidget* /*parent*/, KIPI::Interface *interface)
               : KDialog(0), m_interface(interface)
{
    if (!interface)
    {
        kFatal() << "KIPI::Interface is empty";
    }

    m_importWidget = new KioImportWidget(this, interface);
    setMainWidget(m_importWidget);

    // window setup
    setWindowTitle(i18n("Import from Remote Computer"));
    setDefaultButton(Ok);
    setButtons(Help | User1 | Close);
    setDefaultButton(Close);
    setModal(false);
    enableButton(User1, false);

    setButtonGuiItem(User1, KGuiItem(i18n("Start import"), "network-workgroup",
                     i18n("Start importing the specified images "
                          "into the currently selected album.")));

    // connections

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotImport()));

    connect(m_importWidget->imagesList(), SIGNAL(signalImageListChanged()),
            this, SLOT(slotSourceAndTargetUpdated()));

    connect(m_importWidget->uploadWidget(), SIGNAL(selectionChanged),
            this, SLOT(slotSourceAndTargetUpdated()));

    // about data and help button

    m_about = new KIPIPlugins::KPAboutData(ki18n("Import from remote computer"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to import images over network using KIO-Slave"),
                   ki18n("(c) 2009, Johannes Wienke"));

    m_about->addAuthor(ki18n("Johannes Wienke"),
                       ki18n("Developer and maintainer"),
                       "languitar at semipol dot de");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    slotSourceAndTargetUpdated();
}

KioImportWindow::~KioImportWindow()
{
}

void KioImportWindow::slotImport()
{
    kDebug() << "starting to import urls: " << m_importWidget->sourceUrls();

    // start copying and react on signals
    setEnabled(false);
    KIO::CopyJob *copyJob = KIO::copy(m_importWidget->imagesList()->imageUrls(),
                            m_importWidget->uploadWidget()->selectedImageCollection().uploadPath());

    connect(copyJob, SIGNAL(copyingDone(KIO::Job*, const KUrl&, const KUrl&, time_t, bool, bool)),
            this, SLOT(slotCopyingDone(KIO::Job*, const KUrl&, const KUrl&, time_t, bool, bool)));

    connect(copyJob, SIGNAL(result(KJob*)),
            this, SLOT(slotCopyingFinished(KJob*)));
}

void KioImportWindow::slotCopyingDone(KIO::Job *job, const KUrl& from,
                                      const KUrl& to, time_t mtime, bool directory, bool renamed)
{
    Q_UNUSED(job);
    Q_UNUSED(to);
    Q_UNUSED(mtime);
    Q_UNUSED(directory);
    Q_UNUSED(renamed);

    kDebug() << "copied " << to.prettyUrl();

    m_importWidget->imagesList()->removeItemByUrl(from);
}

void KioImportWindow::slotCopyingFinished(KJob *job)
{
    Q_UNUSED(job);

    setEnabled(true);

    if (!m_importWidget->imagesList()->imageUrls().empty())
    {
        KMessageBox::sorry(this, i18n(
                           "Some of the images have not been transferred "
                           "and are still in the list. "
                           "You can retry to import these images now."), i18n(
                           "Import not completed"));
    }
}

void KioImportWindow::slotSourceAndTargetUpdated()
{
    bool hasUrlToImport = !m_importWidget->sourceUrls().empty();
    bool hasTarget      = m_importWidget->uploadWidget()->selectedImageCollection().uploadPath().isValid();

    kDebug() << "switching import button activity with: hasUrlToImport = "
                  << hasUrlToImport << ", hasTarget = " << hasTarget;

    enableButton(User1, hasUrlToImport && hasTarget);
}

void KioImportWindow::slotHelp()
{
    KToolInvocation::invokeHelp("kioexport", "kipi-plugins");
}

} // namespace KIPIKioExportPlugin
