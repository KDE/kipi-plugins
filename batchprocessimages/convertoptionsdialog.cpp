//////////////////////////////////////////////////////////////////////////////
//
//    CONVERTOPTIONSDIALOG.CPP
//
//    Copyright (C) 2003 Gilles CAULIER <caulier dot gilles at gmail dot com>
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
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
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
#include <qcheckbox.h>

// Include files for KDE

#include <klocale.h>
#include <knuminput.h>

// Local includes

#include "convertoptionsdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

ConvertOptionsDialog::ConvertOptionsDialog(QWidget *parent, int ImageFormatType)
                        : KDialogBase( parent, "ConvertOptionsDialog", true,
                          i18n("Image File Format Options"), Ok|Cancel, Ok, false)
{
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );
    QString whatsThis;

    if (ImageFormatType == 0 || ImageFormatType == 1) // JPEG || PNG
       {
       m_label_imageCompression = new QLabel (i18n("Image compression level:"), box);
       dvlay->addWidget( m_label_imageCompression );
       m_JPEGPNGCompression = new KIntNumInput(75, box);
       m_JPEGPNGCompression->setRange(1, 100, 1, true );
       whatsThis = i18n("<p>The compression value for the target images:<p>");
       whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                    "<b>25</b>: high compression<p>"
                                    "<b>50</b>: medium compression<p>"
                                    "<b>75</b>: low compression (default value)<p>"
                                    "<b>100</b>: no compression");

       QWhatsThis::add( m_JPEGPNGCompression, whatsThis);
       m_label_imageCompression->setBuddy( m_JPEGPNGCompression );
       dvlay->addWidget( m_JPEGPNGCompression );

       if (ImageFormatType == 0) // JPEG
          {
          m_compressLossLess = new QCheckBox( i18n("Use lossless compression"), box);
          QWhatsThis::add( m_compressLossLess, i18n("<p>If this option is enabled, "
                                                    "all JPEG operations will use a lossless compression."));
          dvlay->addWidget( m_compressLossLess );

          connect(m_compressLossLess, SIGNAL( toggled(bool) ), this, SLOT( slotCompressLossLessEnabled(bool) ) );
          }
       }

    if (ImageFormatType == 2) // TIFF
       {
       QLabel *m_label_imageCompression = new QLabel (i18n("Image compression algorithm:"), box);
       dvlay->addWidget( m_label_imageCompression );
       m_TIFFCompressionAlgo = new QComboBox( false, box );
       m_TIFFCompressionAlgo->insertItem("LZW");
       m_TIFFCompressionAlgo->insertItem("JPEG");
       m_TIFFCompressionAlgo->insertItem(i18n("None"));
       QWhatsThis::add( m_TIFFCompressionAlgo, i18n("<p>Select here the compression algorithm.") );
       m_label_imageCompression->setBuddy( m_TIFFCompressionAlgo );
       dvlay->addWidget( m_TIFFCompressionAlgo );
       }

    if (ImageFormatType == 5) // TGA
       {
       QLabel *m_label_imageCompression = new QLabel (i18n("Image compression algorithm:"), box);
       dvlay->addWidget( m_label_imageCompression );
       m_TGACompressionAlgo = new QComboBox( false, box );
       m_TGACompressionAlgo->insertItem("RLE");
       m_TGACompressionAlgo->insertItem(i18n("None"));
       QWhatsThis::add( m_TGACompressionAlgo, i18n("<p>Select here the compression algorithm.") );
       m_label_imageCompression->setBuddy( m_TGACompressionAlgo );
       dvlay->addWidget( m_TGACompressionAlgo );
       }
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

ConvertOptionsDialog::~ConvertOptionsDialog()
{
}


///////////////////////////////////////// SLOTS /////////////////////////////////////////////

void ConvertOptionsDialog::slotCompressLossLessEnabled(bool val)
{
    m_JPEGPNGCompression->setEnabled( !val );
    m_label_imageCompression->setEnabled( !val );
}

}  // NameSpace KIPIBatchProcessImagesPlugin

#include "convertoptionsdialog.moc"
