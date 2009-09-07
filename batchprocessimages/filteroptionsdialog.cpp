/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#include "filteroptionsdialog.h"
#include "filteroptionsdialog.moc"

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
#include <kcombobox.h>
#include <klocale.h>
#include <knuminput.h>

namespace KIPIBatchProcessImagesPlugin
{

FilterOptionsDialog::FilterOptionsDialog(QWidget *parent, int FilterType)
        : KDialog(parent)
{
    setCaption(i18n("Filter Options"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    QWidget* box = new QWidget(this);
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout(box, 10, spacingHint());
    QString whatsThis;

    if (FilterType == 0) { // Add noise
        QLabel *m_label_noiseType = new QLabel(i18n("Noise algorithm:"), box);
        dvlay->addWidget(m_label_noiseType);
        m_noiseType = new KComboBox(false, box);
        m_noiseType->insertItem(i18n("Uniform"));
        m_noiseType->insertItem(i18n("Gaussian"));
        m_noiseType->insertItem(i18n("Multiplicative"));
        m_noiseType->insertItem(i18n("Impulse"));
        m_noiseType->insertItem(i18n("Laplacian"));
        m_noiseType->insertItem(i18n("Poisson"));
        m_noiseType->setWhatsThis(i18n("Select here the algorithm method which will used "
                                       "to add random noise to the images."));
        m_label_noiseType->setBuddy(m_noiseType);
        dvlay->addWidget(m_noiseType);
    }

    if (FilterType == 2) { // Blur
        QLabel *m_label_blurRadius = new QLabel(i18n("Radius (pixels):"), box);
        dvlay->addWidget(m_label_blurRadius);
        m_blurRadius = new KIntNumInput(3, box);
        m_blurRadius->setRange(0, 20);
        m_blurRadius->setSliderEnabled(true);
        m_blurRadius->setWhatsThis(i18n("Select here the blur radius of the Gaussian, "
                                        "not counting the center pixel. For reasonable results, the "
                                        "radius should be larger than deviation. If you use a radius of 0 "
                                        "the blur operations selects a suitable radius."));
        m_label_blurRadius->setBuddy(m_blurRadius);
        dvlay->addWidget(m_blurRadius);

        QLabel *m_label_blurDeviation = new QLabel(i18n("Deviation (pixels):"), box);
        dvlay->addWidget(m_label_blurDeviation);
        m_blurDeviation = new KIntNumInput(1, box);
        m_blurDeviation->setRange(0, 20);
        m_blurDeviation->setSliderEnabled(true);
        m_blurDeviation->setWhatsThis(i18n("Select here the standard deviation of the blur Gaussian."));
        m_label_blurDeviation->setBuddy(m_blurDeviation);
        dvlay->addWidget(m_blurDeviation);
    }

    if (FilterType == 5) { // Median
        QLabel *m_label_medianRadius = new QLabel(i18n("Radius (pixels):"), box);
        dvlay->addWidget(m_label_medianRadius);
        m_medianRadius = new KIntNumInput(3, box);
        m_medianRadius->setRange(0, 20);
        m_medianRadius->setSliderEnabled(true);
        m_medianRadius->setWhatsThis(i18n("Select here the median radius of the pixel neighborhood. "
                                          "The algorithm applies a digital filter that improves the quality "
                                          "of noisy images. Each pixel is replaced by the median in a "
                                          "set of neighboring pixels as defined by the radius."));
        m_label_medianRadius->setBuddy(m_medianRadius);
        dvlay->addWidget(m_medianRadius);
    }


    if (FilterType == 6) { // Noise reduction
        QLabel *m_label_noiseRadius = new QLabel(i18n("Radius (pixels):"), box);
        dvlay->addWidget(m_label_noiseRadius);
        m_noiseRadius = new KIntNumInput(3, box);
        m_noiseRadius->setRange(0, 20);
        m_noiseRadius->setSliderEnabled(true);
        m_noiseRadius->setWhatsThis(i18n("Select here the noise reduction radius value. "
                                         "The algorithm smooths the contours of an image while still "
                                         "preserving edge information. The algorithm works by replacing "
                                         "each pixel with its neighbor closest in value. A neighbor is "
                                         "defined by the radius. If you use a radius of 0 the algorithm "
                                         "selects a suitable radius."));
        m_label_noiseRadius->setBuddy(m_noiseRadius);
        dvlay->addWidget(m_noiseRadius);
    }

    if (FilterType == 7) { // Sharpen
        QLabel *m_label_sharpenRadius = new QLabel(i18n("Radius (pixels):"), box);
        dvlay->addWidget(m_label_sharpenRadius);
        m_sharpenRadius = new KIntNumInput(3, box);
        m_sharpenRadius->setRange(0, 20);
        m_sharpenRadius->setSliderEnabled(true);
        m_sharpenRadius->setWhatsThis(i18n("Select here the radius of the sharpen Gaussian, "
                                           "not counting the center pixel. For reasonable "
                                           "results, the radius should be larger than deviation. "
                                           "If you use a radius of 0 the sharpen operation selects a "
                                           "suitable radius."));
        m_label_sharpenRadius->setBuddy(m_sharpenRadius);
        dvlay->addWidget(m_sharpenRadius);

        QLabel *m_label_sharpenDeviation = new QLabel(i18n("Deviation (pixels):"), box);
        dvlay->addWidget(m_label_sharpenDeviation);
        m_sharpenDeviation = new KIntNumInput(1, box);
        m_sharpenDeviation->setRange(0, 20);
        m_sharpenDeviation->setSliderEnabled(true);
        m_sharpenDeviation->setWhatsThis(i18n("Select here the sharpen deviation value of the "
                                              "Laplacian."));
        m_label_sharpenDeviation->setBuddy(m_sharpenDeviation);
        dvlay->addWidget(m_sharpenDeviation);
    }

    if (FilterType == 8) { // Unsharp
        QLabel *m_label_unsharpenRadius = new QLabel(i18n("Radius (pixels):"), box);
        dvlay->addWidget(m_label_unsharpenRadius);
        m_unsharpenRadius = new KIntNumInput(3, box);
        m_unsharpenRadius->setRange(0, 20);
        m_unsharpenRadius->setSliderEnabled(true);
        m_unsharpenRadius->setWhatsThis(i18n("Select here the radius of the unsharpen Gaussian, "
                                             "not counting the center pixel. The algorithm "
                                             "convolve the image with a Gaussian operator of the given "
                                             "radius and standard deviation. For reasonable results, "
                                             "radius should be larger than sigma. If you use a radius of 0 "
                                             "the algorithm selects a suitable radius."));
        m_label_unsharpenRadius->setBuddy(m_unsharpenRadius);
        dvlay->addWidget(m_unsharpenRadius);

        QLabel *m_label_unsharpenDeviation = new QLabel(i18n("Deviation (pixels):"), box);
        dvlay->addWidget(m_label_unsharpenDeviation);
        m_unsharpenDeviation = new KIntNumInput(1, box);
        m_unsharpenDeviation->setRange(0, 20);
        m_unsharpenDeviation->setSliderEnabled(true);
        m_unsharpenDeviation->setWhatsThis(i18n("Select here the unsharpen deviation value of the "
                                                "Gaussian."));
        m_label_unsharpenDeviation->setBuddy(m_unsharpenDeviation);
        dvlay->addWidget(m_unsharpenDeviation);

        QLabel *m_label_unsharpenPercent = new QLabel(i18n("Amount (percent):"), box);
        dvlay->addWidget(m_label_unsharpenPercent);
        m_unsharpenPercent = new KIntNumInput(100, box);
        m_unsharpenPercent->setRange(1, 200);
        m_unsharpenPercent->setSliderEnabled(true);
        m_unsharpenPercent->setWhatsThis(i18n("Select here the percentage difference between original "
                                              "and blurred image which should be added to original."));
        m_label_unsharpenPercent->setBuddy(m_unsharpenPercent);
        dvlay->addWidget(m_unsharpenPercent);

        QLabel *m_label_unsharpenThreshold = new QLabel(i18n("Threshold (percent):"), box);
        dvlay->addWidget(m_label_unsharpenThreshold);
        m_unsharpenThreshold = new KIntNumInput(5, box);
        m_unsharpenThreshold->setRange(1, 100);
        m_unsharpenThreshold->setSliderEnabled(true);
        m_unsharpenThreshold->setWhatsThis(i18n("Select here the unsharpen threshold value, "
                                                "as a percentage of the maximum color component value,"
                                                "needed to apply the difference amount."));
        m_label_unsharpenThreshold->setBuddy(m_unsharpenThreshold);
        dvlay->addWidget(m_unsharpenThreshold);
    }
}

FilterOptionsDialog::~FilterOptionsDialog()
{
}

}  // namespace KIPIBatchProcessImagesPlugin
