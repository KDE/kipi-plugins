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

#include "slideshowkb.moc"

// C++ includes

#include <cassert>
#include <cmath>

// Qt includes

#include <QList>
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QFont>
#include <QCursor>
#include <QPixmap>
#include <QMouseEvent>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kglobalsettings.h>

// Local includes

#include "commoncontainer.h"
#include "imageloadthread.h"
#include "kbeffect.h"
#include "playbackwidget.h"
#include "screenproperties.h"

namespace KIPIAdvancedSlideshowPlugin
{

ViewTrans::ViewTrans(bool zoomIn, float relAspect)
{
    m_deltaX     = 0.0;
    m_deltaY     = 0.0;
    m_deltaScale = 0.0;
    m_baseScale  = 0.0;
    m_baseX      = 0.0;
    m_baseY      = 0.0;
    m_xScale     = 0.0;
    m_yScale     = 0.0;
    int i        = 0;

    // randomly select sizes of start end end viewport
    double s[2];

    do
    {
        s[0]  = 0.3 * rnd() + 1.0;
        s[1]  = 0.3 * rnd() + 1.0;
    }
    while ((fabs(s[0] - s[1]) < 0.15) && (++i < 10));

    if ((!zoomIn ||  (s[0] > s[1])) ||
        ( zoomIn || !(s[0] > s[1])))
    {
        double tmp = s[0];
        s[0]       = s[1];
        s[1]       = tmp;
    }

    m_deltaScale = s[1] / s[0] - 1.0;
    m_baseScale  = s[0];

    // additional scale factors to ensure proper m_aspect of the displayed image
    double x[2], y[2], xMargin[2], yMargin[2], bestDist;
    double sx, sy;

    if (relAspect > 1.0)
    {
        sx = 1.0;
        sy = relAspect;
    }
    else
    {
        sx = 1.0 / relAspect;
        sy = 1.0;
    }

    m_xScale   = sx;
    m_yScale   = sy;

    // calculate path
    xMargin[0] = (s[0] * sx - 1.0) / 2.0;
    yMargin[0] = (s[0] * sy - 1.0) / 2.0;
    xMargin[1] = (s[1] * sx - 1.0) / 2.0;
    yMargin[1] = (s[1] * sy - 1.0) / 2.0;

    i        = 0;
    bestDist = 0.0;

    do
    {
        double sign = rndSign();
        x[0]        = xMargin[0] * (0.2 * rnd() + 0.8) *  sign;
        y[0]        = yMargin[0] * (0.2 * rnd() + 0.8) * -sign;
        x[1]        = xMargin[1] * (0.2 * rnd() + 0.8) * -sign;
        y[1]        = yMargin[1] * (0.2 * rnd() + 0.8) *  sign;

        if (fabs(x[1] - x[0]) + fabs(y[1] - y[0]) > bestDist)
        {
            m_baseX  = x[0];
            m_baseY  = y[0];
            m_deltaX = x[1] - x[0];
            m_deltaY = y[1] - y[0];
            bestDist = fabs(m_deltaX) + fabs(m_deltaY);
        }

    }
    while ((bestDist < 0.3) && (++i < 10));
}

// -------------------------------------------------------------------------

Image::Image(ViewTrans* const viewTrans, float aspect)
{
    this->m_viewTrans = viewTrans;
    this->m_aspect    = aspect;
    this->m_pos       = 0.0;
    this->m_opacity   = 0.0;
    this->m_paint     = (m_viewTrans) ? true : false;
    this->m_texture   = 0;
}

Image::~Image()
{
    delete m_viewTrans;

    if (glIsTexture(m_texture))
        glDeleteTextures(1, &m_texture);
}

// -------------------------------------------------------------------------

SlideShowKB::SlideShowKB(const QList<QPair<QString, int> >& fileList,
                         const QStringList& commentsList, SharedContainer* const sharedData)
           : QGLWidget()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::Popup);

    QRect deskRect = KGlobalSettings::desktopGeometry( kapp->activeWindow() );
    m_deskX        = deskRect.x();
    m_deskY        = deskRect.y();
    m_deskWidth    = deskRect.width();
    m_deskHeight   = deskRect.height();

    move(m_deskX, m_deskY);
    resize(m_deskWidth, m_deskHeight);

    m_sharedData   = sharedData;

    // Avoid boring compile time "unused parameter" warning :P
    // These parameters could be useful for future implementations
    m_commentsList = commentsList;

    srand(QTime::currentTime().msec());
    readSettings();

    m_screen = new ScreenProperties(this);
    m_screen->enableVSync();

    unsigned frameRate;

    if (m_forceFrameRate == 0)
        frameRate = m_screen->suggestFrameRate() * 2;
    else
        frameRate = m_forceFrameRate;

    m_image[0]    = new Image(0);
    m_image[1]    = new Image(0);

    m_effect      = 0;
    m_step        = 1.0 / ((float) (m_delay * frameRate));
    m_zoomIn      = qrand() < RAND_MAX / 2;
    m_initialized = false;
    m_haveImages  = true;

    QList<QPair<QString, int> > m_fileList = fileList;

    m_imageLoadThread = new ImageLoadThread(m_fileList, width(), height(), m_sharedData->loop);
    m_timer           = new QTimer;
    m_endOfShow       = false;
    m_showingEnd      = false;

    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(moveSlot()));

    connect(m_imageLoadThread, SIGNAL(signalEndOfShow()),
            this, SLOT(slotEndOfShow()));

    // -- hide cursor when not moved --------------------

    m_mouseMoveTimer = new QTimer;

    connect(m_mouseMoveTimer, SIGNAL(timeout()),
            this, SLOT(slotMouseMoveTimeOut()));

    setMouseTracking(true);

    slotMouseMoveTimeOut();

    // -- playback widget -------------------------------

    m_playbackWidget = new PlaybackWidget(this, m_sharedData->soundtrackUrls, m_sharedData);
    m_playbackWidget->hide();
    m_playbackWidget->move(m_deskX, m_deskY);

    // -- load image and let's start

    m_imageLoadThread->start();
    m_timer->start(1000 / frameRate);
}

