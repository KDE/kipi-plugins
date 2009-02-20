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

// Include files for Qt

#include <q3vbox.h>
#include <qlayout.h>
#include <qwidget.h>
#include <q3whatsthis.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <q3groupbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

// Include files for KDE

#include <klocale.h>
#include <knuminput.h>

// Local includes

#include "recompressoptionsdialog.h"
#include "recompressoptionsdialog.moc"

namespace KIPIBatchProcessImagesPlugin
{

RecompressOptionsDialog::RecompressOptionsDialog(QWidget *parent)
                       : KDialogBase( parent)
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
    m_JPEGCompression->setRange(1, 100, 1, true );
    whatsThis = i18n("<p>The compression value for JPEG target images:<p>");
    whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                 "<b>25</b>: high compression<p>"
                                 "<b>50</b>: medium compression<p>"
                                 "<b>75</b>: low compression (default value)<p>"
                                 "<b>100</b>: no compression");

    Q3WhatsThis::add( m_JPEGCompression, whatsThis);
    m_label_JPEGimageCompression->setBuddy( m_JPEGCompression );

    m_compressLossLess = new QCheckBox( i18n("Use lossless compression"), groupBox1);
    Q3WhatsThis::add( m_compressLossLess, i18n("<p>If this option is enabled, "
                                              "all JPEG operations will use lossless compression."));

    connect(m_compressLossLess, SIGNAL( toggled(bool) ),
            this, SLOT( slotCompressLossLessEnabled(bool) ) );

    dvlay->addWidget( groupBox1 );

    // PNG File format.

    Q3GroupBox * groupBox2 = new Q3GroupBox( 2, Qt::Horizontal, i18n("PNG File Format"), box );

    m_label_PNGimageCompression = new QLabel (i18n("Image compression level:"), groupBox2);
    m_PNGCompression = new KIntNumInput(75, groupBox2);
    m_PNGCompression->setRange(1, 100, 1, true );
    whatsThis = i18n("<p>The compression value for PNG target images:<p>");
    whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                 "<b>25</b>: high compression<p>"
                                 "<b>50</b>: medium compression<p>"
                                 "<b>75</b>: low compression (default value)<p>"
                                 "<b>100</b>: no compression");

    Q3WhatsThis::add( m_PNGCompression, whatsThis);
    m_label_PNGimageCompression->setBuddy( m_PNGCompression );

    dvlay->addWidget( groupBox2 );

    // TIFF File format.

    Q3GroupBox * groupBox3 = new Q3GroupBox( 2, Qt::Horizontal, i18n("TIFF File Format"), box );

    m_label_TIFFimageCompression = new QLabel (i18n("Image compression algorithm:"), groupBox3);
    m_TIFFCompressionAlgo = new QComboBox( false, groupBox3 );
    m_TIFFCompressionAlgo->insertItem("LZW");
    m_TIFFCompressionAlgo->insertItem("JPEG");
    m_TIFFCompressionAlgo->insertItem(i18n("None"));
    Q3WhatsThis::add( m_TIFFCompressionAlgo, i18n("<p>Select here the TIFF compression algorithm.") );
    m_label_TIFFimageCompression->setBuddy( m_TIFFCompressionAlgo );

    dvlay->addWidget( groupBox3 );

    // TGA File format.

    Q3GroupBox * groupBox4 = new Q3GroupBox( 2, Qt::Horizontal, i18n("TGA File Format"), box );

    m_label_TGAimageCompression = new QLabel (i18n("Image compression algorithm:"), groupBox4);
    m_TGACompressionAlgo = new QComboBox( false, groupBox4 );
    m_TGACompressionAlgo->insertItem("RLE");
    m_TGACompressionAlgo->insertItem(i18n("None"));
    Q3WhatsThis::add( m_TGACompressionAlgo, i18n("<p>Select here the TGA compression algorithm.") );
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

}  // NameSpace KIPIBatchProcessImagesPlugin
