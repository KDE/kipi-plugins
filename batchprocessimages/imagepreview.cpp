//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QWheelEvent>
#include <Q3GridLayout>
#include <QMouseEvent>
#include <Q3VBoxLayout>
/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
 
// C Ansi includes

extern "C"
{
#include <unistd.h>
}

// C++ includes.

#include <cstdlib>

// Qt includes

#include <qpixmap.h>
#include <qpushbutton.h>
#include <q3vbox.h>
#include <qlayout.h>
#include <q3groupbox.h>
#include <qlabel.h>
#include <qslider.h>
#include <qlcdnumber.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qcursor.h>
#include <QScrollBar>

// KDE includes

#include <kprocess.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <kio/global.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kdeversion.h>
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "imagepreview.h"
#include "imagepreview.moc"

namespace KIPIBatchProcessImagesPlugin
{

int INIT_ZOOM_FACTOR;

ImagePreview::ImagePreview(const QString &fileOrig, const QString &fileDest, const QString &tmpPath,
                           bool cropActionOrig, bool cropActionDest, const QString &EffectName,
                           const QString &FileName, QWidget *parent)
            : KDialog( parent)
{
    setCaption(i18n("Batch Process Preview (%1 - %2)",EffectName,FileName));
    setModal(true);
    setButtons(Ok | Help);
    setDefaultButton(Ok);
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch process images"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("An interface to preview the \"Batch Process Images\" "
                                                     "Kipi plugin.\n"
                                                     "This plugin uses the \"convert\" program from \"ImageMagick\" "
                                                     "package."),
                                           ki18n("(c) 2003-2004, Gilles Caulier"));

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author and maintainer"),
                     "caulier dot gilles at gmail dot com");

    DialogUtils::setupHelpButton(this, m_about);

    //---------------------------------------------

    QWidget* box = new QWidget( this );
    setMainWidget(box);
    resize(700, 400);

    if ( cropActionOrig == true || cropActionDest == true )
        INIT_ZOOM_FACTOR = 20;
    else
        INIT_ZOOM_FACTOR = 5;

    Q3VBoxLayout* ml = new Q3VBoxLayout( box, 10 );

    //---------------------------------------------

    Q3HBoxLayout* h1 = new Q3HBoxLayout( ml );
    Q3VBoxLayout* v1 = new Q3VBoxLayout( h1 );
    h1->addSpacing( 5 );
    Q3GridLayout* g1 = new Q3GridLayout( v1, 1, 2 );

    Q3GroupBox * groupBoxZoomFactor = new Q3GroupBox( 2, Qt::Horizontal, i18n("Zoom Factor"), box );
    LCDZoomFactorValue = new QLCDNumber (4, groupBoxZoomFactor, "ZoomFactorLCDvalue");
    LCDZoomFactorValue->setSegmentStyle ( QLCDNumber::Flat );
    LCDZoomFactorValue->display( QString::number(INIT_ZOOM_FACTOR * 5) );
    LCDZoomFactorValue->setWhatsThis(i18n("<p>The zoom factor value, as a percentage."));

    ZoomFactorSlider = new QSlider (1, 20, 1, INIT_ZOOM_FACTOR, Qt::Horizontal,
                                    groupBoxZoomFactor, "ZoomFactorSlider");
    ZoomFactorSlider->setTracking ( false );
    ZoomFactorSlider->setTickInterval ( 5 );
    ZoomFactorSlider->setWhatsThis(i18n("<p>Moving this slider changes the zoom factor value."));
    g1->addWidget( groupBoxZoomFactor, 0, 0);

    Q3GridLayout* g2 = new Q3GridLayout( v1, 1, 2 );
    Q3GroupBox * groupBox1 = new Q3GroupBox( 1, Qt::Horizontal, i18n("Original Image"), box );
    m_previewOrig = new PixmapView(groupBox1);
    m_previewOrig->setWhatsThis(i18n("<p>This is the original image preview. You can use the mouse "
                                         "wheel to change the zoom factor. Click in and use the mouse "
                                         "to move the image."));
    g2->addWidget( groupBox1 , 0, 0);

    Q3GroupBox * groupBox2 = new Q3GroupBox( 1, Qt::Horizontal, i18n("Destination Image"), box );
    m_previewDest = new PixmapView(groupBox2);
    m_previewDest->setWhatsThis(i18n("<p>This is the destination image preview. You can use the "
                                         "mouse wheel to change the zoom factor. Click in and use the "
                                         "mouse to move the image."));
    g2->setColStretch(0,1);
    g2->setColStretch(1,1);
    g2->addWidget( groupBox2 , 0, 1);

    connect( ZoomFactorSlider, SIGNAL(valueChanged(int)),
             this, SLOT(slotZoomFactorValueChanged(int)) );

    connect( m_previewOrig, SIGNAL(wheelChanged(int)),
             this, SLOT(slotWheelChanged(int)) );

    connect( m_previewDest, SIGNAL(wheelChanged(int)),
             this, SLOT(slotWheelChanged(int)) );

    m_previewOrig->setImage(fileOrig, tmpPath, cropActionOrig);
    m_previewDest->setImage(fileDest, tmpPath, cropActionDest);
}

