/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-13
 * Description : a widget to preview image effect.
 *
 * Copyright (C) 2009-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Kare Sars <kare dot sars at iki dot fi>
 * Copyright (C) 2012      by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef KPPREVIEW_IMAGE_H
#define KPPREVIEW_IMAGE_H

// Qt includes

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QString>
#include <QColor>

// Local includes

#include "kipiplugins_export.h"

class QResizeEvent;
class QWheelEvent;
class QMouseEvent;
class QEvent;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPSelectionItem : public QGraphicsItem
{
public:

    typedef enum
    {
        None,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left,
        TopLeft,
        Move
    } Intersects;

public:

    explicit KPSelectionItem(const QRectF& rect);
    ~KPSelectionItem();

    void setMaxRight(qreal maxRight);
    void setMaxBottom(qreal maxBottom);

    Intersects intersects(QPointF& point);

    void saveZoom(qreal zoom);

    void setRect(const QRectF& rect);
    QPointF fixTranslation(QPointF dp) const;
    QRectF rect() const;

public:

    // Graphics Item methods
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:

    void updateAnchors();

private:

    struct Private;
    Private* const d;
};

// -----------------------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPPreviewImage : public QGraphicsView
{
    Q_OBJECT

public:

    KPPreviewImage(QWidget* const parent);
    ~KPPreviewImage();

    bool load(const QString& file) const;
    bool setImage(const QImage& img) const;
    void enableSelectionArea(bool b);
    QRectF getSelectionArea();
    /**
     * Sets a selection area and show it
     * 
     * @param rectangle This rectangle should have height and width of 1.0
     */
    void setSelectionArea(QRectF rectangle);

public Q_SLOTS:

    void slotZoomIn();
    void slotZoomOut();
//     void slotZoomSel();      // TODO: add a button for that purpose
    void slotZoom2Fit();

    // Selection area specific slots (TL = TopLeft, BR = BottomRight)
    void slotSetTLX(float ratio);
    void slotSetTLY(float ratio);
    void slotSetBRX(float ratio);
    void slotSetBRY(float ratio);

    /** This function is used to set a selection without the user setting it.
     * \note all parameters must be in the range 0.0 -> 1.0.
     * \param tl_x is the x coordinate of the top left corner 0=0 1=image with.
     * \param tl_y is the y coordinate of the top left corner 0=0 1=image height.
     * \param br_x is the x coordinate of the bottom right corner 0=0 1=image with.
     * \param br_y is the y coordinate of the bottom right corner 0=0 1=image height. */
    void slotSetSelection(float tl_x, float tl_y, float br_x, float br_y);
    void slotClearActiveSelection();

    /** This function is used to darken everything except what is inside the given area.
     * \note all parameters must be in the range 0.0 -> 1.0.
     * \param tl_x is the x coordinate of the top left corner 0=0 1=image with.
     * \param tl_y is the y coordinate of the top left corner 0=0 1=image height.
     * \param br_x is the x coordinate of the bottom right corner 0=0 1=image with.
     * \param br_y is the y coordinate of the bottom right corner 0=0 1=image height. */
    void slotSetHighlightArea(float tl_x, float tl_y, float br_x, float br_y);

    /** This function sets the percentage of the highlighted area that is visible.
     *  The rest is hidden. This stacks with the previous highlight area.
     * \param percentage is the percentage of the highlighted area that is shown.
     * \param hideColor is the color to use to hide the highlighted area of the image.*/
    void slotSetHighlightShown(int percentage, QColor highLightColor = Qt::white);

    /** This function removes the highlight area. */
    void slotClearHighlight();

protected:

    void wheelEvent(QWheelEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    bool eventFilter(QObject*, QEvent*);
    void resizeEvent(QResizeEvent*);

    void updateSelVisibility();
    void updateHighlight();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif /* KPPREVIEW_IMAGE_H */
