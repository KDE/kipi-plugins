/* ============================================================
 * File  : slideshowgl.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-01-19
 * Description :
 *
 * Copyright 2004 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <klocale.h>
#include <kdebug.h>
#include <iostream>

#include <qtimer.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qevent.h>
#include <qcursor.h>
#include <qimage.h>
#include <qpainter.h>
#include <qtimer.h>

#include <math.h>
#include <cstdlib>

#include "slideshowgl.h"
#include "toolbar.h"

namespace KIPISlideShowPlugin
{

SlideShowGL::SlideShowGL(const QStringList& fileList,
                         int delay, bool loop,
                         const QString& effectName)
    : QGLWidget(0, 0, 0, WStyle_StaysOnTop | WType_Popup |
                WX11BypassWM | WDestructiveClose)
{
    move(0, 0);
    resize(QApplication::desktop()->size());

    deskWidth_  = QApplication::desktop()->size().width();
    deskHeight_ = QApplication::desktop()->size().height();

    toolBar_ = new ToolBar(this);
    toolBar_->hide();
    if (!loop)
    {
        toolBar_->setEnabledPrev(false);
    }
    connect(toolBar_, SIGNAL(signalPause()),
            SLOT(slotPause()));
    connect(toolBar_, SIGNAL(signalPlay()),
            SLOT(slotPlay()));
    connect(toolBar_, SIGNAL(signalNext()),
            SLOT(slotNext()));
    connect(toolBar_, SIGNAL(signalPrev()),
            SLOT(slotPrev()));
    connect(toolBar_, SIGNAL(signalClose()),
            SLOT(slotClose()));
    
    // -- Minimal texture size (opengl specs) --------------
    
    width_  = 64;
    height_ = 64;

    // --------------------------------------------------

    fileList_   = fileList;
    delay_      = QMAX(delay, 1000);   // at least have 1 second delay
    loop_       = loop;
    effectName_ = effectName;

    // ------------------------------------------------------------------

    fileIndex_  = 0;

    texture_[0] = 0;
    texture_[1] = 0;
    curr_       = 0;
    tex1First_  = true;
    timeout_    = delay_;
    effectRunning_ = false;
    endOfShow_     = false;

    // --------------------------------------------------

    registerEffects();

    if (effectName_ == "Random") {
        effect_ = getRandomEffect();
        random_ = true;
    }
    else {
        effect_ = Effects[effectName_];
        if (!effect_)
            effect_ = Effects["None"];
        random_ = false;
    }

    // --------------------------------------------------

    timer_ = new QTimer();
    connect(timer_, SIGNAL(timeout()),
            SLOT(slotTimeOut()));
    timer_->start(timeout_, true);

    // -- hide cursor when not moved --------------------

    mouseMoveTimer_ = new QTimer;
    connect(mouseMoveTimer_, SIGNAL(timeout()),
            SLOT(slotMouseMoveTimeOut()));
    
    setMouseTracking(true);
    slotMouseMoveTimeOut();
}

SlideShowGL::~SlideShowGL()
{
    delete timer_;
    delete mouseMoveTimer_;
  
    if (texture_[0])
        glDeleteTextures(1, &texture_[0]);
    if (texture_[1])
        glDeleteTextures(1, &texture_[1]);
}

void SlideShowGL::initializeGL()
{
    // Enable Texture Mapping
    glEnable(GL_TEXTURE_2D);
    // Clear The Background Color
    glClearColor(0.0, 0.0, 0.0, 1.0f);

    // Turn Blending On
    glEnable(GL_BLEND);
    // Blending Function For Translucency Based On Source Alpha Value
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Enable perspective vision
    glClearDepth(1.0f);

    // get the maximum texture value.
    GLint maxTexVal;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexVal);

    // allow only maximum texture value of 1024. anything bigger and things slow down
    maxTexVal = QMIN(1024, maxTexVal);

    width_  = QApplication::desktop()->width();
    height_ = QApplication::desktop()->height();

    width_  = 1 << (int)ceil(log((float)width_)/log((float)2)) ;
    height_ = 1 << (int)ceil(log((float)height_)/log((float)2));
    
    width_  = QMIN( maxTexVal, width_ );
    height_ = QMIN( maxTexVal, height_ );

    // load the first image

    loadImage();
}

void SlideShowGL::paintGL()
{
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (endOfShow_)
        showEndOfShow();
    else {
        if (effectRunning_ && effect_)
            (this->*effect_)();
        else
            paintTexture();
    }
}

void SlideShowGL::resizeGL(int w, int h)
{
    // Reset The Current Viewport And Perspective Transformation
    glViewport(0, 0, (GLint)w, (GLint)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void SlideShowGL::keyPressEvent(QKeyEvent *event)
{
    if(!event)
        return;

    toolBar_->keyPressEvent(event);
}

void SlideShowGL::mousePressEvent(QMouseEvent *)
{
    if (endOfShow_)
        close();
}

void SlideShowGL::mouseMoveEvent(QMouseEvent *e)
{
    setCursor(QCursor(Qt::ArrowCursor));
    mouseMoveTimer_->start(1000, true);

    if (!toolBar_->canHide())
        return;
    
    QPoint pos(e->pos());
    
    if (pos.y() > 20 && pos.y() < (deskHeight_-20-1))
    {
        if (toolBar_->isHidden())
            return;
        else
            toolBar_->hide();
        return;
    }

    int w = toolBar_->width();
    int h = toolBar_->height();
    
    if (pos.y() < 20)
    {
        if (pos.x() <= deskWidth_/2)
            // position top left
            toolBar_->move(0,0);
        else
            // position top left
            toolBar_->move(deskWidth_-w-1,0);
    }
    else
    {
        if (pos.x() <= deskWidth_/2)
            // position bot left
            toolBar_->move(0,deskHeight_-h-1);
        else
            // position bot right
            toolBar_->move(deskWidth_-w-1,deskHeight_-h-1);
    }
    toolBar_->show();
}

void SlideShowGL::registerEffects()
{
    Effects.insert("None", &SlideShowGL::effectNone);
    Effects.insert("Blend", &SlideShowGL::effectBlend);
    Effects.insert("Fade", &SlideShowGL::effectFade);
    Effects.insert("Rotate", &SlideShowGL::effectRotate);
    Effects.insert("Bend", &SlideShowGL::effectBend);
    Effects.insert("In Out", &SlideShowGL::effectInOut);
    Effects.insert("Slide", &SlideShowGL::effectSlide);
    Effects.insert("Flutter", &SlideShowGL::effectFlutter);
    Effects.insert("Cube", &SlideShowGL::effectCube);
}

QStringList SlideShowGL::effectNames()
{
    QStringList effects;

    effects.append("None");
    effects.append("Bend");
    effects.append("Blend");
    effects.append("Cube");
    effects.append("Fade");
    effects.append("Flutter");
    effects.append("In Out");
    effects.append("Rotate");
    effects.append("Slide");
    effects.append("Random");

    return effects;
}

QMap<QString,QString> SlideShowGL::effectNamesI18N()
{
    QMap<QString,QString> effects;

    effects["None"]    = i18n("None");
    effects["Bend"]    = i18n("Bend");
    effects["Blend"]   = i18n("Blend");
    effects["Cube"]    = i18n("Cube");
    effects["Fade"]    = i18n("Fade");
    effects["Flutter"] = i18n("Flutter");
    effects["In Out"]  = i18n("In Out");
    effects["Rotate"]  = i18n("Rotate");
    effects["Slide"]   = i18n("Slide");
    effects["Random"]  = i18n("Random");

    return effects;
}

SlideShowGL::EffectMethod SlideShowGL::getRandomEffect()
{
    QMap<QString,EffectMethod>  tmpMap(Effects);

    tmpMap.remove("None");
    QStringList t = tmpMap.keys();

    int count = t.count();

    int i = (int)((float)(count)*rand()/(RAND_MAX+1.0));
    QString key = t[i];

    return tmpMap[key];
}

void SlideShowGL::advanceFrame()
{
    fileIndex_++;
    int num = fileList_.count();
    if (fileIndex_ >= num) {
        if (loop_)
        {
            fileIndex_ = 0;
        }
        else
        {
            fileIndex_ = num-1;
            endOfShow_ = true;
            toolBar_->setEnabledPlay(false);
            toolBar_->setEnabledNext(false);
            toolBar_->setEnabledPrev(false);
        }
    }

    if (!loop_ && !endOfShow_)
    {
        toolBar_->setEnabledPrev(fileIndex_ > 0);
        toolBar_->setEnabledNext(fileIndex_ < num-1);
    }

    tex1First_ = !tex1First_;
    curr_      = (curr_ == 0) ? 1 : 0;
}

void SlideShowGL::previousFrame()
{
    fileIndex_--;
    int num = fileList_.count();
    if (fileIndex_ < 0) {
        if (loop_)
        {
            fileIndex_ = num-1;
        }
        else
        {
            fileIndex_ = 0;
            endOfShow_ = true;
            toolBar_->setEnabledPlay(false);
            toolBar_->setEnabledNext(false);
            toolBar_->setEnabledPrev(false);
        }
    }

    if (!loop_ && !endOfShow_)
    {
        toolBar_->setEnabledPrev(fileIndex_ > 0);
        toolBar_->setEnabledNext(fileIndex_ < num-1);
    }

    tex1First_ = !tex1First_;
    curr_      = (curr_ == 0) ? 1 : 0;
}

void SlideShowGL::loadImage()
{
    QString path(fileList_[fileIndex_]);
    QImage image(path);

    if (!image.isNull()) {

        int a  = tex1First_ ? 0 : 1;
        GLuint& tex = texture_[a];

        if (tex)
            glDeleteTextures(1, &tex);

        QImage black(width(), height(), 32);
        black.fill(Qt::black.rgb());

        image = image.smoothScale(width(), height(),
                                  QImage::ScaleMin);
        montage(image, black);

        black = black.smoothScale(width_, height_);
        QImage t = convertToGLFormat(black);

        /* create the texture */
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        /* actually generate the texture */
        glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
        /* enable linear filtering  */
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    }
}