ImagePreview::~ImagePreview()
{
    delete m_about;
}

void ImagePreview::slotHelp( void )
{
    KToolInvocation::invokeHelp("", "kipi-plugins");
}

void ImagePreview::slotWheelChanged( int delta )
{
    if ( delta > 0 )
        ZoomFactorSlider->setValue ( ZoomFactorSlider->value() - 1 );
    else
        ZoomFactorSlider->setValue ( ZoomFactorSlider->value() + 1 );

    slotZoomFactorValueChanged( ZoomFactorSlider->value() );
}

void ImagePreview::slotZoomFactorValueChanged( int ZoomFactorValue )
{
    LCDZoomFactorValue->display( QString::number(ZoomFactorValue * 5) );

    m_previewOrig->resizeImage( ZoomFactorValue * 5 );
    m_previewDest->resizeImage( ZoomFactorValue * 5 );
}

PixmapView::PixmapView(QWidget *parent)
           : QAbstractScrollArea(parent)
{
    m_pix = NULL;
    m_validPreview = false;
    setMinimumSize(QSize(300,300));
    horizontalScrollBar()->setLineStep( 1 );
    horizontalScrollBar()->setPageStep( 1 );
    verticalScrollBar()->setLineStep( 1 );
    verticalScrollBar()->setPageStep( 1 );
    viewport()->setCursor(Qt::OpenHandCursor);
}

PixmapView::~PixmapView()
{
    if(m_pix) delete m_pix;
}

void PixmapView::setImage(const QString &ImagePath, const QString &tmpPath, bool cropAction)
{
    m_cropAction = cropAction;

    m_previewFileName = tmpPath + "/" + QString::number(getpid()) + "-"
                        + QString::number(random()) + "PreviewImage.PNG";

    if (m_cropAction == true)
       PreviewCal(ImagePath, tmpPath);
    else
       {
       if ( m_img.load(ImagePath) == false )
          PreviewCal(ImagePath, tmpPath);
       else
          {
          if(!m_pix) m_pix = new QPixmap(m_img.width(), m_img.height());
          m_w = m_img.width();
          m_h = m_img.height();
          m_validPreview = true;
          resizeImage( INIT_ZOOM_FACTOR *5 );
          horizontalScrollBar()->setLineStep(1);
          verticalScrollBar()->setLineStep(1);
          }
       }
}

void PixmapView::PreviewCal(const QString &ImagePath, const QString &/*tmpPath*/)
{
    m_pix = new QPixmap(300, 300);
    QPainter p;
    p.begin(m_pix);
    p.fillRect(0, 0, m_pix->width(), m_pix->height(), Qt::white);
    p.setPen(Qt::green);
    p.drawText(0, 0, m_pix->width(), m_pix->height(), Qt::AlignCenter,
               i18n("Preview\nimage\nprocessing\nin\nprogress..."));
    p.end();

    m_previewOutput ="convert";
    m_PreviewProc = new KProcess;
    m_PreviewProc->setOutputChannelMode(KProcess::MergedChannels);
    *m_PreviewProc << "convert";
    *m_PreviewProc << "-verbose";

    if (m_cropAction == true)
       {
       *m_PreviewProc << "-crop" << "300x300+0+0";
       m_previewOutput.append( " -crop 300x300+0+0 " );
       }

    *m_PreviewProc << ImagePath;
    *m_PreviewProc << m_previewFileName;
    m_previewOutput.append( " -verbose " + ImagePath + " " + m_previewFileName + "\n\n");

    connect(m_PreviewProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(slotPreviewProcessFinished()));

    connect(m_PreviewProc, SIGNAL(readyRead()), SLOT(slotPreviewReadyRead()));

    m_PreviewProc->start();
    if(!m_PreviewProc->waitForStarted())
    {
        KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package;\n"
                                      "please check your installation."));
        return;
    }
}

