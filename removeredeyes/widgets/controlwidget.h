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

#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

// Qt includes

#include <QWidget>

class QString;
class QPainter;

namespace KIPIRemoveRedEyesPlugin
{

class ControlWidget : public QWidget
{
    Q_OBJECT

public:

    enum ControllerMode
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

    explicit ControlWidget(QWidget* const parent = 0, int w = 150, int h = 150);
    ~ControlWidget();

    void triggerShow(int ms = 0);
    void triggerHide(int ms = 500);

Q_SIGNALS:

    void correctedClicked();
    void maskClicked();
    void originalClicked();
    void zoomInClicked();
    void zoomOutClicked();

protected:

    void leaveEvent(QEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent* e);

private Q_SLOTS:

    void fadeIn();
    void fadeOut();

private:

    void renderElement(const QString& element, QPainter* const p);
    void setMode(int mode);

private:

    struct Private;
    Private* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* CONTROLWIDGET_H */
