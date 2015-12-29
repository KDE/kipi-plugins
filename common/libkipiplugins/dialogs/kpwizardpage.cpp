/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a template to create wizzard page.
 *
 * Copyright (C) 2009-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QApplication>
#include <QStyle>
#include <QScrollArea>

// Local includes

#include "kptooldialog.h"

namespace KIPIPlugins
{

class KPWizardPage::Private
{
public:

    Private()
    {
        hlay          = 0;
        logo          = 0;
        leftBottomPix = 0;
        leftView      = 0;
        isComplete    = true;
        id            = -1;
        dlg           = 0;
    }

    bool            isComplete;
    int             id;
    
    QWidget*        leftView;
    QLabel*         logo;
    QLabel*         leftBottomPix;

    QHBoxLayout*    hlay;
    
    KPWizardDialog* dlg;
};

KPWizardPage::KPWizardPage(KPWizardDialog* const dlg, const QString& title)
    : QWizardPage(dlg),
      d(new Private)
{
    setTitle(title);

    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QScrollArea* const sv = new QScrollArea(this);
    QWidget* const panel  = new QWidget(sv->viewport());
    sv->setWidget(panel);
    sv->setWidgetResizable(true);

    d->hlay                    = new QHBoxLayout(panel);
    d->leftView                = new QWidget(panel);
    QVBoxLayout* const vboxLay = new QVBoxLayout(d->leftView);
    d->logo                    = new QLabel(d->leftView);
    d->logo->setAlignment(Qt::AlignTop);
    d->logo->setPixmap(QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                      QStringLiteral("kf5/kipi/pics/kipi-logo.svg")))
                               .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QWidget* const space = new QLabel(d->leftView);
    d->leftBottomPix     = new QLabel(d->leftView);
    d->leftBottomPix->setAlignment(Qt::AlignBottom);

    vboxLay->addWidget(d->logo);
    vboxLay->addWidget(space);
    vboxLay->addWidget(d->leftBottomPix);
    vboxLay->setStretchFactor(space, 10);
    vboxLay->setContentsMargins(spacing, spacing, spacing, spacing);
    vboxLay->setSpacing(spacing);

    QFrame* const vline = new QFrame(panel);
    vline->setLineWidth(1);
    vline->setMidLineWidth(0);
    vline->setFrameShape(QFrame::VLine);
    vline->setFrameShadow(QFrame::Sunken);
    vline->setMinimumSize(2, 0);
    vline->updateGeometry();

    d->hlay->addWidget(d->leftView);
    d->hlay->addWidget(vline);
    d->hlay->setContentsMargins(QMargins());
    d->hlay->setSpacing(spacing);
    
    QVBoxLayout* const layout = new QVBoxLayout;
    layout->addWidget(sv);
    setLayout(layout);
    
    d->dlg = dlg;
    d->id  = d->dlg->addPage(this);
}

KPWizardPage::~KPWizardPage()
{
    delete d;
}

void KPWizardPage::setComplete(bool b)
{
    d->isComplete = b;
    emit completeChanged();
}

bool KPWizardPage::isComplete() const
{
    return d->isComplete;
}

int KPWizardPage::id() const
{
    return d->id;
}

void KPWizardPage::setShowLeftView(bool v)
{
    d->leftView->setVisible(v);
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

KPWizardDialog* KPWizardPage::assistant() const
{
    return d->dlg;
}

}   // namespace KIPIPlugins
