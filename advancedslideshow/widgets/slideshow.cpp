/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-02-16
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2006-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
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

#include "slideshow.moc"

// C++ includes

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Qt includes

#include <QCursor>
#include <QFont>
#include <QKeyEvent>
#include <QMatrix>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPolygon>
#include <QSvgRenderer>
#include <QTimer>
#include <QWheelEvent>

// KDE includes

#include <kapplication.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kstandarddirs.h>

// libkipi includes

#include <libkipi/interface.h>

// Local includes

#include "commoncontainer.h"
#include "slideplaybackwidget.h"
#include "slideshowloader.h"
#include "playbackwidget.h"
#include "kpimageinfo.h"

namespace KIPIAdvancedSlideshowPlugin
{

SlideShow::SlideShow(const FileList& fileList, const QStringList& commentsList, SharedContainer* const sharedData)
    : QWidget(0, Qt::WindowStaysOnTopHint | Qt::Popup | Qt::X11BypassWindowManagerHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_sharedData   = sharedData;
    QRect deskRect = KGlobalSettings::desktopGeometry( kapp->activeWindow() );
    m_deskX        = deskRect.x();
    m_deskY        = deskRect.y();
    m_deskWidth    = deskRect.width();
    m_deskHeight   = deskRect.height();

    move( m_deskX, m_deskY );
    resize( m_deskWidth, m_deskHeight );

    m_slidePlaybackWidget = new SlidePlaybackWidget( this );
    m_slidePlaybackWidget->hide();
    m_slidePlaybackWidget->move(m_deskWidth - m_slidePlaybackWidget->width(), m_deskY);

    if ( !m_sharedData->loop )
    {
        m_slidePlaybackWidget->setEnabledPrev( false );
    }

    // -- playback widget -------------------------------

    m_playbackWidget = new PlaybackWidget(this, m_sharedData->soundtrackUrls, m_sharedData);
    m_playbackWidget->hide();
    m_playbackWidget->move(m_deskX, m_deskY);

    connect(m_slidePlaybackWidget, SIGNAL(signalPause()),
            this, SLOT(slotPause()));

    connect(m_slidePlaybackWidget, SIGNAL(signalPlay()),
            this, SLOT(slotPlay()));

    connect(m_slidePlaybackWidget, SIGNAL(signalNext()),
            this, SLOT(slotNext()));

    connect(m_slidePlaybackWidget, SIGNAL(signalPrev()),
            this, SLOT(slotPrev()));

    connect(m_slidePlaybackWidget, SIGNAL(signalClose()),
            this, SLOT(slotClose()));

    // ---------------------------------------------------------------

    m_fileIndex     = -1; // start with -1
    m_effect        = 0;
    m_effectRunning = false;
    m_intArray      = 0;
    m_endOfShow     = false;
    m_simplyShow    = false;
    m_startPainter  = false;

    m_timer = new QTimer(this);

    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(slotTimeOut()));

    m_pa     = QPolygon( 4 );
    m_buffer = QPixmap( size() );
    m_buffer.fill( Qt::black );

    m_fileList     = fileList;
    m_commentsList = commentsList;

    if ( m_sharedData->enableCache )
        m_cacheSize  = m_sharedData->cacheSize;
    else
        m_cacheSize = 1;

    m_imageLoader = new SlideShowLoader( m_fileList, m_cacheSize, width(), height(), m_sharedData, m_fileIndex);

    // --------------------------------------------------

    registerEffects();

    if (m_sharedData->effectName == "Random")
    {
        m_effect = getRandomEffect();
    }
    else
    {
        m_effectName = m_sharedData->effectName;
        m_effect     = Effects[m_sharedData->effectName];

        if (!m_effect)
        {
            m_effect     = Effects["None"];
            m_effectName = "None";
        }
    }

    m_timer->setSingleShot( true );
    m_timer->start( 10 );

    // -- hide cursor when not moved --------------------

    m_mouseMoveTimer = new QTimer;

    connect( m_mouseMoveTimer, SIGNAL(timeout()),
             SLOT(slotMouseMoveTimeOut()) );

    setMouseTracking( true );
    slotMouseMoveTimeOut();

}

SlideShow::~SlideShow()
{
    m_timer->stop();
    delete m_timer;
    m_mouseMoveTimer->stop();
    delete m_mouseMoveTimer;

    if ( m_intArray )
        delete [] m_intArray;

    delete m_imageLoader;
}

void SlideShow::readSettings()
{
}

