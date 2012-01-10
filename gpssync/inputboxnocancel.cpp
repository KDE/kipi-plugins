/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-11-23
 * @brief  A variant of KInputBox without a Cancel button.
 *
 * @author Copyright (C) 2009 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

    connect(d->stringEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotTextChanged(QString)));
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

    delete inputBox;

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