void PixmapView::slotPreviewReadyRead()
{
    QByteArray output = m_PreviewProc->readAll();
    m_previewOutput.append( QString::fromLocal8Bit(output.data(), output.size()) );
}

void PixmapView::slotPreviewProcessFinished()
{
    int ValRet = m_PreviewProc->exitCode();
    kDebug (51000) << "Convert exit (" << ValRet << ")" << endl;

    if ( ValRet == 0 )
       {
       if ( m_img.load( m_previewFileName ) == true )
          {
          if(!m_pix) m_pix = new QPixmap(300, 300);
          m_w = m_img.width();
          m_h = m_img.height();
          m_validPreview = true;
          resizeImage( INIT_ZOOM_FACTOR * 5 );
          horizontalScrollBar()->setLineStep(1);
          verticalScrollBar()->setLineStep(1);
          KUrl deletePreviewImage( m_previewFileName );

#if KDE_VERSION >= 0x30200
          KIO::NetAccess::del( deletePreviewImage, kapp->activeWindow() );
#else
          KIO::NetAccess::del( deletePreviewImage );
#endif
          }
       else
          {
          m_pix = new QPixmap(viewport()->size());
          QPainter p;
          p.begin(m_pix);
          p.fillRect(0, 0, m_pix->width(), m_pix->height(), Qt::white);
          p.setPen(Qt::red);
          p.drawText(0, 0, m_pix->width(), m_pix->height(), Qt::AlignCenter,
                     i18n("Cannot\nprocess\npreview\nimage."));
          p.end();
          viewport()->update();
          m_validPreview = false;
          }
       }
}

void PixmapView::resizeImage(int ZoomFactor)
{
    if ( m_validPreview == false) return;

    int w = m_w - (int)((float)m_w * (100-(float)ZoomFactor) / 100);
    int h = m_h - (int)((float)m_h * (100-(float)ZoomFactor) / 100);

    QImage imgTmp = m_img.scaled(w, h);
    m_pix->convertFromImage(imgTmp);
    updateScrollBars();
    viewport()->update();
}

void PixmapView::paintEvent(QPaintEvent*)
{
    if(!m_pix) return;
    QPainter painter(viewport());
    const int x = horizontalScrollBar()->value();
    const int y = verticalScrollBar()->value();
    painter.drawPixmap(0, 0, *m_pix, x, y, viewport()->width(), viewport()->height());
}

void PixmapView::resizeEvent(QResizeEvent*)
{
    updateScrollBars();
}

void PixmapView::contentsWheelEvent( QWheelEvent * e )
{
    emit wheelChanged(e->delta());
}

void PixmapView::mousePressEvent ( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton ) {
        viewport()->setCursor(Qt::ClosedHandCursor);
        m_dragPos = e->pos();
    }
}

void PixmapView::mouseReleaseEvent ( QMouseEvent * /*e*/ )
{
    viewport()->setCursor(Qt::OpenHandCursor);
}

void PixmapView::mouseMoveEvent( QMouseEvent * e )
{
    if ( e->state() == Qt::LeftButton ) {
        QPoint delta = e->pos() - m_dragPos;
        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() - delta.y());
        m_dragPos = e->pos();
    }
}

void PixmapView::updateScrollBars()
{
    horizontalScrollBar()->setMaximum(m_pix->width() - viewport()->width());
    verticalScrollBar()->setMaximum(m_pix->height() - viewport()->height());
}

}  // NameSpace KIPIBatchProcessImagesPlugin
