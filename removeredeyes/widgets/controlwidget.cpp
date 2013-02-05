/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-22
 * Description : a widget to control the preview modes
 *
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

#include "controlwidget.moc"

// Qt includes

#include <QColor>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QTimer>

// KDE includes

#include <kstandarddirs.h>

namespace KIPIRemoveRedEyesPlugin
{

struct ControlWidget::Private
{
    Private() :
        mode(0),
        renderer(0),
        timerFadeIn(0),
        timerFadeOut(0)
    {
    }

    int           mode;

    QSvgRenderer* renderer;

    QRectF        corrected;
    QRectF        mask;
    QRectF        original;
    QRectF        zoomIn;
    QRectF        zoomOut;

    QTimer*       timerFadeIn;
    QTimer*       timerFadeOut;
};

ControlWidget::ControlWidget(QWidget* const parent, int w, int h)
    : QWidget(parent), d(new Private)
{
    setMinimumSize(w, h);
    setMaximumSize(w, h);
    setMouseTracking(true);
    hide();

    d->mode     = Normal;
    d->renderer = new QSvgRenderer(KGlobal::dirs()->findResource("data",
                                   "kipiplugin_removeredeyes/controlwidget.svg"),
                                   this);

    // ------------------------------------------

    d->original  = d->renderer->boundsOnElement("n_original");
    d->corrected = d->renderer->boundsOnElement("n_corrected");
    d->zoomIn    = d->renderer->boundsOnElement("n_zoomin");
    d->zoomOut   = d->renderer->boundsOnElement("n_zoomout");
    d->mask      = d->renderer->boundsOnElement("n_mask");

    // ------------------------------------------

    d->timerFadeIn  = new QTimer(this);
    d->timerFadeIn->setSingleShot(true);

    d->timerFadeOut = new QTimer(this);
    d->timerFadeOut->setSingleShot(true);

    // ------------------------------------------

    connect(d->timerFadeIn, SIGNAL(timeout()),
            this, SLOT(fadeIn()));

    connect(d->timerFadeOut, SIGNAL(timeout()),
            this, SLOT(fadeOut()));
}

ControlWidget::~ControlWidget()
{
    delete d;
}

void ControlWidget::renderElement(const QString& element, QPainter* const p)
{
    d->renderer->render(p, element, d->renderer->boundsOnElement(element));
}

void ControlWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    d->renderer->render(&p, QString("normal"));

    switch (d->mode)
    {
        case ZoomIn:
            renderElement(QString("h_zoomin"), &p);
            break;

        case ZoomOut:
            renderElement(QString("h_zoomout"), &p);
            break;

        case Mask:
            renderElement(QString("h_mask"), &p);
            break;

        case Original:
            renderElement(QString("h_original"), &p);
            break;

        case Corrected:
            renderElement(QString("h_corrected"), &p);
            break;

        case ZoomInPressed:
            renderElement(QString("p_zoomin"), &p);
            break;

        case ZoomOutPressed:
            renderElement(QString("p_zoomout"), &p);
            break;

        case MaskPressed:
            renderElement(QString("p_mask"), &p);
            break;

        case OriginalPressed:
            renderElement(QString("p_original"), &p);
            break;

        case CorrectedPressed:
            renderElement(QString("p_corrected"), &p);
            break;
    }
}

void ControlWidget::mouseMoveEvent(QMouseEvent* e)
{
    QRectF mousePos(e->pos(), QSize(1, 1));
    d->mode = Normal;

    if (mousePos.intersects(d->zoomOut))
    {
        setMode(ZoomOut);
    }
    else if (mousePos.intersects(d->zoomIn))
    {
        setMode(ZoomIn);
    }
    else if (mousePos.intersects(d->mask))
    {
        setMode(Mask);
    }
    else if (mousePos.intersects(d->original))
    {
        setMode(Original);
    }
    else if (mousePos.intersects(d->corrected))
    {
        setMode(Corrected);
    }
}

void ControlWidget::mousePressEvent(QMouseEvent* e)
{
    QRectF mousePos(e->pos(), QSize(1, 1));
    d->mode = Normal;

    if (mousePos.intersects(d->zoomOut))
    {
        setMode(ZoomOutPressed);
    }
    else if (mousePos.intersects(d->zoomIn))
    {
        setMode(ZoomInPressed);
    }
    else if (mousePos.intersects(d->mask))
    {
        setMode(MaskPressed);
    }
    else if (mousePos.intersects(d->original))
    {
        setMode(OriginalPressed);
    }
    else if (mousePos.intersects(d->corrected))
    {
        setMode(CorrectedPressed);
    }
}

void ControlWidget::leaveEvent(QEvent*)
{
    setMode(Normal);
    triggerHide();
}

void ControlWidget::mouseReleaseEvent(QMouseEvent*)
{
    switch (d->mode)
    {
        case ZoomInPressed:
            setMode(ZoomIn);
            emit zoomInClicked();
            break;

        case ZoomOutPressed:
            setMode(ZoomOut);
            emit zoomOutClicked();
            break;

        case OriginalPressed:
            setMode(Original);
            emit originalClicked();
            break;

        case CorrectedPressed:
            setMode(Corrected);
            emit correctedClicked();
            break;

        case MaskPressed:
            setMode(Mask);
            emit maskClicked();
            break;
    }
}

void ControlWidget::setMode(int mode)
{
    d->mode = mode;
    repaint();
}

void ControlWidget::fadeIn()
{
    // TODO: fade in effect
    show();
}

void ControlWidget::fadeOut()
{
    // TODO: fade out effect
    hide();
}

void ControlWidget::triggerShow(int ms)
{
    d->timerFadeIn->start(ms);
}

void ControlWidget::triggerHide(int ms)
{
    d->timerFadeOut->start(ms);
}

} // namespace KIPIRemoveRedEyesPlugin
