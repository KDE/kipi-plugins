/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-02-16
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2006-2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

// C++ includes.

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <ctime>

// Qt includes.

#include <qtimer.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qevent.h>
#include <qcursor.h>
#include <qfont.h>
#include <qmatrix.h>
#include <qtextcodec.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <Q3PointArray>
#include <QMouseEvent>
#include <QWheelEvent>

// KDE includes.

#include <klocale.h>
#include <kdeversion.h>
#include <kglobalsettings.h>
#include <kdebug.h>
#include <kurl.h>
#include <KConfigGroup>
// Local includes.

#include "toolbar.h"
#include "slideshow.h"
#include "slideshow.moc"

namespace KIPISlideShowPlugin
{

SlideShow::SlideShow(const FileList& fileList, const QStringList& commentsList, bool ImagesHasComments)
    : QWidget(0, Qt::WStyle_StaysOnTop | Qt::WType_Popup | Qt::WX11BypassWM | Qt::WDestructiveClose)
{

    QRect deskRect = KGlobalSettings::desktopGeometry(this);
    m_deskX      = deskRect.x();
    m_deskY      = deskRect.y();
    m_deskWidth  = deskRect.width();
    m_deskHeight = deskRect.height();

    move(m_deskX, m_deskY);
    resize(m_deskWidth, m_deskHeight);

    QPalette paletteSelection = palette();
    paletteSelection.setColor(QPalette::Window, QColor(0,0,0));
    setPalette(paletteSelection);

    m_toolBar = new ToolBar(this);
    m_toolBar->hide();
    if (!m_loop)
    {
        m_toolBar->setEnabledPrev(false);
    }
    connect(m_toolBar, SIGNAL(signalPause()),
            this, SLOT(slotPause()));

    connect(m_toolBar, SIGNAL(signalPlay()),
            this, SLOT(slotPlay()));

    connect(m_toolBar, SIGNAL(signalNext()),
            this, SLOT(slotNext()));

    connect(m_toolBar, SIGNAL(signalPrev()),
            this, SLOT(slotPrev()));

    connect(m_toolBar, SIGNAL(signalClose()),
            this, SLOT(slotClose()));

    // ---------------------------------------------------------------

    m_currImage     = 0;
    m_fileIndex     = -1; // start with -1
    m_effect        = 0;
    m_effectRunning = false;
    m_intArray      = 0;
    m_endOfShow     = false;
    m_simplyShow    = false;
    m_startPainter  = false;

    m_painter = new QPainter();

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(slotTimeOut()));

    // --------------------------------------------------

    m_fileList          = fileList;
    m_commentsList      = commentsList;
    m_ImagesHasComments = ImagesHasComments;

    m_config = new KConfig("kipirc");

    readSettings();

    m_imageLoader = new SlideShowLoader(m_fileList, m_cacheSize, width(), height(), m_fileIndex);

    // --------------------------------------------------

    registerEffects();

    if (m_effectName == "Random")
        m_effect = getRandomEffect();
    else
    {
        m_effect = Effects[m_effectName];
        if (!m_effect)
        {
            m_effect = Effects["None"];
        }
    }

    m_timer->setSingleShot(true);
    m_timer->start(10);

    // -- hide cursor when not moved --------------------

    m_mouseMoveTimer = new QTimer;
    connect(m_mouseMoveTimer, SIGNAL(timeout()),
            SLOT(slotMouseMoveTimeOut()));

    setMouseTracking(true);
    slotMouseMoveTimeOut();

}

SlideShow::~SlideShow()
{
    m_timer->stop();
    delete m_timer;
    m_mouseMoveTimer->stop();
    delete m_mouseMoveTimer;

    if (m_painter->isActive())
        m_painter->end();

    if (m_intArray)
        delete [] m_intArray;

    if (m_currImage)
        delete m_currImage;

    if (m_imageLoader)
        delete m_imageLoader;

    delete m_config;
    delete m_painter;
}

