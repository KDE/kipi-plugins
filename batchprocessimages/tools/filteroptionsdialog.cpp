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

#include "filteroptionsdialog.moc"

// Qt includes

#include <QColor>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QWidget>

// KDE includes

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <klocale.h>
#include <knuminput.h>

namespace KIPIBatchProcessImagesPlugin
{

static void initInput(KIntNumInput* widget, int min, int max, const QString& suffix)
{
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    widget->setMinimumWidth(300);
    widget->setRange(min, max);
    widget->setSliderEnabled(true);
    widget->setSuffix(suffix);
}

FilterOptionsDialog::FilterOptionsDialog(QWidget *parent, int FilterType)
                   : KDialog(parent),
                     m_noiseType(0),
                     m_blurRadius(0),
                     m_blurDeviation(0),
                     m_medianRadius(0),
                     m_noiseRadius(0),
                     m_sharpenRadius(0),
                     m_sharpenDeviation(0),
                     m_unsharpenRadius(0),
                     m_unsharpenDeviation(0),
                     m_unsharpenPercent(0),
                     m_unsharpenThreshold(0)
{
    setCaption(i18n("Filter Options"));
    setModal(true);
    showButtonSeparator(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    QWidget* box       = new QWidget(this);
    QFormLayout *layout = new QFormLayout(box);
    layout->setSpacing(spacingHint());
    layout->setMargin(0);
    setMainWidget(box);

    if (FilterType == 0)
    { // Add noise
        m_noiseType = new KComboBox(false, box);
        m_noiseType->addItem(i18nc("image noise type", "Uniform"));
        m_noiseType->addItem(i18nc("image noise type", "Gaussian"));
        m_noiseType->addItem(i18nc("image noise type", "Multiplicative"));
        m_noiseType->addItem(i18nc("image noise type", "Impulse"));
        m_noiseType->addItem(i18nc("image noise type", "Laplacian"));
        m_noiseType->addItem(i18nc("image noise type", "Poisson"));
        m_noiseType->setWhatsThis(i18n("Select here the algorithm method which will used "
                                       "to add random noise to the images."));
        layout->addRow(i18n("Noise algorithm:"), m_noiseType);
    }
    else if (FilterType == 2)
    { // Blur
        m_blurRadius = new KIntNumInput(3, box);
        initInput(m_blurRadius, 0, 20, i18n("px"));
        m_blurRadius->setWhatsThis(i18n("Select here the blur radius of the Gaussian, "
                                        "not counting the center pixel. For reasonable results, the "
                                        "radius should be larger than deviation. If you use a radius of 0 "
                                        "the blur operation selects a suitable radius."));
        layout->addRow(i18n("Radius:"), m_blurRadius);

        m_blurDeviation = new KIntNumInput(1, box);
        initInput(m_blurDeviation, 0, 20, i18n("px"));
        m_blurDeviation->setWhatsThis(i18n("Select here the standard deviation of the blur Gaussian."));
        layout->addRow(i18n("Deviation:"), m_blurDeviation);
    }
    else if (FilterType == 5)
    { // Median
        m_medianRadius = new KIntNumInput(3, box);
        initInput(m_medianRadius, 0, 20, i18n("px"));
        m_medianRadius->setWhatsThis(i18n("Select here the median radius of the pixel neighborhood. "
                                          "The algorithm applies a digital filter that improves the quality "
                                          "of noisy images. Each pixel is replaced by the median in a "
                                          "set of neighboring pixels as defined by the radius."));
        layout->addRow(i18n("Radius:"), m_medianRadius);
    }
    else if (FilterType == 6)
    { // Noise reduction
        m_noiseRadius = new KIntNumInput(3, box);
        initInput(m_noiseRadius, 0, 20, i18n("px"));
        m_noiseRadius->setWhatsThis(i18n("Select here the noise reduction radius value. "
                                         "The algorithm smooths the contours of an image while still "
                                         "preserving edge information. The algorithm works by replacing "
                                         "each pixel with its neighbor closest in value. A neighbor is "
                                         "defined by the radius. If you use a radius of 0 the algorithm "
                                         "selects a suitable radius."));
        layout->addRow(i18n("Radius:"), m_noiseRadius);
    }
    else if (FilterType == 7)
    { // Sharpen
        m_sharpenRadius = new KIntNumInput(3, box);
        initInput(m_sharpenRadius, 0, 20, i18n("px"));
        m_sharpenRadius->setWhatsThis(i18n("Select here the radius of the sharpen Gaussian, "
                                           "not counting the center pixel. For reasonable "
                                           "results, the radius should be larger than deviation. "
                                           "If you use a radius of 0 the sharpen operation selects a "
                                           "suitable radius."));
        layout->addRow(i18n("Radius:"), m_sharpenRadius);

        m_sharpenDeviation = new KIntNumInput(1, box);
        initInput(m_sharpenDeviation, 0, 20, i18n("px"));
        m_sharpenDeviation->setWhatsThis(i18n("Select here the sharpen deviation value of the "
                                              "Laplacian."));
        layout->addRow(i18n("Deviation:"), m_sharpenDeviation);
    }
    else if (FilterType == 8)
    { // Unsharp
        m_unsharpenRadius = new KIntNumInput(3, box);
        initInput(m_unsharpenRadius, 0, 20, i18n("px"));
        m_unsharpenRadius->setWhatsThis(i18n("Select here the radius of the unsharpen Gaussian, "
                                             "not counting the center pixel. The algorithm "
                                             "convolve the image with a Gaussian operator of the given "
                                             "radius and standard deviation. For reasonable results, "
                                             "radius should be larger than sigma. If you use a radius of 0 "
                                             "the algorithm selects a suitable radius."));
        layout->addRow(i18n("Radius:"), m_unsharpenRadius);

        m_unsharpenDeviation = new KIntNumInput(1, box);
        initInput(m_unsharpenDeviation, 0, 20, i18n("px"));
        m_unsharpenDeviation->setWhatsThis(i18n("Select here the unsharpen deviation value of the "
                                                "Gaussian."));
        layout->addRow(i18n("Deviation:"), m_unsharpenDeviation);

        m_unsharpenPercent = new KIntNumInput(100, box);
        initInput(m_unsharpenPercent, 1, 200, i18n("%"));
        m_unsharpenPercent->setWhatsThis(i18n("Select here the percentage difference between original "
                                              "and blurred image which should be added to original."));
        layout->addRow(i18n("Amount:"), m_unsharpenPercent);

        m_unsharpenThreshold = new KIntNumInput(5, box);
        initInput(m_unsharpenThreshold, 1, 100, i18n("%"));
        m_unsharpenThreshold->setWhatsThis(i18n("Select here the unsharpen threshold value, "
                                                "as a percentage of the maximum color component value, "
                                                "needed to apply the difference amount."));
        layout->addRow(i18n("Threshold:"), m_unsharpenThreshold);
    }
}

FilterOptionsDialog::~FilterOptionsDialog()
{
}

}  // namespace KIPIBatchProcessImagesPlugin
