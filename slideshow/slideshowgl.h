/* ============================================================
 * File  : slideshowgl.h
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

#ifndef SLIDESHOWGL_H
#define SLIDESHOWGL_H

#include <qvaluelist.h>
#include <qstringlist.h>
#include <qpair.h>
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

    SlideShowGL(const QValueList<QPair<QString, int> >& fileList,
                const QStringList& commentsList, bool ImagesHasComments,
                int delay, bool printName, bool printComments, bool loop,
                const QString& effectName,
                const QFont& commentsFont, uint commentsFontColor, uint commentsBgColor, int commentsLinesLength);
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

    int         m_delay;
    QString     m_effectName;
    bool        m_loop;
    bool        m_printName;
    bool        m_printComments;
    
    bool        m_imagesHasComments;
    
    QFont       m_commentsFont;
    uint        m_commentsFontColor;
    uint        m_commentsBgColor;
    int         m_commentsLinesLength;

    // -------------------------
    
    typedef void (SlideShowGL::*EffectMethod)();
    QMap<QString, EffectMethod> m_effects;

    QValueList<QPair<QString, int> >  m_fileList;
    QStringList  m_commentsList;
    QTimer*      m_timer;
    int          m_fileIndex;

    GLuint       m_texture[2];
    bool         m_tex1First;
    int          m_curr;
    
    int          m_width;
    int          m_height;

    EffectMethod m_effect;
    bool         m_effectRunning;
    int          m_timeout;
    bool         m_random;
    bool         m_endOfShow;

    int          m_i;
    int          m_dir;
    float        m_points[40][40][3];

    ToolBar*     m_toolBar;
    QTimer*      m_mouseMoveTimer;

    int          m_deskX;
    int          m_deskY;
    int          m_deskWidth;
    int          m_deskHeight;
    
private:

    void          paintTexture();
    void          advanceFrame();
    void          previousFrame();
    void          loadImage();
    void          montage(QImage& top, QImage& bot);
    EffectMethod  getRandomEffect();
    void          showEndOfShow();
    void          printFilename(QImage& layer);
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
};

}  // NameSpace KIPISlideShowPlugin

#endif /* SLIDESHOWGL_H */