void SlideShow::readSettings()
{
    KConfigGroup grp = m_config->group("SlideShow Settings");
    m_delay            = grp.readEntry("Delay", 1500);
    m_printName        = grp.readEntry("Print Filename", true);
    m_printProgress    = grp.readEntry("Print Progress Indicator", true);
    m_printComments    = grp.readEntry("Print Comments", false);
    m_loop             = grp.readEntry("Loop", false);

    m_effectName       = grp.readEntry("Effect Name", "Random");

    m_enableMouseWheel = grp.readEntry("Enable Mouse Wheel", true);

    // Comments tab settings

    m_commentsFont = new QFont();
    m_commentsFont->setFamily(grp.readEntry("Comments Font Family"));
    m_commentsFont->setPointSize(grp.readEntry("Comments Font Size", 10 ));
    m_commentsFont->setBold(grp.readEntry("Comments Font Bold", false));
    m_commentsFont->setItalic(grp.readEntry("Comments Font Italic", false));
    m_commentsFont->setUnderline(grp.readEntry("Comments Font Underline", false));
    m_commentsFont->setOverline(grp.readEntry("Comments Font Overline", false));
    m_commentsFont->setStrikeOut(grp.readEntry("Comments Font StrikeOut", false));
    m_commentsFont->setFixedPitch(grp.readEntry("Comments Font FixedPitch", false));

    m_commentsFontColor   = grp.readEntry("Comments Font Color", 0xffffff);
    m_commentsBgColor     = grp.readEntry("Comments Bg Color", 0x000000);

    m_commentsLinesLength = grp.readEntry("Comments Lines Length", 72);

    // Advanced settings
    bool enableCache = grp.readEntry("Enable Cache", false);
    if (enableCache)
        m_cacheSize  = grp.readEntry("Cache Size", 1);
    else
        m_cacheSize = 1;
}

void SlideShow::registerEffects()
{
    Effects.insert("None", &SlideShow::effectNone);
    Effects.insert("Chess Board", &SlideShow::effectChessboard);
    Effects.insert("Melt Down", &SlideShow::effectMeltdown);
    Effects.insert("Sweep", &SlideShow::effectSweep);
    Effects.insert("Noise", &SlideShow::effectRandom);
    Effects.insert("Growing", &SlideShow::effectGrowing);
    Effects.insert("Incom_ing Edges", &SlideShow::effectIncom_ingEdges);
    Effects.insert("Horizontal Lines", &SlideShow::effectHorizLines);
    Effects.insert("Vertical Lines", &SlideShow::effectVertLines);
    Effects.insert("Circle Out", &SlideShow::effectCircleOut);
    Effects.insert("MultiCircle Out", &SlideShow::effectMultiCircleOut);
    Effects.insert("Spiral In", &SlideShow::effectSpiralIn);
    Effects.insert("Blobs", &SlideShow::effectBlobs);
}

QStringList SlideShow::effectNames()
{
    QStringList effects;

    effects.append("None");
    effects.append("Chess Board");
    effects.append("Melt Down");
    effects.append("Sweep");
    effects.append("Noise");
    effects.append("Growing");
    effects.append("Incom_ing Edges");
    effects.append("Horizontal Lines");
    effects.append("Vertical Lines");
    effects.append("Circle Out");
    effects.append("MultiCircle Out");
    effects.append("Spiral In");
    effects.append("Blobs");
    effects.append("Random");

    return effects;
}

QMap<QString,QString> SlideShow::effectNamesI18N()
{
    QMap<QString,QString> effects;

    effects["None"] = i18n("None");
    effects["Chess Board"] = i18n("Chess Board");
    effects["Melt Down"] = i18n("Melt Down");
    effects["Sweep"] = i18n("Sweep");
    effects["Noise"] = i18n("Noise");
    effects["Growing"] = i18n("Growing");
    effects["Incom_ing Edges"] = i18n("Incom_ing Edges");
    effects["Horizontal Lines"] = i18n("Horizontal Lines");
    effects["Vertical Lines"] = i18n("Vertical Lines");
    effects["Circle Out"] = i18n("Circle Out");
    effects["MultiCircle Out"] = i18n("MultiCircle Out");
    effects["Spiral In"] = i18n("Spiral In");
    effects["Blobs"] = i18n("Blobs");
    effects["Random"] = i18n("Random");

    return effects;
}

