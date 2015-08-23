/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2007-11-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 *
 * Parts of this code are based on
 * smoothslidesaver by Carsten Weinhold <carsten dot weinhold at gmx dot de>
 * and slideshowgl by Renchi Raju <renchi dot raju at gmail dot com>
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

#ifndef SLIDESHOWKB_H
#define SLIDESHOWKB_H

// C++ includes

#ifndef _MSC_VER
#include <cstdlib>
#else
#include <winsock2.h>
#endif

// Qt includes

#include <QList>
#include <QKeyEvent>
#include <QMap>
#include <QMouseEvent>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QGLWidget>

namespace KIPIAdvancedSlideshowPlugin
{

class KBEffect;
class ImageLoadThread;
class SharedContainer;
class PlaybackWidget;
class ScreenProperties;

// -------------------------------------------------------------------------

class ViewTrans
{

public:

    ViewTrans(bool m_zoomIn, float relAspect);
    ViewTrans()
    {
        m_deltaX     = 0.0;
        m_deltaY     = 0.0;
        m_deltaScale = 0.0;
        m_baseScale  = 0.0;
        m_baseX      = 0.0;
        m_baseY      = 0.0;
        m_xScale     = 0.0;
        m_yScale     = 0.0;
    }

    ~ViewTrans()
    {
    }

    float transX(float pos) const
    {
        return m_baseX + m_deltaX * pos;
    };

    float transY(float pos) const
    {
        return m_baseY + m_deltaY * pos;
    };

    float scale (float pos) const
    {
        return m_baseScale * (1.0 + m_deltaScale * pos);
    };

    float m_xScaleCorrect() const
    {
        return m_xScale;
    };

    float m_yScaleCorrect() const
    {
        return m_yScale;
    };

private:

    double rnd() const
    {
        return (double)qrand() / (double)RAND_MAX;
    };

    double rndSign() const
    {
        return (qrand() < RAND_MAX / 2) ? 1.0 : -1.0;
    };

private:

    // delta and scale values (begin to end) and the needed offsets
    double m_deltaX, m_deltaY, m_deltaScale;
    double m_baseScale, m_baseX, m_baseY;
    float  m_xScale, m_yScale;
};

// -------------------------------------------------------------------------

class Image
{

public:

    explicit Image(ViewTrans* const viewTrans, float aspect = 1.0);
    ~Image();

public:

    ViewTrans* m_viewTrans;
    float      m_aspect;
    float      m_pos;
    float      m_opacity;
    bool       m_paint;
    GLuint     m_texture;
};

// -------------------------------------------------------------------------

class SlideShowKB : public QGLWidget
{
    Q_OBJECT

public:

    SlideShowKB(const QList<QPair<QString, int> >& fileList,
                const QStringList& commentsList, SharedContainer* const sharedData);

    ~SlideShowKB();

    static QStringList effectNames();
    static QMap<QString, QString> effectNamesI18N();

private:

    float aspect() const
    {
        return (float)width() / (float)height();
    };

    bool     setupNewImage(int imageIndex);
    void     startSlideShowOnce();
    void     swapImages();
    void     setNewKBEffect();
    void     endOfShow();

    void     initializeGL();
    void     paintGL();
    void     resizeGL(int w, int h);
    void     applyTexture(Image* const img, const QImage& image);
    void     paintTexture(Image* const img);
    unsigned suggestFrameRate(unsigned forceRate);

    void     readSettings();

protected:

    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);

private Q_SLOTS:

    void moveSlot();
    void slotEndOfShow();
    void slotMouseMoveTimeOut();
    void slotClose();

private:

    int                 m_deskX;
    int                 m_deskY;
    int                 m_deskWidth;
    int                 m_deskHeight;

    QStringList         m_commentsList;

    ImageLoadThread*    m_imageLoadThread;
    QTimer*             m_mouseMoveTimer;
    QTimer*             m_timer;
    ScreenProperties*   m_screen;
    bool                m_haveImages;

    Image*              m_image[2];
    KBEffect*           m_effect;
    int                 m_numKBEffectRepeated;
    bool                m_zoomIn, m_initialized;
    float               m_step;

    bool                m_endOfShow;
    bool                m_showingEnd;

    // settings from config file
    int                 m_delay;
    bool                m_disableFadeInOut;
    bool                m_disableCrossFade;
    unsigned            m_forceFrameRate;

    SharedContainer*    m_sharedData;

    PlaybackWidget*     m_playbackWidget;

    friend class KBEffect;
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif // SLIDESHOWKB_H
