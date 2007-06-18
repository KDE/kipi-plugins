/* ============================================================
 * File  : slideshow.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-16
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>

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

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

// KDE includes

#include <kconfig.h>

// QT includes

#include <qvaluelist.h>
#include <qstringlist.h>
#include <qpair.h>
#include <qstring.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qmap.h>
#include <qfont.h>

// Includes for libKIPI.
#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

class QTimer;

typedef QPair<QString, int> FileAnglePair;
typedef QValueList<FileAnglePair > FileList;

namespace KIPISlideShowPlugin
{

class ImlibIface;
class ImImageSS;
class ToolBar;

class SlideShow;

typedef int (SlideShow::*EffectMethod)(bool);

class SlideShow : public QWidget
{
    Q_OBJECT
    
public:

    SlideShow(const FileList& fileList, const QStringList& commentsList, bool ImagesHasComments);
    ~SlideShow();

    void registerEffects();

    static QStringList effectNames();
    static QMap<QString,QString> effectNamesI18N();
    
private:

    void          loadNextImage();
    void          loadPrevImage();
    void          showCurrentImage();
    void          printFilename();
    void          printComments();
    void          printProgress();
    EffectMethod  getRandomEffect();
    void          showEndOfShow();
    
    void          readSettings();
    
private:

    // config ------------------
    
    KConfig*    m_config;
    
    int         m_delay;
    bool        m_printName;
    bool        m_printComments;
    bool        m_printProgress;
    QString     m_effectName;
    bool        m_loop;

    bool        m_ImagesHasComments;

    QFont*      m_commentsFont;
    uint        m_commentsFontColor;
    uint        m_commentsBgColor;
    int         m_commentsLinesLength;
    
    bool        m_enableMouseWheel;
    // -------------------------
    
    QMap<QString, EffectMethod> Effects;

    ImlibIface*   m_imIface;
    ImImageSS*    m_currImage;
    
    FileList    m_fileList;
    QStringList m_commentsList;
    QTimer*     m_timer;
    int         m_fileIndex;

    EffectMethod m_effect;
    bool         m_effectRunning;

    int         m_commentsLinesLenght;
    
    // values for state of various effects:
    int      m_x, m_y, m_w, m_h, m_dx, m_dy, m_ix, m_iy, m_i, m_j, m_subType;
    int      m_x0, m_y0, m_x1, m_y1, m_wait;
    double   m_fx, m_fy, m_alpha, m_fd;
    int*     m_intArray;
    QPainter m_painter;

    ToolBar*     m_toolBar;
    QTimer*      m_mouseMoveTimer;
    bool         m_endOfShow;
    
    int          m_deskX;
    int          m_deskY;
    int          m_deskWidth;
    int          m_deskHeight;
    
protected:

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void keyPressEvent(QKeyEvent *event);
    
    int effectNone(bool);
    int effectChessboard(bool doInit);
    int effectMeltdown(bool doInit);
    int effectSweep(bool doInit);
    int effectRandom(bool doInit);
    int effectGrowing(bool doInit);
    int effectIncom_ingEdges(bool doInit);
    int effectHorizLines(bool doInit);
    int effectVertLines(bool doInit);
    int effectMultiCircleOut(bool doInit);
    int effectSpiralIn(bool doInit);
    int effectCircleOut(bool doInit);
    int effectBlobs(bool doInit);
    
    void startPainter(Qt::PenStyle penStyle=NoPen);
    
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

#endif /* SLIDESHOW_H */
