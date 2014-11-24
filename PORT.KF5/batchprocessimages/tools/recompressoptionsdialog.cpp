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

#include "recompressoptionsdialog.moc"

// Qt includes

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

// KDE includes

#include <kcombobox.h>
#include <klocale.h>
#include <knuminput.h>

namespace KIPIBatchProcessImagesPlugin
{

RecompressOptionsDialog::RecompressOptionsDialog(QWidget *parent)
                       : KDialog(parent)
{
    setCaption(i18n("Recompression Options"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    QWidget* box       = new QWidget(this);
    QVBoxLayout *dvlay = new QVBoxLayout(box);
    dvlay->setSpacing(spacingHint());
    dvlay->setMargin(spacingHint());
    setMainWidget(box);
    QString whatsThis;

    // JPEG file format.

    m_label_JPEGimageCompression = new QLabel(i18n("Image compression level:"));
    m_JPEGCompression            = new KIntNumInput(75);
    m_JPEGCompression->setRange(1, 100);
    m_JPEGCompression->setSliderEnabled(true);
    whatsThis = i18n("<p>The compression value for JPEG target images:</p>");
    whatsThis = whatsThis + i18n("<p>"
                                 "<b>1</b>: very high compression<br/>"
                                 "<b>25</b>: high compression<br/>"
                                 "<b>50</b>: medium compression<br/>"
                                 "<b>75</b>: low compression (default value)<br/>"
                                 "<b>100</b>: no compression"
                                 "</p>");

    m_JPEGCompression->setWhatsThis(whatsThis);
    m_label_JPEGimageCompression->setBuddy(m_JPEGCompression);

    m_compressLossLess = new QCheckBox(i18n("Use lossless compression"));
    m_compressLossLess->setWhatsThis(i18n("If this option is enabled, "
                                          "all JPEG operations will use lossless compression."));

    connect(m_compressLossLess, SIGNAL(toggled(bool)),
            this, SLOT(slotCompressLossLessEnabled(bool)));

    QGroupBox *groupBox1   = new QGroupBox(i18n("JPEG File Format"));
    QGridLayout *gb1Layout = new QGridLayout;
    gb1Layout->addWidget(m_label_JPEGimageCompression, 0, 0, 1, 1);
    gb1Layout->addWidget(m_JPEGCompression,            0, 1, 1, 1);
    gb1Layout->addWidget(m_compressLossLess,           1, 0, 1, -1);
    groupBox1->setLayout(gb1Layout);

    dvlay->addWidget(groupBox1);

    // PNG File format.

    m_label_PNGimageCompression = new QLabel(i18n("Image compression level:"));
    m_PNGCompression            = new KIntNumInput(75);
    m_PNGCompression->setRange(1, 100);
    m_PNGCompression->setSliderEnabled(true);
    whatsThis = i18n("<p>The compression value for PNG target images:</p>");
    whatsThis = whatsThis + i18n("<p>"
                                 "<b>1</b>: very high compression<br/>"
                                 "<b>25</b>: high compression<br/>"
                                 "<b>50</b>: medium compression<br/>"
                                 "<b>75</b>: low compression (default value)<br/>"
                                 "<b>100</b>: no compression"
                                 "</p>");

    m_PNGCompression->setWhatsThis(whatsThis);
    m_label_PNGimageCompression->setBuddy(m_PNGCompression);

    QGroupBox *groupBox2   = new QGroupBox(i18n("PNG File Format"));
    QGridLayout *gb2Layout = new QGridLayout;
    gb2Layout->addWidget(m_label_PNGimageCompression, 0, 0, 1, 1);
    gb2Layout->addWidget(m_PNGCompression,            0, 1, 1, 1);
    groupBox2->setLayout(gb2Layout);

    dvlay->addWidget(groupBox2);

    // TIFF File format.

    m_label_TIFFimageCompression = new QLabel(i18n("Image compression algorithm:"));
    m_TIFFCompressionAlgo        = new KComboBox;
    m_TIFFCompressionAlgo->addItem("LZW");
    m_TIFFCompressionAlgo->addItem("JPEG");
    m_TIFFCompressionAlgo->addItem(i18nc("image compression", "None"));
    m_TIFFCompressionAlgo->setWhatsThis(i18n("Select here the TIFF compression algorithm."));
    m_label_TIFFimageCompression->setBuddy(m_TIFFCompressionAlgo);

    QGroupBox *groupBox3   = new QGroupBox(i18n("TIFF File Format"));
    QGridLayout *gb3Layout = new QGridLayout;
    gb3Layout->addWidget(m_label_TIFFimageCompression, 0, 0, 1, 1);
    gb3Layout->addWidget(m_TIFFCompressionAlgo,        0, 1, 1, 1);
    groupBox3->setLayout(gb3Layout);

    dvlay->addWidget(groupBox3);

    // TGA File format.

    m_label_TGAimageCompression = new QLabel(i18n("Image compression algorithm:"));
    m_TGACompressionAlgo        = new KComboBox;
    m_TGACompressionAlgo->addItem("RLE");
    m_TGACompressionAlgo->addItem(i18nc("image compression", "None"));
    m_TGACompressionAlgo->setWhatsThis(i18n("Select here the TGA compression algorithm."));
    m_label_TGAimageCompression->setBuddy(m_TGACompressionAlgo);

    QGroupBox *groupBox4   = new QGroupBox(i18n("TGA File Format"));
    QGridLayout *gb4Layout = new QGridLayout;
    gb4Layout->addWidget(m_label_TGAimageCompression, 0, 0, 1, 1);
    gb4Layout->addWidget(m_TGACompressionAlgo,        0, 1, 1, 1);
    groupBox4->setLayout(gb4Layout);

    dvlay->addWidget(groupBox4);
}

RecompressOptionsDialog::~RecompressOptionsDialog()
{
}

void RecompressOptionsDialog::slotCompressLossLessEnabled(bool val)
{
    m_JPEGCompression->setEnabled(!val);
    m_label_JPEGimageCompression->setEnabled(!val);
}

}  // namespace KIPIBatchProcessImagesPlugin
