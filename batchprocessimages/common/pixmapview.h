/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Copyright (C) 2009 by Aurélien Gâteau <agateau@kde.org>
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

#ifndef PIXMAPVIEW_H
#define PIXMAPVIEW_H

// Qt includes

#include <QAbstractScrollArea>
#include <QImage>

class QMouseEvent;
class QPaintEvent;
class QPixmap;
class QResizeEvent;
class QWheelEvent;

class KProcess;

namespace KIPIBatchProcessImagesPlugin
{

class PixmapView : public QAbstractScrollArea
{
    Q_OBJECT

public:

    PixmapView(QWidget *parent = 0);
    ~PixmapView();

    void setImage(const QString& ImagePath, const QString& tmpPath, bool cropAction);
    void setZoom(int zoomFactor);

Q_SIGNALS:

    void wheelChanged(int delta);

protected:

    void contentsWheelEvent(QWheelEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);

private Q_SLOTS:

    void slotPreviewReadyRead();
    void slotPreviewProcessFinished();

private:

    void PreviewCal(const QString& ImagePath, const QString& tmpPath);
    void updateScrollBars();
    void updateView();

private:

    QPixmap  *m_pix;
    QImage    m_img;

    int       m_w;
    int       m_h;
    QPoint    m_dragPos;

    KProcess *m_PreviewProc;

    QString   m_previewOutput;
    QString   m_previewFileName;

    bool      m_validPreview;
    bool      m_cropAction;

    int       m_zoomFactor;
};

} // namespace KIPIBatchProcessImagesPlugin

#endif /* PIXMAPVIEW_H */
