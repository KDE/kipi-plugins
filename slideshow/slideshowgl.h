/* ============================================================
 * File  : slideshowgl.h
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

#ifndef SLIDESHOWGL_H
#define SLIDESHOWGL_H

#include <qstringlist.h>
#include <qstring.h>
#include <qmap.h>
#include <qgl.h>

class QTimer;

namespace KIPISlideShowPlugin
{
class SlideShowGL;
class ToolBar;

class SlideShowGL : public QGLWidget
{
    Q_OBJECT
    
public:

    SlideShowGL(const QStringList& fileList,
                int delay, bool loop,
                const QString& effectName);
    ~SlideShowGL();

    void registerEffects();

    static QStringList effectNames();
    static QMap<QString,QString> effectNamesI18N();
    
protected:

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *event);
        
private:

    // config ------------------

    int         delay_;
    QString     effectName_;
    bool        loop_;

    // -------------------------
    
    typedef void (SlideShowGL::*EffectMethod)();
    QMap<QString, EffectMethod> Effects;

    QStringList  fileList_;
    QTimer*      timer_;
    int          fileIndex_;

    GLuint       texture_[2];
    bool         tex1First_;
    int          curr_;
    
    int          width_;
    int          height_;

    EffectMethod effect_;
    bool         effectRunning_;
    int          timeout_;
    bool         random_;
    bool         endOfShow_;

    int          m_i;
    int          m_dir;
    float        m_points[40][40][3];

    ToolBar*     toolBar_;
    QTimer*      mouseMoveTimer_;
    int          deskWidth_;
    int          deskHeight_;
    
private:

    void          paintTexture();
    void          advanceFrame();
    void          previousFrame();
    void          loadImage();
    void          montage(QImage& top, QImage& bot);
    EffectMethod  getRandomEffect();
    void          showEndOfShow();

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
};

}  // NameSpace KIPISlideShowPlugin

#endif /* SLIDESHOWGL_H */
