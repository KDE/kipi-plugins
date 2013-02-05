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

#include "infomessagewidget.moc"

// Qt includes

#include <QPainter>
#include <QPalette>
#include <QTimer>

// KDE includes

#include <kapplication.h>
#include <kiconloader.h>

namespace KIPIRemoveRedEyesPlugin
{

struct InfoMessageWidget::Private
{
    Private()
    {
        timer  = 0;
        hidden = false;
    }

    bool    hidden;
    QString message;
    QPixmap symbol;
    QTimer* timer;
};

InfoMessageWidget::InfoMessageWidget(QWidget* const parent)
    : QWidget(parent), d(new Private)
{
    setFocusPolicy(Qt::NoFocus);

    QPalette pal = palette();
    pal.setColor(QPalette::Active, QPalette::Window, KApplication::palette().color(QPalette::Active, QPalette::Window));
    setPalette(pal);

    move(10, 10);
    resize(0, 0);
    hide();
}

InfoMessageWidget::~InfoMessageWidget()
{
    delete d;
}

void InfoMessageWidget::display(const QString& message, Icon icon, int durationMs)
{
    // set text
    d->message     = message;

    // determine text rectangle
    QRect textRect = fontMetrics().boundingRect(d->message);
    textRect.translate(-textRect.left(), -textRect.top());
    textRect.adjust(0, 0, 2, 2);

    int width      = textRect.width(), height = textRect.height();

    // load icon (if set) and update geometry
    d->symbol      = QPixmap();

    if (icon != None)
    {
        switch (icon)
        {
            case Warning:
                d->symbol = SmallIcon("dialog-warning");
                break;

            default:
                d->symbol = SmallIcon("dialog-information");
                break;
        }

        width += 2 + d->symbol.width();
        height = qMax(height, d->symbol.height());
    }

    // resize widget
    resize(QRect(0, 0, width + 10, height + fontMetrics().height() / 2).size());

    // if the layout is RtL, we can move it to the right place only after we
    // know how much size it will take
    if (layoutDirection() == Qt::RightToLeft)
    {
        move(parentWidget()->width() - geometry().width() - 10 - 1, 10);
    }

    // show widget and schedule a repaint
    if (!d->hidden)
    {
        show();
    }

    update();

    // close the message window after given mS
    if (durationMs > 0)
    {
        if (!d->timer)
        {
            d->timer = new QTimer(this);
            d->timer->setSingleShot(true);

            connect(d->timer, SIGNAL(timeout()),
                    SLOT(hide()));
        }

        d->timer->start(durationMs);
    }
    else if (d->timer)
    {
        d->timer->stop();
    }
}

void InfoMessageWidget::paintEvent( QPaintEvent* /* e */)
{
    QRect textRect = fontMetrics().boundingRect( d->message );
    textRect.translate( -textRect.left(), -textRect.top() );
    textRect.adjust( 0, 0, 2, 2 );

    int textXOffset = 0,
        textYOffset = geometry().height() - textRect.height() / 2,
        iconXOffset = 0,
        iconYOffset = !d->symbol.isNull() ? ( geometry().height() - d->symbol.height() ) / 2
                      : 0, shadowOffset = 1;

    if ( layoutDirection() == Qt::RightToLeft )
    {
        iconXOffset = 2 + textRect.width();
    }
    else
    {
        textXOffset = 2 + d->symbol.width();
    }

    // draw background
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( Qt::black );

    QColor semiTransBg = palette().color(QPalette::Window);
    semiTransBg.setAlpha(190);

    painter.setBrush(semiTransBg);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.translate( 0.5, 0.5 );
    painter.drawRoundRect( 1, 1, width()-2, height()-2, 1600 / width(), 1600 / height() );

    // draw icon if present
    if ( !d->symbol.isNull() )
    {
        painter.drawPixmap(5 + iconXOffset, iconYOffset, d->symbol, 0, 0, d->symbol.width(), d->symbol.height());
    }

    // draw shadow and text
    painter.setPen( palette().color( QPalette::Window ).dark( 115 ) );
    painter.drawText( 5 + textXOffset + shadowOffset, textYOffset + shadowOffset, d->message );
    painter.setPen( palette().color( QPalette::WindowText ) );
    painter.drawText( 5 + textXOffset, textYOffset, d->message );
}

void InfoMessageWidget::reset()
{
    d->hidden = false;
    hide();
    d->message.clear();
}

} // namespace KIPIRemoveRedEyesPlugin