SlideShowKB::~SlideShowKB()
{
    delete m_effect;
    delete m_image[0];
    delete m_image[1];

    m_imageLoadThread->quit();
    bool terminated = m_imageLoadThread->wait(10000);

    if (!terminated)
    {
        m_imageLoadThread->terminate();
        terminated = m_imageLoadThread->wait(3000);
    }

    delete m_imageLoadThread;
    delete m_screen;
    delete m_mouseMoveTimer;
    delete m_timer;
}

void SlideShowKB::setNewKBEffect()
{
    KBEffect::Type type;
    bool needFadeIn = ((m_effect == 0) || (m_effect->type() == KBEffect::Fade));

    // we currently only have two effects

    if (m_disableFadeInOut)
        type = KBEffect::Blend;
    else if (m_disableCrossFade)
        type = KBEffect::Fade;
    else
        type = KBEffect::chooseKBEffect((m_effect) ? m_effect->type() : KBEffect::Fade);

    delete m_effect;

    switch (type)
    {

        case KBEffect::Fade:
            m_effect = new FadeKBEffect(this, needFadeIn);
            break;

        case KBEffect::Blend:
            m_effect = new BlendKBEffect(this, needFadeIn);
            break;

        default:
            kDebug() << "Unknown transition effect, falling back to crossfade";
            m_effect = new BlendKBEffect(this, needFadeIn);
            break;
    }
}

void SlideShowKB::moveSlot()
{
    if (m_initialized)
    {
        if (m_effect->done())
        {
            setNewKBEffect();
            m_imageLoadThread->requestNewImage();
        }

        m_effect->advanceTime(m_step);
    }

    updateGL();
}

bool SlideShowKB::setupNewImage(int idx)
{
    assert(idx >= 0 && idx < 2);

    if ( !m_haveImages)
        return false;

    bool ok  = false;
    m_zoomIn = !m_zoomIn;

    if (m_imageLoadThread->grabImage())
    {
        delete m_image[idx];

        // we have the image lock and there is an image
        float imageAspect          = m_imageLoadThread->imageAspect();
        ViewTrans* const viewTrans = new ViewTrans(m_zoomIn, aspect() / imageAspect);
        m_image[idx]               = new Image(viewTrans, imageAspect);

        applyTexture(m_image[idx], m_imageLoadThread->image());
        ok = true;

    }
    else
    {
        m_haveImages = false;
    }

    // don't forget to release the lock on the copy of the image
    // owned by the image loader thread
    m_imageLoadThread->ungrabImage();

    return ok;
}

void SlideShowKB::startSlideShowOnce()
{
    // when the image loader thread is ready, it will already have loaded
    // the first image
    if (m_initialized == false && m_imageLoadThread->ready())
    {
        setupNewImage(0);                     // setup the first image and
        m_imageLoadThread->requestNewImage(); // load the next one in background
        setNewKBEffect();                     // set the initial effect

        m_initialized = true;
    }
}

void SlideShowKB::swapImages()
{
    Image* const tmp = m_image[0];
    m_image[0]       = m_image[1];
    m_image[1]       = tmp;
}

void SlideShowKB::initializeGL()
{
    // Enable Texture Mapping
    glEnable(GL_TEXTURE_2D);

    // Clear The Background Color
    glClearColor(0.0, 0.0, 0.0, 1.0f);

    glEnable (GL_TEXTURE_2D);
    glShadeModel (GL_SMOOTH);

    // Turn Blending On
    glEnable(GL_BLEND);
    // Blending Function For Translucency Based On Source Alpha Value
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable perspective vision
    glClearDepth(1.0f);
}

void SlideShowKB::paintGL()
{
    startSlideShowOnce();

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // only clear the color buffer, if none of the active images is fully opaque

    if (!((m_image[0]->m_paint && m_image[0]->m_opacity == 1.0) || (m_image[1]->m_paint && m_image[1]->m_opacity == 1.0)))
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (m_endOfShow && m_image[0]->m_paint && m_image[1]->m_paint)
    {
        endOfShow();
        m_timer->stop();
    }
    else
    {
        if (m_image[1]->m_paint)
            paintTexture(m_image[1]);

        if (m_image[0]->m_paint)
            paintTexture(m_image[0]);
    }

    glFlush();
}

