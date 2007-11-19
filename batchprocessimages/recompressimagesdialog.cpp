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

#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Local includes

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "recompressoptionsdialog.h"
#include "outputdialog.h"
#include "recompressimagesdialog.h"
#include "recompressimagesdialog.moc"

namespace KIPIBatchProcessImagesPlugin
{

RecompressImagesDialog::RecompressImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
                      : BatchProcessImagesDialog( urlList, interface, i18n("Batch Recompress Images"), parent )
{
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Batch recompress images"),
                                           NULL,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to batch recompress images\n"
                                                     "This plugin uses the \"convert\" program from \"ImageMagick\" package."),
                                           "(c) 2003-2007, Gilles Caulier");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                       "caulier dot gilles at gmail dot com");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Batch Recompress Images Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    groupBox1->setTitle( i18n("Image Recompression Options") );

    m_labelType->hide();
    m_Type->hide();
    m_previewButton->hide();
    m_smallPreview->hide();

    //---------------------------------------------

    readSettings();
    listImageFiles();
}

RecompressImagesDialog::~RecompressImagesDialog()
{
    delete m_about;
}

void RecompressImagesDialog::slotHelp( void )
{
    KApplication::kApplication()->invokeHelp("recompressimages", "kipi-plugins");
}

void RecompressImagesDialog::slotOptionsClicked(void)
{
    RecompressOptionsDialog *optionsDialog = new RecompressOptionsDialog(this);

    optionsDialog->m_JPEGCompression->setValue(m_JPEGCompression);
    optionsDialog->m_compressLossLess->setChecked(m_compressLossLess);
    optionsDialog->m_PNGCompression->setValue(m_PNGCompression);
    optionsDialog->m_TIFFCompressionAlgo->setCurrentText(m_TIFFCompressionAlgo);
    optionsDialog->m_TGACompressionAlgo->setCurrentText(m_TGACompressionAlgo);

    if ( optionsDialog->exec() == KMessageBox::Ok )
       {
       m_JPEGCompression = optionsDialog->m_JPEGCompression->value();
       m_compressLossLess = optionsDialog->m_compressLossLess->isChecked();
       m_PNGCompression = optionsDialog->m_PNGCompression->value();
       m_TIFFCompressionAlgo = optionsDialog->m_TIFFCompressionAlgo->currentText();
       m_TGACompressionAlgo = optionsDialog->m_TGACompressionAlgo->currentText();
       }

    delete optionsDialog;
}

void RecompressImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("RecompressImages Settings");

    m_JPEGCompression = m_config->readNumEntry("JPEGCompression", 75);

    if ( m_config->readEntry("CompressLossLess", "false") == "true")
       m_compressLossLess = true;
    else
       m_compressLossLess = false;

    m_PNGCompression = m_config->readNumEntry("PNGCompression", 75);
    m_TIFFCompressionAlgo = m_config->readEntry("TIFFCompressionAlgo", i18n("None"));
    m_TGACompressionAlgo = m_config->readEntry("TGACompressionAlgo", i18n("None"));

    m_overWriteMode->setCurrentItem(m_config->readNumEntry("OverWriteMode", 2));  // 'Rename' per default...

    if (m_config->readEntry("RemoveOriginal", "false") == "true")
        m_removeOriginal->setChecked( true );
    else
        m_removeOriginal->setChecked( false );

    delete m_config;
}

void RecompressImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("RecompressImages Settings");

    m_config->writeEntry("JPEGCompression", m_JPEGCompression);
    m_config->writeEntry("PNGCompression", m_PNGCompression);
    m_config->writeEntry("CompressLossLess", m_compressLossLess);
    m_config->writeEntry("TIFFCompressionAlgo", m_TIFFCompressionAlgo);
    m_config->writeEntry("TGACompressionAlgo", m_TGACompressionAlgo);

    m_config->writeEntry("OverWriteMode", m_overWriteMode->currentItem());
    m_config->writeEntry("RemoveOriginal", m_removeOriginal->isChecked());

    m_config->sync();

    delete m_config;
}

QString RecompressImagesDialog::makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                                            const QString& albumDest, bool previewMode)
{
    *proc << "convert";

    if ( previewMode && m_smallPreview->isChecked() )    // Preview mode and small preview enabled !
       {
       *m_PreviewProc << "-crop" << "300x300+0+0";
       m_previewOutput.append( " -crop 300x300+0+0 ");
       }

    QString imageExt = item->nameSrc().section('.', -1 );

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
          *proc << Temp.setNum( m_JPEGCompression );
          }
       }

    else if (imageExt == "PNG" || imageExt == "png")
       {
       *proc << "-quality";
       QString Temp;
       *proc << Temp.setNum( m_PNGCompression );
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

    if ( !previewMode )   // No preview mode !
       {
       *proc << albumDest + "/" + item->nameDest();
       }

    return(extractArguments(proc));
}

bool RecompressImagesDialog::prepareStartProcess(BatchProcessImagesItem *item,
                                                 const QString& /*albumDest*/)
{
    QString imageExt = item->nameSrc().section('.', -1 );

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

}  // NameSpace KIPIBatchProcessImagesPlugin
