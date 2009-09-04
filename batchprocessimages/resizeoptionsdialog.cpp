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

#include "resizeoptionsdialog.h"
#include "resizeoptionsdialog.moc"

// Qt includes

#include <QCheckBox>
#include <QColor>
#include <QGridLayout>
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
#include <kmessagebox.h>
#include <knuminput.h>

namespace KIPIBatchProcessImagesPlugin
{

ResizeOptionsDialog::ResizeOptionsDialog(QWidget *parent, int ResizeType)
        : KDialog(parent)
{
    setCaption(i18n("Image-Resize Options"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    m_Type = ResizeType;
    QWidget* box = new QWidget(this);
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout(box, 10, spacingHint());
    QString whatsThis;
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));

    if (m_Type == 0) { // Proportional (1 dim.)
        m_size = new KIntNumInput(640);
        m_size->setRange(10, 10000);
        m_size->setSliderEnabled(true);
        m_size->setLabel(i18n("New size (pixels):"));
        m_size->setWhatsThis(i18n("The new images' size in pixels."));

        m_quality = new KIntNumInput(75);
        m_quality->setRange(0, 100);
        m_quality->setSliderEnabled(true);
        m_quality->setLabel(i18n("Image quality (percent):"));
        m_quality->setWhatsThis(i18n("Quality for JPEG images."));

        m_label_resizeFilter = new QLabel(i18n("Filter name:"));

        m_resizeFilter = new KComboBox;
        m_resizeFilter->insertItem(i18nc("Filter name", "Bessel"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Blackman"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Box"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Catrom"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Cubic"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Gaussian"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hermite"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hanning"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hamming"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Lanczos"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Mitchell"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Point"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Quadratic"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Sinc"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Triangle"));
        m_resizeFilter->setWhatsThis(i18n("Select here the filter name for the resize-image process. "
                                          "This filter will be used like a kernel convolution process "
                                          "during the increased image size rendering. The default filter "
                                          "is 'Lanczos'."));
        m_label_resizeFilter->setBuddy(m_resizeFilter);

        QGroupBox *groupBox1   = new QGroupBox(i18n("Resize Options"));
        QGridLayout *gb1Layout = new QGridLayout;
        gb1Layout->addWidget(m_size,               0, 0, 1, -1);
        gb1Layout->addWidget(m_quality,            1, 0, 1, -1);
        gb1Layout->addWidget(m_label_resizeFilter, 2, 0, 1, 1);
        gb1Layout->addWidget(m_resizeFilter,       2, 1, 1, 1);
        groupBox1->setLayout(gb1Layout);

        dvlay->addWidget(groupBox1);

        m_label_size = new QLabel(i18n("Note: the images will be resized to\n"
                                       "this size. The width or the height of the\n"
                                       "images will be automatically\n"
                                       "selected in depending of the images orientation.\n"
                                       "The images' aspect ratios are preserved."), box);
        dvlay->addWidget(m_label_size);
    }

    if (m_Type == 1) { // Proportional (2 dim.)
        m_label_Width = new QLabel(i18n("Width (pixels):"));
        m_Width = new KIntNumInput(1024);
        m_Width->setRange(100, 10000);
        m_Width->setSliderEnabled(true);
        m_Width->setWhatsThis(i18n("The new images' width in pixels."));
        m_label_Width->setBuddy(m_Width);

        m_label_Height = new QLabel(i18n("Height (pixels):"));
        m_Height = new KIntNumInput(768);
        m_Height->setRange(100, 10000);
        m_Height->setSliderEnabled(true);
        m_Height->setWhatsThis(i18n("The new images' height in pixels."));
        m_label_Height->setBuddy(m_Height);

        m_label_quality = new QLabel(i18n("Image quality (percent):"));
        m_quality = new KIntNumInput(75);
        m_quality->setRange(0, 100);
        m_quality->setSliderEnabled(true);
        m_quality->setWhatsThis(i18n("Quality for JPEG images."));
        m_label_quality->setBuddy(m_quality);

        QGroupBox *groupBox1   = new QGroupBox(i18n("Size Settings"));
        QGridLayout *gb1Layout = new QGridLayout;
        gb1Layout->addWidget(m_label_Width,   0, 0, 1, -1);
        gb1Layout->addWidget(m_Width,         1, 0, 1, -1);
        gb1Layout->addWidget(m_label_Height,  2, 0, 1, -1);
        gb1Layout->addWidget(m_Height,        3, 0, 1, -1);
        gb1Layout->addWidget(m_label_quality, 4, 0, 1, -1);
        gb1Layout->addWidget(m_quality,       5, 1, 1, -1);
        groupBox1->setLayout(gb1Layout);

        dvlay->addWidget(groupBox1);

        // ----------------------------------------------------

        m_label_bgColor = new QLabel(i18n("Background color:"));
        QColor bgColor = QColor(0, 0, 0);                           // Black per default.
        m_button_bgColor = new KColorButton(bgColor);
        m_button_bgColor->setWhatsThis(i18n("You can select here the background color to "
                                            "be used when adapting the images' sizes."));
        m_label_bgColor->setBuddy(m_button_bgColor);

        m_label_resizeFilter = new QLabel(i18n("Filter name:"));
        m_resizeFilter = new KComboBox;
        m_resizeFilter->insertItem(i18nc("Filter name", "Bessel"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Blackman"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Box"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Catrom"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Cubic"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Gaussian"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hermite"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hanning"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hamming"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Lanczos"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Mitchell"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Point"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Quadratic"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Sinc"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Triangle"));
        m_resizeFilter->setWhatsThis(i18n("Select here the filter name for the resize-image process. "
                                          "This filter will be used like a kernel convolution process "
                                          "during the increased image size rendering. The default filter "
                                          "is 'Lanczos'."));
        m_label_resizeFilter->setBuddy(m_resizeFilter);

        m_label_border = new QLabel(i18n("Border size (pixels):"));
        m_Border = new KIntNumInput(100);
        m_Border->setRange(0, 1000);
        m_Border->setSliderEnabled(true);
        m_Border->setWhatsThis(i18n("The border size around the images in pixels."));
        m_label_border->setBuddy(m_Border);

        QGroupBox *groupBox2   = new QGroupBox(i18n("Render Settings"));
        QGridLayout *gb2Layout = new QGridLayout;
        gb2Layout->addWidget(m_label_bgColor,      0, 0, 1, 1);
        gb2Layout->addWidget(m_button_bgColor,     0, 1, 1, 1);
        gb2Layout->addWidget(m_label_resizeFilter, 1, 0, 1, 1);
        gb2Layout->addWidget(m_resizeFilter,       1, 1, 1, 1);
        gb2Layout->addWidget(m_label_border,       2, 0, 1, -1);
        gb2Layout->addWidget(m_Border,             3, 0, 1, -1);
        groupBox2->setLayout(gb2Layout);

        dvlay->addWidget(groupBox2);
    }

    if (m_Type == 2) { // Non proportional
        m_fixedWidth = new KIntNumInput(640);
        m_fixedWidth->setRange(10, 10000);
        m_fixedWidth->setSliderEnabled(true);
        m_fixedWidth->setLabel(i18n("New width (pixels):"));
        m_fixedWidth->setWhatsThis(i18n("The new images' width in pixels."));

        m_fixedHeight = new KIntNumInput(480);
        m_fixedHeight->setRange(10, 10000);
        m_fixedHeight->setSliderEnabled(true);
        m_fixedHeight->setLabel(i18n("New height (pixels):"));
        m_fixedHeight->setWhatsThis(i18n("The new images' height in pixels."));

        m_quality = new KIntNumInput(75);
        m_quality->setRange(0, 100);
        m_quality->setSliderEnabled(true);
        m_quality->setLabel(i18n("Image quality (percent):"));
        m_quality->setWhatsThis(i18n("Quality for JPEG images."));

        m_label_resizeFilter = new QLabel(i18n("Filter name:"));

        m_resizeFilter = new KComboBox;
        m_resizeFilter->insertItem(i18nc("Filter name", "Bessel"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Blackman"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Box"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Catrom"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Cubic"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Gaussian"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hermite"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hanning"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hamming"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Lanczos"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Mitchell"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Point"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Quadratic"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Sinc"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Triangle"));
        m_resizeFilter->setWhatsThis(i18n("Select here the filter name for the resize-image process. "
                                          "This filter will be used like a kernel convolution process "
                                          "during the increased image size rendering. The default filter "
                                          "is 'Lanczos'."));
        m_label_resizeFilter->setBuddy(m_resizeFilter);

        QGroupBox *groupBox1   = new QGroupBox(i18n("Resize Options"));
        QGridLayout *gb1Layout = new QGridLayout;
        gb1Layout->addWidget(m_fixedWidth,         0, 0, 1, -1);
        gb1Layout->addWidget(m_fixedHeight,        1, 0, 1, -1);
        gb1Layout->addWidget(m_quality,            2, 0, 1, -1);
        gb1Layout->addWidget(m_label_resizeFilter, 3, 0, 1, 1);
        gb1Layout->addWidget(m_resizeFilter,       3, 1, 1, 1);
        groupBox1->setLayout(gb1Layout);

        dvlay->addWidget(groupBox1);
    }

    if (m_Type == 3) { // Prepare to print
        m_customSettings = new QCheckBox(i18n("Use custom settings"), box);
        m_customSettings->setWhatsThis(i18n("If this option is enabled, "
                                            "all printing settings can be customized."));
        dvlay->addWidget(m_customSettings);

        m_label_paperSize = new QLabel(i18n("Paper size (cm):"));
        m_paperSize = new KComboBox;
        m_paperSize->insertItem("9x13");
        m_paperSize->insertItem("10x15");
        m_paperSize->insertItem("13x19");
        m_paperSize->insertItem("15x21");
        m_paperSize->insertItem("18x24");
        m_paperSize->insertItem("20x30");
        m_paperSize->insertItem("21x30");
        m_paperSize->insertItem("30x40");
        m_paperSize->insertItem("30x45");
        m_paperSize->insertItem("40x50");
        m_paperSize->insertItem("50x75");
        m_paperSize->setWhatsThis(i18n("The standard photographic paper sizes in centimeters."));
        m_label_paperSize->setBuddy(m_paperSize);

        m_label_printDpi = new QLabel(i18n("Print resolution (dpi):"));
        m_printDpi = new KComboBox;
        m_printDpi->insertItem("75");
        m_printDpi->insertItem("150");
        m_printDpi->insertItem("300");
        m_printDpi->insertItem("600");
        m_printDpi->insertItem("1200");
        m_printDpi->insertItem("1400");
        m_printDpi->insertItem("2400");
        m_printDpi->setWhatsThis(i18n("The standard print resolutions in dots per inch."));
        m_label_printDpi->setBuddy(m_printDpi);

        QGroupBox *groupBox1   = new QGroupBox(i18n("Printing Standard Settings"));
        QGridLayout *gb1Layout = new QGridLayout;
        gb1Layout->addWidget(m_label_paperSize, 0, 0, 1, 1);
        gb1Layout->addWidget(m_paperSize,       0, 1, 1, 1);
        gb1Layout->addWidget(m_label_printDpi,  1, 0, 1, 1);
        gb1Layout->addWidget(m_printDpi,        1, 1, 1, 1);
        groupBox1->setLayout(gb1Layout);

        dvlay->addWidget(groupBox1);

        // ----------------------------------------------------

        m_label_customXSize = new QLabel(i18n("Paper width (cm):"));
        m_customXSize = new KIntNumInput(10);
        m_customXSize->setRange(1, 100);
        m_customXSize->setSliderEnabled(true);
        m_customXSize->setWhatsThis(i18n("The customized width of the photographic paper size "
                                         "in centimeters."));
        m_label_customXSize->setBuddy(m_customXSize);

        m_label_customYSize = new QLabel(i18n("Paper height (cm):"));
        m_customYSize = new KIntNumInput(15);
        m_customYSize->setRange(1, 100);
        m_customYSize->setSliderEnabled(true);
        m_customYSize->setWhatsThis(i18n("The customized height of the photographic paper size "
                                         "in centimeters."));
        m_label_customYSize->setBuddy(m_customYSize);

        m_label_customDpi = new QLabel(i18n("Print resolution (dpi):"));
        m_customDpi = new KIntNumInput(300);
        m_customDpi->setRange(10, 5000, 10);
        m_customDpi->setSliderEnabled(true);
        m_customDpi->setWhatsThis(i18n("The customized print resolution in dots per inch."));
        m_label_customDpi->setBuddy(m_customDpi);

        QGroupBox *groupBox2   = new QGroupBox(i18n("Printing Custom Settings"));
        QGridLayout *gb2Layout = new QGridLayout;
        gb2Layout->addWidget(m_label_customXSize, 0, 0, 1, -1);
        gb2Layout->addWidget(m_customXSize,       1, 0, 1, -1);
        gb2Layout->addWidget(m_label_customYSize, 2, 0, 1, -1);
        gb2Layout->addWidget(m_customYSize,       3, 0, 1, -1);
        gb2Layout->addWidget(m_label_customDpi,   4, 0, 1, -1);
        gb2Layout->addWidget(m_customDpi,         5, 1, 1, -1);
        groupBox2->setLayout(gb2Layout);

        dvlay->addWidget(groupBox2);

        // ----------------------------------------------------

        m_label_backgroundColor = new QLabel(i18n("Background color:"));
        QColor backgroundColor = QColor(255, 255, 255);                           // White per default.
        m_button_backgroundColor = new KColorButton(backgroundColor);
        m_button_backgroundColor->setWhatsThis(i18n("You can select here the background color to "
                                               "be used when adapting the images' sizes."));
        m_label_backgroundColor->setBuddy(m_button_backgroundColor);

        m_label_resizeFilter = new QLabel(i18n("Filter name:"));
        m_resizeFilter = new KComboBox;
        m_resizeFilter->insertItem(i18nc("Filter name", "Bessel"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Blackman"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Box"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Catrom"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Cubic"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Gaussian"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hermite"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hanning"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Hamming"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Lanczos"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Mitchell"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Point"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Quadratic"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Sinc"));
        m_resizeFilter->insertItem(i18nc("Filter name", "Triangle"));
        m_resizeFilter->setWhatsThis(i18n("Select here the filter name for the resize-image process. "
                                          "This filter will be used like a kernel convolution process "
                                          "during the increased image size rendering. The default filter "
                                          "is 'Lanczos'."));
        m_label_resizeFilter->setBuddy(m_resizeFilter);

        m_label_marging = new QLabel(i18n("Margin size (mm):"));
        m_marging = new KIntNumInput(1);
        m_marging->setRange(0, 80);
        m_marging->setSliderEnabled(true);
        m_marging->setWhatsThis(i18n("The margin around the images in millimeters."));
        m_label_marging->setBuddy(m_marging);

        m_label_quality = new QLabel(i18n("Image quality (percent):"));
        m_quality = new KIntNumInput(75);
        m_quality->setRange(0, 100);
        m_quality->setSliderEnabled(true);
        m_quality->setWhatsThis(i18n("Quality for JPEG images."));
        m_label_quality->setBuddy(m_quality);

        QGroupBox *groupBox3   = new QGroupBox(i18n("Printing Custom Settings"));
        QGridLayout *gb3Layout = new QGridLayout;
        gb3Layout->addWidget(m_label_backgroundColor,  0, 0, 1, 1);
        gb3Layout->addWidget(m_button_backgroundColor, 0, 1, 1, 1);
        gb3Layout->addWidget(m_label_resizeFilter,     1, 0, 1, -1);
        gb3Layout->addWidget(m_resizeFilter,           2, 0, 1, -1);
        gb3Layout->addWidget(m_label_marging,          3, 0, 1, -1);
        gb3Layout->addWidget(m_marging,                4, 0, 1, -1);
        gb3Layout->addWidget(m_label_quality,          5, 0, 1, -1);
        gb3Layout->addWidget(m_quality,                6, 0, 1, -1);
        groupBox3->setLayout(gb3Layout);

        dvlay->addWidget(groupBox3);

        connect(m_customSettings, SIGNAL(toggled(bool)),
                this, SLOT(slotCustomSettingsEnabled(bool)));

        slotCustomSettingsEnabled(false);
    }
}

ResizeOptionsDialog::~ResizeOptionsDialog()
{
}

void ResizeOptionsDialog::slotCustomSettingsEnabled(bool val)
{
    m_label_paperSize->setEnabled(!val);
    m_paperSize->setEnabled(!val);
    m_label_printDpi->setEnabled(!val);
    m_printDpi->setEnabled(!val);

    m_label_customXSize->setEnabled(val);
    m_customXSize->setEnabled(val);
    m_label_customYSize->setEnabled(val);
    m_customYSize->setEnabled(val);
    m_label_customDpi->setEnabled(val);
    m_customDpi->setEnabled(val);
}

void ResizeOptionsDialog::slotOk()
{
    if (m_Type == 3) { // Prepare to print
        if (m_customSettings->isChecked() == true) {
            if (m_customXSize > m_customYSize) {
                KMessageBox::sorry(this, i18n("You must enter a custom height greater than the custom width: "
                                              "the photographic paper must be vertically orientated."));
                return;
            }
        }
    }

    accept();
}

}  // namespace KIPIBatchProcessImagesPlugin
