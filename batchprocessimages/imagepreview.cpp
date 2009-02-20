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
#include <q3whatsthis.h>
#include <qapplication.h>
#include <qcursor.h>

// KDE includes

#include <k3process.h>
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
    setCaption(i18n("Batch Process Preview (%1 - %2)").arg(EffectName).arg(FileName));
    setModal(true);
    setButtons(Ok | Help);
    setDefaultButton(Ok);
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Batch process images"),
                                           0,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("An interface to preview the \"Batch Process Images\" "
                                                     "Kipi plugin.\n"
                                                     "This plugin uses the \"convert\" program from \"ImageMagick\" "
                                                     "package."),
                                           "(c) 2003-2004, Gilles Caulier");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at gmail dot com");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbooks"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

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
    Q3WhatsThis::add( LCDZoomFactorValue, i18n("<p>The zoom factor value, as a percentage."));

    ZoomFactorSlider = new QSlider (1, 20, 1, INIT_ZOOM_FACTOR, Qt::Horizontal,
                                    groupBoxZoomFactor, "ZoomFactorSlider");
    ZoomFactorSlider->setTracking ( false );
    ZoomFactorSlider->setTickInterval ( 5 );
    Q3WhatsThis::add( ZoomFactorSlider, i18n("<p>Moving this slider changes the zoom factor value."));
    g1->addWidget( groupBoxZoomFactor, 0, 0);

    Q3GridLayout* g2 = new Q3GridLayout( v1, 1, 2 );
    Q3GroupBox * groupBox1 = new Q3GroupBox( 1, Qt::Horizontal, i18n("Original Image"), box );
    m_previewOrig = new PixmapView(cropActionOrig, groupBox1);
    Q3WhatsThis::add( m_previewOrig, i18n("<p>This is the original image preview. You can use the mouse "
                                         "wheel to change the zoom factor. Click in and use the mouse "
                                         "to move the image."));
    g2->addWidget( groupBox1 , 0, 0);

    Q3GroupBox * groupBox2 = new Q3GroupBox( 1, Qt::Horizontal, i18n("Destination Image"), box );
    m_previewDest = new PixmapView(cropActionDest, groupBox2);
    Q3WhatsThis::add( m_previewDest, i18n("<p>This is the destination image preview. You can use the "
                                         "mouse wheel to change the zoom factor. Click in and use the "
                                         "mouse to move the image."));
    g2->setColStretch(0,1);
    g2->setColStretch(1,1);
    g2->addWidget( groupBox2 , 0, 1);

    connect( ZoomFactorSlider, SIGNAL(valueChanged(int)),
             this, SLOT(slotZoomFactorValueChanged(int)) );

    connect( m_previewOrig, SIGNAL(wheelEvent(int)),
             this, SLOT(slotWheelChanged(int)) );

    connect( m_previewDest, SIGNAL(wheelEvent(int)),
             this, SLOT(slotWheelChanged(int)) );

    m_previewOrig->setImage(fileOrig, tmpPath);
    m_previewDest->setImage(fileDest, tmpPath);
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

PixmapView::PixmapView(bool cropAction, QWidget *parent, const char *name)
           : Q3ScrollView(parent, name)
{
    m_cropAction = cropAction;
    m_pix = NULL;
    m_validPreview = false;
    setMinimumSize(QSize(300,300));
    horizontalScrollBar()->setLineStep( 1 );
    horizontalScrollBar()->setPageStep( 1 );
    verticalScrollBar()->setLineStep( 1 );
    verticalScrollBar()->setPageStep( 1 );
    KGlobal::dirs()->addResourceType("kipi_handcursor", KGlobal::dirs()->kde_default("data")
                   + "kipi/data");
    m_handCursor = new QCursor( KGlobal::dirs()->findResourceDir("kipi_handcursor", "handcursor.png")
                   + "handcursor.png" );
}

PixmapView::~PixmapView()
{
    if(m_pix) delete m_pix;
}

void PixmapView::setImage(const QString &ImagePath, const QString &tmpPath)
{

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
    m_PreviewProc = new K3Process;
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

    connect(m_PreviewProc, SIGNAL(processExited(K3Process *)),
            this, SLOT(PreviewProcessDone(K3Process*)));

    connect(m_PreviewProc, SIGNAL(receivedStdout(K3Process *, char*, int)),
            this, SLOT(slotPreviewReadStd(K3Process*, char*, int)));

    connect(m_PreviewProc, SIGNAL(receivedStderr(K3Process *, char*, int)),
            this, SLOT(slotPreviewReadStd(K3Process*, char*, int)));

    bool result = m_PreviewProc->start(K3Process::NotifyOnExit, K3Process::All);
    if(!result)
    {
        KMessageBox::error(this, i18n("Cannot start 'convert' program from 'ImageMagick' package;\n"
                                      "please check your installation."));
        return;
    }
}

void PixmapView::slotPreviewReadStd(K3Process* /*proc*/, char *buffer, int buflen)
{
    m_previewOutput.append( QString::fromLocal8Bit(buffer, buflen) );
}

void PixmapView::PreviewProcessDone(K3Process* proc)
{
    int ValRet = proc->exitStatus();
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
          m_pix = new QPixmap(visibleWidth(), visibleHeight());
          QPainter p;
          p.begin(m_pix);
          p.fillRect(0, 0, m_pix->width(), m_pix->height(), Qt::white);
          p.setPen(Qt::red);
          p.drawText(0, 0, m_pix->width(), m_pix->height(), Qt::AlignCenter,
                     i18n("Cannot\nprocess\npreview\nimage."));
          p.end();
          repaintContents();
          m_validPreview = false;
          }
       }
}

void PixmapView::resizeImage(int ZoomFactor)
{
    if ( m_validPreview == false) return;

    int w = m_w - (int)((float)m_w * (100-(float)ZoomFactor) / 100);
    int h = m_h - (int)((float)m_h * (100-(float)ZoomFactor) / 100);

    QImage imgTmp = m_img.scale(w, h);
    m_pix->convertFromImage(imgTmp);
    resizeContents(w, h);
    repaintContents(false);
}

void PixmapView::drawContents(QPainter *p, int x, int y, int w, int h)
{
    if(!m_pix) return;
    else p->drawPixmap(x, y, *m_pix, x, y, w, h);
}

void PixmapView::contentsWheelEvent( QWheelEvent * e )
{
    emit wheelEvent(e->delta());
}

void PixmapView::contentsMousePressEvent ( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
       {
       m_xpos = e->x();
       m_ypos = e->y();
       setCursor ( *m_handCursor );
       }
}

void PixmapView::contentsMouseReleaseEvent ( QMouseEvent * /*e*/ )
{
    setCursor ( Qt::ArrowCursor );
}

void PixmapView::contentsMouseMoveEvent( QMouseEvent * e )
{
     if ( e->state() == Qt::LeftButton )
     {
         uint newxpos = e->x();
         uint newypos = e->y();

         scrollBy (-(newxpos - m_xpos), -(newypos - m_ypos));

         m_xpos = newxpos - (newxpos-m_xpos);
         m_ypos = newypos - (newypos-m_ypos);
     }
}

}  // NameSpace KIPIBatchProcessImagesPlugin