void SlideShow::slotTimeOut()
{
    if (!m_effect) return;                         // No effect -> bye !

    int tmout = -1;

    if (m_effectRunning)                           // Effect under progress ?
    {
        tmout = (this->*m_effect)(false);
    }
    else
    {
        loadNextImage();

        if (!m_currImage || m_fileList.isEmpty())   // End of slideshow ?
        {
            showEndOfShow();
            return;
        }

        if (m_effectName == "Random")              // Take a random effect.
        {
            m_effect = getRandomEffect();
            if (!m_effect) return;
        }

        m_effectRunning = true;
        tmout = (this->*m_effect)(true);
    }

    if (tmout <= 0)                               // Effect finished -> delay.
    {
        tmout = m_delay;
        m_effectRunning = false;
    }

    m_timer->setSingleShot(true);
    m_timer->start(tmout);
}

void SlideShow::loadNextImage()
{
    delete m_currImage;
    m_currImage = 0;

    m_fileIndex++;
    m_imageLoader->next();
    int num = m_fileList.count();
    if (m_fileIndex >= num)
    {
        if (m_loop)
        {
            m_fileIndex = 0;
        }
        else
        {
            m_fileIndex = num-1;
            return;
        }
    }

    if (!m_loop)
    {
        m_toolBar->setEnabledPrev(m_fileIndex > 0);
        m_toolBar->setEnabledNext(m_fileIndex < num-1);
    }

    QPixmap* oldPixmap = m_currImage;
    QPixmap* newPixmap = new QPixmap(QPixmap::fromImage(m_imageLoader->getCurrent()));

    QPixmap pixmap(width(),height());
    pixmap.fill(Qt::black);

    QPainter p(&pixmap);
    p.drawPixmap((width()-newPixmap->width())/2,
                    (height()-newPixmap->height())/2, *newPixmap,
                    0, 0, newPixmap->width(), newPixmap->height());

    delete newPixmap;
    m_currImage = new QPixmap(pixmap);
    delete oldPixmap;

    if (m_printName)
        printFilename();

    if (m_printProgress)
        printProgress();

    if (m_printComments && m_ImagesHasComments)
        printComments();
}

void SlideShow::loadPrevImage()
{
    if (m_currImage)
        delete m_currImage;
    m_currImage = 0;

    m_fileIndex--;
    m_imageLoader->prev();
    int num = m_fileList.count();
    if (m_fileIndex < 0)
    {
        if (m_loop)
        {
            m_fileIndex = num-1;
        }
        else
        {
            m_fileIndex = -1; // set this to -1.
            return;
        }
    }

    if (!m_loop)
    {
        m_toolBar->setEnabledPrev(m_fileIndex > 0);
        m_toolBar->setEnabledNext(m_fileIndex < num-1);
    }

    QPixmap* oldPixmap = m_currImage;
    QPixmap* newPixmap = new QPixmap(QPixmap::fromImage(m_imageLoader->getCurrent()));

    QPixmap pixmap(width(),height());
    pixmap.fill(Qt::black);

    QPainter p(&pixmap);
    p.drawPixmap((width()-newPixmap->width())/2,
                    (height()-newPixmap->height())/2, *newPixmap,
                    0, 0, newPixmap->width(), newPixmap->height());

    delete newPixmap;
    m_currImage = new QPixmap(pixmap);
    delete oldPixmap;

    if (m_printName)
        printFilename();

    if (m_printProgress)
        printProgress();

    if (m_printComments)
        printComments();
}

void SlideShow::showCurrentImage()
{
    if (!m_currImage)
        return;
    //Port it
#if 0
    bitBlt(this, 0, 0, m_currImage,
            0, 0, m_currImage->width(),
            m_currImage->height(), Qt::CopyROP, true);
#endif
  m_simplyShow = true;
//  update();
  repaint();
}

void SlideShow::printFilename()
{
    if (!m_currImage) return;

    QPainter p;
    p.begin(m_currImage);

    p.setPen(Qt::black);
    for (int x=9; x<=11; x++)
        for (int y=31; y>=29; y--)
        p.drawText(x, height()-y, m_imageLoader->currFileName());

    p.setPen(QColor(Qt::white));
    p.drawText(10, height()-30, m_imageLoader->currFileName());
}

