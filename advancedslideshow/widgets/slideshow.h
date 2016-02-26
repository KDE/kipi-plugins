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
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
class QWheelEvent;

namespace KIPIAdvancedSlideshowPlugin
{

typedef QPair<QString, int>  FileAnglePair;
typedef QList<FileAnglePair> FileList;

class SharedContainer;

class SlideShow : public QWidget
{
    Q_OBJECT

    typedef int (SlideShow::*EffectMethod)(bool);

public:

    SlideShow(const FileList& fileList,
              const QStringList& commentsList,
              SharedContainer* const sharedData);
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
    int     m_px;
    int     m_py;
    int     m_psx;
    int     m_psy;
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
    
    class Private;
    Private* const d;
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif /* SLIDESHOW_H */
