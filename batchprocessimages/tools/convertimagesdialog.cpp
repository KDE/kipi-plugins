/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "convertimagesdialog.moc"

// Qt includes

#include <QGroupBox>
#include <QCheckBox>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QPushButton>
#include <QPointer>

// KDE includes

#include <kapplication.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>
#include <kurlrequester.h>

// Local includes

#include "batchprocessimageslist.h"
#include "convertoptionsdialog.h"
#include "kpversion.h"
#include "kpmetadata.h"

namespace KIPIBatchProcessImagesPlugin
{

ConvertImagesDialog::ConvertImagesDialog(const KUrl::List& urlList, QWidget* parent)
    : BatchProcessImagesDialog(urlList, i18n("Batch Convert Images"), parent)
{
    setOptionBoxTitle(i18n("Image Conversion Options"));

    m_labelType->setText(i18n("Format:"));

    m_Type->addItem("JPEG");
    m_Type->addItem("PNG");
    m_Type->addItem("TIFF");
    m_Type->addItem("PPM");
    m_Type->addItem("BMP");
    m_Type->addItem("TGA");
    m_Type->addItem("EPS");
    m_Type->setCurrentItem("JPEG");
    QString whatsThis = i18n("<p>Select here the target image file format.</p>");
    whatsThis = whatsThis + i18n("<p><b>JPEG</b>: The Joint Photographic Experts Group's file format is a "
                                 "good Web file format but it uses lossy data compression.</p>"
                                 "<p><b>PNG</b>: the Portable Network Graphics format is an extensible file "
                                 "format for the lossless, portable, well-compressed storage of raster images. "
                                 "PNG provides a patent-free replacement for GIF and can also replace many common "
                                 "uses of TIFF. PNG is designed to work well in online viewing applications, such "
                                 "as the World Wide Web, so it is fully streamable with a progressive display "
                                 "option. Also, PNG can store gamma and chromaticity data for improved color "
                                 "matching on heterogeneous platforms.</p>");
    whatsThis = whatsThis + i18n("<p><b>TIFF</b>: the Tag Image File Format is a rather old standard that is "
                                 "still very popular today. It is a highly flexible and platform-independent "
                                 "format which is supported by numerous image processing applications and "
                                 "virtually all prepress software on the market.</p>");
    whatsThis = whatsThis + i18n("<p><b>PPM</b>: the Portable Pixel Map file format is used as an "
                                 "intermediate format for storing color bitmap information. PPM files "
                                 "may be either binary or ASCII and store pixel values up to 24 bits in size. "
                                 "This format generates the largest-sized text files to encode images without "
                                 "losing quality.</p>");
    whatsThis = whatsThis + i18n("<p><b>BMP</b>: the BitMaP file format is a popular image format from Win32 "
                                 "environment. It efficiently stores mapped or unmapped RGB graphics data with "
                                 "pixels 1-, 4-, 8-, or 24-bits in size. Data may be stored raw or compressed "
                                 "using a 4-bit or 8-bit RLE data compression algorithm. BMP is an excellent "
                                 "choice for a simple bitmap format which supports a wide range of RGB image "
                                 "data.</p>");
    whatsThis = whatsThis + i18n("<p><b>TGA</b>: the TarGA image file format is one of the most widely used "
                                 "bitmap file formats for storage of 24 and 32 bits truecolor images.  "
                                 "TGA supports colormaps, alpha channel, gamma value, postage stamp image, "
                                 "textual information, and developer-definable data.</p>");
    whatsThis = whatsThis + i18n("<p><b>EPS</b>: the  Adobe Encapsulated PostScript image file format "
                                 "is a PostScript language program describing the appearance of a single page. "
                                 "Usually, the purpose of the EPS file is to be embedded inside another PostScript "
                                 "language page description.</p>");

    m_Type->setWhatsThis(whatsThis);

    setPreviewOptionsVisible(false);

    //---------------------------------------------

    readSettings();
    slotTypeChanged(m_Type->currentIndex());
}

ConvertImagesDialog::~ConvertImagesDialog()
{
}

void ConvertImagesDialog::slotTypeChanged(int type)
{
    if (type == 3 || type == 4 || type == 6)   // PPM || BMP || EPS
        m_optionsButton->setEnabled(false);
    else
        m_optionsButton->setEnabled(true);

    m_listFiles->clear();
    listImageFiles();
}

void ConvertImagesDialog::slotOptionsClicked()
{
    int Type = m_Type->currentIndex();
    QPointer<ConvertOptionsDialog> optionsDialog = new ConvertOptionsDialog(this, Type);

    if (Type == 0)
    { // JPEG
        optionsDialog->m_JPEGPNGCompression->setValue(m_JPEGPNGCompression);
        optionsDialog->m_compressLossLess->setChecked(m_compressLossLess);
    }
    else if (Type == 1) // PNG
    {
        optionsDialog->m_JPEGPNGCompression->setValue(m_JPEGPNGCompression);
    }
    else if (Type == 2) // TIFF
    {
        int index = optionsDialog->m_TIFFCompressionAlgo->findText(m_TIFFCompressionAlgo);
        if (index != -1) optionsDialog->m_TIFFCompressionAlgo->setCurrentIndex(index);
    }
    else if (Type == 5) // TGA
    {
        int index = optionsDialog->m_TGACompressionAlgo->findText(m_TGACompressionAlgo);
        if (index != -1) optionsDialog->m_TGACompressionAlgo->setCurrentIndex(index);
    }

    if (optionsDialog->exec() == KMessageBox::Ok)
    {
        if (Type == 0)
        { // JPEG
            m_JPEGPNGCompression = optionsDialog->m_JPEGPNGCompression->value();
            m_compressLossLess = optionsDialog->m_compressLossLess->isChecked();
        }
	else if (Type == 1) // PNG
            m_JPEGPNGCompression = optionsDialog->m_JPEGPNGCompression->value();
	else if (Type == 2) // TIFF
            m_TIFFCompressionAlgo = optionsDialog->m_TIFFCompressionAlgo->currentText();
	else if (Type == 5) // TGA
            m_TGACompressionAlgo = optionsDialog->m_TGACompressionAlgo->currentText();
    }

    delete optionsDialog;
}

void ConvertImagesDialog::readSettings()
{
    // Read all settings from configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("ConvertImages Settings");

    m_Type->setCurrentIndex(group.readEntry("ImagesFormat", 0));  // JPEG per default
    if (group.readEntry("CompressLossLess", "false") == "true")
        m_compressLossLess = true;
    else
        m_compressLossLess = false;

    m_JPEGPNGCompression  = group.readEntry("JPEGPNGCompression", 75);
    m_TIFFCompressionAlgo = group.readEntry("TIFFCompressionAlgo", i18nc("image compression type", "None"));
    m_TGACompressionAlgo  = group.readEntry("TGACompressionAlgo", i18nc("image compression type", "None"));

    readCommonSettings(group);
}

void ConvertImagesDialog::saveSettings()
{
    // Write all settings in configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("ConvertImages Settings");

    group.writeEntry("ImagesFormat", m_Type->currentIndex());
    group.writeEntry("JPEGPNGCompression", m_JPEGPNGCompression);
    group.writeEntry("CompressLossLess", m_compressLossLess);
    group.writeEntry("TIFFCompressionAlgo", m_TIFFCompressionAlgo);
    group.writeEntry("TGACompressionAlgo", m_TGACompressionAlgo);

    saveCommonSettings(group);
}

void ConvertImagesDialog::initProcess(KProcess* proc, BatchProcessImagesItem *item,
                                      const QString& albumDest, bool previewMode)
{
    *proc << "convert";

    if (m_Type->currentIndex() == 0)
    { // JPEG
        if (m_compressLossLess == true)
        {
            *proc << "-compress" << "Lossless";
        }
        else
        {
            *proc << "-quality";
            QString Temp;
            *proc << Temp.setNum(m_JPEGPNGCompression);
        }
    }
    else if (m_Type->currentIndex() == 1)
    { // PNG
        *proc << "-quality";
        QString Temp;
        *proc << Temp.setNum(m_JPEGPNGCompression);
    }
    else if (m_Type->currentIndex() == 2)
    { // TIFF
        *proc << "-compress";

        if (m_TIFFCompressionAlgo == i18nc("image compression type", "None"))
        {
            *proc << "None";
        }
        else
        {
            *proc << m_TIFFCompressionAlgo;
        }
    }
    else if (m_Type->currentIndex() == 5)
    { // TGA
        *proc << "-compress";

        if (m_TGACompressionAlgo == i18nc("image compression type", "None"))
        {
            *proc << "None";
        }
        else
        {
            *proc << m_TGACompressionAlgo;
        }
    }

    *proc << item->pathSrc() + "[0]";

    if (!previewMode)
    {   // No preview mode !
        *proc << albumDest + '/' + item->nameDest();
    }
}

void ConvertImagesDialog::processDone()
{
    if (m_Type->currentIndex() == 0)
    {
        // JPEG file, we remove IPTC preview.

        BatchProcessImagesItem* item = dynamic_cast<BatchProcessImagesItem*>(**m_listFile2Process_iterator);
        if (item)
        {
            QString src = item->pathSrc();
            QString tgt = destinationUrl().path() + '/' + item->nameDest();
            QFileInfo fi(tgt);

            kDebug() << src ;
            kDebug() << tgt << fi.size();

            KPMetadata metaIn(src);

            // Update Iptc preview.
            // NOTE: see bug #130525. a JPEG segment is limited to 64K. If the IPTC byte array is
            // bigger than 64K duing of image preview tag size, the target JPEG image will be
            // broken. Note that IPTC image preview tag is limited to 256K!!!
            // There is no limitation with TIFF and PNG about IPTC byte array size.

            metaIn.removeIptcTag("Iptc.Application2.Preview");
            metaIn.removeIptcTag("Iptc.Application2.PreviewFormat");
            metaIn.removeIptcTag("Iptc.Application2.PreviewVersion");

            KPMetadata metaOut(tgt);
            metaOut.setIptc(metaIn.getIptc());
            metaOut.applyChanges();
        }
    }

    BatchProcessImagesDialog::processDone();
}

QString ConvertImagesDialog::oldFileName2NewFileName(const QString& fileName)
{
    QString Temp;

    Temp = fileName.left(fileName.lastIndexOf('.', -1));           // The source file name without extension.
    Temp = Temp + '.' + ImageFileExt(m_Type->currentText());       // Added new file extension.

    return Temp;
}

QString ConvertImagesDialog::ImageFileExt(const QString& Ext)
{
    if (Ext == "TIFF" || Ext == "tiff")
        return ("tif");
    else if (Ext == "JPEG" || Ext == "jpeg" || Ext == "JPE" || Ext == "jpe")
        return ("jpg");
    else
        return (Ext.toLower());
}

}  // namespace KIPIBatchProcessImagesPlugin