void SlideShowGL::montage(QImage& top, QImage& bot)
{
    int tw = top.width(); int th = top.height();
    int bw = bot.width(); int bh = bot.height();

    if (tw > bw || th > bh)
        qFatal("Top Image should be smaller or same size as Bottom Image");

    if (top.depth() != 32) top = top.convertDepth(32);
    if (bot.depth() != 32) bot = bot.convertDepth(32);

    int sw = bw/2 - tw/2; //int ew = bw/2 + tw/2;
    int sh = bh/2 - th/2; int eh = bh/2 + th/2;


    unsigned int *tdata = (unsigned int*) top.scanLine(0);
    unsigned int *bdata = 0;

    for (int y = sh; y < eh; y++) {

        bdata = (unsigned int*) bot.scanLine(y) + sw;
        for (int x = 0; x < tw; x++) {
            *(bdata++) = *(tdata++);
        }

    }
}

void SlideShowGL::showEndOfShow()
{
    QPixmap pix(512,512);
    pix.fill(Qt::black);

    QFont fn(font());
    fn.setPointSize(fn.pointSize()+10);
    fn.setBold(true);

    QPainter p(&pix);
    p.setPen(Qt::white);
    p.setFont(fn);
    p.drawText(20, 50, i18n("SlideShow Completed."));
    p.drawText(20, 100, i18n("Click To Exit..."));
    p.end();

    QImage image(pix.convertToImage());
    QImage t = convertToGLFormat(image);

    GLuint tex;

    /* create the texture */
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    /* actually generate the texture */
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
    /* enable linear filtering  */
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

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

}

