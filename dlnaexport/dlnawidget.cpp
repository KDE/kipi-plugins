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

#include <khbox.h>
#include <klocale.h>
#include <kdialog.h>
#include <kpushbutton.h>
#include <kconfig.h>
#include <kcolorscheme.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobalsettings.h>
#include <kdeversion.h>
#include <kiconloader.h>

// Libkipi includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

// Local includes

#include "mediaserver.h"

namespace KIPIDLNAExportPlugin
{

class DLNAWidget::Private
{
public:

    Private()
    {
        iconLbl      = 0;
        titleLbl     = 0;
        headerLbl    = 0;
        selectBtn    = 0;
        dlna         = 0;
        directoryLbl = 0;
    }

    QLabel*      iconLbl;
    QLabel*      titleLbl;
    QLabel*      headerLbl;

    KPushButton* selectBtn;

    MediaServer* dlna;

    QLabel*      directoryLbl;
};

DLNAWidget::DLNAWidget(Interface* const /*interface*/, const QString& /*tmpFolder*/, QWidget* const parent)
    : QWidget(parent), d(new Private)
{
    QVBoxLayout* mainLayout        = new QVBoxLayout(this);
    QWidget* settingsBox           = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    // -------------------------------------------------------------------

    KHBox* hbox = new KHBox(settingsBox);
    d->iconLbl  = new QLabel(hbox);
    d->iconLbl->setPixmap(KIconLoader::global()->loadIcon("dlna", KIconLoader::NoGroup, 64));
    d->iconLbl->setAlignment(Qt::AlignLeft);

    d->titleLbl = new QLabel(hbox);
    d->titleLbl->setOpenExternalLinks(true);
    d->titleLbl->setFocusPolicy(Qt::NoFocus);
    d->titleLbl->setAlignment(Qt::AlignLeft);
    d->titleLbl->setText(QString("<b><h2><a href='http://www.dlna.org'>"
                                "<font color=\"#9ACD32\">DLNA Export</font>"
                                "</a></h2></b>"));

    d->headerLbl = new QLabel(settingsBox);
    d->headerLbl->setText("Please select a folder containing JPEG images only");

    // ------------------------------------------------------------------------

    d->directoryLbl = new QLabel(settingsBox);

    // ------------------------------------------------------------------------

    d->selectBtn    = new KPushButton(KGuiItem(i18n("Select Directory"), "list-add",
                                              i18n("Select Directory to Export")),
                                              settingsBox);

    // ------------------------------------------------------------------------

    settingsBoxLayout->addWidget(hbox);
    settingsBoxLayout->addWidget(d->headerLbl);
    settingsBoxLayout->addWidget(d->directoryLbl);
    settingsBoxLayout->addWidget(d->selectBtn);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    // ------------------------------------------------------------------------

    connect(d->selectBtn, SIGNAL(clicked()),
            this, SLOT(slotSelectDirectory()));
}

DLNAWidget::~DLNAWidget()
{
    delete d;
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

    d->directoryLbl->setText(path);
    kDebug() << path;

    // TODO : stop properly previous server instance if exist.
    d->dlna = new MediaServer();
    d->dlna->onAddContentButtonClicked(path, true);
}

} // namespace KIPIDLNAExportPlugin
