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

#include "recompressoptionsdialog.h"
#include "recompressoptionsdialog.moc"

// Qt includes

#include <Q3GroupBox>
#include <Q3VBox>
#include <Q3VBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QWidget>

// KDE includes

#include <kcombobox.h>
#include <klocale.h>
#include <knuminput.h>

namespace KIPIBatchProcessImagesPlugin
{

RecompressOptionsDialog::RecompressOptionsDialog(QWidget *parent)
                       : KDialog( parent)
{
    setCaption(i18n("Recompression Options"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    Q3VBoxLayout *dvlay = new Q3VBoxLayout( box, 10, spacingHint() );
    QString whatsThis;

    // JPEG file format.

    Q3GroupBox * groupBox1 = new Q3GroupBox( 2, Qt::Horizontal, i18n("JPEG File Format"), box );

    m_label_JPEGimageCompression = new QLabel (i18n("Image compression level:"), groupBox1);
    m_JPEGCompression = new KIntNumInput(75, groupBox1);
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
    m_label_JPEGimageCompression->setBuddy( m_JPEGCompression );

    m_compressLossLess = new QCheckBox( i18n("Use lossless compression"), groupBox1);
    m_compressLossLess->setWhatsThis(i18n("If this option is enabled, "
                                          "all JPEG operations will use lossless compression."));

    connect(m_compressLossLess, SIGNAL( toggled(bool) ),
            this, SLOT( slotCompressLossLessEnabled(bool) ) );

    dvlay->addWidget( groupBox1 );

    // PNG File format.

    Q3GroupBox * groupBox2 = new Q3GroupBox( 2, Qt::Horizontal, i18n("PNG File Format"), box );

    m_label_PNGimageCompression = new QLabel (i18n("Image compression level:"), groupBox2);
    m_PNGCompression = new KIntNumInput(75, groupBox2);
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
    m_label_PNGimageCompression->setBuddy( m_PNGCompression );

    dvlay->addWidget( groupBox2 );

    // TIFF File format.

    Q3GroupBox * groupBox3 = new Q3GroupBox( 2, Qt::Horizontal, i18n("TIFF File Format"), box );

    m_label_TIFFimageCompression = new QLabel (i18n("Image compression algorithm:"), groupBox3);
    m_TIFFCompressionAlgo = new KComboBox(groupBox3);
    m_TIFFCompressionAlgo->insertItem("LZW");
    m_TIFFCompressionAlgo->insertItem("JPEG");
    m_TIFFCompressionAlgo->insertItem(i18n("None"));
    m_TIFFCompressionAlgo->setWhatsThis(i18n("Select here the TIFF compression algorithm.") );
    m_label_TIFFimageCompression->setBuddy( m_TIFFCompressionAlgo );

    dvlay->addWidget( groupBox3 );

    // TGA File format.

    Q3GroupBox * groupBox4 = new Q3GroupBox( 2, Qt::Horizontal, i18n("TGA File Format"), box );

    m_label_TGAimageCompression = new QLabel (i18n("Image compression algorithm:"), groupBox4);
    m_TGACompressionAlgo = new KComboBox(groupBox4);
    m_TGACompressionAlgo->insertItem("RLE");
    m_TGACompressionAlgo->insertItem(i18n("None"));
    m_TGACompressionAlgo->setWhatsThis(i18n("Select here the TGA compression algorithm.") );
    m_label_TGAimageCompression->setBuddy( m_TGACompressionAlgo );

    dvlay->addWidget( groupBox4 );
}

RecompressOptionsDialog::~RecompressOptionsDialog()
{
}

void RecompressOptionsDialog::slotCompressLossLessEnabled(bool val)
{
    m_JPEGCompression->setEnabled( !val );
    m_label_JPEGimageCompression->setEnabled( !val );
}

}  // namespace KIPIBatchProcessImagesPlugin