void SlideShowGL::slotTimeOut()
{
    if (!effect_) {
        kdWarning( 51000 ) << "SlideShowGL: No transition method"
                    << endl;
        effect_ = &SlideShowGL::effectNone;
    }

    if (effectRunning_) {
        timeout_ = 10;
    }
    else {
        if (timeout_ == -1) {
            // effect was running and is complete now
            // run timer while showing current image
            timeout_ = delay_;
            m_i     = 0;
        }
        else {

            // timed out after showing current image
            // load next image and start effect
            if (random_)
                effect_ = getRandomEffect();

            advanceFrame();
            if (endOfShow_) {
                updateGL();
                return;
            }

            loadImage();

            timeout_ = 10;
            effectRunning_ = true;
            m_i = 0;

        }
    }

    updateGL();
    timer_->start(timeout_, true);
}

void SlideShowGL::slotMouseMoveTimeOut()
{
    QPoint pos(QCursor::pos());
    if (pos.y() < 20 || pos.y() > ( deskHeight_-20-1))
        return;
    
    setCursor(QCursor(Qt::BlankCursor));
}

void SlideShowGL::paintTexture()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLuint& tex = texture_[curr_];

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
}

void SlideShowGL::effectNone()
{
    paintTexture();
    effectRunning_ = false;
    timeout_ = -1;
    return;
}

