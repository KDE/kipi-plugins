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

#include "convertoptionsdialog.moc"

// Qt includes

#include <QCheckBox>
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

ConvertOptionsDialog::ConvertOptionsDialog(QWidget *parent, int ImageFormatType)
                    : KDialog(parent),
                    m_label_imageCompression(0),
                    m_JPEGPNGCompression(0),
                    m_compressLossLess(0),
                    m_TIFFCompressionAlgo(0),
                    m_TGACompressionAlgo(0)
{
    setCaption(i18n("Image File Format Options"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    QWidget* box       = new QWidget(this);
    QVBoxLayout *dvlay = new QVBoxLayout(box);
    dvlay->setSpacing(spacingHint());
    dvlay->setMargin(spacingHint());
    setMainWidget(box);
    QString whatsThis;

    if (ImageFormatType == 0 || ImageFormatType == 1)
    { // JPEG || PNG
        m_label_imageCompression = new QLabel(i18n("Image compression level:"), box);
        dvlay->addWidget(m_label_imageCompression);
        m_JPEGPNGCompression     = new KIntNumInput(75, box);
        m_JPEGPNGCompression->setRange(1, 100);
        m_JPEGPNGCompression->setSliderEnabled(true);
        whatsThis = i18n("<p>The compression value for the target images:</p>");
        whatsThis = whatsThis + i18n("<p>"
                                     "<b>1</b>: very high compression<br/>"
                                     "<b>25</b>: high compression<br/>"
                                     "<b>50</b>: medium compression<br/>"
                                     "<b>75</b>: low compression (default value)<br/>"
                                     "<b>100</b>: no compression"
                                     "</p>");

        m_JPEGPNGCompression->setWhatsThis(whatsThis);
        m_label_imageCompression->setBuddy(m_JPEGPNGCompression);
        dvlay->addWidget(m_JPEGPNGCompression);

        if (ImageFormatType == 0)
        { // JPEG
            m_compressLossLess = new QCheckBox(i18n("Use lossless compression"), box);
            m_compressLossLess->setWhatsThis(i18n("If this option is enabled, "
                                                  "all JPEG operations will use lossless compression."));
            dvlay->addWidget(m_compressLossLess);

            connect(m_compressLossLess, SIGNAL(toggled(bool)),
                    this, SLOT(slotCompressLossLessEnabled(bool)));
        }
    }

    if (ImageFormatType == 2)
    { // TIFF
        QLabel *m_label_imageCompression = new QLabel(i18n("Image compression algorithm:"), box);
        dvlay->addWidget(m_label_imageCompression);
        m_TIFFCompressionAlgo = new KComboBox(box);
        m_TIFFCompressionAlgo->addItem("LZW");
        m_TIFFCompressionAlgo->addItem("JPEG");
        m_TIFFCompressionAlgo->addItem(i18nc("No TIFF compression", "None"));
        m_TIFFCompressionAlgo->setWhatsThis(i18n("Select here the compression algorithm."));
        m_label_imageCompression->setBuddy(m_TIFFCompressionAlgo);
        dvlay->addWidget(m_TIFFCompressionAlgo);
    }

    if (ImageFormatType == 5)
      { // TGA
        QLabel *m_label_imageCompression = new QLabel(i18n("Image compression algorithm:"), box);
        dvlay->addWidget(m_label_imageCompression);
        m_TGACompressionAlgo = new KComboBox(box);
        m_TGACompressionAlgo->addItem("RLE");
        m_TGACompressionAlgo->addItem(i18nc("No TGA compression", "None"));
        m_TGACompressionAlgo->setWhatsThis(i18n("Select here the compression algorithm."));
        m_label_imageCompression->setBuddy(m_TGACompressionAlgo);
        dvlay->addWidget(m_TGACompressionAlgo);
    }
}

ConvertOptionsDialog::~ConvertOptionsDialog()
{
}

void ConvertOptionsDialog::slotCompressLossLessEnabled(bool val)
{
    m_JPEGPNGCompression->setEnabled(!val);
    m_label_imageCompression->setEnabled(!val);
}

}  // namespace KIPIBatchProcessImagesPlugin
