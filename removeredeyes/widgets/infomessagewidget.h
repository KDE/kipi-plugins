/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-30
 * Description : a widget to display an info message
 *
 * Copyright (C) 2004-2005 by Enrico Ros <eros.kde@email.it>
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef INFOMESSAGEWIDGET_H
#define INFOMESSAGEWIDGET_H

// Qt includes

#include <QWidget>

namespace KIPIRemoveRedEyesPlugin
{

class InfoMessageWidget : public QWidget
{
    Q_OBJECT

public:

    enum Icon
    {
        None = 0,
        Info,
        Warning
    };

public:

    explicit InfoMessageWidget(QWidget* const parent = 0);
    ~InfoMessageWidget();

    void display(const QString& message, Icon icon = Info, int durationMs = 0);
    void reset();

protected:

    void paintEvent(QPaintEvent* e);

private:

    struct Private;
    Private* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* INFOMESSAGEWIDGET_H */