void SlideShowKB::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLint) w, (GLint) h);
}

void SlideShowKB::applyTexture(Image* const img, const QImage &texture)
{
    /* create the texture */
    glGenTextures(1, &img->m_texture);
    glBindTexture(GL_TEXTURE_2D, img->m_texture);

    /* actually generate the texture */
    glTexImage2D(GL_TEXTURE_2D, 0, 3, texture.width(), texture.height(), 0,GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());

    /* enable linear filtering  */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void SlideShowKB::paintTexture(Image* const img)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float sx = img->m_viewTrans->m_xScaleCorrect();
    float sy = img->m_viewTrans->m_yScaleCorrect();

    glTranslatef(img->m_viewTrans->transX(img->m_pos) * 2.0, img->m_viewTrans->transY(img->m_pos) * 2.0, 0.0);
    glScalef(img->m_viewTrans->scale(img->m_pos), img->m_viewTrans->scale(img->m_pos), 0.0);

    GLuint& tex = img->m_texture;

    glBindTexture(GL_TEXTURE_2D, tex);

    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, img->m_opacity);
        glTexCoord2f(0, 0);
        glVertex3f(-sx, -sy, 0);

        glTexCoord2f(1, 0);
        glVertex3f(sx, -sy, 0);

        glTexCoord2f(1, 1);
        glVertex3f(sx, sy, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-sx, sy, 0);
    }

    glEnd();
}

void SlideShowKB::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Advanced Slideshow Settings");

    m_delay            = group.readEntry("Delay", 8000) / 1000;
    m_disableFadeInOut = group.readEntry("KB Disable FadeInOut", false);
    m_disableCrossFade = group.readEntry("KB Disable Crossfade", false);
    m_forceFrameRate   = group.readEntry("KB Force Framerate", 0);

    if (m_delay < 5)
        m_delay = 5;

//       if (m_delay > 20) m_delay = 20;
    if (m_forceFrameRate > 120)
        m_forceFrameRate = 120;
}

void SlideShowKB::endOfShow()
{
    QPixmap pix(512, 512);
    pix.fill(Qt::black);

    QFont fn(font());
    fn.setPointSize(fn.pointSize() + 10);
    fn.setBold(true);

    QPainter p(&pix);
    p.setPen(Qt::white);
    p.setFont(fn);
    p.drawText(20, 50, i18n("SlideShow Completed"));
    p.drawText(20, 100, i18n("Click to Exit..."));
    p.end();

    QImage image = pix.toImage();
    QImage t     = convertToGLFormat(image);

    GLuint tex;

    /* create the texture */
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    /* actually generate the texture */
    glTexImage2D(GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits());

    /* enable linear filtering  */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /* paint the texture */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, tex);

    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0, -1.0, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0, -1.0, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0, 1.0, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0, 1.0, 0);
    }

    glEnd();

    m_showingEnd = true;
}

QStringList SlideShowKB::effectNames()
{
    QStringList effects;

    effects.append("Ken Burns");
    return effects;
}

QMap<QString, QString> SlideShowKB::effectNamesI18N()
{
    QMap<QString, QString> effects;

    effects["Ken Burns"] = i18n("Ken Burns");

    return effects;
}

void SlideShowKB::keyPressEvent(QKeyEvent* event)
{
    if (!event)
        return;

    m_playbackWidget->keyPressEvent(event);

    if (event->key() == Qt::Key_Escape)
        close();
}

void SlideShowKB::mousePressEvent(QMouseEvent* e)
{
    if ( !e )
        return;

    if (m_endOfShow && m_showingEnd)
        slotClose();
}

void SlideShowKB::mouseMoveEvent(QMouseEvent* e)
{
    setCursor(QCursor(Qt::ArrowCursor));
    m_mouseMoveTimer->start(1000);
    m_mouseMoveTimer->setSingleShot(true);

    if (!m_playbackWidget->canHide())
        return;

    QPoint pos(e->pos());

    if ((pos.y() > (m_deskY + 20)) && (pos.y() < (m_deskY + m_deskHeight - 20 - 1)))
    {
        if (m_playbackWidget->isHidden())
            return;
        else
            m_playbackWidget->hide();

        return;
    }

    m_playbackWidget->show();
}

void SlideShowKB::slotEndOfShow()
{
    m_endOfShow = true;
}

void SlideShowKB::slotMouseMoveTimeOut()
{
    QPoint pos(QCursor::pos());

    if ((pos.y() < (m_deskY + 20)) || (pos.y() > (m_deskY + m_deskHeight - 20 - 1)))
        return;

    setCursor(QCursor(Qt::BlankCursor));
}

void SlideShowKB::slotClose()
{
    close();
}

}  // namespace KIPIAdvancedSlideshowPlugin