void SlideShow::printComments()
{
    if (!m_currImage) return;

    QString comments = m_commentsList[m_fileIndex];

    int yPos = 30; // Text Y coordinate
    if (m_printName) yPos = 50;

    QStringList commentsByLines;

    uint commentsIndex = 0; // Comments QString index

    while (commentsIndex < (uint) comments.length())
    {
        QString newLine;
        bool breakLine = FALSE; // End Of Line found
        uint currIndex; //  Comments QString current index

        // Check minimal lines dimension

        uint commentsLinesLengthLocal = m_commentsLinesLength;

        for ( currIndex = commentsIndex; currIndex < (uint) comments.length() && !breakLine; currIndex++ )
            if( comments[currIndex] == QChar('\n') || comments[currIndex].isSpace() ) breakLine = TRUE;

        if (commentsLinesLengthLocal <= (currIndex - commentsIndex))
            commentsLinesLengthLocal = (currIndex - commentsIndex);

        breakLine = FALSE;

        for ( currIndex = commentsIndex; currIndex <= commentsIndex + commentsLinesLengthLocal &&
                                        currIndex < (uint) comments.length() &&
                                        !breakLine; currIndex++ )
            {
                breakLine = (comments[currIndex] == QChar('\n')) ? TRUE : FALSE;

                if (breakLine)
                    newLine.append( ' ' );
                else
                newLine.append( comments[currIndex] );
            }

            commentsIndex = currIndex; // The line is ended

        if ( commentsIndex != (uint) comments.length() )
            while ( !newLine.endsWith(" ") )
            {
                newLine.truncate(newLine.length() - 1);
                commentsIndex--;
            }

        commentsByLines.prepend(newLine.trimmed());
    }

    QPainter p;
    p.begin(m_currImage);
    p.setFont(*m_commentsFont);

    for ( int lineNumber = 0; lineNumber < (int)commentsByLines.count(); lineNumber++ ) {

        p.setPen(QColor(m_commentsBgColor));

        // coefficient 1.5 is used to maintain distance between different lines

        for (int x=9; x<=11; x++)
            for (int y = (int)(yPos + lineNumber * 1.5 * m_commentsFont->pointSize()  + 1);
                        y >= (int)(yPos + lineNumber* 1.5 * m_commentsFont->pointSize()  - 1); y--)
                p.drawText(x, height()-y, commentsByLines[lineNumber]);

        p.setPen(QColor(m_commentsFontColor));

        p.drawText(10, height()-(int)(lineNumber * 1.5 * m_commentsFont->pointSize() + yPos), commentsByLines[lineNumber]);
    }
}

void SlideShow::printProgress()
{
    if (!m_currImage) return;

    QPainter p;
    p.begin(m_currImage);

    QString progress(QString::number(m_fileIndex+1) + "/" + QString::number(m_fileList.count()));

    int stringLenght = p.fontMetrics().width(progress) * progress.length();

    p.setPen(QColor(Qt::black));
    for (int x=9; x<=11; x++)
        for (int y=21; y>=19; y--)
            p.drawText(x, height()-y, progress);

    p.setPen(QColor(Qt::white));
    p.drawText(width() - stringLenght - 10, 20, progress);
}

SlideShow::EffectMethod SlideShow::getRandomEffect()
{
    QStringList effs = Effects.keys();
    effs.removeAt(effs.indexOf("None"));

    int count = effs.count();

    int i = rand() % count;
    QString key = effs[i];

    return Effects[key];
}

void SlideShow::showEndOfShow()
{
/*    QPainter p;
    p.begin(this); */
    m_endOfShow = true;

//     p.fillRect(0, 0, width(), height(), Qt::black);
// 
//     QFont fn(font());
//     fn.setPointSize(fn.pointSize()+10);
//     fn.setBold(true);
// 
//     p.setFont(fn);
//     p.setPen(Qt::white);
//     p.drawText(100, 100, i18n("SlideShow Completed."));
//     p.drawText(100, 150, i18n("Click To Exit..."));
//     p.end();
    update(); 
    m_toolBar->setEnabledPlay(false);
    m_toolBar->setEnabledNext(false);
    m_toolBar->setEnabledPrev(false);
}

void SlideShow::keyPressEvent(QKeyEvent *event)
{
    if (!event)
        return;

    m_toolBar->keyPressEvent(event);
}

void SlideShow::mousePressEvent(QMouseEvent *e)
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

