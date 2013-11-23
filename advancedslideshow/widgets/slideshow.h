/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-02-16
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2006-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
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

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

// Qt includes

#include <QList>
#include <QMap>
#include <QPair>
#include <QPixmap>
#include <QPolygon>
#include <QString>
#include <QStringList>
#include <QWidget>

class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QTimer;
class QWheelEvent;

typedef QPair<QString, int>  FileAnglePair;
typedef QList<FileAnglePair> FileList;

namespace KIPIAdvancedSlideshowPlugin
{

class SlideShowLoader;
class SlidePlaybackWidget;
class SharedContainer;
class PlaybackWidget;

class SlideShow : public QWidget
{
    Q_OBJECT

    typedef int (SlideShow::*EffectMethod)(bool);

public:

    SlideShow(const FileList& fileList, const QStringList& commentsList, SharedContainer* const sharedData);
    ~SlideShow();

    void registerEffects();

    static QStringList            effectNames();
    static QMap<QString, QString> effectNamesI18N();

protected:

    void    mousePressEvent(QMouseEvent*);
    void    mouseMoveEvent(QMouseEvent*);
    void    wheelEvent(QWheelEvent*);
    void    keyPressEvent(QKeyEvent*);

    int     effectNone(bool);
    int     effectChessboard(bool doInit);
    int     effectMeltdown(bool doInit);
    int     effectSweep(bool doInit);
    int     effectMosaic(bool doInit);
    int     effectCubism(bool doInit);
    int     effectRandom(bool doInit);
    int     effectGrowing(bool doInit);
    int     effectHorizLines(bool doInit);
    int     effectVertLines(bool doInit);
    int     effectMultiCircleOut(bool doInit);
    int     effectSpiralIn(bool doInit);
    int     effectCircleOut(bool doInit);
    int     effectBlobs(bool doInit);

    void    paintEvent(QPaintEvent*);
    void    startPainter();

protected:

    bool    m_simplyShow;
    bool    m_startPainter;
    int     m_px, m_py, m_psx, m_psy, m_psw, m_psh;
    bool    m_endOfShow;
    QPixmap m_buffer;

private Q_SLOTS:

    void    slotTimeOut();
    void    slotMouseMoveTimeOut();

    void    slotPause();
    void    slotPlay();
    void    slotPrev();
    void    slotNext();
    void    slotClose();

private:

    void         loadNextImage();
    void         loadPrevImage();
    void         showCurrentImage();
    void         printFilename();
    void         printComments();
    void         printProgress();
    void         showEndOfShow();
    void         readSettings();
    EffectMethod getRandomEffect();

private:

    SharedContainer*            m_sharedData;

    uint                        m_cacheSize;

    // -------------------------

    QMap<QString, EffectMethod> Effects;

    SlideShowLoader*            m_imageLoader;
    PlaybackWidget*             m_playbackWidget;
    QPixmap                     m_currImage;

    FileList                    m_fileList;
    QStringList                 m_commentsList;
    QTimer*                     m_timer;
    int                         m_fileIndex;

    EffectMethod                m_effect;
    bool                        m_effectRunning;
    QString                     m_effectName;

    // values for state of various effects:
    int                         m_x, m_y, m_w, m_h, m_dx, m_dy, m_ix, m_iy, m_i, m_j, m_subType;
    int                         m_x0, m_y0, m_x1, m_y1, m_wait;
    double                      m_fx, m_fy, m_alpha, m_fd;
    int*                        m_intArray;
    bool                        m_pdone;
    bool**                      m_pixelMatrix;

    //static
    QPolygon                    m_pa;

    SlidePlaybackWidget*        m_slidePlaybackWidget;
    QTimer*                     m_mouseMoveTimer;

    int                         m_deskX;
    int                         m_deskY;
    int                         m_deskWidth;
    int                         m_deskHeight;
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif /* SLIDESHOW_H */
