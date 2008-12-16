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

#include "previewzoombar.h"
#include "previewzoombar.moc"

// Qt includes.

#include <QGridLayout>

namespace KIPIRemoveRedEyesPlugin
{

PreviewZoomBarButton::PreviewZoomBarButton(ButtonType type, int size, QWidget* parent)
                    : QToolButton(parent)
{
    QString toolBtnStyle("QToolButton {"
                         "    border: 1px solid #333333;"
                         "    border-radius: 6px;"
                         "    background-color: rgb(230, 230, 230);"
                         "    font-size: 16px;"
                         "    color: #000000;"
                         "}"

                         "QToolButton:hover {"
                         "    border: 2px solid #870000;"
                         "    color: #870000;"
                         "}"

                         "QToolButton:pressed {"
                         "    border: 2px solid #870000;"
                         "    color: rgb(230, 230, 230);"
                         "    background-color: #870000;"
                         "}"
    );
    setStyleSheet(toolBtnStyle);

    switch (type)
    {
        case ZoomIn:
            setText("+");
            break;

        case ZoomOut:
            setText("-");
            break;
    }

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMinimumSize(size, size);
    setMaximumSize(size, size);
}

PreviewZoomBarButton::~PreviewZoomBarButton()
{
}

// ------------------------------------------------

class PreviewZoomBarPriv
{
public:

    PreviewZoomBarPriv()
    {
        btnSize    = 22;
        btnZoomIn  = 0;
        btnZoomOut = 0;
    }

    int                   btnSize;

    PreviewZoomBarButton* btnZoomIn;
    PreviewZoomBarButton* btnZoomOut;
};

PreviewZoomBar::PreviewZoomBar(QWidget* parent)
              : QFrame(parent),
                d(new PreviewZoomBarPriv)
{
    QString mainStyle("background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                      "            stop:0 rgba(156, 156, 156, 0)"
                      "            stop:0.3 rgba(156, 156, 156, 0)"
                      "            stop:1 rgb(156, 156, 156));"
    );
    setStyleSheet(mainStyle);

    // ------------------------------------------------

    d->btnZoomIn  = new PreviewZoomBarButton(PreviewZoomBarButton::ZoomIn);
    d->btnZoomOut = new PreviewZoomBarButton(PreviewZoomBarButton::ZoomOut);

    // ------------------------------------------------

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(d->btnZoomOut, 0, 1, 1, 1);
    layout->addWidget(d->btnZoomIn,  0, 2, 1, 1);
    layout->setSpacing(5);
    layout->setColumnStretch(0, 10);
    setLayout(layout);

    // ------------------------------------------------

    connect(d->btnZoomIn, SIGNAL(clicked()),
            this, SIGNAL(zoomInClicked()));

    connect(d->btnZoomOut, SIGNAL(clicked()),
            this, SIGNAL(zoomOutClicked()));
}

PreviewZoomBar::~PreviewZoomBar()
{
    delete d;
}

void PreviewZoomBar::setButtonSize(int size)
{
    d->btnSize = size;

    d->btnZoomIn->setMinimumSize(d->btnSize, d->btnSize);
    d->btnZoomIn->setMaximumSize(d->btnSize, d->btnSize);

    d->btnZoomOut->setMinimumSize(d->btnSize, d->btnSize);
    d->btnZoomOut->setMaximumSize(d->btnSize, d->btnSize);
}

int PreviewZoomBar::buttonSize() const
{
    return d->btnSize;
}

void PreviewZoomBar::setButtonSpacing(int spacing)
{
    layout()->setSpacing(spacing);
}

int PreviewZoomBar::buttonSpacing() const
{
    return layout()->spacing();
}

void PreviewZoomBar::setMinMaxWidth(int width)
{
    setMinimumWidth(width);
    setMaximumWidth(width);
}

void PreviewZoomBar::setMinMaxHeight(int height)
{
    setMinimumHeight(height);
    setMaximumHeight(height);
}

} // namespace KIPIRemoveRedEyesPlugin