void SlideShow::mouseMoveEvent(QMouseEvent *e)
{
    setCursor(QCursor(Qt::ArrowCursor));
    m_mouseMoveTimer->setSingleShot(true);
    m_mouseMoveTimer->start(1000);

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

void SlideShow::wheelEvent(QWheelEvent *e)
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

void SlideShow::slotMouseMoveTimeOut()
{
    QPoint pos(QCursor::pos());
    if ((pos.y() < (m_deskY+20)) ||
        (pos.y() > (m_deskY+m_deskHeight-20-1)))
        return;

    setCursor(QCursor(Qt::BlankCursor));
}

int SlideShow::effectNone(bool /* aInit */)
{
    showCurrentImage();
    return -1;
}


int SlideShow::effectChessboard(bool aInit)
{
    int y;

    if (aInit)
    {
        m_w  = width();
        m_h  = height();
        m_dx = 8;         // width of one tile
        m_dy = 8;         // height of one tile
        m_j  = (m_w+m_dx-1)/m_dx; // number of tiles
        m_x  = m_j*m_dx;    // shrinking x-offset from screen border
        m_ix = 0;         // growing x-offset from screen border
        m_iy = 0;         // 0 or m_dy for growing tiling effect
        m_y  = m_j&1 ? 0 : m_dy; // 0 or m_dy for shrinking tiling effect
        m_wait = 800 / m_j; // timeout between effects
    }

    if (m_ix >= m_w)
    {
        showCurrentImage();
        return -1;
    }

    m_ix += m_dx;
    m_x  -= m_dx;
    m_iy = m_iy ? 0 : m_dy;
    m_y  = m_y ? 0 : m_dy;

    for (y=0; y<m_w; y+=(m_dy<<1))
    {
        //PORT to kde4
#if 0
        bitBlt(this, m_ix, y+m_iy, m_currImage, m_ix, y+m_iy,
                m_dx, m_dy, CopyROP, true);
        bitBlt(this, m_x, y+m_y, m_currImage, m_x, y+m_y,
                m_dx, m_dy, CopyROP, true);
#endif
    m_px = m_ix;
    m_py = y+m_iy;
    m_psx = m_ix;
    m_psy = y+m_iy;
    m_psw = m_dx;
    m_psh = m_dy;
    repaint();

    m_px = m_x;
    m_py = y+m_y;
    m_psx = m_x;
    m_psy = y+m_y;
    m_psw = m_dx;
    m_psh = m_dy;
    repaint();


    }

    return m_wait;
}

int SlideShow::effectMeltdown(bool aInit)
{
    int i, x, y;
    bool done;

    if (aInit)
    {
        delete [] m_intArray;
        m_w = width();
        m_h = height();
        m_dx = 4;
        m_dy = 16;
        m_ix = m_w / m_dx;
        m_intArray = new int[m_ix];
        for (i=m_ix-1; i>=0; i--)
            m_intArray[i] = 0;
    }

    done = true;
    for (i=0,x=0; i<m_ix; i++,x+=m_dx)
    {
        y = m_intArray[i];
        if (y >= m_h) continue;
        done = false;
        if ((rand()&15) < 6) continue;
        //PORT to kde4
#if 0
        bitBlt(this, x, y+m_dy, this, x, y, m_dx, m_h-y-m_dy, CopyROP, true);
        bitBlt(this, x, y, m_currImage, x, y, m_dx, m_dy, CopyROP, true);
#endif
        m_intArray[i] += m_dy;
    }

    if (done)
    {
        delete [] m_intArray;
        m_intArray = NULL;
        return -1;
    }

    return 15;
}

int SlideShow::effectSweep(bool aInit)
{
    int w, h, x, y, i;

    if (aInit)
    {
        // subtype: 0=sweep right to left, 1=sweep left to right
        //          2=sweep bottom to top, 3=sweep top to bottom
        m_subType = rand() % 4;
        m_w  = width();
        m_h  = height();
        m_dx = (m_subType==1 ? 16 : -16);
        m_dy = (m_subType==3 ? 16 : -16);
        m_x  = (m_subType==1 ? 0 : m_w);
        m_y  = (m_subType==3 ? 0 : m_h);
    }

    if (m_subType==0 || m_subType==1)
    {
        // horizontal sweep
        if ((m_subType==0 && m_x < -64) ||
            (m_subType==1 && m_x > m_w+64))
        {
            return -1;
        }
        for (w=2,i=4,x=m_x; i>0; i--, w<<=1, x-=m_dx)
        {
            //PORT to kde4
#if 0
            bitBlt(this, x, 0, m_currImage, x, 0, w, m_h, CopyROP, true);
#endif
        }
        m_x += m_dx;
    }
    else
    {
        // vertical sweep
        if ((m_subType==2 && m_y < -64) ||
            (m_subType==3 && m_y > m_h+64))
        {
            return -1;
        }
        for (h=2,i=4,y=m_y; i>0; i--, h<<=1, y-=m_dy)
        {
            //PORT to kde4
#if 0
            bitBlt(this, 0, y, m_currImage, 0, y, m_w, h, CopyROP, true);
#endif
        }
        m_y += m_dy;
    }

    return 20;
}

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
        //PORT to kde4
#if 0
        bitBlt(this, x, y, m_currImage, x, y, sz, sz, CopyROP, true);
#endif
    }

    showCurrentImage();

    return -1;
}