void SlideShow::registerEffects()
{
    Effects.insert( "None", &SlideShow::effectNone );
    Effects.insert( "Chess Board", &SlideShow::effectChessboard );
    Effects.insert( "Melt Down", &SlideShow::effectMeltdown );
    Effects.insert( "Sweep", &SlideShow::effectSweep );
    Effects.insert( "Mosaic", &SlideShow::effectMosaic );
    Effects.insert( "Cubism", &SlideShow::effectCubism );
    Effects.insert( "Growing", &SlideShow::effectGrowing );
    Effects.insert( "Horizontal Lines", &SlideShow::effectHorizLines );
    Effects.insert( "Vertical Lines", &SlideShow::effectVertLines );
    Effects.insert( "Circle Out", &SlideShow::effectCircleOut );
    Effects.insert( "MultiCircle Out", &SlideShow::effectMultiCircleOut );
    Effects.insert( "Spiral In", &SlideShow::effectSpiralIn );
    Effects.insert( "Blobs", &SlideShow::effectBlobs );
}

QStringList SlideShow::effectNames()
{
    QStringList effects;

    effects.append( "None" );
    effects.append( "Chess Board" );
    effects.append( "Melt Down" );
    effects.append( "Sweep" );
    effects.append( "Mosaic" );
    effects.append( "Cubism" );
    effects.append( "Growing" );
    effects.append( "Horizontal Lines" );
    effects.append( "Vertical Lines" );
    effects.append( "Circle Out" );
    effects.append( "MultiCircle Out" );
    effects.append( "Spiral In" );
    effects.append( "Blobs" );
    effects.append( "Random" );

    return effects;
}

QMap<QString, QString> SlideShow::effectNamesI18N()
{
    QMap<QString, QString> effects;

    effects["None"]             = i18nc("Filter Effect: No effect",        "None" );
    effects["Chess Board"]      = i18nc("Filter Effect: Chess Board",      "Chess Board" );
    effects["Melt Down"]        = i18nc("Filter Effect: Melt Down",        "Melt Down" );
    effects["Sweep"]            = i18nc("Filter Effect: Sweep",            "Sweep" );
    effects["Mosaic"]           = i18nc("Filter Effect: Mosaic",           "Mosaic" );
    effects["Cubism"]           = i18nc("Filter Effect: Cubism",           "Cubism" );
    effects["Growing"]          = i18nc("Filter Effect: Growing",          "Growing" );
    effects["Horizontal Lines"] = i18nc("Filter Effect: Horizontal Lines", "Horizontal Lines" );
    effects["Vertical Lines"]   = i18nc("Filter Effect: Vertical Lines",   "Vertical Lines" );
    effects["Circle Out"]       = i18nc("Filter Effect: Circle Out",       "Circle Out" );
    effects["MultiCircle Out"]  = i18nc("Filter Effect: Multi-Circle Out", "Multi-Circle Out" );
    effects["Spiral In"]        = i18nc("Filter Effect: Spiral In",        "Spiral In" );
    effects["Blobs"]            = i18nc("Filter Effect: Blobs",            "Blobs" );
    effects["Random"]           = i18nc("Filter Effect: Random effect",    "Random" );

    return effects;
}

void SlideShow::slotTimeOut()
{
    if ( !m_effect ) return;                       // No effect -> bye !

    int tmout = -1;

    if ( m_effectRunning )                         // Effect under progress ?
    {
        tmout = ( this->*m_effect )( false );
    }
    else
    {
        loadNextImage();

        if ( m_currImage.isNull() || m_fileList.isEmpty() ) // End of slideshow ?
        {
            showEndOfShow();
            return;
        }

        if ( m_sharedData->effectName  == "Random" )            // Take a random effect.
        {
            m_effect = getRandomEffect();

            if ( !m_effect ) return;
        }

        m_effectRunning = true;

        tmout = ( this->*m_effect )( true );
    }

    if ( tmout <= 0 )                             // Effect finished -> delay.
    {
        tmout           = m_sharedData->delay;
        m_effectRunning = false;
    }

    m_timer->setSingleShot( true );

    m_timer->start( tmout );
}

void SlideShow::loadNextImage()
{
    if ( !m_currImage.isNull() )
    {
        m_buffer = m_currImage;
    }
    else
    {
        m_buffer = QPixmap( size() );
        m_buffer.fill( Qt::black );
    }

    m_fileIndex++;

    m_imageLoader->next();
    int num = m_fileList.count();

    if ( m_fileIndex >= num )
    {
        if ( m_sharedData->loop )
        {
            m_fileIndex = 0;
        }
        else
        {
            m_currImage = QPixmap( 0,0 );
            m_fileIndex = num - 1;
            return;
        }
    }

    if ( !m_sharedData->loop )
    {
        m_slidePlaybackWidget->setEnabledPrev( m_fileIndex > 0 );
        m_slidePlaybackWidget->setEnabledNext( m_fileIndex < num - 1 );
    }

    QPixmap newPixmap = QPixmap( QPixmap::fromImage( m_imageLoader->getCurrent() ) );
    QPixmap pixmap( width(), height() );
    pixmap.fill( Qt::black );
    QPainter p( &pixmap );
    p.drawPixmap(( width() - newPixmap.width() ) / 2,
                 ( height() - newPixmap.height() ) / 2, newPixmap,
                 0, 0, newPixmap.width(), newPixmap.height() );

    m_currImage = QPixmap( pixmap );
}

