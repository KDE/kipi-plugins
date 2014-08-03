/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a template to create wizzard page.
 *
 * Copyright (C) 2009-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpwizardpage.h"

// Qt includes

#include <QHBoxLayout>
#include <QLabel>

// KDE includes

#include <kvbox.h>
#include <kiconloader.h>
#include <kseparator.h>
#include <kstandarddirs.h>
#include <kassistantdialog.h>
#include <kpagewidgetmodel.h>

namespace KIPIPlugins
{

class KPWizardPage::Private
{
public:

    Private()
    {
        hlay          = 0;
        page          = 0;
        logo          = 0;
        leftBottomPix = 0;
    }

    QLabel*          logo;
    QLabel*          leftBottomPix;

    QHBoxLayout*     hlay;

    KPageWidgetItem* page;
};

KPWizardPage::KPWizardPage(KAssistantDialog* const dlg, const QString& title)
    : QScrollArea(dlg), d(new Private)
{
    QWidget* const panel = new QWidget(viewport());
    panel->setAutoFillBackground(false);
    setWidget(panel);
    setWidgetResizable(true);
    viewport()->setAutoFillBackground(false);

    d->hlay           = new QHBoxLayout(panel);
    KVBox* const vbox = new KVBox(panel);
    d->logo           = new QLabel(vbox);
    d->logo->setAlignment(Qt::AlignTop);
    d->logo->setPixmap(QPixmap(KStandardDirs::locate("data", "kipi/data/kipi-logo.svg"))
                               .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel* const space = new QLabel(vbox);
    d->leftBottomPix    = new QLabel(vbox);
    d->leftBottomPix->setAlignment(Qt::AlignBottom);
    vbox->setStretchFactor(space, 10);
    vbox->setMargin(KDialog::spacingHint());
    vbox->setSpacing(KDialog::spacingHint());

    KSeparator* const line = new KSeparator(Qt::Vertical, panel);

    d->hlay->addWidget(vbox);
    d->hlay->addWidget(line);
    d->hlay->setMargin(0);
    d->hlay->setSpacing(KDialog::spacingHint());

    d->page = dlg->addPage(this, title);
}

KPWizardPage::~KPWizardPage()
{
    delete d;
}

KPageWidgetItem* KPWizardPage::page() const
{
    return d->page;
}

void KPWizardPage::setPageWidget(QWidget* const w)
{
    d->hlay->addWidget(w);
    d->hlay->setStretchFactor(w, 10);
}
void KPWizardPage::removePageWidget(QWidget* const w)
{
    d->hlay->removeWidget(w);
}

void KPWizardPage::setLeftBottomPix(const QPixmap& pix)
{
    d->leftBottomPix->setPixmap(pix);
}

}   // namespace KIPIPlugins