int SlideShow::effectGrowing(bool aInit)
{
    if (aInit)
    {
        m_w = width();
        m_h = height();
        m_x = m_w >> 1;
        m_y = m_h >> 1;
        m_i = 0;
        m_fx = m_x / 100.0;
        m_fy = m_y / 100.0;
    }

    m_x = (m_w>>1) - (int)(m_i * m_fx);
    m_y = (m_h>>1) - (int)(m_i * m_fy);
    m_i++;

    if (m_x<0 || m_y<0)
    {
        showCurrentImage();
        return -1;
    }
    //PORT to kde4
#if 0
    bitBlt(this, m_x, m_y, m_currImage, m_x, m_y,
            m_w - (m_x<<1), m_h - (m_y<<1), CopyROP, true);
#endif
    return 20;
}

int SlideShow::effectIncom_ingEdges(bool aInit)
{
    int x1, y1;

    if (aInit)
    {
        m_w = width();
        m_h = height();
        m_ix = m_w >> 1;
        m_iy = m_h >> 1;
        m_fx = m_ix / 100.0;
        m_fy = m_iy / 100.0;
        m_i = 0;
        m_subType = rand() & 1;
    }

    m_x = (int)(m_fx * m_i);
    m_y = (int)(m_fy * m_i);

    if (m_x>m_ix || m_y>m_iy)
    {
        showCurrentImage();
        return -1;
    }

    x1 = m_w - m_x;
    y1 = m_h - m_y;
    m_i++;

    if (m_subType)
    {
        //PORT to kde4
#if 0
        // moving image edges
        bitBlt(this,  0,  0, m_currImage, m_ix-m_x, m_iy-m_y, m_x, m_y, CopyROP, true);
        bitBlt(this, x1,  0, m_currImage, m_ix, m_iy-m_y, m_x, m_y, CopyROP, true);
        bitBlt(this,  0, y1, m_currImage, m_ix-m_x, m_iy, m_x, m_y, CopyROP, true);
        bitBlt(this, x1, y1, m_currImage, m_ix, m_iy, m_x, m_y, CopyROP, true);
#endif
    }
    else
    {
        //PORT to kde4
#if 0
        // fixed image edges
        bitBlt(this,  0,  0, m_currImage,  0,  0, m_x, m_y, CopyROP, true);
        bitBlt(this, x1,  0, m_currImage, x1,  0, m_x, m_y, CopyROP, true);
        bitBlt(this,  0, y1, m_currImage,  0, y1, m_x, m_y, CopyROP, true);
        bitBlt(this, x1, y1, m_currImage, x1, y1, m_x, m_y, CopyROP, true);
#endif
    }
    return 20;
}

int SlideShow::effectHorizLines(bool aInit)
{
    static int iyPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
    int y;

    if (aInit)
    {
        m_w = width();
        m_h = height();
        m_i = 0;
    }

    if (iyPos[m_i] < 0) return -1;

    for (y=iyPos[m_i]; y<m_h; y+=8)
    {
        //PORT to kde4
#if 0
        bitBlt(this, 0, y, m_currImage, 0, y, m_w, 1, CopyROP, true);
#endif
    }

    m_i++;
    if (iyPos[m_i] >= 0) return 160;
    return -1;
}