void SlideShow::loadPrevImage()
{
    m_fileIndex--;
    m_imageLoader->prev();

    int num = m_fileList.count();

    if ( m_fileIndex < 0 )
    {
        if ( m_sharedData->loop )
        {
            m_fileIndex = num - 1;
        }
        else
        {
            m_fileIndex = -1; // set this to -1.
            return;
        }
    }

    if ( !m_sharedData->loop )
    {
        m_slidePlaybackWidget->setEnabledPrev( m_fileIndex > 0 );
        m_slidePlaybackWidget->setEnabledNext( m_fileIndex < num - 1 );
    }

    QPixmap newPixmap = QPixmap( QPixmap::fromImage( m_imageLoader->getCurrent() ) );
    QPixmap pixmap( width(), height() );
    pixmap.fill( Qt::black );
    QPainter p( &pixmap );
    p.drawPixmap(( width() - newPixmap.width() ) / 2,
                 ( height() - newPixmap.height() ) / 2, newPixmap,
                 0, 0, newPixmap.width(), newPixmap.height() );

    m_currImage = QPixmap( pixmap );
}

void SlideShow::showCurrentImage()
{
    if ( m_currImage.isNull() )
        return;

    m_simplyShow = true;

    repaint();
}

void SlideShow::printFilename()
{
    if ( m_currImage.isNull() )
        return;

    QPainter p;

    p.begin( &m_currImage );
    p.setPen( Qt::black );

    for ( int x = 9; x <= 11; ++x )
    {
        for ( int y = 31; y >= 29; y-- )
        {
            p.drawText( x, height() - y, m_imageLoader->currFileName() );
        }
    }

    p.setPen( QColor( Qt::white ) );
    p.drawText( 10, height() - 30, m_imageLoader->currFileName() );
}

void SlideShow::printComments()
{
    if (m_currImage.isNull())
        return;

    KIPIPlugins::KPImageInfo info(m_imageLoader->currPath());
    QString comments = info.description();

    int yPos = 30; // Text Y coordinate

    if (m_sharedData->printFileName)
        yPos = 50;

    QStringList commentsByLines;

    uint commentsIndex = 0; // Comments QString index

    while (commentsIndex < (uint) comments.length())
    {
        QString newLine;
        bool breakLine = false; // End Of Line found
        uint currIndex; //  Comments QString current index

        // Check minimal lines dimension

        uint commentsLinesLengthLocal = m_sharedData->commentsLinesLength;

        for (currIndex = commentsIndex; currIndex < (uint) comments.length() && !breakLine; ++currIndex)
        {
            if (comments[currIndex] == QChar('\n') || comments[currIndex].isSpace())
            {
                breakLine = true;
            }
        }

        if (commentsLinesLengthLocal <= (currIndex - commentsIndex))
            commentsLinesLengthLocal = (currIndex - commentsIndex);

        breakLine = false;

        for (currIndex = commentsIndex; currIndex <= commentsIndex + commentsLinesLengthLocal &&
             currIndex < (uint) comments.length() && !breakLine; ++currIndex)
        {
            breakLine = (comments[currIndex] == QChar('\n')) ? true : false;

            if (breakLine)
                newLine.append(' ');
            else
                newLine.append(comments[currIndex]);
        }

        commentsIndex = currIndex; // The line is ended

        if (commentsIndex != (uint) comments.length())
        {
            while (!newLine.endsWith(' '))
            {
                newLine.truncate(newLine.length() - 1);
                commentsIndex--;
            }
        }

        commentsByLines.prepend(newLine.trimmed());
    }

    QPainter p;

    p.begin(&m_currImage);
    p.setFont(*m_sharedData->captionFont);

    for (int lineNumber = 0; lineNumber < (int) commentsByLines.count(); ++lineNumber)
    {
        p.setPen(QColor(m_sharedData->commentsBgColor));

        // coefficient 1.5 is used to maintain distance between different lines

        for (int x = 9; x <= 11; ++x)
        {
            for (int y = (int) (yPos + lineNumber * 1.5 * m_sharedData->captionFont->pointSize() + 1);
                 y >= (int) (yPos + lineNumber * 1.5 * m_sharedData->captionFont->pointSize() - 1); y--)
            {
                p.drawText(x, height() - y, commentsByLines[lineNumber]);
            }
        }

        p.setPen(QColor(m_sharedData->commentsFontColor));
        p.drawText(10, height() - (int) (lineNumber * 1.5 * m_sharedData->captionFont->pointSize() + yPos),
                   commentsByLines[lineNumber]);
    }
}

