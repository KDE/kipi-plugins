/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-23
 * Description : A variant of KInputBox without a Cancel button
 *
 * Copyright (C) 2009 by Michael G. Hansen <mike at mghansen dot de>
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

#include "inputboxnocancel.h"
#include "inputboxnocancel.moc"

// Qt includes

#include <QCloseEvent>
#include <QLabel>
#include <QPointer>

// KDE includes

#include <klineedit.h>
#include <kvbox.h>

namespace KIPIGPSSyncPlugin
{

class InputBoxNoCancelPrivate
{
public:

    InputBoxNoCancelPrivate()
    : workString(0),
      stringEdit(0),
      okayPressed(false)
    {
    }
    QString   *workString;
    KLineEdit *stringEdit;
    bool       okayPressed;
};

InputBoxNoCancel::InputBoxNoCancel(const QString& title, const QString& label, QString* const inputString, QWidget* parent)
: KDialog(parent), d(new InputBoxNoCancelPrivate)
{
    d->workString = inputString;
    
    setCaption(title);
    setButtons(Ok);
    setDefaultButton(Ok);
    setModal(true);

    KVBox* box = new KVBox(this);
    setMainWidget(box);

    new QLabel(label, box);
    d->stringEdit = new KLineEdit(box);
    d->stringEdit->setClearButtonShown(true);
    d->stringEdit->setText(*(d->workString));
    d->stringEdit->setFocus();
    d->stringEdit->selectAll();

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOkClicked()));

    connect(d->stringEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
}

InputBoxNoCancel::~InputBoxNoCancel()
{
    delete d;
}

void InputBoxNoCancel::slotOkClicked()
{
    d->okayPressed = true;

    *(d->workString) = d->stringEdit->text();
}

void InputBoxNoCancel::slotTextChanged(const QString& text)
{
    Q_UNUSED(text)
    enableButtonOk(!d->stringEdit->text().isEmpty());
}

QString InputBoxNoCancel::AskForString(const QString& title, const QString& label, const QString& initialString, QWidget* const parent)
{
    QString workString = initialString;
    QPointer<InputBoxNoCancel> inputBox(new InputBoxNoCancel(title, label, &workString, parent));
    inputBox->exec();

    if (!inputBox.isNull())
        delete inputBox.data();

    return workString;
}

void InputBoxNoCancel::closeEvent(QCloseEvent *e)
{
    if (!e) return;

    // make sure the user can not close unless text was entered
    if (!d->okayPressed)
    {
        e->ignore();
        return;
    }

    e->accept();
}

} /* KIPIGPSSyncPlugin */

