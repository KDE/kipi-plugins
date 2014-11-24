/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#include "firstrunpage.moc"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QFrame>
#include <QLayout>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kurllabel.h>
#include <kurlrequester.h>
#include <kiconloader.h>

// LibKIPI includes

#include <libkipi/imagecollectionselector.h>

// Local includes

#include "kpversion.h"

namespace KIPIFlashExportPlugin
{

class FirstRunPage::FirstRunPagePriv
{
public:

    FirstRunPagePriv()
    {
        urlRequester = 0;
    }

    KUrl           url;

    KUrlRequester* urlRequester;
};

// link this page to SimpleViewer to gain access to settings container.
FirstRunPage::FirstRunPage(KAssistantDialog* const dlg)
    : KPWizardPage(dlg, i18n("First Run")),
      d(new FirstRunPagePriv)
{
    KVBox* vbox   = new KVBox(this);
//    QVBoxLayout* topLayout = new QVBoxLayout(vbox);
    QLabel* info1 = new QLabel(vbox);
    info1->setWordWrap(true);
    info1->setText( i18n("<p>SimpleViewer's plugins are Flash components which are free to use, "
                         "but use a license which comes into conflict with several distributions. "
                         "Due to the license it is not possible to ship it with this tool.</p>"
                         "<p>You can now download plugin from its homepage and point this tool "
                         "to the downloaded archive. The archive will be stored with the plugin configuration, "
                         "so it is available for further use.</p>"
                         "<p><b>Note: Please download the plugin that you selected on the first page.</b></p>"));

    QLabel* info2   = new QLabel(vbox);
    info2->setText(i18n("<p>1.) Download plugin from the following url:</p>"));

    KUrlLabel* link = new KUrlLabel(vbox);
    link->setText("http://www.simpleviewer.net");
    link->setUrl("http://www.simpleviewer.net");

    connect(link, SIGNAL(leftClickedUrl(QString)),
            this, SLOT(slotDownload(QString)));

    QLabel* info3   = new QLabel(vbox);
    info3->setText(i18n("<p>2.) Point this tool to the downloaded archive</p>"));

    d->urlRequester = new KUrlRequester(vbox);
    connect(d->urlRequester, SIGNAL(urlSelected(KUrl)),
            this, SLOT(slotUrlSelected(KUrl)));

    setPageWidget(vbox);
    setLeftBottomPix(DesktopIcon("kipi-flash", 128));
}

FirstRunPage::~FirstRunPage()
{
    delete d;
}

void FirstRunPage::slotDownload(const QString& url)
{
    KToolInvocation::invokeBrowser(url);
}

void FirstRunPage::slotUrlSelected(const KUrl& url)
{
    d->url = url;
    emit signalUrlObtained();
}

KUrl FirstRunPage::getUrl() const
{
    return d->url;
}

}   // namespace KIPIFlashExportPlugin
