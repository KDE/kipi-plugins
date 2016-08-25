/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#include "rajcewindow.h"

// Qt includes

#include <QAction>
#include <QMenu>
#include <QCloseEvent>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "kpaboutdata.h"
#include "rajcewidget.h"

namespace KIPIRajcePlugin
{

RajceWindow::RajceWindow(const QString& tmpFolder, QWidget* const /*parent*/)
    : KPToolDialog(0)
{
    m_widget = new RajceWidget(iface(), tmpFolder, this);
    m_widget->readSettings();

    setMainWidget(m_widget);
    setWindowIcon(QIcon::fromTheme(QString::fromLatin1("kipi-rajce")));
    setModal(false);

    setWindowTitle(i18n("Export to Rajce.net"));

    startButton()->setText(i18n("Start Upload"));
    startButton()->setToolTip(i18n("Start upload to Rajce.net"));

    m_widget->setMinimumSize(700, 500);

    connect(startButton(), SIGNAL(clicked()),
            m_widget, SLOT(startUpload()));

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));

    connect(m_widget, SIGNAL(loginStatusChanged(bool)),
            this, SLOT(slotSetUploadButtonEnabled(bool)));

    //--------------------------------------------------------------------

    KPAboutData* const about = new KPAboutData(ki18n("Rajce.net Export"),
                                   ki18n("A tool to export image collections to "
                                   "Rajce.net."),
                                   ki18n( "(c) 2011-2013, Lukas Krejci" ));

    about->addAuthor(ki18n( "Lukas Krejci" ).toString(),
                     ki18n("Author").toString(),
                     QString::fromLatin1("metlosh at gmail dot com"));

    about->setHandbookEntry(QString::fromLatin1("tool-rajceexport"));
    setAboutData(about);

    startButton()->setEnabled(false);
}

RajceWindow::~RajceWindow()
{
}

void RajceWindow::reactivate()
{
    m_widget->reactivate();
    show();
}

void RajceWindow::slotSetUploadButtonEnabled(bool enabled)
{
    startButton()->setEnabled(enabled);
}

void RajceWindow::slotFinished()
{
    m_widget->cancelUpload();
    m_widget->writeSettings();
}

void RajceWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

} //namespace KIPIRajcePlugin
