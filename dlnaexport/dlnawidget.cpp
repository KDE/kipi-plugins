/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#include "dlnawidget.moc"

// Qt includes

#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QDesktopServices>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <kpushbutton.h>
#include <kconfig.h>
#include <kcolorscheme.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobalsettings.h>
#include <kdeversion.h>

// Libkipi includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// Local includes

#include "mediaserver_window.h"

namespace KIPIDLNAExportPlugin
{

DLNAWidget::DLNAWidget(Interface* const /*interface*/, const QString& /*tmpFolder*/, QWidget* const parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // -------------------------------------------------------------------

    QWidget* settingsBox           = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    m_headerLbl    = new QLabel(settingsBox);
    m_headerLbl->setText("Please select a folder containing jpeg/jpg images only");

    // ------------------------------------------------------------------------

    m_directoryLbl = new QLabel(settingsBox);

    // ------------------------------------------------------------------------

    m_selectBtn    = new KPushButton(KGuiItem(i18n("Select Directory"), "list-add",
                                              i18n("Select Directory for transfer")),
                                              settingsBox);

    // ------------------------------------------------------------------------

    settingsBoxLayout->addWidget(m_headerLbl);
    settingsBoxLayout->addWidget(m_directoryLbl);
    settingsBoxLayout->addWidget(m_selectBtn);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    // ------------------------------------------------------------------------

    connect(m_selectBtn, SIGNAL(clicked()),
            this, SLOT(slotSelectDirectory()));

}

DLNAWidget::~DLNAWidget()
{
}

void DLNAWidget::reactivate()
{
}

void DLNAWidget::slotSelectDirectory()
{
    QString startingPath;
#if KDE_IS_VERSION(4,1,61)
    startingPath = KGlobalSettings::picturesPath();
#else
    startingPath = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#endif
    QString path = KFileDialog::getExistingDirectory(startingPath, this,
                                                     i18n("Select folder to parse"));

    if (path.isEmpty())
    {
        return;
    }

    m_directoryLbl->setText(path);
    kDebug() << path;
    m_dlna = new MediaServerWindow();
    m_dlna->on_addContentButton_clicked(path, true);
}

} // namespace KIPIDLNAExportPlugin