int SlideShow::effectVertLines(bool aInit)
{
    static int ixPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
    int x;

    if (aInit)
    {
        m_w = width();
        m_h = height();
        m_i = 0;
    }

    if (ixPos[m_i] < 0) return -1;

    for (x=ixPos[m_i]; x<m_w; x+=8)
    {
        //PORT to kde4
#if 0
        bitBlt(this, x, 0, m_currImage, x, 0, 1, m_h, CopyROP, true);
#endif
    }

    m_i++;
    if (ixPos[m_i] >= 0) return 160;
    return -1;
}

int SlideShow::effectMultiCircleOut(bool aInit)
{
    int x, y, i;
    double alpha;
    static Q3PointArray pa(4);

    if (aInit)
    {
        startPainter();
	//update();
        m_w = width();
        m_h = height();
        m_x = m_w;
        m_y = m_h>>1;
        pa.setPoint(0, m_w>>1, m_h>>1);
        pa.setPoint(3, m_w>>1, m_h>>1);
        m_fy = sqrt((double)m_w*m_w + m_h*m_h) / 2;
        m_i  = rand()%15 + 2;
        m_fd = M_PI*2/m_i;
        m_alpha = m_fd;
        m_wait = 10 * m_i;
        m_fx = M_PI/32;  // divisor must be powers of 8
    }

    if (m_alpha < 0)
    {
        m_painter->end();
        showCurrentImage();
        return -1;
    }

    for (alpha=m_alpha, i=m_i; i>=0; i--, alpha+=m_fd)
    {
        x = (m_w>>1) + (int)(m_fy * cos(-alpha));
        y = (m_h>>1) + (int)(m_fy * sin(-alpha));

        m_x = (m_w>>1) + (int)(m_fy * cos(-alpha + m_fx));
        m_y = (m_h>>1) + (int)(m_fy * sin(-alpha + m_fx));

        pa.setPoint(1, x, y);
        pa.setPoint(2, m_x, m_y);

        m_painter->drawPolygon(pa);
    }

    m_alpha -= m_fx;

    return m_wait;
}

int SlideShow::effectSpiralIn(bool aInit)
{
    if (aInit)
    {
   //    startPainter();
	update();
        m_w = width();
        m_h = height();
        m_ix = m_w / 8;
        m_iy = m_h / 8;
        m_x0 = 0;
        m_x1 = m_w - m_ix;
        m_y0 = m_iy;
        m_y1 = m_h - m_iy;
        m_dx = m_ix;
        m_dy = 0;
        m_i = 0;
        m_j = 16 * 16;
        m_x = 0;
        m_y = 0;
    }

    if (m_i==0 && m_x0>=m_x1)
    {
        m_painter->end();
        showCurrentImage();
        return -1;
    }

    if (m_i==0 && m_x>=m_x1) // switch to: down on right side
    {
        m_i = 1;
        m_dx = 0;
        m_dy = m_iy;
        m_x1 -= m_ix;
    }
    else if (m_i==1 && m_y>=m_y1) // switch to: right to left on bottom side
    {
        m_i = 2;
        m_dx = -m_ix;
        m_dy = 0;
        m_y1 -= m_iy;
    }
    else if (m_i==2 && m_x<=m_x0) // switch to: up on left side
    {
        m_i = 3;
        m_dx = 0;
        m_dy = -m_iy;
        m_x0 += m_ix;
    }
    else if (m_i==3 && m_y<=m_y0) // switch to: left to right on top side
    {
        m_i = 0;
        m_dx = m_ix;
        m_dy = 0;
        m_y0 += m_iy;
    }
    //PORT to kde4
#if 0
    bitBlt(this, m_x, m_y, m_currImage, m_x, m_y, m_ix, m_iy, CopyROP, true);
#endif
    m_x += m_dx;
    m_y += m_dy;
    m_j--;

    return 8;
}

int SlideShow::effectCircleOut(bool aInit)
{
    int x, y;
    static Q3PointArray pa(4);

    if (aInit)
    {
   //     startPainter();
	update();
	m_w = width();
        m_h = height();
        m_x = m_w;
        m_y = m_h>>1;
        m_alpha = 2*M_PI;
        pa.setPoint(0, m_w>>1, m_h>>1);
        pa.setPoint(3, m_w>>1, m_h>>1);
        m_fx = M_PI/16;  // divisor must be powers of 8
        m_fy = sqrt((double)m_w*m_w + m_h*m_h) / 2;
    }

    if (m_alpha < 0)
    {
        m_painter->end();
        showCurrentImage();
        return -1;
    }

    x = m_x;
    y = m_y;
    m_x = (m_w>>1) + (int)(m_fy * cos(m_alpha));
    m_y = (m_h>>1) + (int)(m_fy * sin(m_alpha));
    m_alpha -= m_fx;

    pa.setPoint(1, x, y);
    pa.setPoint(2, m_x, m_y);

    m_painter->drawPolygon(pa);

    return 20;
}


