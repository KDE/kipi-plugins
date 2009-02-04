/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-01-19
 * Description : a kipi plugin to slide images.
 *
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright 2006-2008 by Valerio Fuoglio <valerio.fuoglio@gmail.com>
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

#ifndef SLIDESHOWGL_H
#define SLIDESHOWGL_H

#ifdef _MSC_VER
#include <winsock2.h>
#endif

// Qt includes.
#include <Q3ValueList>
#include <QStringList>
#include <QPair>
#include <QString>
#include <QMap>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGLWidget>

// KDE includes.
#include <kconfig.h>

// Local includes.
#include "slideshowloader.h"
#include "slideplaybackwidget.h"
#include "playbackwidget.h"
#include "common.h"

class QTimer;

namespace KIPIAdvancedSlideshowPlugin
{

class SlideShowGL;

class SlideShowGL : public QGLWidget
{
    Q_OBJECT

public:

    SlideShowGL(const Q3ValueList<QPair<QString, int> >& fileList,
                const QStringList& commentsList, SharedData* sharedData);
    ~SlideShowGL();

    void registerEffects();

    static QStringList effectNames();
    static QMap<QString, QString> effectNamesI18N();

protected:

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *event);

private:

    typedef void (SlideShowGL::*EffectMethod)();

    void          paintTexture();
    void          advanceFrame();
    void          previousFrame();
    void          loadImage();
    void          montage(QImage& top, QImage& bot);
    EffectMethod  getRandomEffect();
    void          showEndOfShow();
    void          printFilename(QImage& layer);
    void          printProgress(QImage& layer);
    void          printComments(QImage& layer);

    void          effectNone();
    void          effectBlend();
    void          effectFade();
    void          effectRotate();
    void          effectBend();
    void          effectInOut();
    void          effectSlide();
    void          effectFlutter();
    void          effectCube();


private slots:

    void slotTimeOut();
    void slotMouseMoveTimeOut();

    void slotPause();
    void slotPlay();
    void slotPrev();
    void slotNext();
    void slotClose();

private:

    // config ------------------

    uint     m_cacheSize;

    // -------------------------


    QMap<QString, EffectMethod>       m_effects;

    Q3ValueList<QPair<QString, int> > m_fileList;
    QStringList                       m_commentsList;
    QTimer*                           m_timer;
    int                               m_fileIndex;

    SlideShowLoader*                  m_imageLoader;
    GLuint                            m_texture[2];
    bool                              m_tex1First;
    int                               m_curr;

    int                               m_width;
    int                               m_height;
    int                               m_xMargin;
    int                               m_yMargin;


    EffectMethod                      m_effect;
    bool                              m_effectRunning;
    int                               m_timeout;
    bool                              m_random;
    bool                              m_endOfShow;

    int                               m_i;
    int                               m_dir;
    float                             m_points[40][40][3];

    SlidePlaybackWidget*              m_slidePlaybackWidget;
    PlaybackWidget*                   m_playbackWidget;
    QTimer*                           m_mouseMoveTimer;

    int                               m_deskX;
    int                               m_deskY;
    int                               m_deskWidth;
    int                               m_deskHeight;

    SharedData*                       m_sharedData;
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif /* SLIDESHOWGL_H */