void SlideShow::printProgress()
{
    if ( m_currImage.isNull() )
        return;

    QPainter p;
    p.begin( &m_currImage );

    QString progress( QString::number( m_fileIndex + 1 ) + '/' + QString::number( m_fileList.count() ) );

    int stringLength = p.fontMetrics().width( progress ) * progress.length();

    p.setPen( QColor( Qt::black ) );

    for ( int x = 9; x <= 11; ++x )
    {
        for ( int y = 21; y >= 19; y-- )
        {
            p.drawText( width() - stringLength - x, y, progress );
        }
    }

    p.setPen( QColor( Qt::white ) );
    p.drawText( width() - stringLength - 10, 20, progress );
}

SlideShow::EffectMethod SlideShow::getRandomEffect()
{
    QStringList effs = Effects.keys();
    effs.removeAt( effs.indexOf( "None" ) );

    int count    = effs.count();
    int i        = qrand() % count;
    QString key  = effs[i];
    m_effectName = key;

    return Effects[key];
}

void SlideShow::showEndOfShow()
{
    m_endOfShow = true;
    update();

    m_slidePlaybackWidget->setEnabledPlay( false );
    m_slidePlaybackWidget->setEnabledNext( false );
    m_slidePlaybackWidget->setEnabledPrev( false );
}

void SlideShow::keyPressEvent(QKeyEvent* event)
{
    if (!event)
        return;

    m_playbackWidget->keyPressEvent(event);
    m_slidePlaybackWidget->keyPressEvent(event);
}

void SlideShow::mousePressEvent( QMouseEvent* e )
{
    if ( m_endOfShow )
        slotClose();

    if ( e->button() == Qt::LeftButton )
    {
        m_timer->stop();
        m_slidePlaybackWidget->setPaused( true );
        slotNext();
    }
    else if ( e->button() == Qt::RightButton && m_fileIndex - 1 >= 0 )
    {
        m_timer->stop();
        m_slidePlaybackWidget->setPaused( true );
        slotPrev();
    }
}

void SlideShow::mouseMoveEvent( QMouseEvent* e )
{
    setCursor( QCursor( Qt::ArrowCursor ) );
    m_mouseMoveTimer->setSingleShot( true );
    m_mouseMoveTimer->start( 1000 );

    if (!m_slidePlaybackWidget->canHide() || !m_playbackWidget->canHide())
        return;

    QPoint pos( e->pos() );

    if (( pos.y() > ( m_deskY + 20 ) ) &&
            ( pos.y() < ( m_deskY + m_deskHeight - 20 - 1 ) ) )
    {
        if (!m_slidePlaybackWidget->canHide() || !m_playbackWidget->canHide())
        {
            return;
        }
        else
        {
            m_slidePlaybackWidget->hide();
            m_playbackWidget->hide();
        }

        return;
    }

//    int w = m_slidePlaybackWidget->width();
//    int h = m_slidePlaybackWidget->height();
//
//    if ( pos.y() < ( m_deskY + 20 ) )
//    {
//        if ( pos.x() <= ( m_deskX + m_deskWidth / 2 ) )
//            // position top left
//            m_slidePlaybackWidget->move( m_deskX, m_deskY );
//        else
//            // position top right
//            m_slidePlaybackWidget->move( m_deskX + m_deskWidth - w - 1, m_deskY );
//    }
//    else
//    {
//        if ( pos.x() <= ( m_deskX + m_deskWidth / 2 ) )
//            // position bot left
//            m_slidePlaybackWidget->move( m_deskX, m_deskY + m_deskHeight - h - 1 );
//        else
//            // position bot right
//            m_slidePlaybackWidget->move( m_deskX + m_deskWidth - w - 1, m_deskY + m_deskHeight - h - 1 );
//    }

    m_slidePlaybackWidget->show();
    m_playbackWidget->show();
}

void SlideShow::wheelEvent( QWheelEvent* e )
{
    if ( !m_sharedData->enableMouseWheel ) 
        return;

    if ( m_endOfShow )
        slotClose();

    int delta = e->delta();

    if ( delta < 0 )
    {
        m_timer->stop();
        m_slidePlaybackWidget->setPaused( true );
        slotNext();
    }
    else if ( delta > 0 && m_fileIndex - 1 >= 0 )
    {
        m_timer->stop();
        m_slidePlaybackWidget->setPaused( true );
        slotPrev();
    }
}

