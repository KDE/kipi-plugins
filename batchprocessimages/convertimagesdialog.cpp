//////////////////////////////////////////////////////////////////////////////
//
//    CONVERTIMAGESDIALOG.CPP
//
//    Copyright (C) 2003-2004 Gilles CAULIER <caulier dot gilles at free.fr>
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

#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <qdir.h>

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>

// Local includes

#include "convertimagesdialog.h"
#include "convertoptionsdialog.h"
#include "outputdialog.h"


//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

ConvertImagesDialog::ConvertImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
                 : BatchProcessImagesDialog( urlList, interface, parent )
{
    setCaption(i18n("Batch Convert Images options"));
    setHelp("plugin-convertimages.anchor", "kipi");

    //---------------------------------------------

    groupBox1->setTitle( i18n("Convert images options") );

    m_labelType->setText( i18n("Target Image files format:") );

    m_Type->insertItem("JPEG");
    m_Type->insertItem("PNG");
    m_Type->insertItem("TIFF");
    m_Type->insertItem("PPM");
    m_Type->insertItem("BMP");
    m_Type->insertItem("TGA");
    m_Type->setCurrentText("JPEG");
    whatsThis = i18n("<p>Select here the target image file format.<p>");
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts Group file format is a "
                                 "good Web file format but it use a compression with data lost.<p>"
                                 "<b>PNG</b>: the Portable Network Graphics format is an extensible file "
                                 "format for the lossless, portable, well-compressed storage of raster images. "
                                 "PNG provides a patent-free replacement for GIF and can also replace many common "
                                 "uses of TIFF. PNG is designed to work well in online viewing applications, such "
                                 "as the World Wide Web, so it is fully streamable with a progressive display "
                                 "option. Also, PNG can store gamma and chromaticity data for improved color "
                                 "matching on heterogeneous platforms.");
    whatsThis = whatsThis + i18n("<p><b>TIFF</b>: the Tag Image File Format is a rather old standard that is "
                                 "still very popular today. It is a highly flexible and platform-independent "
                                 "format which is supported by numerous image processing applications and "
                                 "virtually all prepress software on the market.");
    whatsThis = whatsThis + i18n("<p><b>PPM</b>: the Portable Pixel Map file format is used as an "
                                 "intermediate format for storing color bitmap informations. PPM files "
                                 "may be either binary or ASCII and store pixel values up to 24 bits in size. "
                                 "This format generate very biggest sized text files for encode images without "
                                 "losing quality");
    whatsThis = whatsThis + i18n("<p><b>BMP</b>: the BitMaP file format is a popular image format from Win32 "
                                 "environment. It efficiently stores mapped or unmapped RGB graphics data with "
                                 "pixels 1-, 4-, 8-, or 24-bits in size. Data may be stored raw or compressed "
                                 "using a 4-bit or 8-bit RLE data compression algorithm. BMP is an excellent "
                                 "choice for a simple bitmap format which supports a wide range of RGB image "
                                 "data.");
    whatsThis = whatsThis + i18n("<p><b>TGA</b>: the TarGA image file format is one of the most widely used "
                                 "bitmap file formats for storage of 24 and 32 bits truecolor images.  "
                                 "TGA supports colormaps, alpha channel, gamma value, postage stamp image, "
                                 "textual information, and developer-definable data.");

    QWhatsThis::add( m_Type, whatsThis );

    m_previewButton->hide();
    m_smallPreview->hide();

    //---------------------------------------------

    readSettings();
    slotTypeChanged(m_Type->currentText());
    slotOverWriteModeActived(m_overWriteMode->currentText());
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

