/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
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

#ifndef INPUTBOXNOCANCEL_H
#define INPUTBOXNOCANCEL_H

#include <KDialog>

class QCloseEvent;

namespace KIPIGPSSyncPlugin
{

class InputBoxNoCancelPrivate;

class InputBoxNoCancel : public KDialog
{
    Q_OBJECT

public:
    InputBoxNoCancel(const QString& title, const QString& label, QString* const workString, QWidget* parent);
    ~InputBoxNoCancel();

    static QString AskForString(const QString& title, const QString& label, const QString& initialString, QWidget* const parent);

private Q_SLOTS:
    void slotOkClicked();
    void slotTextChanged(const QString& text);

protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    InputBoxNoCancelPrivate* const d;

};

} /* KIPIGPSSyncPlugin */

#endif // INPUTBOXNOCANCEL_H