void SlideShowGL::effectBlend()
{
    if (m_i > 100) {
        paintTexture();
        effectRunning_ = false;
        timeout_ = -1;
        return;
    }

    int a = (curr_ == 0) ? 1 : 0;
    int b =  curr_;

    GLuint& ta = texture_[a];
    GLuint& tb = texture_[b];

    glBindTexture(GL_TEXTURE_2D, ta);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tb);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0/(100.0)*(float)m_i);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();

    m_i++;
}

void SlideShowGL::effectFade()
{
    if (m_i > 100) {
        paintTexture();
        effectRunning_ = false;
        timeout_ = -1;
        return;
    }

    int a;
    float opacity;
    if (m_i <= 50) {
        a =  (curr_ == 0) ? 1 : 0;
        opacity = 1.0 - 1.0/50.0*(float)(m_i);
    }
    else {
        opacity = 1.0/50.0*(float)(m_i-50.0);
        a = curr_;
    }

    GLuint& ta = texture_[a];

    glBindTexture(GL_TEXTURE_2D, ta);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, opacity);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();


    m_i++;
}

void SlideShowGL::effectRotate()
{
    if (m_i > 100) {
        paintTexture();
        effectRunning_ = false;
        timeout_ = -1;
        return;
    }

    if (m_i == 0)
        m_dir = (int)((2.0*rand()/(RAND_MAX+1.0)));

    int a = (curr_ == 0) ? 1 : 0;
    int b =  curr_;

    GLuint& ta = texture_[a];
    GLuint& tb = texture_[b];

    glBindTexture(GL_TEXTURE_2D, tb);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float rotate = 360.0/100.0*(float)m_i;
    glRotatef( ((m_dir == 0) ? -1 : 1) * rotate,
               0.0, 0.0, 1.0);
    float scale = 1.0/100.0*(100.0-(float)(m_i));
    glScalef(scale,scale,1.0);

    glBindTexture(GL_TEXTURE_2D, ta);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();

    m_i++;
}

void SlideShowGL::effectBend()
{
    if (m_i > 100) {
        paintTexture();
        effectRunning_ = false;
        timeout_ = -1;
        return;
    }

    if (m_i == 0)
        m_dir = (int)((2.0*rand()/(RAND_MAX+1.0)));

    int a = (curr_ == 0) ? 1 : 0;
    int b =  curr_;

    GLuint& ta = texture_[a];
    GLuint& tb = texture_[b];

    glBindTexture(GL_TEXTURE_2D, tb);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90.0/100.0*(float)m_i,
              (m_dir == 0) ? 1.0 : 0.0,
              (m_dir == 1) ? 1.0 : 0.0,
              0.0);

    glBindTexture(GL_TEXTURE_2D, ta);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();

    m_i++;
}

