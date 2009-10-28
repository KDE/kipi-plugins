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

#include "recompressimagesdialog.h"
#include "recompressimagesdialog.moc"

// Qt includes

#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QPointer>

// KDE includes

#include <kapplication.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "kpaboutdata.h"
#include "outputdialog.h"
#include "pluginsversion.h"
#include "recompressoptionsdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

RecompressImagesDialog::RecompressImagesDialog(const KUrl::List& urlList, KIPI::Interface* interface, QWidget *parent)
                      : BatchProcessImagesDialog(urlList, interface, i18n("Batch Recompress Images"), parent)
{
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("Batch recompress images"),
                                           QByteArray(),
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to batch recompress images.\n"
                                                 "This plugin uses the \"convert\" program from the \"ImageMagick\" package."),
                                           ki18n("(c) 2003-2009, Gilles Caulier\n"
                                                 "(c) 2007-2009, Aurélien Gateau"));

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor(ki18n("Aurelien Gateau"), ki18n("Maintainer"),
                       "aurelien dot gateau at free dot fr");

    DialogUtils::setupHelpButton(this, m_about);
    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    setOptionBoxTitle(i18n("Image Recompression Options"));

    m_labelType->hide();
    m_Type->hide();
    setPreviewOptionsVisible(false);

    //---------------------------------------------

    readSettings();
    listImageFiles();
}

RecompressImagesDialog::~RecompressImagesDialog()
{
    delete m_about;
}

void RecompressImagesDialog::slotHelp()
{
    KToolInvocation::invokeHelp("recompressimages", "kipi-plugins");
}

void RecompressImagesDialog::slotOptionsClicked()
{
    QPointer<RecompressOptionsDialog> optionsDialog = new RecompressOptionsDialog(this);

    optionsDialog->m_JPEGCompression->setValue(m_JPEGCompression);
    optionsDialog->m_compressLossLess->setChecked(m_compressLossLess);
    optionsDialog->m_PNGCompression->setValue(m_PNGCompression);

    int index = optionsDialog->m_TIFFCompressionAlgo->findText(m_TIFFCompressionAlgo);
    if (index != -1) optionsDialog->m_TIFFCompressionAlgo->setCurrentIndex(index);

    index = optionsDialog->m_TGACompressionAlgo->findText(m_TGACompressionAlgo);
    if (index != -1) optionsDialog->m_TGACompressionAlgo->setCurrentIndex(index);

    if (optionsDialog->exec() == KMessageBox::Ok)
    {
        m_JPEGCompression     = optionsDialog->m_JPEGCompression->value();
        m_compressLossLess    = optionsDialog->m_compressLossLess->isChecked();
        m_PNGCompression      = optionsDialog->m_PNGCompression->value();
        m_TIFFCompressionAlgo = optionsDialog->m_TIFFCompressionAlgo->currentText();
        m_TGACompressionAlgo  = optionsDialog->m_TGACompressionAlgo->currentText();
    }

    delete optionsDialog;
}

void RecompressImagesDialog::readSettings()
{
    // Read all settings from configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("RecompressImages Settings");

    m_JPEGCompression = group.readEntry("JPEGCompression", 75);

    if (group.readEntry("CompressLossLess", "false") == "true")
        m_compressLossLess = true;
    else
        m_compressLossLess = false;

    m_PNGCompression = group.readEntry("PNGCompression", 75);
    m_TIFFCompressionAlgo = group.readEntry("TIFFCompressionAlgo", i18n("None"));
    m_TGACompressionAlgo = group.readEntry("TGACompressionAlgo", i18n("None"));

    readCommonSettings(group);
}

void RecompressImagesDialog::saveSettings()
{
    // Write all settings in configuration file.

    KConfig config("kipirc");
    KConfigGroup group = config.group("RecompressImages Settings");

    group.writeEntry("JPEGCompression", m_JPEGCompression);
    group.writeEntry("PNGCompression", m_PNGCompression);
    group.writeEntry("CompressLossLess", m_compressLossLess);
    group.writeEntry("TIFFCompressionAlgo", m_TIFFCompressionAlgo);
    group.writeEntry("TGACompressionAlgo", m_TGACompressionAlgo);

    saveCommonSettings(group);
}

void RecompressImagesDialog::initProcess(KProcess* proc, BatchProcessImagesItem *item,
        const QString& albumDest, bool previewMode)
{
    *proc << "convert";

    QString imageExt = item->nameSrc().section('.', -1);

    if (imageExt == "JPEG" || imageExt == "jpeg" || imageExt == "JPG" || imageExt == "jpg")
    {
        if (m_compressLossLess == true)
        {
            *proc << "-compress" << "Lossless";
        }
        else
        {
            *proc << "-quality";
            QString Temp;
            *proc << Temp.setNum(m_JPEGCompression);
        }
    }

    else if (imageExt == "PNG" || imageExt == "png")
    {
        *proc << "-quality";
        QString Temp;
        *proc << Temp.setNum(m_PNGCompression);
    }

    else if (imageExt == "TIFF" || imageExt == "tiff" || imageExt == "TIF" || imageExt == "tif")
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

    else if (imageExt == "TGA" || imageExt == "tga")
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

    if (!previewMode) {   // No preview mode !
        *proc << albumDest + "/" + item->nameDest();
    }
}

bool RecompressImagesDialog::prepareStartProcess(BatchProcessImagesItem *item, const QString& /*albumDest*/)
{
    QString imageExt = item->nameSrc().section('.', -1);

    if (imageExt != "JPEG" && imageExt != "jpeg" &&
            imageExt != "JPG"  && imageExt != "jpg"  &&
            imageExt != "JPE"  && imageExt != "jpe"  &&
            imageExt != "PNG"  && imageExt != "png"  &&
            imageExt != "TIFF" && imageExt != "tiff" &&
            imageExt != "TIF"  && imageExt != "tif"  &&
            imageExt != "TGA"  && imageExt != "tga")
    {
        item->changeResult(i18n("Skipped."));
        item->changeError(i18n("image file format unsupported."));
        return false;
    }

    return true;
}

}  // namespace KIPIBatchProcessImagesPlugin
