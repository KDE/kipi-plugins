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

#include "firstrunpage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QFrame>
#include <QLayout>
#include <QIcon>
#include <QDesktopServices>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/ImageCollectionSelector>

// Local includes

#include "kpversion.h"
#include "kipiplugins_debug.h"
#include "kputil.h"

namespace KIPIFlashExportPlugin
{

class FirstRunPage::Private
{
public:

    Private()
    {
        urlRequester = 0;
    }

    QUrl           url;

    KPFileSelector* urlRequester;
};

// link this page to SimpleViewer to gain access to settings container.
FirstRunPage::FirstRunPage(KPWizardDialog* const dlg)
    : KPWizardPage(dlg, i18n("First Run")),
      d(new Private)
{
    KPVBox* const vbox   = new KPVBox(this);
    QLabel* const info1 = new QLabel(vbox);
    info1->setWordWrap(true);
    info1->setText( i18n("<p>SimpleViewer's plugins are Flash components which are free to use, "
                         "but use a license which comes into conflict with several distributions. "
                         "Due to the license it is not possible to ship it with this tool.</p>"
                         "<p>You can now download plugin from its homepage and point this tool "
                         "to the downloaded archive. The archive will be stored with the plugin configuration, "
                         "so it is available for further use.</p>"
                         "<p><b>Note: Please download the plugin that you selected on the first page.</b></p>"));

    QLabel* const info2   = new QLabel(vbox);
    info2->setText(i18n("<p>1.) Download plugin from the following url:</p>"));

    QLabel* const link = new QLabel(vbox);
    link->setAlignment(Qt::AlignLeft);
    link->setOpenExternalLinks(true);
    link->setTextFormat(Qt::RichText);
    link->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    link->setText(QString::fromLatin1("<a href=\"%1\">%2</a>")
                          .arg(QLatin1String("http://www.simpleviewer.net"))
                          .arg(QLatin1String("http://www.simpleviewer.net")));

    QLabel* const info3   = new QLabel(vbox);
    info3->setText(i18n("<p>2.) Point this tool to the downloaded archive</p>"));

    d->urlRequester = new KPFileSelector(vbox);
    d->urlRequester->lineEdit()->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    d->urlRequester->setFileDlgTitle(i18n("Select downloaded archive"));
    d->urlRequester->setFileDlgMode(QFileDialog::ExistingFile);

    connect(d->urlRequester, SIGNAL(signalUrlSelected(QUrl)),
            this, SLOT(slotUrlSelected(QUrl)));

    setPageWidget(vbox);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("kipi-flash")).pixmap(128));
    setComplete(false);
}

FirstRunPage::~FirstRunPage()
{
    delete d;
}

void FirstRunPage::slotUrlSelected(const QUrl& url)
{
    qCDebug(KIPIPLUGINS_LOG) << "Using archive from " << url;
    d->url = url;
    emit signalUrlObtained();
}

QUrl FirstRunPage::getUrl() const
{
    return d->url;
}

}   // namespace KIPIFlashExportPlugin
