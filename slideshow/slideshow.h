/* ============================================================
 * File  : slideshow.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-16
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

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

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <qstringlist.h>
#include <qstring.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qmap.h>

class QTimer;
class QMouseEvent;

class ImlibIface;
class ImImageSS;

class SlideShow;
typedef int (SlideShow::*EffectMethod)(bool);

class SlideShow : public QWidget
{
    Q_OBJECT
    
public:

    SlideShow(const QStringList& fileList,
              int delay, bool printName, bool loop,
              const QString& effectName);
    ~SlideShow();

    void registerEffects();

    static QStringList effectNames();
    static QMap<QString,QString> effectNamesI18N();
    
private:

    void          loadNextImage();
    void          loadPrevImage();
    void          showCurrentImage();
    void          printFilename();
    EffectMethod  getRandomEffect();
    void          showEndOfShow();
    
private:

    // config ------------------
    int         delay_;
    bool        printName_;
    QString     effectName_;
    bool        loop_;
    // -------------------------
    
    QMap<QString, EffectMethod> Effects;

    ImlibIface *imIface_;
    ImImageSS    *currImage_;
    ImImageSS    *nextImage_;
    
    QStringList fileList_;
    QTimer     *timer_;
    QTimer     *mouseMoveTimer_;
    int         fileIndex_;

    EffectMethod effect_;
    bool         effectRunning_;

    // values for state of various effects:
    int mx, my, mw, mh, mdx, mdy, mix, miy, mi, mj, mSubType;
    int mx0, my0, mx1, my1, mwait;
    double mfx, mfy, mAlpha, mfd;
    int* mIntArray;
    QPainter mPainter;

    
protected:

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    
    int effectNone(bool);
    int effectChessboard(bool doInit);
    int effectMeltdown(bool doInit);
    int effectSweep(bool doInit);
    int effectRandom(bool doInit);
    int effectGrowing(bool doInit);
    int effectIncomingEdges(bool doInit);
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
};

#endif /* SLIDESHOW_H */