void SlideShowGL::effectInOut()
{
    if (m_i > 100) {
        paintTexture();
        effectRunning_ = false;
        timeout_ = -1;
        return;
    }

    if (m_i == 0) {
        m_dir = 1 + (int)((4.0*rand()/(RAND_MAX+1.0)));
    }

    int a;
    bool out;
    if (m_i <= 50) {
        a   = (curr_ == 0) ? 1 : 0;
        out = 1;
    }
    else {
        a   = curr_;
        out = 0;
    }

    GLuint& ta = texture_[a];

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float t = out ? 1.0/50.0*(50.0-m_i) : 1.0/50.0*(m_i-50.0);
    glScalef(t, t, 1.0);
    t = 1.0 - t;
    glTranslatef((m_dir % 2 == 0) ? ((m_dir == 2)? 1 : -1) * t : 0.0,
                 (m_dir % 2 == 1) ? ((m_dir == 1)? 1 : -1) * t : 0.0,
                 0.0);

    glBindTexture(GL_TEXTURE_2D, ta);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);

        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();


    m_i++;
}

void SlideShowGL::effectSlide()
{
    if (m_i > 100) {
        paintTexture();
        effectRunning_ = false;
        timeout_ = -1;
        return;
    }

    if (m_i == 0)
        m_dir = 1 + (int)((4.0*rand()/(RAND_MAX+1.0)));

    int a = (curr_ == 0) ? 1 : 0;
    int b =  curr_;

    GLuint& ta = texture_[a];
    GLuint& tb = texture_[b];

    glBindTexture(GL_TEXTURE_2D, tb);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float trans = 2.0/100.0*(float)m_i;
    glTranslatef((m_dir % 2 == 0) ? ((m_dir == 2)? 1 : -1) * trans : 0.0,
                 (m_dir % 2 == 1) ? ((m_dir == 1)? 1 : -1) * trans : 0.0,
                 0.0);

    glBindTexture(GL_TEXTURE_2D, ta);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);

        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();

    m_i++;
}

void SlideShowGL::effectFlutter()
{
    if (m_i > 100) {
        paintTexture();
        effectRunning_ = false;
        timeout_ = -1;
        return;
    }

    int a = (curr_ == 0) ? 1 : 0;
    int b =  curr_;

    GLuint& ta = texture_[a];
    GLuint& tb = texture_[b];

    if (m_i == 0) {
        for (int x = 0; x<40; x++) {
            for (int y = 0; y < 40; y++) {
                m_points[x][y][0] = (float) (x / 20.0f - 1.0f);
                m_points[x][y][1] = (float) (y / 20.0f - 1.0f);
                m_points[x][y][2] = (float) sin((x / 20.0f - 1.0f) *
                                                3.141592654*2.0f)/5.0;
            }
        }
    }


    glBindTexture(GL_TEXTURE_2D, tb);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0f, -1.0f, 0);

        glTexCoord2f(1, 0);
        glVertex3f(1.0f, -1.0f, 0);

        glTexCoord2f(1, 1);
        glVertex3f(1.0f, 1.0f, 0);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0f, 1.0f, 0);
    }
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float rotate = 60.0/100.0*(float)m_i;
    glRotatef(rotate, 1.0f, 0.0f, 0.0f);
    float scale = 1.0/100.0*(100.0-(float)m_i);
    glScalef(scale, scale, scale);
    glTranslatef(1.0/100.0*(float)m_i, 1.0/100.0*(float)m_i, 0.0);

    glBindTexture(GL_TEXTURE_2D, ta);
    glBegin(GL_QUADS);
    {
        glColor4f(1.0, 1.0, 1.0, 1.0);

        float float_x, float_y, float_xb, float_yb;
        int x, y;

        for (x = 0; x < 39; x++)
        {
            for (y = 0; y < 39; y++)
            {
                float_x = (float) x / 40.0f;
                float_y = (float) y / 40.0f;
                float_xb = (float) (x + 1) / 40.0f;
                float_yb = (float) (y + 1) / 40.0f;
                glTexCoord2f(float_x, float_y);
                glVertex3f(m_points[x][y][0], m_points[x][y][1], m_points[x][y][2]);
                glTexCoord2f(float_x, float_yb);
                glVertex3f(m_points[x][y + 1][0], m_points[x][y + 1][1],
                           m_points[x][y + 1][2]);
                glTexCoord2f(float_xb, float_yb);
                glVertex3f(m_points[x + 1][y + 1][0], m_points[x + 1][y + 1][1],
                           m_points[x + 1][y + 1][2]);
                glTexCoord2f(float_xb, float_y);
                glVertex3f(m_points[x + 1][y][0], m_points[x + 1][y][1],
                           m_points[x + 1][y][2]);
            }
        }
    }
    glEnd();

    // wave every two iterations
    if (m_i%2 == 0) {

        float hold;
        int x, y;
        for (y = 0; y < 40; y++)
        {
            hold = m_points[0][y][2];
            for (x = 0; x < 39; x++)
            {
                m_points[x][y][2] = m_points[x + 1][y][2];
            }
            m_points[39][y][2] = hold;
        }
    }
    m_i++;
}

