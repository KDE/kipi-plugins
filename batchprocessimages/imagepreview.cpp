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

// KDE includes

#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "pixmapview.h"
#include "imagepreview.moc"

namespace KIPIBatchProcessImagesPlugin
{

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

    m_previewOrig->setZoom(INIT_ZOOM_FACTOR * 5);
    m_previewDest->setZoom(INIT_ZOOM_FACTOR * 5);

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

    m_previewOrig->setZoom( ZoomFactorValue * 5 );
    m_previewDest->setZoom( ZoomFactorValue * 5 );
}

}  // NameSpace KIPIBatchProcessImagesPlugin
