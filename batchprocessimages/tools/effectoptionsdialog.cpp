/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "effectoptionsdialog.moc"

// Qt includes

#include <QColor>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

// KDE includes

#include <kcolorbutton.h>
#include <klocale.h>
#include <knuminput.h>

namespace KIPIBatchProcessImagesPlugin
{

EffectOptionsDialog::EffectOptionsDialog(QWidget *parent, int EffectType)
        : KDialog(parent),
        m_latWidth(0),
        m_latHeight(0),
        m_latOffset(0),
        m_charcoalRadius(0),
        m_charcoalDeviation(0),
        m_edgeRadius(0),
        m_embossRadius(0),
        m_embossDeviation(0),
        m_implodeFactor(0),
        m_paintRadius(0),
        m_shadeAzimuth(0),
        m_shadeElevation(0),
        m_solarizeFactor(0),
        m_spreadRadius(0),
        m_swirlDegrees(0),
        m_waveAmplitude(0),
        m_waveLength(0)
{
    setCaption(i18n("Effect Options"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    QWidget* box       = new QWidget(this);
    QVBoxLayout *dvlay = new QVBoxLayout(box);
    dvlay->setSpacing(spacingHint());
    dvlay->setMargin(spacingHint());
    setMainWidget(box);
    QString whatsThis;

    if (EffectType == 0)
    { // Adaptive threshold
        QLabel *m_label_latWidth = new QLabel(i18n("Width:"), box);
        dvlay->addWidget(m_label_latWidth);
        m_latWidth = new KIntNumInput(50, box);
        m_latWidth->setRange(0, 200);
        m_latWidth->setSliderEnabled(true);
        m_latWidth->setWhatsThis(i18n("Select here the value which represents the width "
                                      "of the local neighborhood."));
        m_label_latWidth->setBuddy(m_latWidth);
        dvlay->addWidget(m_latWidth);

        QLabel *m_label_latHeight = new QLabel(i18n("Height:"), box);
        dvlay->addWidget(m_label_latHeight);
        m_latHeight = new KIntNumInput(50, box);
        m_latHeight->setRange(0, 200);
        m_latHeight->setSliderEnabled(true);
        m_latHeight->setWhatsThis(i18n("Select here the value which represents the height of "
                                       "the local neighborhood."));
        m_label_latHeight->setBuddy(m_latHeight);
        dvlay->addWidget(m_latHeight);

        QLabel *m_label_latOffset = new QLabel(i18n("Offset:"), box);
        dvlay->addWidget(m_label_latOffset);
        m_latOffset = new KIntNumInput(1, box);
        m_latOffset->setRange(0, 200);
        m_latOffset->setSliderEnabled(true);
        m_latOffset->setWhatsThis(i18n("Select here the value which represents the mean offset."));
        m_label_latOffset->setBuddy(m_latOffset);
        dvlay->addWidget(m_latOffset);
    }

    if (EffectType == 1)
    { // Charcoal
        QLabel *m_label_charcoalRadius = new QLabel(i18n("Radius:"), box);
        dvlay->addWidget(m_label_charcoalRadius);
        m_charcoalRadius = new KIntNumInput(3, box);
        m_charcoalRadius->setRange(0, 20);
        m_charcoalRadius->setSliderEnabled(true);
        m_charcoalRadius->setWhatsThis(i18n("Select here the value which represents the "
                                            "radius of the pixel neighborhood."));
        m_label_charcoalRadius->setBuddy(m_charcoalRadius);
        dvlay->addWidget(m_charcoalRadius);

        QLabel *m_label_charcoalDeviation = new QLabel(i18n("Deviation:"), box);
        dvlay->addWidget(m_label_charcoalDeviation);
        m_charcoalDeviation = new KIntNumInput(3, box);
        m_charcoalDeviation->setRange(0, 20);
        m_charcoalDeviation->setSliderEnabled(true);
        m_charcoalDeviation->setWhatsThis(i18n("Select here the value which represents the "
                                               "standard deviation of the Gaussian, in pixels."));
        m_label_charcoalDeviation->setBuddy(m_charcoalDeviation);
        dvlay->addWidget(m_charcoalDeviation);
    }

    if (EffectType == 2)
    { // Detect edges
        QLabel *m_label_edgeRadius = new QLabel(i18n("Radius:"), box);
        dvlay->addWidget(m_label_edgeRadius);
        m_edgeRadius = new KIntNumInput(3, box);
        m_edgeRadius->setRange(0, 20);
        m_edgeRadius->setSliderEnabled(true);
        m_edgeRadius->setWhatsThis(i18n("Select here the value which represents the radius of "
                                        "the pixel neighborhood. Radius defines the radius of the "
                                        "convolution filter. If you use a radius of 0 the algorithm selects "
                                        "a suitable radius."));
        m_label_edgeRadius->setBuddy(m_edgeRadius);
        dvlay->addWidget(m_edgeRadius);
    }

    if (EffectType == 3)
    { // Emboss
        QLabel *m_label_embossRadius = new QLabel(i18n("Radius:"), box);
        dvlay->addWidget(m_label_embossRadius);
        m_embossRadius = new KIntNumInput(3, box);
        m_embossRadius->setRange(0, 20);
        m_embossRadius->setSliderEnabled(true);
        m_embossRadius->setWhatsThis(i18n("Select here the value which represents the radius of the "
                                          "pixel neighborhood. For reasonable results, radius should be "
                                          "larger than deviation. If you use a radius of 0 the algorithm "
                                          "selects a suitable radius."));
        m_label_embossRadius->setBuddy(m_embossRadius);
        dvlay->addWidget(m_embossRadius);

        QLabel *m_label_embossDeviation = new QLabel(i18n("Deviation:"), box);
        dvlay->addWidget(m_label_embossDeviation);
        m_embossDeviation = new KIntNumInput(3, box);
        m_embossDeviation->setRange(0, 20);
        m_embossDeviation->setSliderEnabled(true);
        m_embossDeviation->setWhatsThis(i18n("Select here the value which represents the standard "
                                             "deviation of the Gaussian, in pixels."));
        m_label_embossDeviation->setBuddy(m_embossDeviation);
        dvlay->addWidget(m_embossDeviation);
    }

    if (EffectType == 4)
    { // Implode
        QLabel *m_label_implodeFactor = new QLabel(i18n("Factor:"), box);
        dvlay->addWidget(m_label_implodeFactor);
        m_implodeFactor = new KIntNumInput(1, box);
        m_implodeFactor->setRange(0, 20);
        m_implodeFactor->setSliderEnabled(true);
        m_implodeFactor->setWhatsThis(i18n("Select here the value which represents the extent of "
                                           "the implosion."));
        m_label_implodeFactor->setBuddy(m_implodeFactor);
        dvlay->addWidget(m_implodeFactor);
    }

    if (EffectType == 5)
    { // Paint
        QLabel *m_label_paintRadius = new QLabel(i18n("Radius:"), box);
        dvlay->addWidget(m_label_paintRadius);
        m_paintRadius = new KIntNumInput(3, box);
        m_paintRadius->setRange(0, 20);
        m_paintRadius->setSliderEnabled(true);
        m_paintRadius->setWhatsThis(i18n("Select here the value which represents the radius of "
                                         "the circular neighborhood. Each pixel is replaced by the "
                                         "most frequent color occurring in a circular region defined "
                                         "by the radius."));
        m_label_paintRadius->setBuddy(m_paintRadius);
        dvlay->addWidget(m_paintRadius);
    }

    if (EffectType == 6)
    { // Shade light
        QLabel *m_label_shadeAzimuth = new QLabel(i18n("Azimuth:"), box);
        dvlay->addWidget(m_label_shadeAzimuth);
        m_shadeAzimuth = new KIntNumInput(40, box);
        m_shadeAzimuth->setRange(0, 360);
        m_shadeAzimuth->setSliderEnabled(true);
        m_shadeAzimuth->setWhatsThis(i18n("Select here the value which represents the azimuth of "
                                          "the light source direction. The azimuth is measured in degrees "
                                          "above the x axis."));
        m_label_shadeAzimuth->setBuddy(m_shadeAzimuth);
        dvlay->addWidget(m_shadeAzimuth);

        QLabel *m_label_shadeElevation = new QLabel(i18n("Elevation:"), box);
        dvlay->addWidget(m_label_shadeElevation);
        m_shadeElevation = new KIntNumInput(40, box);
        m_shadeElevation->setRange(0, 500);
        m_shadeElevation->setSliderEnabled(true);
        m_shadeElevation->setWhatsThis(i18n("Select here the value which represents the elevation of "
                                            "the light source direction. The elevation is measured in "
                                            "pixels above the Z axis."));
        m_label_shadeElevation->setBuddy(m_shadeElevation);
        dvlay->addWidget(m_shadeElevation);
    }

    if (EffectType == 7)
    { // Solarize
        QLabel *m_label_solarizeFactor = new QLabel(i18n("Factor:"), box);
        dvlay->addWidget(m_label_solarizeFactor);
        m_solarizeFactor = new KIntNumInput(3, box);
        m_solarizeFactor->setRange(0, 99);
        m_solarizeFactor->setSliderEnabled(true);
        m_solarizeFactor->setWhatsThis(i18n("Select here the value which represents the percent-"
                                            "threshold of the solarize intensity."));
        m_label_solarizeFactor->setBuddy(m_solarizeFactor);
        dvlay->addWidget(m_solarizeFactor);
    }

    if (EffectType == 8)
    { // Spread
        QLabel *m_label_spreadRadius = new QLabel(i18n("Radius:"), box);
        dvlay->addWidget(m_label_spreadRadius);
        m_spreadRadius = new KIntNumInput(10, box);
        m_spreadRadius->setRange(0, 200);
        m_spreadRadius->setSliderEnabled(true);
        m_spreadRadius->setWhatsThis(i18n("Select here the value which represents the random "
                                          "pixel in a neighborhood of this extent."));
        m_label_spreadRadius->setBuddy(m_spreadRadius);
        dvlay->addWidget(m_spreadRadius);
    }

    if (EffectType == 9)
    { // Swirl
        QLabel *m_label_swirlDegrees = new QLabel(i18n("Degrees:"), box);
        dvlay->addWidget(m_label_swirlDegrees);
        m_swirlDegrees = new KIntNumInput(45, box);
        m_swirlDegrees->setRange(0, 360);
        m_swirlDegrees->setSliderEnabled(true);
        m_swirlDegrees->setWhatsThis(i18n("Select here the value which represents the tightness of "
                                          "the swirling effect. You get a more dramatic effect as the "
                                          "degrees move from 1 to 360."));
        m_label_swirlDegrees->setBuddy(m_swirlDegrees);
        dvlay->addWidget(m_swirlDegrees);
    }

    if (EffectType == 10)
    { // Wave
        QLabel *m_label_waveAmplitude = new QLabel(i18n("Amplitude:"), box);
        dvlay->addWidget(m_label_waveAmplitude);
        m_waveAmplitude = new KIntNumInput(50, box);
        m_waveAmplitude->setRange(0, 200);
        m_waveAmplitude->setSliderEnabled(true);
        m_waveAmplitude->setWhatsThis(i18n("Select here the value which represents the amplitude of "
                                           "the sine wave."));
        m_label_waveAmplitude->setBuddy(m_waveAmplitude);
        dvlay->addWidget(m_waveAmplitude);

        QLabel *m_label_waveLength = new QLabel(i18n("Length:"), box);
        dvlay->addWidget(m_label_waveLength);
        m_waveLength = new KIntNumInput(100, box);
        m_waveLength->setRange(0, 200);
        m_waveLength->setSliderEnabled(true);
        m_waveLength->setWhatsThis(i18n("Select here the value which represents the wave length "
                                        "of the sine wave."));
        m_label_waveLength->setBuddy(m_waveLength);
        dvlay->addWidget(m_waveLength);
    }
}

EffectOptionsDialog::~EffectOptionsDialog()
{
}

}  // namespace KIPIBatchProcessImagesPlugin