void SlideShow::slotMouseMoveTimeOut()
{
    QPoint pos( QCursor::pos() );

    if (( pos.y() < ( m_deskY + 20 ) ) ||
            ( pos.y() > ( m_deskY + m_deskHeight - 20 - 1 ) ) )
        return;

    setCursor( QCursor( Qt::BlankCursor ) );
}

int SlideShow::effectNone( bool /* aInit */ )
{
    showCurrentImage();
    return -1;
}

int SlideShow::effectChessboard( bool aInit )
{
    if ( aInit )
    {
        m_w    = width();
        m_h    = height();
        m_dx   = 8;                         // width of one tile
        m_dy   = 8;                         // height of one tile
        m_j    = ( m_w + m_dx - 1 ) / m_dx; // number of tiles
        m_x    = m_j * m_dx;                // shrinking x-offset from screen border
        m_ix   = 0;                         // growing x-offset from screen border
        m_iy   = 0;                         // 0 or m_dy for growing tiling effect
        m_y    = m_j & 1 ? 0 : m_dy;        // 0 or m_dy for shrinking tiling effect
        m_wait = 800 / m_j;                 // timeout between effects
    }

    if ( m_ix >= m_w )
    {
        showCurrentImage();
        return -1;
    }

    m_ix += m_dx;
    m_x  -= m_dx;
    m_iy  = m_iy ? 0 : m_dy;
    m_y   = m_y  ? 0 : m_dy;

    QPainter bufferPainter( &m_buffer );
    QBrush brush = QBrush( m_currImage );

    for ( int y = 0; y < m_w; y += ( m_dy << 1 ) )
    {
        bufferPainter.fillRect( m_ix, y + m_iy, m_dx, m_dy, brush );
        bufferPainter.fillRect( m_x, y + m_y, m_dx, m_dy, brush );
    }

    repaint();

    return m_wait;
}

int SlideShow::effectMeltdown( bool aInit )
{
    int i;

    if ( aInit )
    {
        delete [] m_intArray;
        m_w        = width();
        m_h        = height();
        m_dx       = 4;
        m_dy       = 16;
        m_ix       = m_w / m_dx;
        m_intArray = new int[m_ix];

        for (i = m_ix - 1; i >= 0; --i)
            m_intArray[i] = 0;
    }

    m_pdone = true;

    int y, x;
    QPainter bufferPainter( &m_buffer );

    for ( i = 0, x = 0; i < m_ix; ++i, x += m_dx )
    {
        y = m_intArray[i];

        if ( y >= m_h )
            continue;

        m_pdone = false;

        if (( qrand()&15 ) < 6 )
            continue;

        bufferPainter.drawPixmap(x, y + m_dy, m_buffer, x, y, m_dx, m_h - y - m_dy);
        bufferPainter.drawPixmap(x, y, m_currImage, x, y, m_dx, m_dy);

        m_intArray[i] += m_dy;
    }

    bufferPainter.end();

    repaint();

    if ( m_pdone )
    {
        delete [] m_intArray;
        m_intArray = NULL;
        showCurrentImage();
        return -1;
    }

    return 15;
}

int SlideShow::effectSweep( bool aInit )
{
    if ( aInit )
    {
        // subtype: 0=sweep right to left, 1=sweep left to right
        //          2=sweep bottom to top, 3=sweep top to bottom
        m_subType = qrand() % 4;
        m_w       = width();
        m_h       = height();
        m_dx      = ( m_subType == 1 ? 16 : -16 );
        m_dy      = ( m_subType == 3 ? 16 : -16 );
        m_x       = ( m_subType == 1 ? 0 : m_w );
        m_y       = ( m_subType == 3 ? 0 : m_h );
    }

    if ( m_subType == 0 || m_subType == 1 )
    {
        // horizontal sweep
        if (( m_subType == 0 && m_x < -64 ) || ( m_subType == 1 && m_x > m_w + 64 ) )
        {
            showCurrentImage();
            return -1;
        }

        int w;
        int x;
        int i;
        for ( w = 2, i = 4, x = m_x; i > 0; i--, w <<= 1, x -= m_dx )
        {
            m_px  = x;
            m_py  = 0;
            m_psx = w;
            m_psy = m_h;

            QPainter bufferPainter( &m_buffer );
            bufferPainter.fillRect( m_px, m_py, m_psx, m_psy, QBrush( m_currImage ) );
            bufferPainter.end();

            repaint();
        }

        m_x += m_dx;
    }
    else
    {
        // vertical sweep
        if (( m_subType == 2 && m_y < -64 ) || ( m_subType == 3 && m_y > m_h + 64 ) )
        {
            showCurrentImage();
            return -1;
        }

        int h;
        int y;
        int i;
        for ( h = 2, i = 4, y = m_y; i > 0; i--, h <<= 1, y -= m_dy )
        {
            m_px  = 0;
            m_py  = y;
            m_psx = m_w;
            m_psy = h;

            QPainter bufferPainter( &m_buffer );
            bufferPainter.fillRect( m_px, m_py, m_psx, m_psy, QBrush( m_currImage ) );
            bufferPainter.end();

            repaint();
        }

        m_y += m_dy;
    }

    return 20;
}

