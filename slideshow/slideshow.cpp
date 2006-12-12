/* ============================================================
 * File  : slideshow.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-16
 * Description :
 *
 * Copyright 2003 by Renchi Raju

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

#include <klocale.h>
#include <kdeversion.h>
#include <kglobalsettings.h>

#include <qtimer.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qevent.h>
#include <qcursor.h>
#include <qfont.h>

#include <kdebug.h>
#include <qtextcodec.h>

extern "C"
{
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
}

#include "imlibiface.h"
#include "slideshow.h"
#include "toolbar.h"

#include <kdebug.h>

namespace KIPISlideShowPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////////////

SlideShow::SlideShow(const FileList& fileList,
                     const QStringList& commentsList, bool ImagesHasComments,
                     int delay, bool printName, bool printComments, bool loop,
                     const QString& effectName,
                     const QFont &commentsFont, uint commentsFontColor, uint commentsBgColor, int commentsLinesLength)
                     : QWidget(0, 0, WStyle_StaysOnTop | WType_Popup |
                               WX11BypassWM | WDestructiveClose)
{
#if KDE_IS_VERSION(3,2,0)
    QRect deskRect = KGlobalSettings::desktopGeometry(this);
    deskX_      = deskRect.x();
    deskY_      = deskRect.y();
    deskWidth_  = deskRect.width();
    deskHeight_ = deskRect.height();
#else
    QRect deskRect = QApplication::desktop()->screenGeometry(this);
    deskX_      = deskRect.x();
    deskY_      = deskRect.y();
    deskWidth_  = deskRect.width();
    deskHeight_ = deskRect.height();
#endif    
    
    move(deskX_, deskY_);
    resize(deskWidth_, deskHeight_);
    setPaletteBackgroundColor(black);

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

    // ---------------------------------------------------------------
    
    imIface_   = new ImlibIface(this);
    currImage_ = 0;
    fileIndex_ = -1; // start with -1
    effect_        = 0;
    effectRunning_ = false;
    timer_ = new QTimer();
    connect(timer_, SIGNAL(timeout()), SLOT(slotTimeOut()));
    mIntArray  = 0;
    endOfShow_ = false;

    // --------------------------------------------------

    fileList_       = fileList;
    commentsList_   = commentsList;
    delay_          = QMAX(delay, 300); // at least have 0.3 second delay
    loop_           = loop;
    printName_      = printName;
    printComments_  = printComments;
    effectName_     = effectName;

    ImagesHasComments_   = ImagesHasComments; 

    commentsFont_        = commentsFont;
    commentsFontColor_   = commentsFontColor;
    commentsBgColor_     = commentsBgColor;
    commentsLinesLength_ = commentsLinesLength;

    // --------------------------------------------------

    registerEffects();

    if (effectName_ == "Random")
        effect_ = getRandomEffect();
    else
    {
        effect_ = Effects[effectName_];
        if (!effect_)
        {
            effect_ = Effects["None"];
        }
    }

    timer_->start(10, true);

    // -- hide cursor when not moved --------------------

    mouseMoveTimer_ = new QTimer;
    connect(mouseMoveTimer_, SIGNAL(timeout()),
            SLOT(slotMouseMoveTimeOut()));
    
    setMouseTracking(true);
    slotMouseMoveTimeOut();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

SlideShow::~SlideShow()
{
    timer_->stop();
    delete timer_;
    mouseMoveTimer_->stop();
    delete mouseMoveTimer_;

    if (mPainter.isActive())
        mPainter.end();

    if (mIntArray)
        delete [] mIntArray;

    if (currImage_)
        delete currImage_;
    if (imIface_)
        delete imIface_;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::registerEffects()
{
    Effects.insert("None", &SlideShow::effectNone);
    Effects.insert("Chess Board", &SlideShow::effectChessboard);
    Effects.insert("Melt Down", &SlideShow::effectMeltdown);
    Effects.insert("Sweep", &SlideShow::effectSweep);
    Effects.insert("Noise", &SlideShow::effectRandom);
    Effects.insert("Growing", &SlideShow::effectGrowing);
    Effects.insert("Incoming Edges", &SlideShow::effectIncomingEdges);
    Effects.insert("Horizontal Lines", &SlideShow::effectHorizLines);
    Effects.insert("Vertical Lines", &SlideShow::effectVertLines);
    Effects.insert("Circle Out", &SlideShow::effectCircleOut);
    Effects.insert("MultiCircle Out", &SlideShow::effectMultiCircleOut);
    Effects.insert("Spiral In", &SlideShow::effectSpiralIn);
    Effects.insert("Blobs", &SlideShow::effectBlobs);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QStringList SlideShow::effectNames()
{
    QStringList effects;

    effects.append("None");
    effects.append("Chess Board");
    effects.append("Melt Down");
    effects.append("Sweep");
    effects.append("Noise");
    effects.append("Growing");
    effects.append("Incoming Edges");
    effects.append("Horizontal Lines");
    effects.append("Vertical Lines");
    effects.append("Circle Out");
    effects.append("MultiCircle Out");
    effects.append("Spiral In");
    effects.append("Blobs");
    effects.append("Random");

    return effects;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QMap<QString,QString> SlideShow::effectNamesI18N()
{
    QMap<QString,QString> effects;

    effects["None"] = i18n("None");
    effects["Chess Board"] = i18n("Chess Board");
    effects["Melt Down"] = i18n("Melt Down");
    effects["Sweep"] = i18n("Sweep");
    effects["Noise"] = i18n("Noise");
    effects["Growing"] = i18n("Growing");
    effects["Incoming Edges"] = i18n("Incoming Edges");
    effects["Horizontal Lines"] = i18n("Horizontal Lines");
    effects["Vertical Lines"] = i18n("Vertical Lines");
    effects["Circle Out"] = i18n("Circle Out");
    effects["MultiCircle Out"] = i18n("MultiCircle Out");
    effects["Spiral In"] = i18n("Spiral In");
    effects["Blobs"] = i18n("Blobs");
    effects["Random"] = i18n("Random");

    return effects;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::slotTimeOut()
{
    if (!effect_) return;                         // No effect -> bye !

    int tmout = -1;

    if (effectRunning_)                           // Effect under progress ?
    {
        tmout = (this->*effect_)(false);
    }
    else
    {
        loadNextImage();

	if (!currImage_ || fileList_.isEmpty())   // End of slideshow ?
        {
            showEndOfShow();
            return;
        }

        if (effectName_ == "Random")              // Take a random effect.
        {
            effect_ = getRandomEffect();
            if (!effect_) return;
        }

        effectRunning_ = true;
        tmout = (this->*effect_)(true);
    }

    if (tmout <= 0)                               // Effect finished -> delay.
    {
        tmout = delay_;
        effectRunning_ = false;
    }

    timer_->start(tmout, true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::loadNextImage()
{
    if (currImage_)
        delete currImage_;
    currImage_ = 0;

    fileIndex_++;
    int num = fileList_.count();
    if (fileIndex_ >= num)
    {
        if (loop_)
        {
            fileIndex_ = 0;
        }
        else
        {
            fileIndex_ = num-1;
            return;
        }
    }

    if (!loop_)
    {
        toolBar_->setEnabledPrev(fileIndex_ > 0);
        toolBar_->setEnabledNext(fileIndex_ < num-1);
    }

    FileAnglePair fileAngle = fileList_[fileIndex_];
    QString file(fileAngle.first);
    int     angle(fileAngle.second);
    
    currImage_ = new ImImageSS(imIface_, file, angle);
    currImage_->fitSize(width(), height());
    currImage_->render();

    if (printName_)
        printFilename();
    
    if (printComments_ && ImagesHasComments_)
        printComments();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::loadPrevImage()
{
    if (currImage_)
        delete currImage_;
    currImage_ = 0;

    fileIndex_--;
    int num = fileList_.count();
    if (fileIndex_ < 0)
    {
        if (loop_)
        {
            fileIndex_ = num-1;
        }
        else
        {
            fileIndex_ = -1; // set this to -1.
            return;
        }
    }

    if (!loop_)
    {
        toolBar_->setEnabledPrev(fileIndex_ > 0);
        toolBar_->setEnabledNext(fileIndex_ < num-1);
    }
    
    FileAnglePair fileAngle = fileList_[fileIndex_];
    QString file(fileAngle.first);
    int     angle(fileAngle.second);
    
    currImage_ = new ImImageSS(imIface_, file, angle);
    currImage_->fitSize(width(), height());
    currImage_->render();

    if (printName_)
        printFilename();
    
    if (printComments_)
        printComments();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::showCurrentImage()
{
    if (!currImage_)
        return;
    
    bitBlt(this, 0, 0, currImage_->qpixmap(),
           0, 0, currImage_->qpixmap()->width(),
           currImage_->qpixmap()->height(), Qt::CopyROP, true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::printFilename()
{
    if (!currImage_) return;

    QPainter p;
    p.begin(currImage_->qpixmap());

    QString filename(currImage_->filename());
    filename += " (";
    filename += QString::number(fileIndex_ + 1);
    filename += "/";
    filename += QString::number(fileList_.count());
    filename += ")";

    p.setPen(QColor("black"));
    for (int x=9; x<=11; x++)
        for (int y=21; y>=19; y--)
            p.drawText(x, height()-y, filename);

    p.setPen(QColor("white"));
    p.drawText(10, height()-20, filename);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::printComments()
{
    if (!currImage_) return;

    QString comments = commentsList_[fileIndex_];

    int yPos = 20; // Text Y coordinate
    if (printName_) yPos = 40;

    QStringList commentsByLines;

    uint commentsIndex = 0; // Comments QString index

    while (commentsIndex < comments.length())
    {
        QString newLine; 
        bool breakLine = FALSE; // End Of Line found
        uint currIndex; //  Comments QString current index

        // Check miminal lines dimension

        uint commentsLinesLengthLocal = commentsLinesLength_;

        for ( currIndex = commentsIndex; currIndex < comments.length() && !breakLine; currIndex++ )
            if( comments[currIndex] == QChar('\n') || comments[currIndex].isSpace() ) breakLine = TRUE;

        if (commentsLinesLengthLocal <= (currIndex - commentsIndex)) 
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

    QPainter p;
    p.begin(currImage_->qpixmap());
    p.setFont(commentsFont_);

    for ( int lineNumber = 0; lineNumber < (int)commentsByLines.count(); lineNumber++ ) {

        p.setPen(QColor(commentsBgColor_));

        // coefficient 1.5 is used to maintain distance between different lines

         for (int x=9; x<=11; x++)
             for (int y = (int)(yPos + lineNumber * 1.5 * commentsFont_.pointSize()  + 1); 
                      y >= (int)(yPos + lineNumber* 1.5 * commentsFont_.pointSize()  - 1); y--)
                p.drawText(x, height()-y, commentsByLines[lineNumber]);

        p.setPen(QColor(commentsFontColor_));

        p.drawText(10, height()-(int)(lineNumber * 1.5 * commentsFont_.pointSize() + yPos), commentsByLines[lineNumber]);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

EffectMethod SlideShow::getRandomEffect()
{
    QStringList effs = Effects.keys();
    effs.remove("None");

    int count = effs.count();

    int i = rand() % count;
    QString key = effs[i];

    return Effects[key];
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::showEndOfShow()
{
    QPainter p;
    p.begin(this);
    p.fillRect(0, 0, width(), height(), Qt::black);

    QFont fn(font());
    fn.setPointSize(fn.pointSize()+10);
    fn.setBold(true);

    p.setFont(fn);
    p.setPen(Qt::white);
    p.drawText(100, 100, i18n("SlideShow Completed."));
    p.drawText(100, 150, i18n("Click To Exit..."));
    p.end();

    endOfShow_ = true;
    toolBar_->setEnabledPlay(false);
    toolBar_->setEnabledNext(false);
    toolBar_->setEnabledPrev(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::keyPressEvent(QKeyEvent *event)
{
    if (!event)
        return;

    toolBar_->keyPressEvent(event);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::mousePressEvent(QMouseEvent *)
{
    if (endOfShow_)
        close();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::mouseMoveEvent(QMouseEvent *e)
{
    setCursor(QCursor(Qt::ArrowCursor));
    mouseMoveTimer_->start(1000, true);

    if (!toolBar_->canHide())
        return;
    
    QPoint pos(e->pos());
    
    if ((pos.y() > (deskY_+20)) &&
        (pos.y() < (deskY_+deskHeight_-20-1)))
    {
        if (toolBar_->isHidden())
            return;
        else
            toolBar_->hide();
        return;
    }

    int w = toolBar_->width();
    int h = toolBar_->height();
    
    if (pos.y() < (deskY_+20))
    {
        if (pos.x() <= (deskX_+deskWidth_/2))
            // position top left
            toolBar_->move(deskX_, deskY_);
        else
            // position top right
            toolBar_->move(deskX_+deskWidth_-w-1, deskY_);
    }
    else
    {
        if (pos.x() <= (deskX_+deskWidth_/2))
            // position bot left
            toolBar_->move(deskX_, deskY_+deskHeight_-h-1);
        else
            // position bot right
            toolBar_->move(deskX_+deskWidth_-w-1, deskY_+deskHeight_-h-1);
    }
    toolBar_->show();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::slotMouseMoveTimeOut()
{
    QPoint pos(QCursor::pos());
    if ((pos.y() < (deskY_+20)) ||
        (pos.y() > (deskY_+deskHeight_-20-1)))
        return;
    
    setCursor(QCursor(Qt::BlankCursor));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectNone(bool /* aInit */)
{
    showCurrentImage();
    return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectChessboard(bool aInit)
{
    int y;

    if (aInit)
    {
        mw  = width();
        mh  = height();
        mdx = 8;         // width of one tile
        mdy = 8;         // height of one tile
        mj  = (mw+mdx-1)/mdx; // number of tiles
        mx  = mj*mdx;    // shrinking x-offset from screen border
        mix = 0;         // growing x-offset from screen border
        miy = 0;         // 0 or mdy for growing tiling effect
        my  = mj&1 ? 0 : mdy; // 0 or mdy for shrinking tiling effect
        mwait = 800 / mj; // timeout between effects
    }

    if (mix >= mw)
    {
        showCurrentImage();
        return -1;
    }

    mix += mdx;
    mx  -= mdx;
    miy = miy ? 0 : mdy;
    my  = my ? 0 : mdy;

    for (y=0; y<mw; y+=(mdy<<1))
    {
        bitBlt(this, mix, y+miy, currImage_->qpixmap(), mix, y+miy,
               mdx, mdy, CopyROP, true);
        bitBlt(this, mx, y+my, currImage_->qpixmap(), mx, y+my,
               mdx, mdy, CopyROP, true);
    }

    return mwait;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectMeltdown(bool aInit)
{
    int i, x, y;
    bool done;

    if (aInit)
    {
        delete [] mIntArray;
        mw = width();
        mh = height();
        mdx = 4;
        mdy = 16;
        mix = mw / mdx;
        mIntArray = new int[mix];
        for (i=mix-1; i>=0; i--)
            mIntArray[i] = 0;
    }

    done = true;
    for (i=0,x=0; i<mix; i++,x+=mdx)
    {
        y = mIntArray[i];
        if (y >= mh) continue;
        done = false;
        if ((rand()&15) < 6) continue;
        bitBlt(this, x, y+mdy, this, x, y, mdx, mh-y-mdy, CopyROP, true);
        bitBlt(this, x, y, currImage_->qpixmap(), x, y, mdx, mdy, CopyROP, true);
        mIntArray[i] += mdy;
    }

    if (done)
    {
        delete [] mIntArray;
        mIntArray = NULL;
        return -1;
    }

    return 15;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectSweep(bool aInit)
{
    int w, h, x, y, i;

    if (aInit)
    {
        // subtype: 0=sweep right to left, 1=sweep left to right
        //          2=sweep bottom to top, 3=sweep top to bottom
        mSubType = rand() % 4;
        mw  = width();
        mh  = height();
        mdx = (mSubType==1 ? 16 : -16);
        mdy = (mSubType==3 ? 16 : -16);
        mx  = (mSubType==1 ? 0 : mw);
        my  = (mSubType==3 ? 0 : mh);
    }

    if (mSubType==0 || mSubType==1)
    {
        // horizontal sweep
        if ((mSubType==0 && mx < -64) ||
            (mSubType==1 && mx > mw+64))
        {
            return -1;
        }
        for (w=2,i=4,x=mx; i>0; i--, w<<=1, x-=mdx)
        {
            bitBlt(this, x, 0, currImage_->qpixmap(), x, 0, w, mh, CopyROP, true);
        }
        mx += mdx;
    }
    else
    {
        // vertical sweep
        if ((mSubType==2 && my < -64) ||
            (mSubType==3 && my > mh+64))
        {
            return -1;
        }
        for (h=2,i=4,y=my; i>0; i--, h<<=1, y-=mdy)
        {
            bitBlt(this, 0, y, currImage_->qpixmap(), 0, y, mw, h, CopyROP, true);
        }
        my += mdy;
    }

    return 20;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectRandom(bool /*aInit*/)
{
    int x, y, i, w, h, fact, sz;

    fact = (rand() % 3) + 1;

    w = width() >> fact;
    h = height() >> fact;
    sz = 1 << fact;

    for (i = (w*h)<<1; i > 0; i--)
    {
        x = (rand() % w) << fact;
        y = (rand() % h) << fact;
        bitBlt(this, x, y, currImage_->qpixmap(), x, y, sz, sz, CopyROP, true);
    }

    showCurrentImage();

    return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectGrowing(bool aInit)
{
    if (aInit)
    {
        mw = width();
        mh = height();
        mx = mw >> 1;
        my = mh >> 1;
        mi = 0;
        mfx = mx / 100.0;
        mfy = my / 100.0;
    }

    mx = (mw>>1) - (int)(mi * mfx);
    my = (mh>>1) - (int)(mi * mfy);
    mi++;

    if (mx<0 || my<0)
    {
        showCurrentImage();
        return -1;
    }

    bitBlt(this, mx, my, currImage_->qpixmap(), mx, my,
           mw - (mx<<1), mh - (my<<1), CopyROP, true);

    return 20;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectIncomingEdges(bool aInit)
{
    int x1, y1;

    if (aInit)
    {
        mw = width();
        mh = height();
        mix = mw >> 1;
        miy = mh >> 1;
        mfx = mix / 100.0;
        mfy = miy / 100.0;
        mi = 0;
        mSubType = rand() & 1;
    }

    mx = (int)(mfx * mi);
    my = (int)(mfy * mi);

    if (mx>mix || my>miy)
    {
        showCurrentImage();
        return -1;
    }

    x1 = mw - mx;
    y1 = mh - my;
    mi++;

    if (mSubType)
    {
        // moving image edges
        bitBlt(this,  0,  0, currImage_->qpixmap(), mix-mx, miy-my, mx, my, CopyROP, true);
        bitBlt(this, x1,  0, currImage_->qpixmap(), mix, miy-my, mx, my, CopyROP, true);
        bitBlt(this,  0, y1, currImage_->qpixmap(), mix-mx, miy, mx, my, CopyROP, true);
        bitBlt(this, x1, y1, currImage_->qpixmap(), mix, miy, mx, my, CopyROP, true);
    }
    else
    {
        // fixed image edges
        bitBlt(this,  0,  0, currImage_->qpixmap(),  0,  0, mx, my, CopyROP, true);
        bitBlt(this, x1,  0, currImage_->qpixmap(), x1,  0, mx, my, CopyROP, true);
        bitBlt(this,  0, y1, currImage_->qpixmap(),  0, y1, mx, my, CopyROP, true);
        bitBlt(this, x1, y1, currImage_->qpixmap(), x1, y1, mx, my, CopyROP, true);
    }
    return 20;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectHorizLines(bool aInit)
{
    static int iyPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
    int y;

    if (aInit)
    {
        mw = width();
        mh = height();
        mi = 0;
    }

    if (iyPos[mi] < 0) return -1;

    for (y=iyPos[mi]; y<mh; y+=8)
    {
        bitBlt(this, 0, y, currImage_->qpixmap(), 0, y, mw, 1, CopyROP, true);
    }

    mi++;
    if (iyPos[mi] >= 0) return 160;
    return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectVertLines(bool aInit)
{
    static int ixPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
    int x;

    if (aInit)
    {
        mw = width();
        mh = height();
        mi = 0;
    }

    if (ixPos[mi] < 0) return -1;

    for (x=ixPos[mi]; x<mw; x+=8)
    {
        bitBlt(this, x, 0, currImage_->qpixmap(), x, 0, 1, mh, CopyROP, true);
    }

    mi++;
    if (ixPos[mi] >= 0) return 160;
    return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectMultiCircleOut(bool aInit)
{
    int x, y, i;
    double alpha;
    static QPointArray pa(4);

    if (aInit)
    {
        startPainter();
        mw = width();
        mh = height();
        mx = mw;
        my = mh>>1;
        pa.setPoint(0, mw>>1, mh>>1);
        pa.setPoint(3, mw>>1, mh>>1);
        mfy = sqrt((double)mw*mw + mh*mh) / 2;
        mi  = rand()%15 + 2;
        mfd = M_PI*2/mi;
        mAlpha = mfd;
        mwait = 10 * mi;
        mfx = M_PI/32;  // divisor must be powers of 8
    }

    if (mAlpha < 0)
    {
        mPainter.end();
        showCurrentImage();
        return -1;
    }

    for (alpha=mAlpha, i=mi; i>=0; i--, alpha+=mfd)
    {
        x = (mw>>1) + (int)(mfy * cos(-alpha));
        y = (mh>>1) + (int)(mfy * sin(-alpha));

        mx = (mw>>1) + (int)(mfy * cos(-alpha + mfx));
        my = (mh>>1) + (int)(mfy * sin(-alpha + mfx));

        pa.setPoint(1, x, y);
        pa.setPoint(2, mx, my);

        mPainter.drawPolygon(pa);
    }

    mAlpha -= mfx;

    return mwait;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectSpiralIn(bool aInit)
{
    if (aInit)
    {
        startPainter();
        mw = width();
        mh = height();
        mix = mw / 8;
        miy = mh / 8;
        mx0 = 0;
        mx1 = mw - mix;
        my0 = miy;
        my1 = mh - miy;
        mdx = mix;
        mdy = 0;
        mi = 0;
        mj = 16 * 16;
        mx = 0;
        my = 0;
    }

    if (mi==0 && mx0>=mx1)
    {
        mPainter.end();
        showCurrentImage();
        return -1;
    }

    if (mi==0 && mx>=mx1) // switch to: down on right side
    {
        mi = 1;
        mdx = 0;
        mdy = miy;
        mx1 -= mix;
    }
    else if (mi==1 && my>=my1) // switch to: right to left on bottom side
    {
        mi = 2;
        mdx = -mix;
        mdy = 0;
        my1 -= miy;
    }
    else if (mi==2 && mx<=mx0) // switch to: up on left side
    {
        mi = 3;
        mdx = 0;
        mdy = -miy;
        mx0 += mix;
    }
    else if (mi==3 && my<=my0) // switch to: left to right on top side
    {
        mi = 0;
        mdx = mix;
        mdy = 0;
        my0 += miy;
    }

    bitBlt(this, mx, my, currImage_->qpixmap(), mx, my, mix, miy, CopyROP, true);

    mx += mdx;
    my += mdy;
    mj--;

    return 8;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectCircleOut(bool aInit)
{
    int x, y;
    static QPointArray pa(4);

    if (aInit)
    {
        startPainter();
        mw = width();
        mh = height();
        mx = mw;
        my = mh>>1;
        mAlpha = 2*M_PI;
        pa.setPoint(0, mw>>1, mh>>1);
        pa.setPoint(3, mw>>1, mh>>1);
        mfx = M_PI/16;  // divisor must be powers of 8
        mfy = sqrt((double)mw*mw + mh*mh) / 2;
    }

    if (mAlpha < 0)
    {
        mPainter.end();
        showCurrentImage();
        return -1;
    }

    x = mx;
    y = my;
    mx = (mw>>1) + (int)(mfy * cos(mAlpha));
    my = (mh>>1) + (int)(mfy * sin(mAlpha));
    mAlpha -= mfx;

    pa.setPoint(1, x, y);
    pa.setPoint(2, mx, my);

    mPainter.drawPolygon(pa);

    return 20;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

int SlideShow::effectBlobs(bool aInit)
{
    int r;

    if (aInit)
    {
        startPainter();
        mAlpha = M_PI * 2;
        mw = width();
        mh = height();
        mi = 150;
    }

    if (mi <= 0)
    {
        mPainter.end();
        showCurrentImage();
        return -1;
    }

    mx = rand() % mw;
    my = rand() % mh;
    r = (rand() % 200) + 50;

    mPainter.drawEllipse(mx-r, my-r, r, r);
    mi--;

    return 10;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SlideShow::startPainter(Qt::PenStyle aPen)
{
    QBrush brush;
    brush.setPixmap(*(currImage_->qpixmap()));

    if (mPainter.isActive())
        mPainter.end();

    mPainter.begin(this);
    mPainter.setBrush(brush);
    mPainter.setPen(aPen);
}

void SlideShow::slotPause()
{
    timer_->stop();

    if (toolBar_->isHidden())
    {
        int w = toolBar_->width();
        toolBar_->move(deskWidth_-w-1,0);
        toolBar_->show();
    }
}

void SlideShow::slotPlay()
{
    toolBar_->hide();
    slotTimeOut();
}

void SlideShow::slotPrev()
{
    loadPrevImage();
    if (!currImage_ || fileList_.isEmpty())   
    {
        showEndOfShow();
        return;
    }
    effectRunning_ = false;
    showCurrentImage();
}

void SlideShow::slotNext()
{
    loadNextImage();
    if (!currImage_ || fileList_.isEmpty())   
    {
        showEndOfShow();
        return;
    }
    effectRunning_ = false;
    showCurrentImage();
}

void SlideShow::slotClose()
{
    close();    
}

}  // NameSpace KIPISlideShowPlugin

#include "slideshow.moc"
