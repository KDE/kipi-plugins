//////////////////////////////////////////////////////////////////////////////
//
//    EFFECTOPTIONSDIALOG.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
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
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

#include "effectoptionsdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

EffectOptionsDialog::EffectOptionsDialog(QWidget *parent, QString EffectType)
                        : KDialogBase( parent, "EffectOptionsDialog", true,
                          i18n("Effect options"), Ok|Cancel, Ok, false)
{
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );
    QString whatsThis;


    if (EffectType == i18n("Adaptive threshold"))
       {
       QLabel *m_label_latWidth = new QLabel (i18n("Width:"), box);
       dvlay->addWidget( m_label_latWidth );
       m_latWidth = new KIntNumInput(50, box);
       m_latWidth->setRange(0, 200, 1, true );
       QWhatsThis::add( m_latWidth, i18n("<p>Select here the value which represents the width "
                                         "of the local neighborhood.") );
       m_label_latWidth->setBuddy( m_latWidth );
       dvlay->addWidget( m_latWidth );

       QLabel *m_label_latHeight = new QLabel (i18n("Height:"), box);
       dvlay->addWidget( m_label_latHeight );
       m_latHeight = new KIntNumInput(50, box);
       m_latHeight->setRange(0, 200, 1, true );
       QWhatsThis::add( m_latHeight, i18n("<p>Select here the value which represents the height of "
                                          "the local neighborhood.") );
       m_label_latHeight->setBuddy( m_latHeight );
       dvlay->addWidget( m_latHeight );

       QLabel *m_label_latOffset = new QLabel (i18n("Offset:"), box);
       dvlay->addWidget( m_label_latOffset );
       m_latOffset = new KIntNumInput(1, box);
       m_latOffset->setRange(0, 200, 1, true );
       QWhatsThis::add( m_latOffset, i18n("<p>Select here the value which represents the mean offset.") );
       m_label_latOffset->setBuddy( m_latOffset );
       dvlay->addWidget( m_latOffset );
       }

    if (EffectType == i18n("Charcoal"))
       {
       QLabel *m_label_charcoalRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_charcoalRadius );
       m_charcoalRadius = new KIntNumInput(3, box);
       m_charcoalRadius->setRange(0, 20, 1, true );
       QWhatsThis::add( m_charcoalRadius, i18n("<p>Select here the value which represents the "
                                               "radius of the pixel neighborhood.") );
       m_label_charcoalRadius->setBuddy( m_charcoalRadius );
       dvlay->addWidget( m_charcoalRadius );

       QLabel *m_label_charcoalDeviation = new QLabel (i18n("Deviation:"), box);
       dvlay->addWidget( m_label_charcoalDeviation );
       m_charcoalDeviation = new KIntNumInput(3, box);
       m_charcoalDeviation->setRange(0, 20, 1, true );
       QWhatsThis::add( m_charcoalDeviation, i18n("<p>Select here the value which represents the "
                                                  "standard deviation of the Gaussian, in pixels.") );
       m_label_charcoalDeviation->setBuddy( m_charcoalDeviation );
       dvlay->addWidget( m_charcoalDeviation );
       }

    if (EffectType == i18n("Detect edges"))
       {
       QLabel *m_label_edgeRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_edgeRadius );
       m_edgeRadius = new KIntNumInput(3, box);
       m_edgeRadius->setRange(0, 20, 1, true );
       QWhatsThis::add( m_edgeRadius, i18n("<p>Select here the value which represents the radius of "
                                           "the pixel neighborhood. Radius defines the radius of the "
                                           "convolution filter. If you use a radius of 0 the algorithm selects "
                                           "a suitable radius.") );
       m_label_edgeRadius->setBuddy( m_edgeRadius );
       dvlay->addWidget( m_edgeRadius );
       }

    if (EffectType == i18n("Emboss"))
       {
       QLabel *m_label_embossRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_embossRadius );
       m_embossRadius = new KIntNumInput(3, box);
       m_embossRadius->setRange(0, 20, 1, true );
       QWhatsThis::add( m_embossRadius, i18n("<p>Select here the value which represents the radius of the "
                                             "pixel neighborhood. For reasonable results, radius should be "
                                             "larger than deviation. If you use a radius of 0 the algorithm "
                                             "selects a suitable radius.") );
       m_label_embossRadius->setBuddy( m_embossRadius );
       dvlay->addWidget( m_embossRadius );

       QLabel *m_label_embossDeviation = new QLabel (i18n("Deviation:"), box);
       dvlay->addWidget( m_label_embossDeviation );
       m_embossDeviation = new KIntNumInput(3, box);
       m_embossDeviation->setRange(0, 20, 1, true );
       QWhatsThis::add( m_embossDeviation, i18n("<p>Select here the value whcih represents the standard "
                                                "deviation of the Gaussian, in pixels.") );
       m_label_embossDeviation->setBuddy( m_embossDeviation );
       dvlay->addWidget( m_embossDeviation );
       }

    if (EffectType == i18n("Implode"))
       {
       QLabel *m_label_implodeFactor = new QLabel (i18n("Factor:"), box);
       dvlay->addWidget( m_label_implodeFactor );
       m_implodeFactor = new KIntNumInput(1, box);
       m_implodeFactor->setRange(0, 20, 1, true );
       QWhatsThis::add( m_implodeFactor, i18n("<p>Select here the value which represents the extent of "
                                              "the implosion.") );
       m_label_implodeFactor->setBuddy( m_implodeFactor );
       dvlay->addWidget( m_implodeFactor );
       }

    if (EffectType == i18n("Paint"))
       {
       QLabel *m_label_paintRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_paintRadius );
       m_paintRadius = new KIntNumInput(3, box);
       m_paintRadius->setRange(0, 20, 1, true );
       QWhatsThis::add( m_paintRadius, i18n("<p>Select here the value which represents the radius of "
                                            "the circular neighborhood. Each pixel is replaced by the "
                                            "most frequent color occurring in a circular region defined "
                                            "by the radius.") );
       m_label_paintRadius->setBuddy( m_paintRadius );
       dvlay->addWidget( m_paintRadius );
       }

    if (EffectType == i18n("Shade light"))
       {
       QLabel *m_label_shadeAzimuth = new QLabel (i18n("Azimuth:"), box);
       dvlay->addWidget( m_label_shadeAzimuth );
       m_shadeAzimuth = new KIntNumInput(40, box);
       m_shadeAzimuth->setRange(0, 360, 1, true );
       QWhatsThis::add( m_shadeAzimuth, i18n("<p>Select here the value which represents the azimuth of "
                                             "the light source direction. The azimuth is measured in degrees "
                                             "above the x axis.") );
       m_label_shadeAzimuth->setBuddy( m_shadeAzimuth );
       dvlay->addWidget( m_shadeAzimuth );

       QLabel *m_label_shadeElevation = new QLabel (i18n("Elevation:"), box);
       dvlay->addWidget( m_label_shadeElevation );
       m_shadeElevation = new KIntNumInput(40, box);
       m_shadeElevation->setRange(0, 500, 1, true );
       QWhatsThis::add( m_shadeElevation, i18n("<p>Select here the value which represents the elevation of "
                                               "the light source direction. The elevation is measured in "
                                               "pixels above the Z axis.") );
       m_label_shadeElevation->setBuddy( m_shadeElevation );
       dvlay->addWidget( m_shadeElevation );
       }

    if (EffectType == i18n("Solarize"))
       {
       QLabel *m_label_solarizeFactor = new QLabel (i18n("Factor:"), box);
       dvlay->addWidget( m_label_solarizeFactor );
       m_solarizeFactor = new KIntNumInput(3, box);
       m_solarizeFactor->setRange(0, 99, 1, true );
       QWhatsThis::add( m_solarizeFactor, i18n("<p>Select here the value which represents the percent-"
                                               "threshold of the solarize intensity.") );
       m_label_solarizeFactor->setBuddy( m_solarizeFactor );
       dvlay->addWidget( m_solarizeFactor );
       }

    if (EffectType == i18n("Spread"))
       {
       QLabel *m_label_spreadRadius = new QLabel (i18n("Radius:"), box);
       dvlay->addWidget( m_label_spreadRadius );
       m_spreadRadius = new KIntNumInput(10, box);
       m_spreadRadius->setRange(0, 200, 1, true );
       QWhatsThis::add( m_spreadRadius, i18n("<p>Select here the value which represents the random "
                                             "pixel in a neighborhood of this extent.") );
       m_label_spreadRadius->setBuddy( m_spreadRadius );
       dvlay->addWidget( m_spreadRadius );
       }

    if (EffectType == i18n("Swirl"))
       {
       QLabel *m_label_swirlDegrees = new QLabel (i18n("Degrees:"), box);
       dvlay->addWidget( m_label_swirlDegrees );
       m_swirlDegrees = new KIntNumInput(45, box);
       m_swirlDegrees->setRange(0, 360, 1, true );
       QWhatsThis::add( m_swirlDegrees, i18n("<p>Select here the value which represents the tightness of "
                                             "the swirling effect. You get a more dramatic effect as the "
                                             "degrees move from 1 to 360.") );
       m_label_swirlDegrees->setBuddy( m_swirlDegrees );
       dvlay->addWidget( m_swirlDegrees );
       }

    if (EffectType == i18n("Wave"))
       {
       QLabel *m_label_waveAmplitude = new QLabel (i18n("Amplitude:"), box);
       dvlay->addWidget( m_label_waveAmplitude );
       m_waveAmplitude = new KIntNumInput(50, box);
       m_waveAmplitude->setRange(0, 200, 1, true );
       QWhatsThis::add( m_waveAmplitude, i18n("<p>Select here the value which represents the amplitude of "
                                              "the sine wave.") );
       m_label_waveAmplitude->setBuddy( m_waveAmplitude );
       dvlay->addWidget( m_waveAmplitude );

       QLabel *m_label_waveLenght = new QLabel (i18n("Length:"), box);
       dvlay->addWidget( m_label_waveLenght );
       m_waveLenght = new KIntNumInput(100, box);
       m_waveLenght->setRange(0, 200, 1, true );
       QWhatsThis::add( m_waveLenght, i18n("<p>Select here the value which represents the wave length "
                                           "of the sine wave.") );
       m_label_waveLenght->setBuddy( m_waveLenght );
       dvlay->addWidget( m_waveLenght );
       }
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

EffectOptionsDialog::~EffectOptionsDialog()
{
}

}  // NameSpace KIPIBatchProcessImagesPlugin

#include "effectoptionsdialog.moc"