int SlideShow::effectMosaic( bool aInit )
{
    int dim    = 10; // Size of a cell (dim x dim)
    int margin = dim + ( int )( dim/4 );

    if ( aInit )
    {
        m_i           = 30; // giri totali
        m_pixelMatrix = new bool*[width()];

        for ( int x=0; x<width(); ++x )
        {
            m_pixelMatrix[x] = new bool[height()];

            for ( int y=0; y<height(); ++y )
            {
                m_pixelMatrix[x][y] = false;
            }
        }
    }

    if ( m_i <= 0 )
    {
        showCurrentImage();
        return -1;
    }

    int w = width();
    int h = height();

    QPainter bufferPainter( &m_buffer );

    for ( int x=0; x<w; x+=( qrand()%margin )+dim )
    {

        for ( int y=0; y<h; y+=( qrand()%margin )+dim )
        {
            if ( m_pixelMatrix[x][y] == true )
            {
                if ( y!=0 ) y--;

                continue;
            }

            bufferPainter.fillRect( x, y, dim, dim, QBrush( m_currImage ) );

            for ( int i=0; i<dim && ( x+i )<w; ++i )
            {
                for ( int j=0; j<dim && ( y+j )<h; ++j )
                {
                    m_pixelMatrix[x+i][y+j] = true;
                }
            }
        }
    }

    bufferPainter.end();
    repaint();
    m_i--;

    return 20;
}

int SlideShow::effectCubism( bool aInit )
{
    if ( aInit )
    {
        m_alpha = M_PI * 2;
        m_w     = width();
        m_h     = height();
        m_i     = 150;
    }

    if ( m_i <= 0 )
    {
        showCurrentImage();
        return -1;
    }

    QPainterPath painterPath;
    QPainter bufferPainter( &m_buffer );

    m_x   = qrand() % m_w;
    m_y   = qrand() % m_h;
    int r = ( qrand()%100 )+100;
    m_px  = m_x - r;
    m_py  = m_y - r;
    m_psx = r;
    m_psy = r;

    QMatrix matrix;
    matrix.rotate(( qrand()%20 )-10 );
    QRect rect( m_px, m_py, m_psx, m_psy );
    bufferPainter.setMatrix( matrix );
    bufferPainter.fillRect( rect, QBrush( m_currImage ) );
    bufferPainter.end();
    repaint();

    m_i--;

    return 10;
}

int SlideShow::effectRandom( bool /*aInit*/ )
{
    m_fileIndex--;

    return -1;
}

int SlideShow::effectGrowing( bool aInit )
{
    if ( aInit )
    {
        m_w  = width();
        m_h  = height();
        m_x  = m_w >> 1;
        m_y  = m_h >> 1;
        m_i  = 0;
        m_fx = m_x / 100.0;
        m_fy = m_y / 100.0;
    }

    m_x = ( m_w >> 1 ) - ( int )( m_i * m_fx );
    m_y = ( m_h >> 1 ) - ( int )( m_i * m_fy );
    m_i++;

    if ( m_x < 0 || m_y < 0 )
    {
        showCurrentImage();
        return -1;
    }

    m_px  = m_x;
    m_py  = m_y;
    m_psx = m_w - ( m_x << 1 );
    m_psy = m_h - ( m_y << 1 );

    QPainter bufferPainter( &m_buffer );
    bufferPainter.fillRect( m_px, m_py, m_psx, m_psy, QBrush( m_currImage ) );
    bufferPainter.end();
    repaint();

    return 20;
}

int SlideShow::effectHorizLines( bool aInit )
{
    static int iyPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };

    if ( aInit )
    {
        m_w = width();
        m_h = height();
        m_i = 0;
    }

    if ( iyPos[m_i] < 0 )
        return -1;

    int iPos;
    int until = m_h;

    QPainter bufferPainter( &m_buffer );
    QBrush brush = QBrush( m_currImage );

    for ( iPos = iyPos[m_i]; iPos < until; iPos += 8 )
        bufferPainter.fillRect( 0, iPos, m_w, 1, brush );

    bufferPainter.end();
    repaint();

    m_i++;

    if ( iyPos[m_i] >= 0 )
        return 160;

    showCurrentImage();

    return -1;
}