ConvertImagesDialog::~ConvertImagesDialog()
{
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void ConvertImagesDialog::slotAbout( void )
{
    KMessageBox::about(this, i18n("A Digikam plugin for batch convert images\n\n"
                                  "Author: Gilles Caulier\n\n"
                                  "Email: caulier dot gilles at free.fr\n\n"
                                  "This plugin use the \"convert\" program from \"ImageMagick\" package.\n"),
                                  i18n("About Digikam batch convert images"));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ConvertImagesDialog::slotTypeChanged(const QString &string)
{
    if ( string == "BMP" || string == "PPM" )
       m_optionsButton->setEnabled(false);
    else
       m_optionsButton->setEnabled(true);

    m_listFiles->clear();
    listImageFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ConvertImagesDialog::slotOptionsClicked(void)
{
    QString Type = m_Type->currentText();
    ConvertOptionsDialog *optionsDialog = new ConvertOptionsDialog(this, Type);

    if (Type == "JPEG")
       {
       optionsDialog->m_JPEGPNGCompression->setValue(m_JPEGPNGCompression);
       optionsDialog->m_compressLossLess->setChecked(m_compressLossLess);
       }
    if (Type == "PNG")
       {
       optionsDialog->m_JPEGPNGCompression->setValue(m_JPEGPNGCompression);
       }
    if (Type == "TIFF")
       optionsDialog->m_TIFFCompressionAlgo->setCurrentText(m_TIFFCompressionAlgo);
    if (Type == "TGA")
       optionsDialog->m_TGACompressionAlgo->setCurrentText(m_TGACompressionAlgo);

    if ( optionsDialog->exec() == KMessageBox::Ok )
       {
       if (Type == "JPEG")
          {
          m_JPEGPNGCompression = optionsDialog->m_JPEGPNGCompression->value();
          m_compressLossLess = optionsDialog->m_compressLossLess->isChecked();
          }
       if (Type == "PNG")
          m_JPEGPNGCompression = optionsDialog->m_JPEGPNGCompression->value();
       if (Type == "TIFF")
          m_TIFFCompressionAlgo = optionsDialog->m_TIFFCompressionAlgo->currentText();
       if (Type == "TGA")
          m_TGACompressionAlgo = optionsDialog->m_TGACompressionAlgo->currentText();
       }

    delete optionsDialog;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ConvertImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("digikamrc");
    m_config->setGroup("ConvertImages Settings");

    m_Type->setCurrentText(m_config->readEntry("ImagesFormat", "JPEG"));
    if ( m_config->readEntry("CompressLossLess", "false") == "true")
       m_compressLossLess = true;
    else
       m_compressLossLess = false;

    m_JPEGPNGCompression = m_config->readNumEntry("JPEGPNGCompression", 75);
    m_TIFFCompressionAlgo = m_config->readEntry("TIFFCompressionAlgo", i18n("None"));
    m_TGACompressionAlgo = m_config->readEntry("TGACompressionAlgo", i18n("None"));

    m_overWriteMode->setCurrentItem(m_config->readNumEntry("OverWriteMode", 2));  // 'Rename' per default...

    if (m_config->readEntry("RemoveOriginal", "false") == "true")
        m_removeOriginal->setChecked( true );
    else
        m_removeOriginal->setChecked( false );

    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ConvertImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("digikamrc");
    m_config->setGroup("ConvertImages Settings");

    m_config->writeEntry("ImagesFormat", m_Type->currentText());
    m_config->writeEntry("JPEGPNGCompression", m_JPEGPNGCompression);
    m_config->writeEntry("CompressLossLess", m_compressLossLess);
    m_config->writeEntry("TIFFCompressionAlgo", m_TIFFCompressionAlgo);
    m_config->writeEntry("TGACompressionAlgo", m_TGACompressionAlgo);

    m_config->writeEntry("OverWriteMode", m_overWriteMode->currentItem());
    m_config->writeEntry("RemoveOriginal", m_removeOriginal->isChecked());
    m_config->sync();

    delete m_config;
}


////////////////////////////////////////////// FONCTIONS ////////////////////////////////////////////

#ifdef TEMPORARILY_REMOVED
QString ConvertImagesDialog::makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                                         Digikam::AlbumInfo *albumDest)
{
    *proc << "convert";

    if ( albumDest == 0L && m_smallPreview->isChecked() )    // Preview mode and small preview enabled !
       {
       *m_PreviewProc << "-crop" << "300x300+0+0";
       m_previewOutput.append( " -crop 300x300+0+0 ");
       }

    if (m_Type->currentText() == "JPEG")
       {
       if (m_compressLossLess == true)
          {
          *proc << "-compress" << "Lossless";
          }
       else
          {
          *proc << "-quality";
          QString Temp;
          *proc << Temp.setNum( m_JPEGPNGCompression );
          }
       }

    if (m_Type->currentText() == "PNG")
       {
       *proc << "-quality";
       QString Temp;
       *proc << Temp.setNum( m_JPEGPNGCompression );
       }

    if (m_Type->currentText() == "TIFF")
       {
       *proc << "-compress";

       if (m_TIFFCompressionAlgo == i18n("None"))
          {
          *proc << "None";
          }
       else
          {
          *proc << m_TIFFCompressionAlgo;
          }
       }

    if (m_Type->currentText() == "TGA")
       {
       *proc << "-compress";

       if (m_TGACompressionAlgo == i18n("None"))
          {
          *proc << "None";
          }
       else
          {
          *proc << m_TGACompressionAlgo;
          }
       }

    *proc << "-verbose";

    *proc << item->pathSrc();

    if ( albumDest != 0L )   // No preview mode !
       {
       *proc << albumDest->getPath() + "/" + item->nameDest();
       }


    return(extractArguments(proc));
}
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString ConvertImagesDialog::oldFileName2NewFileName(QString fileName)
{
    qDebug("ConvertImagesDialog::oldFileName2NewFileName is commented out!");
#ifdef TEMPORARILY_REMOVED
    QString Temp;

    Temp = fileName.left( fileName.findRev('.', -1) );             // The source file name without extension.
    Temp = Temp + "." + ImageFileExt(m_Type->currentText());       // Added new file extension.

    return Temp;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString ConvertImagesDialog::ImageFileExt(QString Ext)
{
    if ( Ext == "TIFF" || Ext == "tiff" )
       return ("tif");
    else if ( Ext == "JPEG" || Ext == "jpeg" )
       return ("jpg");
    else
       return (Ext.lower());
}
