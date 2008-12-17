/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-16
 * Description : a widget to display a zoombar with a
 *               semi-transparent background color
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef PREVIEWZOOMBAR_H
#define PREVIEWZOOMBAR_H

// Qt includes.

#include <QFrame>
#include <QToolButton>

namespace KIPIRemoveRedEyesPlugin
{

class PreviewZoomBarPriv;

class PreviewZoomBarButton : public QToolButton
{
    Q_OBJECT

public:

    enum ButtonType
    {
        ZoomIn = 0,
        ZoomOut
    };

public:

    explicit PreviewZoomBarButton(ButtonType type, int size = 22, QWidget* parent = 0);
    ~PreviewZoomBarButton();
};

// ---------------------------------------------

class PreviewZoomBar : public QFrame
{
    Q_OBJECT

public:

    PreviewZoomBar(QWidget* parent = 0);
    ~PreviewZoomBar();

    void setButtonSize(int size);
    int buttonSize() const;

    void setButtonSpacing(int spacing);
    int buttonSpacing() const;

    void setMinMaxWidth(int width);
    void setMinMaxHeight(int height);

signals:

    void zoomInClicked();
    void zoomOutClicked();

private:

    PreviewZoomBarPriv* const d;
};

}

#endif /* PREVIEWZOOMBAR_H */