int SlideShow::effectVertLines( bool aInit )
{
    static int ixPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };

    if ( aInit )
    {
        m_w = width();
        m_h = height();
        m_i = 0;
    }

    if ( ixPos[m_i] < 0 )
        return -1;

    int iPos;
    int until = m_w;

    QPainter bufferPainter( &m_buffer );
    QBrush brush = QBrush( m_currImage );

    for ( iPos = ixPos[m_i]; iPos < until; iPos += 8 )
        bufferPainter.fillRect( iPos, 0, 1, m_h, brush );

    bufferPainter.end();
    repaint();

    m_i++;

    if ( ixPos[m_i] >= 0 )
        return 160;

    showCurrentImage();

    return -1;
}

int SlideShow::effectMultiCircleOut( bool aInit )
{
    int x, y, i;
    double alpha;

    if ( aInit )
    {
        startPainter();
        m_w     = width();
        m_h     = height();
        m_x     = m_w;
        m_y     = m_h >> 1;
        m_pa.setPoint( 0, m_w >> 1, m_h >> 1 );
        m_pa.setPoint( 3, m_w >> 1, m_h >> 1 );
        m_fy    = sqrt(( double )m_w * m_w + m_h * m_h ) / 2;
        m_i     = qrand() % 15 + 2;
        m_fd    = M_PI * 2 / m_i;
        m_alpha = m_fd;
        m_wait  = 10 * m_i;
        m_fx    = M_PI / 32;  // divisor must be powers of 8
    }

    if ( m_alpha < 0 )
    {
        showCurrentImage();
        return -1;
    }

    for ( alpha = m_alpha, i = m_i; i >= 0; i--, alpha += m_fd )
    {
        x   = ( m_w >> 1 ) + ( int )( m_fy * cos( -alpha ) );
        y   = ( m_h >> 1 ) + ( int )( m_fy * sin( -alpha ) );
        m_x = ( m_w >> 1 ) + ( int )( m_fy * cos( -alpha + m_fx ) );
        m_y = ( m_h >> 1 ) + ( int )( m_fy * sin( -alpha + m_fx ) );

        m_pa.setPoint( 1, x, y );
        m_pa.setPoint( 2, m_x, m_y );

        QPainterPath painterPath;
        painterPath.addPolygon( QPolygon( m_pa ) );

        QPainter bufferPainter( &m_buffer );
        bufferPainter.fillPath( painterPath, QBrush( m_currImage ) );
        bufferPainter.end();

        repaint();
    }

    m_alpha -= m_fx;

    return m_wait;
}

int SlideShow::effectSpiralIn( bool aInit )
{
    if ( aInit )
    {
        update();
        m_w  = width();
        m_h  = height();
        m_ix = m_w / 8;
        m_iy = m_h / 8;
        m_x0 = 0;
        m_x1 = m_w - m_ix;
        m_y0 = m_iy;
        m_y1 = m_h - m_iy;
        m_dx = m_ix;
        m_dy = 0;
        m_i  = 0;
        m_j  = 16 * 16;
        m_x  = 0;
        m_y  = 0;
    }

    if ( m_i == 0 && m_x0 >= m_x1 )
    {
        showCurrentImage();
        return -1;
    }

    if ( m_i == 0 && m_x >= m_x1 ) // switch to: down on right side
    {
        m_i = 1;
        m_dx = 0;
        m_dy = m_iy;
        m_x1 -= m_ix;
    }
    else if ( m_i == 1 && m_y >= m_y1 ) // switch to: right to left on bottom side
    {
        m_i = 2;
        m_dx = -m_ix;
        m_dy = 0;
        m_y1 -= m_iy;
    }
    else if ( m_i == 2 && m_x <= m_x0 ) // switch to: up on left side
    {
        m_i = 3;
        m_dx = 0;
        m_dy = -m_iy;
        m_x0 += m_ix;
    }
    else if ( m_i == 3 && m_y <= m_y0 ) // switch to: left to right on top side
    {
        m_i = 0;
        m_dx = m_ix;
        m_dy = 0;
        m_y0 += m_iy;
    }

    m_px  = m_x;
    m_py  = m_y;
    m_psx = m_ix;
    m_psy = m_iy;

    QPainter bufferPainter( &m_buffer );
    bufferPainter.fillRect( m_px, m_py, m_psx, m_psy, QBrush( m_currImage ) );
    bufferPainter.end();
    repaint();

    m_x += m_dx;
    m_y += m_dy;
    m_j--;

    return 8;
}

