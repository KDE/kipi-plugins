/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-24
 * Description : a dialog to display processed messages in background
 *
 * Copyright (C) 2009-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpoutputdialog.h"

// Qt includes

#include <QLabel>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "kpaboutdata.h"

namespace KIPIPlugins
{

class KPOutputDialog::Private
{
public:

    Private()
    {
        debugView = 0;
    }

    QTextBrowser*     debugView;
    QDialogButtonBox* buttonBox;
};

KPOutputDialog::KPOutputDialog(QWidget* const parent, const QString& caption,
                               const QString& messages, const QString& header)
    : QDialog(parent),
      KPDialogBase(this),
      d(new Private)
{
    setModal(true);
    setWindowTitle(caption);

    // Create dialog buttons
    d->buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Help, this);
    d->buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

    QPushButton* const copyButton = new QPushButton(QIcon::fromTheme("edit-copy"), i18n("Copy to Clip&board"));
    d->buttonBox->addButton(copyButton, QDialogButtonBox::ActionRole);

    // Create dialog contents
    QLabel* const lHeader = new QLabel(this);
    d->debugView = new QTextBrowser(this);
    d->debugView->append(messages);
    lHeader->setText(header);

    // Put contained widgets together in a vertical layout
    QVBoxLayout* const mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(lHeader);
    mainLayout->addWidget(d->debugView);
    mainLayout->addWidget(d->buttonBox);

    connect(d->buttonBox, &QDialogButtonBox::accepted,
            this, &KPOutputDialog::accept);

    connect(copyButton, &QPushButton::clicked,
            this, &KPOutputDialog::slotCopyToCliboard);

    resize(600, 400);
}

KPOutputDialog::~KPOutputDialog()
{
    delete d;
}

QPushButton* KPOutputDialog::helpButton() const
{
    return d->buttonBox->button(QDialogButtonBox::Help);
}

void KPOutputDialog::slotCopyToCliboard()
{
    d->debugView->selectAll();
    d->debugView->copy();
    d->debugView->setPlainText(d->debugView->toPlainText());
}

}  // namespace KIPIPlugins