int SlideShow::effectBlobs(bool aInit)
{
    int r;

    if (aInit)
    {
        startPainter();
	m_alpha = M_PI * 2;
        m_w = width();
        m_h = height();
        m_i = 150;
    }

    if (m_i <= 0)
    {
        m_painter->end();
        showCurrentImage();
        return -1;
    }

    m_x = rand() % m_w;
    m_y = rand() % m_h;
    r = (rand() % 200) + 50;

    m_px = m_x-r;
    m_py = m_y-r;
    m_psx = r;
    m_psy = r;

   // update();
    repaint();
    m_i--;

    return 10;
}

void SlideShow::paintEvent(QPaintEvent *)
{ 
    if ( m_startPainter == true ) {
      QBrush brush;
      Qt::PenStyle aPen = Qt::NoPen;
      brush.setTexture(*(m_currImage));

      if (m_painter->isActive())
	  m_painter->end();
      
      m_painter->begin(this);

      m_painter->setBrush(brush);
      m_painter->setPen(aPen);
      m_startPainter = false;
      return;
    }

    QPainter p(this);

    //That's a call from ShowCurrentImage() 
    if ( m_simplyShow == true ) {
      p.drawPixmap(0, 0, *(m_currImage),
		   0, 0, m_currImage->width(), m_currImage->height());
      p.end();
      m_simplyShow = false;
    return;
    }


    if ( m_endOfShow == true ) {
      p.fillRect(0, 0, width(), height(), Qt::black);

      QFont fn(font());
      fn.setPointSize(fn.pointSize()+10);
      fn.setBold(true);

      p.setFont(fn);
      p.setPen(Qt::white);
      p.drawText(100, 100, i18n("SlideShow Completed."));
      p.drawText(100, 150, i18n("Click To Exit..."));
      p.end();
      m_endOfShow = false;
      return;
    }

    //Different behaviour on different effect 
    
    if ( QString::compare(m_effectName, "Chess Board") == 0 ) {
      	p.drawPixmap(m_px, m_py, *m_currImage, m_psx, m_psy, m_psw, m_psh);
	p.end();
    }
    
    if ( QString::compare(m_effectName, "Blobs") == 0 ) {
        m_painter->drawEllipse(m_px, m_py, m_psx, m_psy);
	return;
    }

/*
    switch ( m_effectName ) {
      case "Chess Board" :
	break;*/
      /*case "Melt Down" :
      case "Sweep" :
      case "Noise" :
      case "Growing" :
      case "Incom_ing Edges" :
      case "Horizontal Lines" :
      case "Vertical Lines" :
      case "Circle Out" :
      case "MultiCircle Out" :
      case "Spiral In" :
      case "Blobs" :
    */
//       default :
//     }
}

void SlideShow::startPainter(Qt::PenStyle aPen)
{
    m_startPainter = true;
    //update();
    repaint();
}

void SlideShow::slotPause()
{
    m_timer->stop();

    if (m_toolBar->isHidden())
    {
        int w = m_toolBar->width();
        m_toolBar->move(m_deskWidth-w-1,0);
        m_toolBar->show();
    }
}

void SlideShow::slotPlay()
{
    m_toolBar->hide();
    slotTimeOut();
}

void SlideShow::slotPrev()
{
    loadPrevImage();
    if (!m_currImage || m_fileList.isEmpty())
    {
        showEndOfShow();
        return;
    }
    m_effectRunning = false;
    showCurrentImage();
}

void SlideShow::slotNext()
{
    loadNextImage();
    if (!m_currImage || m_fileList.isEmpty())
    {
        showEndOfShow();
        return;
    }
    m_effectRunning = false;
    showCurrentImage();
}

void SlideShow::slotClose()
{
    close();
}

}  // NameSpace KIPISlideShowPlugin