int SlideShow::effectCircleOut( bool aInit )
{
    int x, y;

    if ( aInit )
    {
        startPainter();
        m_w     = width();
        m_h     = height();
        m_x     = m_w;
        m_y     = m_h >> 1;
        m_alpha = 2 * M_PI;
        m_pa.setPoint( 0, m_w >> 1, m_h >> 1 );
        m_pa.setPoint( 3, m_w >> 1, m_h >> 1 );
        m_fx    = M_PI / 16;  // divisor must be powers of 8
        m_fy    = sqrt(( double )m_w * m_w + m_h * m_h ) / 2;
    }

    if ( m_alpha < 0 )
    {
        showCurrentImage();
        return -1;
    }

    x        = m_x;
    y        = m_y;
    m_x      = ( m_w >> 1 ) + ( int )( m_fy * cos( m_alpha ) );
    m_y      = ( m_h >> 1 ) + ( int )( m_fy * sin( m_alpha ) );
    m_alpha -= m_fx;

    m_pa.setPoint( 1, x, y );
    m_pa.setPoint( 2, m_x, m_y );

    QPainterPath painterPath;
    painterPath.addPolygon( QPolygon( m_pa ) );
    QPainter bufferPainter( &m_buffer );
    bufferPainter.fillPath( painterPath, QBrush( m_currImage ) );
    bufferPainter.end();
    repaint();

    return 20;
}

int SlideShow::effectBlobs( bool aInit )
{
    int r;

    if ( aInit )
    {
        m_alpha = M_PI * 2;
        m_w     = width();
        m_h     = height();
        m_i     = 150;
    }

    if ( m_i <= 0 )
    {
        showCurrentImage();
        return -1;
    }

    m_x   = qrand() % m_w;
    m_y   = qrand() % m_h;
    r     = ( qrand() % 200 ) + 50;
    m_px  = m_x - r;
    m_py  = m_y - r;
    m_psx = r;
    m_psy = r;

    QPainterPath painterPath;
    painterPath.addEllipse( m_px, m_py, m_psx, m_psy );
    QPainter bufferPainter( &m_buffer );
    bufferPainter.fillPath( painterPath, QBrush( m_currImage ) );
    bufferPainter.end();
    repaint();

    m_i--;

    return 10;
}

void SlideShow::paintEvent( QPaintEvent* )
{

    QPainter p( this );

    if ( m_simplyShow == true )
    {
        if ( m_sharedData->printFileName )
            printFilename();

        if ( m_sharedData->printProgress )
            printProgress();

        if ( m_sharedData->printFileComments && m_sharedData->ImagesHasComments )
            printComments();

        p.drawPixmap( 0, 0, m_currImage,
                      0, 0, m_currImage.width(), m_currImage.height() );

        p.end();

        m_simplyShow = false;

        return;
    }


    if ( m_endOfShow == true )
    {
        p.fillRect( 0, 0, width(), height(), Qt::black );

        QFont fn( font() );
        fn.setPointSize( fn.pointSize() + 10 );
        fn.setBold( true );

        p.setFont( fn );
        p.setPen( Qt::white );
        p.drawText( 100, 100, i18n( "Slideshow Completed" ) );
        p.drawText( 100, 100+10+fn.pointSize(), i18n( "Click to Exit..." ) );

        QSvgRenderer svgRenderer( KStandardDirs::locate("data", "kipi/data/kipi-icon.svg") );
        QPixmap kipiLogoPixmap = QPixmap( width()/6, width()/6 );
        kipiLogoPixmap.fill( Qt::black );
        QPaintDevice* pdp = &kipiLogoPixmap;
        QPainter painter( pdp );
        svgRenderer.render( &painter );

        p.drawPixmap( width()-( width()/12 )-kipiLogoPixmap.width(),
                      height()-( height()/12 )-kipiLogoPixmap.height(),
                      kipiLogoPixmap );

        p.end();
        return;
    }

    // If execution reach this line, an effect is running
    p.drawPixmap( 0,0, m_buffer );
}

void SlideShow::startPainter()
{
    m_startPainter = true;
    repaint();
}

void SlideShow::slotPause()
{
    m_timer->stop();

    if ( m_slidePlaybackWidget->isHidden() )
    {
        int w = m_slidePlaybackWidget->width();
        m_slidePlaybackWidget->move( m_deskWidth - w - 1, 0 );
        m_slidePlaybackWidget->show();
    }
}

void SlideShow::slotPlay()
{
    m_slidePlaybackWidget->hide();
    slotTimeOut();
}

void SlideShow::slotPrev()
{
    loadPrevImage();

    if ( m_currImage.isNull() || m_fileList.isEmpty() )
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

    if ( m_currImage.isNull() || m_fileList.isEmpty() )
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

}  // namespace KIPIAdvancedSlideshowPlugin
