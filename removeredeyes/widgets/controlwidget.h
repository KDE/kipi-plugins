/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : Dec 21, 2008
 * Description : XXXXXXXXXXXXXXXXXXXXXXXx
 *
 * Copyright (C) 2008 by andi <xxxxxxxxxxxxx>
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

#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

// Qt includes.

#include <QWidget>

class QString;
class QPainter;

namespace KIPIRemoveRedEyesPlugin
{

class ControlWidgetPriv;

class ControlWidget : public QWidget
{
    Q_OBJECT

public:

    enum ButtonCode
    {
        Normal = 0,
        ZoomIn,
        ZoomOut,
        Original,
        Corrected,
        Mask,
        ZoomInPressed,
        ZoomOutPressed,
        OriginalPressed,
        CorrectedPressed,
        MaskPressed
    };

public:

    ControlWidget(QWidget* parent = 0, int w = 150, int h = 150);
    ~ControlWidget();

    void triggerShow(int ms = 0);
    void triggerHide(int ms = 500);

signals:

    void zoomInClicked();
    void zoomOutClicked();
    void originalClicked();
    void correctedClicked();
    void maskClicked();

protected:

    void paintEvent(QPaintEvent* e);
    void mouseMoveEvent (QMouseEvent* e);
    void mouseReleaseEvent (QMouseEvent* e);
    void mousePressEvent (QMouseEvent* e);
    void leaveEvent (QEvent * e);

private slots:

    void fadeIn();
    void fadeOut();

private:

    void setMode(int mode);
    void renderElement(const QString& element, QPainter* p);

private:

    ControlWidgetPriv* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* CONTROLWIDGET_H */
