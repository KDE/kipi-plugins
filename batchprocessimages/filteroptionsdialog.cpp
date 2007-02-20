//////////////////////////////////////////////////////////////////////////////
//
//    FILTEROPTIONSDIALOG.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at gmail dot com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Include files for Qt

#include <qvbox.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qcolor.h>

// Include files for KDE

#include <klocale.h>
#include <knuminput.h>
#include <kcolorbutton.h>

// Local includes

#include "filteroptionsdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

FilterOptionsDialog::FilterOptionsDialog(QWidget *parent, int FilterType)
                        : KDialogBase( parent, "FilterOptionsDialog", true,
                          i18n("Filter Options"), Ok|Cancel, Ok, false)
{
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );
    QString whatsThis;

    if (FilterType == 0) // Add noise
       {
       QLabel *m_label_noiseType = new QLabel (i18n("Noise algorithm:"), box);
       dvlay->addWidget( m_label_noiseType );
       m_noiseType = new QComboBox( false, box );
       m_noiseType->insertItem(i18n("Uniform"));
       m_noiseType->insertItem(i18n("Gaussian"));
       m_noiseType->insertItem(i18n("Multiplicative"));
       m_noiseType->insertItem(i18n("Impulse"));
       m_noiseType->insertItem(i18n("Laplacian"));
       m_noiseType->insertItem(i18n("Poisson"));
       QWhatsThis::add( m_noiseType, i18n("<p>Select here the algorithm method which will used "
                                          "to add random noise to the images.") );
       m_label_noiseType->setBuddy( m_noiseType );
       dvlay->addWidget( m_noiseType );
       }

    if (FilterType == 2) // Blur
       {
       QLabel *m_label_blurRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_blurRadius );
       m_blurRadius = new KIntNumInput(3, box);
       m_blurRadius->setRange(0, 20, 1, true );
       QWhatsThis::add( m_blurRadius, i18n("<p>Select here the blur radius of the Gaussian, in pixels, "
                                           "not counting the center pixel. For reasonable results, the "
                                           "radius should be larger than deviation. If you use a radius of 0 "
                                           "the blur operations selects a suitable radius.") );
       m_label_blurRadius->setBuddy( m_blurRadius );
       dvlay->addWidget( m_blurRadius );

       QLabel *m_label_blurDeviation = new QLabel (i18n("Deviation:"), box);
       dvlay->addWidget( m_label_blurDeviation );
       m_blurDeviation = new KIntNumInput(1, box);
       m_blurDeviation->setRange(0, 20, 1, true );
       QWhatsThis::add( m_blurDeviation, i18n("<p>Select here the standard deviation of the blur Gaussian, "
                                              "in pixels.") );
       m_label_blurDeviation->setBuddy( m_blurDeviation );
       dvlay->addWidget( m_blurDeviation );
       }

    if (FilterType == 5) // Median
       {
       QLabel *m_label_medianRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_medianRadius );
       m_medianRadius = new KIntNumInput(3, box);
       m_medianRadius->setRange(0, 20, 1, true );
       QWhatsThis::add( m_medianRadius, i18n("<p>Select here the median radius of the pixel neighborhood. "
                                             "The algorithm applies a digital filter that improves the quality "
                                             "of noisy images. Each pixel is replaced by the median in a "
                                             "set of neighboring pixels as defined by the radius.") );
       m_label_medianRadius->setBuddy( m_medianRadius );
       dvlay->addWidget( m_medianRadius );
       }


    if (FilterType == 6) // Noise reduction
       {
       QLabel *m_label_noiseRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_noiseRadius );
       m_noiseRadius = new KIntNumInput(3, box);
       m_noiseRadius->setRange(0, 20, 1, true );
       QWhatsThis::add( m_noiseRadius, i18n("<p>Select here the noise reduction radius value, in pixels. "
                                            "The algorithm smooths the contours of an image while still "
                                            "preserving edge information. The algorithm works by replacing "
                                            "each pixel with its neighbor closest in value. A neighbor is "
                                            "defined by the radius. If you use a radius of 0 the algorithm "
                                            "selects a suitable radius.") );
       m_label_noiseRadius->setBuddy( m_noiseRadius );
       dvlay->addWidget( m_noiseRadius );
       }

    if (FilterType == 7) // Sharpen
       {
       QLabel *m_label_sharpenRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_sharpenRadius );
       m_sharpenRadius = new KIntNumInput(3, box);
       m_sharpenRadius->setRange(0, 20, 1, true );
       QWhatsThis::add( m_sharpenRadius, i18n("<p>Select here the radius of the sharpen Gaussian, in "
                                              "pixels, not counting the center pixel. For reasonable "
                                              "results, the radius should be larger than deviation. "
                                              "if you use a radius of 0 the sharpen operation selects a "
                                              "suitable radius.") );
       m_label_sharpenRadius->setBuddy( m_sharpenRadius );
       dvlay->addWidget( m_sharpenRadius );

       QLabel *m_label_sharpenDeviation = new QLabel (i18n("Deviation:"), box);
       dvlay->addWidget( m_label_sharpenDeviation );
       m_sharpenDeviation = new KIntNumInput(1, box);
       m_sharpenDeviation->setRange(0, 20, 1, true );
       QWhatsThis::add( m_sharpenDeviation, i18n("<p>Select here the sharpen deviation value of the "
                                                 "Laplacian in pixels.") );
       m_label_sharpenDeviation->setBuddy( m_sharpenDeviation );
       dvlay->addWidget( m_sharpenDeviation );
       }

    if (FilterType == 8) // Unsharp
       {
       QLabel *m_label_unsharpenRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_unsharpenRadius );
       m_unsharpenRadius = new KIntNumInput(3, box);
       m_unsharpenRadius->setRange(0, 20, 1, true );
       QWhatsThis::add( m_unsharpenRadius, i18n("<p>Select here the radius of the unsharpen Gaussian, "
                                                "in pixels, not counting the center pixel. The algorithm "
                                                "convolve the image with a Gaussian operator of the given "
                                                "radius and standard deviation. For reasonable results, "
                                                "radius should be larger than sigma. If you use a radius of 0 "
                                                "the algorithm selects a suitable radius.") );
       m_label_unsharpenRadius->setBuddy( m_unsharpenRadius );
       dvlay->addWidget( m_unsharpenRadius );

       QLabel *m_label_unsharpenDeviation = new QLabel (i18n("Deviation:"), box);
       dvlay->addWidget( m_label_unsharpenDeviation );
       m_unsharpenDeviation = new KIntNumInput(1, box);
       m_unsharpenDeviation->setRange(0, 20, 1, true );
       QWhatsThis::add( m_unsharpenDeviation, i18n("<p>Select here the unsharpen deviation value of the "
                                                   "Gaussian, in pixels.") );
       m_label_unsharpenDeviation->setBuddy( m_unsharpenDeviation );
       dvlay->addWidget( m_unsharpenDeviation );

       QLabel *m_label_unsharpenPercent = new QLabel (i18n("Percent:"), box);
       dvlay->addWidget( m_label_unsharpenPercent );
       m_unsharpenPercent = new KIntNumInput(3, box);
       m_unsharpenPercent->setRange(1, 100, 1, true );
       QWhatsThis::add( m_unsharpenPercent, i18n("<p>Select here the percentage difference between original "
                                                 "and blurred image which should be added to original.") );
       m_label_unsharpenPercent->setBuddy( m_unsharpenPercent );
       dvlay->addWidget( m_unsharpenPercent );

       QLabel *m_label_unsharpenThreshold = new QLabel (i18n("Threshold:"), box);
       dvlay->addWidget( m_label_unsharpenThreshold );
       m_unsharpenThreshold = new KIntNumInput(1, box);
       m_unsharpenThreshold->setRange(0, 20, 1, true );
       QWhatsThis::add( m_unsharpenThreshold, i18n("<p>Select here the unsharpen threshold value, in "
                                                   "pixels, needed to apply the diffence amount."));
       m_label_unsharpenThreshold->setBuddy( m_unsharpenThreshold );
       dvlay->addWidget( m_unsharpenThreshold );
       }
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

FilterOptionsDialog::~FilterOptionsDialog()
{
}

}  // NameSpace KIPIBatchProcessImagesPlugin

#include "filteroptionsdialog.moc"
