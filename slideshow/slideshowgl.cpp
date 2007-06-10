/* ============================================================
 * File  : slideshowgl.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-01-19
 * Description :
 *
 * Copyright 2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>

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

#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kglobalsettings.h>
#include <kimageeffect.h>

#include <qtimer.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qevent.h>
#include <qcursor.h>
#include <qimage.h>
#include <qpainter.h>
#include <qfileinfo.h>
#include <qfontmetrics.h>
#include <qwmatrix.h>

#include <math.h>
#include <cstdlib>

#include "slideshowgl.h"
#include "toolbar.h"

namespace KIPISlideShowPlugin
{

SlideShowGL::SlideShowGL(const QValueList<QPair<QString, int> >& fileList,
                         const QStringList& commentsList, bool ImagesHasComments)
    : QGLWidget(0, 0, 0, WStyle_StaysOnTop | WType_Popup |
                WX11BypassWM | WDestructiveClose)
{
#if KDE_IS_VERSION(3,2,0)
    QRect deskRect = KGlobalSettings::desktopGeometry(this);
    m_deskX      = deskRect.x();
    m_deskY      = deskRect.y();
    m_deskWidth  = deskRect.width();
    m_deskHeight = deskRect.height();
#else
    QRect deskRect = QApplication::desktop()->screenGeometry(this);
    m_deskX      = deskRect.x();
    m_deskY      = deskRect.y();
    m_deskWidth  = deskRect.width();
    m_deskHeight = deskRect.height();
#endif    
    
    move(m_deskX, m_deskY);
    resize(m_deskWidth, m_deskHeight);

    m_toolBar = new ToolBar(this);
    m_toolBar->hide();
    if (!m_loop)
    {
        m_toolBar->setEnabledPrev(false);
    }
    connect(m_toolBar, SIGNAL(signalPause()),
            SLOT(slotPause()));
    connect(m_toolBar, SIGNAL(signalPlay()),
            SLOT(slotPlay()));
    connect(m_toolBar, SIGNAL(signalNext()),
            SLOT(slotNext()));
    connect(m_toolBar, SIGNAL(signalPrev()),
            SLOT(slotPrev()));
    connect(m_toolBar, SIGNAL(signalClose()),
            SLOT(slotClose()));
    
    // -- Minimal texture size (opengl specs) --------------
    
    m_width  = 64;
    m_height = 64;

    // --------------------------------------------------

    m_fileList       = fileList;
    m_commentsList   = commentsList;

    m_imagesHasComments   = ImagesHasComments;

    m_config = new KConfig("kipirc");
    m_config->setGroup("SlideShow Settings");

    readSettings();

    // ------------------------------------------------------------------

    m_fileIndex  = 0;

    m_texture[0] = 0;
    m_texture[1] = 0;
    m_curr       = 0;
    m_tex1First  = true;
    m_timeout    = m_delay;
    m_effectRunning = false;
    m_endOfShow     = false;

    // --------------------------------------------------

    registerEffects();

    if (m_effectName == "Random") {
        m_effect = getRandomEffect();
        m_random = true;
    }
    else {
        m_effect = m_effects[m_effectName];
        if (!m_effect)
            m_effect = m_effects["None"];
        m_random = false;
    }

    // --------------------------------------------------

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()),
            SLOT(slotTimeOut()));
    m_timer->start(m_timeout, true);

    // -- hide cursor when not moved --------------------

    m_mouseMoveTimer = new QTimer;
    connect(m_mouseMoveTimer, SIGNAL(timeout()),
            SLOT(slotMouseMoveTimeOut()));
    
    setMouseTracking(true);
    slotMouseMoveTimeOut();
}

SlideShowGL::~SlideShowGL()
{
    delete m_timer;
    delete m_mouseMoveTimer;
  
    if (m_texture[0])
        glDeleteTextures(1, &m_texture[0]);
    if (m_texture[1])
        glDeleteTextures(1, &m_texture[1]);

    if (m_config) {
        delete m_config;
    }
}

void SlideShowGL::readSettings()
{
    m_delay                 = m_config->readNumEntry("Delay", 1500);
    m_printName         = m_config->readBoolEntry("Print Filename", true);
    m_printComments     = m_config->readBoolEntry("Print Comments", false);
    m_loop                  = m_config->readBoolEntry("Loop", false);
    
    m_effectName         = m_config->readEntry("Effect Name (OpenGL)", "Random");

    m_enableMouseWheel      = m_config->readBoolEntry("Enable Mouse Wheel", true);
 
    // Comments tab settings
    
    m_commentsFont = new QFont();
    m_commentsFont->setFamily(m_config->readEntry("Comments Font Family"));
    m_commentsFont->setPointSize(m_config->readNumEntry("Comments Font Size", 10 ));
    m_commentsFont->setBold(m_config->readBoolEntry("Comments Font Bold", false));
    m_commentsFont->setItalic(m_config->readBoolEntry("Comments Font Italic", false));
    m_commentsFont->setUnderline(m_config->readBoolEntry("Comments Font Underline", false));
    m_commentsFont->setOverline(m_config->readBoolEntry("Comments Font Overline", false));
    m_commentsFont->setStrikeOut(m_config->readBoolEntry("Comments Font StrikeOut", false));
    m_commentsFont->setFixedPitch(m_config->readBoolEntry("Comments Font FixedPitch", false));
    
    m_commentsFontColor     = m_config->readUnsignedNumEntry("Comments Font Color", 0xffffff);
    m_commentsBgColor       = m_config->readUnsignedNumEntry("Comments Bg Color", 0x000000);
    
    m_commentsLinesLength   = m_config->readNumEntry("Comments Lines Length", 72);
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

    m_width  = QApplication::desktop()->width();
    m_height = QApplication::desktop()->height();

    m_width  = 1 << (int)ceil(log((float)m_width)/log((float)2)) ;
    m_height = 1 << (int)ceil(log((float)m_height)/log((float)2));
    
    m_width  = QMIN( maxTexVal, m_width );
    m_height = QMIN( maxTexVal, m_height );

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

    if (m_endOfShow)
        showEndOfShow();
    else {
        if (m_effectRunning && m_effect)
            (this->*m_effect)();
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

    m_toolBar->keyPressEvent(event);
}

void SlideShowGL::mousePressEvent(QMouseEvent *e)
{
    if (m_endOfShow)
        slotClose();

    if (e->button() == Qt::LeftButton)
    {
        m_timer->stop();
        m_toolBar->setPaused(true);
        slotNext();
    }
    else if (e->button() == Qt::RightButton && m_fileIndex-1 >= 0)
    {
        m_timer->stop();
        m_toolBar->setPaused(true);
        slotPrev();
    }
}

void SlideShowGL::mouseMoveEvent(QMouseEvent *e)
{
    setCursor(QCursor(Qt::ArrowCursor));
    m_mouseMoveTimer->start(1000, true);

    if (!m_toolBar->canHide())
        return;
    
    QPoint pos(e->pos());
    
    if ((pos.y() > (m_deskY+20)) &&
        (pos.y() < (m_deskY+m_deskHeight-20-1)))
    {
        if (m_toolBar->isHidden())
            return;
        else
            m_toolBar->hide();
        return;
    }

    int w = m_toolBar->width();
    int h = m_toolBar->height();
    
    if (pos.y() < (m_deskY+20))
    {
        if (pos.x() <= (m_deskX+m_deskWidth/2))
            // position top left
            m_toolBar->move(m_deskX, m_deskY);
        else
            // position top right
            m_toolBar->move(m_deskX+m_deskWidth-w-1, m_deskY);
    }
    else
    {
        if (pos.x() <= (m_deskX+m_deskWidth/2))
            // position bot left
            m_toolBar->move(m_deskX, m_deskY+m_deskHeight-h-1);
        else
            // position bot right
            m_toolBar->move(m_deskX+m_deskWidth-w-1, m_deskY+m_deskHeight-h-1);
    }
    m_toolBar->show();
}

void SlideShowGL::wheelEvent(QWheelEvent *e)
{    
    if (!m_enableMouseWheel) return;
    
    if (m_endOfShow)
        slotClose();

    int delta = e->delta();
    
    if (delta < 0)
    {
        m_timer->stop();
        m_toolBar->setPaused(true);
        slotNext();
    }
    else if (delta > 0 && m_fileIndex-1 >= 0)
    {
        m_timer->stop();
        m_toolBar->setPaused(true);
        slotPrev();
    } 
}

void SlideShowGL::registerEffects()
{
    m_effects.insert("None", &SlideShowGL::effectNone);
    m_effects.insert("Blend", &SlideShowGL::effectBlend);
    m_effects.insert("Fade", &SlideShowGL::effectFade);
    m_effects.insert("Rotate", &SlideShowGL::effectRotate);
    m_effects.insert("Bend", &SlideShowGL::effectBend);
    m_effects.insert("In Out", &SlideShowGL::effectInOut);
    m_effects.insert("Slide", &SlideShowGL::effectSlide);
    m_effects.insert("Flutter", &SlideShowGL::effectFlutter);
    m_effects.insert("Cube", &SlideShowGL::effectCube);
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
    QMap<QString,EffectMethod>  tmpMap(m_effects);

    tmpMap.remove("None");
    QStringList t = tmpMap.keys();

    int count = t.count();

    int i = (int)((float)(count)*rand()/(RAND_MAX+1.0));
    QString key = t[i];

    return tmpMap[key];
}

void SlideShowGL::advanceFrame()
{
    m_fileIndex++;
    int num = m_fileList.count();
    if (m_fileIndex >= num) {
        if (m_loop)
        {
            m_fileIndex = 0;
        }
        else
        {
            m_fileIndex = num-1;
            m_endOfShow = true;
            m_toolBar->setEnabledPlay(false);
            m_toolBar->setEnabledNext(false);
            m_toolBar->setEnabledPrev(false);
        }
    }

    if (!m_loop && !m_endOfShow)
    {
        m_toolBar->setEnabledPrev(m_fileIndex > 0);
        m_toolBar->setEnabledNext(m_fileIndex < num-1);
    }

    m_tex1First = !m_tex1First;
    m_curr      = (m_curr == 0) ? 1 : 0;
}

void SlideShowGL::previousFrame()
{
    m_fileIndex--;
    int num = m_fileList.count();
    if (m_fileIndex < 0) {
        if (m_loop)
        {
            m_fileIndex = num-1;
        }
        else
        {
            m_fileIndex = 0;
            m_endOfShow = true;
            m_toolBar->setEnabledPlay(false);
            m_toolBar->setEnabledNext(false);
            m_toolBar->setEnabledPrev(false);
        }
    }

    if (!m_loop && !m_endOfShow)
    {
        m_toolBar->setEnabledPrev(m_fileIndex > 0);
        m_toolBar->setEnabledNext(m_fileIndex < num-1);
    }

    m_tex1First = !m_tex1First;
    m_curr      = (m_curr == 0) ? 1 : 0;
}

void SlideShowGL::loadImage()
{
    QPair<QString, int> fileAngle = m_fileList[m_fileIndex];
    QString path(fileAngle.first);
    int     angle(fileAngle.second);
    QImage image(path);
    if (angle != 0)
    {
        QWMatrix wm;
        wm.rotate(angle);
        image = image.xForm(wm);
    }

    if (!image.isNull()) {

        int a  = m_tex1First ? 0 : 1;
        GLuint& tex = m_texture[a];

        if (tex)
            glDeleteTextures(1, &tex);

        QImage black(width(), height(), 32);
        black.fill(Qt::black.rgb());
	    
        image = image.smoothScale(width(), height(),
                                  QImage::ScaleMin);
        montage(image, black);

        black = black.smoothScale(m_width, m_height);
        
	if (m_printName)
	    printFilename(black);
	
        if (m_printComments && m_imagesHasComments)
            printComments(black);
        
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

void SlideShowGL::printFilename(QImage& layer)
{ 
    QFileInfo fileinfo(m_fileList[m_fileIndex].first);
    QString filename = fileinfo.fileName();
    filename += " (";
    filename += QString::number(m_fileIndex + 1);
    filename += "/";
    filename += QString::number(m_fileList.count());
    filename += ")";    
       
    QFont fn(font());
    fn.setPointSize(fn.pointSize());
    fn.setBold(true);
   
    QFontMetrics fm(fn);
    QRect rect=fm.boundingRect(filename);
    rect.addCoords( 0, 0, 2, 2 );
    
    QPixmap pix(rect.width(),rect.height());
    pix.fill(Qt::black);    

    QPainter p(&pix);
    p.setPen(Qt::white);
    p.setFont(fn);
    p.drawText(1,fn.pointSize()+1 , filename);
    p.end();
   
    QImage textimage(pix.convertToImage());
    KImageEffect::blendOnLower(0,m_height-rect.height(),textimage,layer);
}

void SlideShowGL::printComments(QImage& layer)
{ 
    QString comments = m_commentsList[m_fileIndex];

    int yPos = 5; // Text Y coordinate
    if (m_printName) yPos += 20;

    QStringList commentsByLines;

    uint commentsIndex = 0; // Comments QString index

    while (commentsIndex < comments.length())
    {
        QString newLine; 
        bool breakLine = FALSE; // End Of Line found
        uint currIndex; //  Comments QString current index

        // Check miminal lines dimension

        int commentsLinesLengthLocal = m_commentsLinesLength;

        for ( currIndex = commentsIndex; currIndex < comments.length() && !breakLine; currIndex++ )
            if( comments[currIndex] == QChar('\n') || comments[currIndex].isSpace() ) breakLine = TRUE;

        if (commentsLinesLengthLocal <= (int)((currIndex - commentsIndex)))
            commentsLinesLengthLocal = (currIndex - commentsIndex);

        breakLine = FALSE;

        for ( currIndex = commentsIndex; currIndex <= commentsIndex + commentsLinesLengthLocal && 
              currIndex < comments.length() &&  
                      !breakLine; currIndex++ )
        {
            breakLine = (comments[currIndex] == QChar('\n')) ? TRUE : FALSE;

            if (breakLine)
                newLine.append( ' ' );
            else
                newLine.append( comments[currIndex] );
        }

        commentsIndex = currIndex; // The line is ended

        if ( commentsIndex != comments.length() )
            while ( !newLine.endsWith(" ") )
        {
            newLine.truncate(newLine.length() - 1);
            commentsIndex--;
        }

        commentsByLines.prepend(newLine.stripWhiteSpace());
    }

    QFontMetrics fm(*m_commentsFont);

    for ( int lineNumber = 0; lineNumber < (int)commentsByLines.count(); lineNumber++ ) {

        yPos += 1.5 * m_commentsFont->pointSize();

        QRect rect=fm.boundingRect(commentsByLines[lineNumber]);
        rect.addCoords( 0, 0, 2, 2 );

        QPixmap pix(rect.width(),rect.height());
        pix.fill(QColor(m_commentsBgColor)); 

        QPainter p(&pix);
        p.setPen(QColor(m_commentsFontColor));
        p.setFont(*m_commentsFont);
        p.drawText(1,m_commentsFont->pointSize()+0 , commentsByLines[lineNumber]);
        p.end();

        QImage textimage(pix.convertToImage());
        KImageEffect::blendOnLower(0,m_height-rect.height()-yPos,textimage,layer);
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
    if (!m_effect) {
        kdWarning( 51000 ) << "SlideShowGL: No transition method"
                    << endl;
        m_effect = &SlideShowGL::effectNone;
    }

    if (m_effectRunning) {
        m_timeout = 10;
    }
    else {
        if (m_timeout == -1) {
            // effect was running and is complete now
            // run timer while showing current image
            m_timeout = m_delay;
            m_i     = 0;
        }
        else {

            // timed out after showing current image
            // load next image and start effect
            if (m_random)
                m_effect = getRandomEffect();

            advanceFrame();
            if (m_endOfShow) {
                updateGL();
                return;
            }

            loadImage();
            m_timeout = 10;
            m_effectRunning = true;
            m_i = 0;

        }
    }

    updateGL();
    m_timer->start(m_timeout, true);
}

void SlideShowGL::slotMouseMoveTimeOut()
{
    QPoint pos(QCursor::pos());
    if ((pos.y() < (m_deskY+20)) ||
        (pos.y() > (m_deskY+m_deskHeight-20-1)))
        return;

    setCursor(QCursor(Qt::BlankCursor));
}

void SlideShowGL::paintTexture()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GLuint& tex = m_texture[m_curr];

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
    m_effectRunning = false;
    m_timeout = -1;
    return;
}

void SlideShowGL::effectBlend()
{
    if (m_i > 100) {
        paintTexture();
        m_effectRunning = false;
        m_timeout = -1;
        return;
    }

    int a = (m_curr == 0) ? 1 : 0;
    int b =  m_curr;

    GLuint& ta = m_texture[a];
    GLuint& tb = m_texture[b];

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
        m_effectRunning = false;
        m_timeout = -1;
        return;
    }

    int a;
    float opacity;
    if (m_i <= 50) {
        a =  (m_curr == 0) ? 1 : 0;
        opacity = 1.0 - 1.0/50.0*(float)(m_i);
    }
    else {
        opacity = 1.0/50.0*(float)(m_i-50.0);
        a = m_curr;
    }

    GLuint& ta = m_texture[a];

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
        m_effectRunning = false;
        m_timeout = -1;
        return;
    }

    if (m_i == 0)
        m_dir = (int)((2.0*rand()/(RAND_MAX+1.0)));

    int a = (m_curr == 0) ? 1 : 0;
    int b =  m_curr;

    GLuint& ta = m_texture[a];
    GLuint& tb = m_texture[b];

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
        m_effectRunning = false;
        m_timeout = -1;
        return;
    }

    if (m_i == 0)
        m_dir = (int)((2.0*rand()/(RAND_MAX+1.0)));

    int a = (m_curr == 0) ? 1 : 0;
    int b =  m_curr;

    GLuint& ta = m_texture[a];
    GLuint& tb = m_texture[b];

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
        m_effectRunning = false;
        m_timeout = -1;
        return;
    }

    if (m_i == 0) {
        m_dir = 1 + (int)((4.0*rand()/(RAND_MAX+1.0)));
    }

    int a;
    bool out;
    if (m_i <= 50) {
        a   = (m_curr == 0) ? 1 : 0;
        out = 1;
    }
    else {
        a   = m_curr;
        out = 0;
    }

    GLuint& ta = m_texture[a];

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
        m_effectRunning = false;
        m_timeout = -1;
        return;
    }

    if (m_i == 0)
        m_dir = 1 + (int)((4.0*rand()/(RAND_MAX+1.0)));

    int a = (m_curr == 0) ? 1 : 0;
    int b =  m_curr;

    GLuint& ta = m_texture[a];
    GLuint& tb = m_texture[b];

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
        m_effectRunning = false;
        m_timeout = -1;
        return;
    }

    int a = (m_curr == 0) ? 1 : 0;
    int b =  m_curr;

    GLuint& ta = m_texture[a];
    GLuint& tb = m_texture[b];

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
        m_effectRunning = false;
        m_timeout = -1;
        return;
    }

    // Enable perspective vision
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    int a = (m_curr == 0) ? 1 : 0;
    int b =  m_curr;

    GLuint& ta = m_texture[a];
    GLuint& tb = m_texture[b];

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
    m_timer->stop();

    if (m_toolBar->isHidden())
    {
        int w = m_toolBar->width();
        m_toolBar->move(m_deskWidth-w-1,0);
        m_toolBar->show();
    }
}

void SlideShowGL::slotPlay()
{
    m_toolBar->hide();
    slotTimeOut();
}

void SlideShowGL::slotPrev()
{
    previousFrame();
    if (m_endOfShow) {
        updateGL();
        return;
    }

    m_effectRunning = false;
    loadImage();
    updateGL();
}

void SlideShowGL::slotNext()
{
    advanceFrame();
    if (m_endOfShow) {
        updateGL();
        return;
    }

    m_effectRunning = false;
    loadImage();
    updateGL();
}

void SlideShowGL::slotClose()
{
    close();    
}

}  // NameSpace KIPISlideShowPlugin

#include "slideshowgl.moc"