void SlideShowGL::effectCube()
{
    int tot = 200;
    int rotStart = 50;

    if (m_i > tot) {
        paintTexture();
        effectRunning_ = false;
        timeout_ = -1;
        return;
    }

    // Enable perspective vision
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    int a = (curr_ == 0) ? 1 : 0;
    int b =  curr_;

    GLuint& ta = texture_[a];
    GLuint& tb = texture_[b];

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float PI = 4.0 * atan(1.0);
    float znear = 3.0;
    float theta = 2.0 * atan2((float)2.0/(float)2.0, (float)znear);
    theta = theta * 180.0/PI;

    glFrustum(-1.0,1.0,-1.0,1.0, znear-0.01,10.0);


    static float xrot;
    static float yrot;
    static float zrot;

    if (m_i == 0) {
        xrot = 0.0;
        yrot = 0.0;
        zrot = 0.0;
    }

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    float trans = 5.0 * (float)((m_i <= tot/2) ? m_i : tot-m_i)/(float)tot;
    glTranslatef(0.0,0.0, -znear - 1.0 - trans);

    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);


    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_QUADS);
    {
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

        /* Front Face */
        glVertex3f( -1.00f, -1.00f,  0.99f );
        glVertex3f(  1.00f, -1.00f,  0.99f );
        glVertex3f(  1.00f,  1.00f,  0.99f );
        glVertex3f( -1.00f,  1.00f,  0.99f );

        /* Back Face */
        glVertex3f( -1.00f, -1.00f, -0.99f );
        glVertex3f( -1.00f,  1.00f, -0.99f );
        glVertex3f(  1.00f,  1.00f, -0.99f );
        glVertex3f(  1.00f, -1.00f, -0.99f );

        /* Top Face */
        glVertex3f( -1.00f,  0.99f, -1.00f );
        glVertex3f( -1.00f,  0.99f,  1.00f );
        glVertex3f(  1.00f,  0.99f,  1.00f );
        glVertex3f(  1.00f,  0.99f, -1.00f );

        /* Bottom Face */
        glVertex3f( -1.00f, -0.99f, -1.00f );
        glVertex3f(  1.00f, -0.99f, -1.00f );
        glVertex3f(  1.00f, -0.99f,  1.00f );
        glVertex3f( -1.00f, -0.99f,  1.00f );

        /* Right face */
        glVertex3f( 0.99f, -1.00f, -1.00f );
        glVertex3f( 0.99f,  1.00f, -1.00f );
        glVertex3f( 0.99f,  1.00f,  1.00f );
        glVertex3f( 0.99f, -1.00f,  1.00f );

        /* Left Face */
        glVertex3f( -0.99f, -1.00f, -1.00f );
        glVertex3f( -0.99f, -1.00f,  1.00f );
        glVertex3f( -0.99f,  1.00f,  1.00f );
        glVertex3f( -0.99f,  1.00f, -1.00f );

    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, ta);
    glBegin(GL_QUADS);
    {
        glColor4d(1.0, 1.0, 1.0, 1.0);

        // Front Face
        glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f, -1.0f,  1.00f );
        glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  1.0f, -1.0f,  1.00f );
        glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  1.0f,  1.0f,  1.00f );
        glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f,  1.0f,  1.00f );


        // Top Face
        glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -1.0f,  1.00f, -1.0f );
        glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -1.0f,  1.00f,  1.0f );
        glTexCoord2f( 0.0f, 0.0f ); glVertex3f(  1.0f,  1.00f,  1.0f );
        glTexCoord2f( 0.0f, 1.0f ); glVertex3f(  1.0f,  1.00f, -1.0f );

        // Bottom Face
        glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f, -1.00f, -1.0f );
        glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  1.0f, -1.00f, -1.0f );
        glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  1.0f, -1.00f,  1.0f );
        glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f, -1.00f,  1.0f );

        // Right face
        glTexCoord2f( 0.0f, 0.0f ); glVertex3f( 1.00f, -1.0f, -1.0f );
        glTexCoord2f( 0.0f, 1.0f ); glVertex3f( 1.00f, -1.0f,  1.0f );
        glTexCoord2f( 1.0f, 1.0f ); glVertex3f( 1.00f,  1.0f,  1.0f );
        glTexCoord2f( 1.0f, 0.0f ); glVertex3f( 1.00f,  1.0f, -1.0f );

        // Left Face
        glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -1.00f, -1.0f, -1.0f );
        glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.00f,  1.0f, -1.0f );
        glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.00f,  1.0f,  1.0f );
        glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -1.00f, -1.0f,  1.0f );

    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tb);
    glBegin(GL_QUADS);
    {
        glColor4d(1.0, 1.0, 1.0, 1.0);

        // Back Face
        glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -1.0f, -1.0f, -1.00f );
        glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -1.0f,  1.0f, -1.00f );
        glTexCoord2f( 0.0f, 1.0f ); glVertex3f(  1.0f,  1.0f, -1.00f );
        glTexCoord2f( 0.0f, 0.0f ); glVertex3f(  1.0f, -1.0f, -1.00f );
    }
    glEnd();

    if (m_i >= rotStart && m_i < (tot-rotStart)) {
        xrot += 360.0f/(float)(tot-2*rotStart);
        yrot += 180.0f/(float)(tot-2*rotStart);
    }

    m_i++;
}

void SlideShowGL::slotPause()
{
    timer_->stop();

    if (toolBar_->isHidden())
    {
        int w = toolBar_->width();
        toolBar_->move(deskWidth_-w-1,0);
        toolBar_->show();
    }
}

void SlideShowGL::slotPlay()
{
    toolBar_->hide();
    slotTimeOut();
}

void SlideShowGL::slotPrev()
{
    previousFrame();
    if (endOfShow_) {
        updateGL();
        return;
    }

    effectRunning_ = false;
    loadImage();
    updateGL();
}

void SlideShowGL::slotNext()
{
    advanceFrame();
    if (endOfShow_) {
        updateGL();
        return;
    }

    effectRunning_ = false;
    loadImage();
    updateGL();
}

void SlideShowGL::slotClose()
{
    close();    
}

}  // NameSpace KIPISlideShowPlugin

#include "slideshowgl.moc"
