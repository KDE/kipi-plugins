/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-01-06
 * Description : a plugin to export image collections using SimpleViewer.
 *
 * Copyright (C) 2006 by Joern Ahrens <joern dot ahrens at kdemail dot net>
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

#include "firstrundlg.h"
#include "firstrundlg.moc"

// Qt includes

#include <QFrame>
#include <QLabel>
#include <QLayout>

// KDE includes

#include <khelpmenu.h>
#include <klocale.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kurllabel.h>
#include <kurlrequester.h>

// LibKIPI includes

#include "pluginsversion.h"
#include <libkipi/imagecollectionselector.h>

// Local includes

#include "kpaboutdata.h"

namespace KIPIFlashExportPlugin
{

class FirstRunDlgPriv
{
public:

    FirstRunDlgPriv()
    {
        urlRequester = 0;
        about        = 0;
    }

    KUrl                      url;

    KUrlRequester            *urlRequester;

    KIPIPlugins::KPAboutData *about;
};

FirstRunDlg::FirstRunDlg(QWidget *parent)
           : KDialog(parent), d(new FirstRunDlgPriv)
{
    setButtons(Help | Ok | Cancel);
    setDefaultButton(Ok);
    setModal(true);
    setCaption(i18n("Flash Export"));
    setWindowIcon(KIcon("flash"));
    enableButton(Ok, false);

    QFrame *page = new QFrame(this);
    setMainWidget(page);

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

    // ---------------------------------------------------------------

    QVBoxLayout *topLayout = new QVBoxLayout(page);

    QLabel *info1 = new QLabel(page);
    info1->setWordWrap(true);
    info1->setText( i18n("<p>SimpleViewer is a Flash component which is free to use, "
                         "but uses a license which comes into conflict with several distributions. "
                         "Due to the license it is not possible to ship it with this plugin.</p>"
                         "<p>You can now download SimpleViewer from its homepage and point this tool "
                         "to the downloaded archive. The archive will be stored with the plugin configuration, "
                         "so it is available for further use.</p>"));

    QLabel *info2 = new QLabel(page);
    info2->setText(i18n("<p>1.) Download SimpleViewer Version 1.9.x</p>"));

    KUrlLabel *link = new KUrlLabel(page);
    link->setText("http://www.airtightinteractive.com/simpleviewer");
    link->setUrl("http://www.airtightinteractive.com/simpleviewer");

    connect(link, SIGNAL(leftClickedUrl(const QString&)),
            this, SLOT(slotDownload(const QString&)));

    QLabel *info3 = new QLabel(page);
    info3->setText(i18n("<p>2.) Point this tool to the downloaded archive</p>"));

    d->urlRequester = new KUrlRequester(page);
    connect(d->urlRequester, SIGNAL(urlSelected(const KUrl&)),
            this, SLOT(slotUrlSelected(const KUrl&)));

    topLayout->setMargin(0);
    topLayout->setSpacing(spacingHint());
    topLayout->addWidget(info1);
    topLayout->addWidget(info2);
    topLayout->addWidget(link);
    topLayout->addWidget(info3);
    topLayout->addWidget(d->urlRequester);
    topLayout->addStretch(10);
}

FirstRunDlg::~FirstRunDlg()
{
    delete d->about;
    delete d;
}

void FirstRunDlg::slotHelp()
{
    KToolInvocation::invokeHelp("flashexport", "kipi-plugins");
}

void FirstRunDlg::slotDownload(const QString &url)
{
    KToolInvocation::invokeBrowser(url);
}

void FirstRunDlg::slotUrlSelected(const KUrl &url)
{
    enableButton(Ok, true);
    d->url = url;
}

KUrl FirstRunDlg::getUrl()
{
    return d->url;
}

} // namespace KIPIFlashExportPlugin
