/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-01-19
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2004      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2009 by Valerio Fuoglio <valerio.fuoglio@gmail.com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
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

// Qt includes

#include <QGLWidget>
#include <QKeyEvent>
#include <QList>
#include <QMap>
#include <QMouseEvent>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QWheelEvent>

// KDE includes

#include <kconfig.h>

class QColor;
class QFont;
class QTimer;

namespace KIPIAdvancedSlideshowPlugin
{

class SlidePlaybackWidget;
class PlaybackWidget;
class SlideShowLoader;
class SharedContainer;

class SlideShowGL : public QGLWidget
{
    Q_OBJECT

public:

    SlideShowGL(const QList<QPair<QString, int> >& fileList,
                const QStringList& commentsList, SharedContainer* const sharedData);
    ~SlideShowGL();

    void registerEffects();

    static QStringList effectNames();
    static QMap<QString, QString> effectNamesI18N();

protected:

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);
    void keyPressEvent(QKeyEvent*);

private:

    typedef void (SlideShowGL::*EffectMethod)();

    QPixmap       generateOutlinedTextPixmap(const QString& text);
    QPixmap       generateOutlinedTextPixmap(const QString& text, QFont& fn);
    QPixmap       generateCustomOutlinedTextPixmap(const QString& text,
                                                   QFont& fn, QColor& fgColor, QColor& bgColor,
                                                   int opacity, bool transBg = true);

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

private Q_SLOTS:

    void slotTimeOut();
    void slotMouseMoveTimeOut();

    void slotPause();
    void slotPlay();
    void slotPrev();
    void slotNext();
    void slotClose();

private:

    // config ------------------

    uint                              m_cacheSize;

    // -------------------------


    QMap<QString, EffectMethod>       m_effects;

    QList<QPair<QString, int> >       m_fileList;
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

    SharedContainer*                  m_sharedData;
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif /* SLIDESHOWGL_H */
