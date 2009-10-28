/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2003-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005 by Owen Hirst <n8rider@sbcglobal.net>
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

#include "renameimagesdialog.h"
#include "renameimagesdialog.moc"

// Qt includes

#include <QVBoxLayout>
#include <QLayout>
#include <QPushButton>

// KDE includes

#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "renameimageswidget.h"

namespace KIPIBatchProcessImagesPlugin
{

RenameImagesDialog::RenameImagesDialog(const KUrl::List& images, KIPI::Interface* interface, QWidget* parent)
                  : KDialog(parent)
{
    setCaption(i18n("Rename Images"));
    setModal(true);
    setButtons(Help | User1 | Close);
    setButtonText(User1, i18n("&Start"));
    setDefaultButton(Close);

    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch-rename images"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to batch-rename images"),
                                           ki18n("(c) 2003-2009, Gilles Caulier\n"
                                                 "(c) 2007-2009, Aurélien Gateau"));

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor(ki18n("Aurelien Gateau"), ki18n("Maintainer"),
                       "aurelien dot gateau at free dot fr");

    DialogUtils::setupHelpButton(this, m_about);

    // gui

    QWidget* box     = new QWidget(this);
    QVBoxLayout* lay = new QVBoxLayout(box);
    m_widget         = new RenameImagesWidget(box, interface, images);
    lay->addWidget(m_widget);
    setMainWidget(box);

    connect(this, SIGNAL(user1Clicked()),
            m_widget, SLOT(slotStart()));

    connect(this, SIGNAL(helpClicked()),
            this, SLOT(slotHelp()));

    adjustSize();
}

RenameImagesDialog::~RenameImagesDialog()
{
    delete m_about;
}

void RenameImagesDialog::slotHelp(void)
{
    KToolInvocation::invokeHelp("renameimages", "kipi-plugins");
}

}  // namespace KIPIBatchProcessImagesPlugin
