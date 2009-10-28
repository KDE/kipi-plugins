/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "outputdialog.h"
#include "outputdialog.moc"

// Qt includes

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

// KDE includes

#include <kapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "pluginsversion.h"

namespace KIPIBatchProcessImagesPlugin
{

OutputDialog::OutputDialog(QWidget* parent, const QString& caption,
                           const QString& Messages, const QString& Header)
            : KDialog(parent)
{
    setCaption(caption);
    setModal(true);
    setButtons(Ok | Help | User1);
    setButtonText(User1, i18n("Copy to Clip&board"));
    setDefaultButton(Ok);

    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch processes images"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("An interface to show the output of the \"Batch Process "
                                                 "Images\" Kipi plugin.\n"
                                                 "This plugin uses the \"convert\" program from the \"ImageMagick\" "
                                                 "package."),
                                           ki18n("(c) 2003-2009, Gilles Caulier\n"
                                                 "(c) 2007-2009, Aurélien Gateau"));

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor(ki18n("Aurelien Gateau"), ki18n("Maintainer"),
                       "aurelien dot gateau at free dot fr");

    DialogUtils::setupHelpButton(this, m_about);

    //---------------------------------------------

    QWidget* box        = new QWidget(this);
    QVBoxLayout *dvlay  = new QVBoxLayout(box);
    QLabel *labelHeader = new QLabel(Header, box);
    m_debugView         = new QTextBrowser(box);
    m_debugView->append(Messages);

    dvlay->addWidget(labelHeader);
    dvlay->addWidget(m_debugView);
    dvlay->setSpacing(spacingHint());
    dvlay->setMargin(spacingHint());

    setMainWidget(box);

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotCopyToCliboard()));

    resize(600, 400);
}

OutputDialog::~OutputDialog()
{
    delete m_about;
}

void OutputDialog::slotHelp()
{
    KToolInvocation::invokeHelp("", "kipi-plugins");
}

void OutputDialog::slotCopyToCliboard()
{
    m_debugView->selectAll();
    m_debugView->copy();
    m_debugView->setPlainText(m_debugView->toPlainText());
}

}  // namespace KIPIBatchProcessImagesPlugin
