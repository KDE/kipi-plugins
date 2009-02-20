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

#include <q3groupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <q3whatsthis.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <k3process.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <ktoolinvocation.h>

// Local includes

#include "dialogutils.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "filteroptionsdialog.h"
#include "outputdialog.h"
#include "imagepreview.h"
#include "filterimagesdialog.h"
#include "filterimagesdialog.moc"

namespace KIPIBatchProcessImagesPlugin
{

FilterImagesDialog::FilterImagesDialog( KUrl::List urlList, KIPI::Interface* interface, QWidget *parent )
                  : BatchProcessImagesDialog( urlList, interface, i18n("Batch Image Filtering"), parent )
{
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Batch image filtering"),
                                           0,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to batch filter images\n"
                                                    "This plugin uses the \"convert\" program from \"ImageMagick\" package."),
                                           "(c) 2003-2007, Gilles Caulier");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                       "caulier dot gilles at gmail dot com");

    DialogUtils::setupHelpButton(this, m_about);

    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    groupBox1->setTitle( i18n("Image Filtering Options") );

    m_labelType->setText( i18n("Filter:") );

    m_Type->insertItem(i18n("Add Noise"));       // 0
    m_Type->insertItem(i18n("Antialias"));
    m_Type->insertItem(i18n("Blur"));
    m_Type->insertItem(i18n("Despeckle"));
    m_Type->insertItem(i18n("Enhance"));         // 4
    m_Type->insertItem(i18n("Median"));
    m_Type->insertItem(i18n("Noise Reduction"));
    m_Type->insertItem(i18n("Sharpen"));
    m_Type->insertItem(i18n("Unsharp"));         // 8
    m_Type->setCurrentText(i18n("Sharpen"));
    whatsThis = i18n("<p>Select here the filter type for your images:<p>"
                     "<b>Add noise</b>: add artificial noise to an image.<p>"
                     "<b>Antialias</b>: remove pixel aliasing.<p>"
                     "<b>Blur</b>: blur the image with a Gaussian operator.<p>"
                     "<b>Despeckle</b>: reduces the speckle noise in an image while perserving the "
                     "edges of the original image.<p>"
                     "<b>Enhance</b>: apply a digital filter to enhance a noisy image.<p>"
                     "<b>Median</b>: apply a median filter to an image.<p>"
                     "<b>Noise reduction</b>: reduce noise in an image. <p>"
                     "<b>Sharpen</b>: sharpen the image with a Gaussian operator.<p>"
                     "<b>Unsharp</b>: sharpen the image with an unsharp mask operator.<p>");

    Q3WhatsThis::add( m_Type, whatsThis );

    //---------------------------------------------

    readSettings();
    listImageFiles();
    slotTypeChanged(m_Type->currentItem());
}

FilterImagesDialog::~FilterImagesDialog()
{
    delete m_about;
}

void FilterImagesDialog::slotHelp( void )
{
    KToolInvocation::invokeHelp("filterimages", "kipi-plugins");
}

void FilterImagesDialog::slotTypeChanged(int type)
{
    if ( type == 1 ||      // Antialias
         type == 3 ||      // Despeckle
         type == 4 )       // Enhance
       m_optionsButton->setEnabled(false);
    else
       m_optionsButton->setEnabled(true);
}

void FilterImagesDialog::slotOptionsClicked(void)
{
    int Type = m_Type->currentItem();
    FilterOptionsDialog *optionsDialog = new FilterOptionsDialog(this, Type);

    if ( Type == 0) // Add noise
       optionsDialog->m_noiseType->setCurrentText(m_noiseType);

    if ( Type == 2) // Blur
       {
       optionsDialog->m_blurRadius->setValue(m_blurRadius);
       optionsDialog->m_blurDeviation->setValue(m_blurDeviation);
       }

    if ( Type == 5) // Median
       optionsDialog->m_medianRadius->setValue(m_medianRadius);

    if ( Type == 6) // Noise reduction
       optionsDialog->m_noiseRadius->setValue(m_noiseRadius);

    if ( Type == 7) // Sharpen
       {
       optionsDialog->m_sharpenRadius->setValue(m_sharpenRadius);
       optionsDialog->m_sharpenDeviation->setValue(m_sharpenDeviation);
       }

    if ( Type == 8) // Unsharp
       {
       optionsDialog->m_unsharpenRadius->setValue(m_unsharpenRadius);
       optionsDialog->m_unsharpenDeviation->setValue(m_unsharpenDeviation);
       optionsDialog->m_unsharpenPercent->setValue(m_unsharpenPercent);
       optionsDialog->m_unsharpenThreshold->setValue(m_unsharpenThreshold);
       }

    if ( optionsDialog->exec() == KMessageBox::Ok )
       {
       if ( Type == 0) // Add noise
          m_noiseType = optionsDialog->m_noiseType->currentText();

       if ( Type == 2) // Blur
          {
          m_blurRadius = optionsDialog->m_blurRadius->value();
          m_blurDeviation = optionsDialog->m_blurDeviation->value();
          }

       if ( Type == 5) // Median
          m_medianRadius = optionsDialog->m_medianRadius->value();

       if ( Type == 6) // Noise reduction
          m_noiseRadius = optionsDialog->m_noiseRadius->value();

       if ( Type == 7) // Sharpen
          {
          m_sharpenRadius = optionsDialog->m_sharpenRadius->value();
          m_sharpenDeviation = optionsDialog->m_sharpenDeviation->value();
          }

       if ( Type == 8) // Unsharp
          {
          m_unsharpenRadius = optionsDialog->m_unsharpenRadius->value();
          m_unsharpenDeviation = optionsDialog->m_unsharpenDeviation->value();
          m_unsharpenPercent = optionsDialog->m_unsharpenPercent->value();
          m_unsharpenThreshold = optionsDialog->m_unsharpenThreshold->value();
          }
       }

    delete optionsDialog;
}

void FilterImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("FilterImages Settings");

    m_Type->setCurrentItem(m_config->readNumEntry("FilterType", 7));      // Sharpen per default
    m_noiseType = m_config->readEntry("NoiseType", i18n("Gaussian"));
    m_blurRadius = m_config->readNumEntry("BlurRadius", 3);
    m_blurDeviation = m_config->readNumEntry("BlurDeviation", 1);
    m_medianRadius = m_config->readNumEntry("MedianRadius", 3);
    m_noiseRadius = m_config->readNumEntry("NoiseRadius", 3);
    m_sharpenRadius = m_config->readNumEntry("SharpenRadius", 3);
    m_sharpenDeviation = m_config->readNumEntry("SharpenDeviation", 1);
    m_unsharpenRadius = m_config->readNumEntry("UnsharpenRadius", 3);
    m_unsharpenDeviation = m_config->readNumEntry("UnsharpenDeviation", 1);
    m_unsharpenPercent = m_config->readNumEntry("UnsharpenPercent", 3);
    m_unsharpenThreshold = m_config->readNumEntry("UnsharpenThreshold", 1);

    if (m_config->readEntry("SmallPreview", "true") == "true")
        m_smallPreview->setChecked( true );
    else
        m_smallPreview->setChecked( false );

    m_overWriteMode->setCurrentItem(m_config->readNumEntry("OverWriteMode", 2));  // 'Rename' per default...

    if (m_config->readEntry("RemoveOriginal", "false") == "true")
        m_removeOriginal->setChecked( true );
    else
        m_removeOriginal->setChecked( false );

    delete m_config;
}

void FilterImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("FilterImages Settings");

    m_config->writeEntry("FilterType", m_Type->currentItem());
    m_config->writeEntry("NoiseType", m_noiseType);
    m_config->writeEntry("BlurRadius", m_blurRadius);
    m_config->writeEntry("BlurDeviation", m_blurDeviation);
    m_config->writeEntry("MedianRadius", m_medianRadius);
    m_config->writeEntry("NoiseRadius", m_noiseRadius);
    m_config->writeEntry("SharpenRadius", m_sharpenRadius);
    m_config->writeEntry("SharpenDeviation", m_sharpenDeviation);
    m_config->writeEntry("UnsharpenRadius", m_unsharpenRadius);
    m_config->writeEntry("UnsharpenDeviation", m_unsharpenDeviation);
    m_config->writeEntry("UnsharpenPercent", m_unsharpenPercent);
    m_config->writeEntry("UnsharpenThreshold", m_unsharpenThreshold);

    m_config->writeEntry("SmallPreview", m_smallPreview->isChecked());
    m_config->writeEntry("OverWriteMode", m_overWriteMode->currentItem());
    m_config->writeEntry("RemoveOriginal", m_removeOriginal->isChecked());

    m_config->sync();

    delete m_config;
}

QString FilterImagesDialog::makeProcess(K3Process* proc, BatchProcessImagesItem *item,
                                        const QString& albumDest, bool previewMode)
{
    *proc << "convert";

    if ( previewMode && m_smallPreview->isChecked() )    // Preview mode and small preview enabled !
       {
       *m_PreviewProc << "-crop" << "300x300+0+0";
       m_previewOutput.append( " -crop 300x300+0+0 ");
       }

    if (m_Type->currentItem() == 0) // Add noise
       {
       QString Temp;
       if ( m_noiseType == i18n("Uniform") ) Temp = "Uniform";
       if ( m_noiseType == i18n("Gaussian") ) Temp = "Gaussian";
       if ( m_noiseType == i18n("Multiplicative") ) Temp = "Multiplicative";
       if ( m_noiseType == i18n("Impulse") ) Temp = "Impulse";
       if ( m_noiseType == i18n("Laplacian") ) Temp = "Laplacian";
       if ( m_noiseType == i18n("Poisson") ) Temp = "Poisson";
       *proc << "+noise" << Temp;
       }

    if (m_Type->currentItem() == 1) // Antialias
       {
       *proc << "-antialias";
       }

    if (m_Type->currentItem() == 2) // Blur
       {
       *proc << "-blur";
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_blurRadius ) + "x";
       Temp2.append(Temp.setNum( m_blurDeviation ));
       *proc << Temp2;
       }

    if (m_Type->currentItem() == 3) // Despeckle
       {
       *proc << "-despeckle";
       }

    if (m_Type->currentItem() == 4) // Enhance
       {
       *proc << "-enhance";
       }

    if (m_Type->currentItem() == 5) // Median
       {
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_medianRadius );
       *proc << "-median" << Temp2;
       }

    if (m_Type->currentItem() == 6) // Noise reduction
       {
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_noiseRadius );
       *proc << "-noise" << Temp2;
       }

    if (m_Type->currentItem() == 7) // Sharpen
       {
       *proc << "-sharpen";
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_sharpenRadius ) + "x";
       Temp2.append(Temp.setNum( m_sharpenDeviation ));
       *proc << Temp2;
       }

    if (m_Type->currentItem() == 8) // Unsharp
       {
       *proc << "-unsharp";
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_unsharpenRadius ) + "x";
       Temp2.append(Temp.setNum( m_unsharpenDeviation ));
       Temp2.append( "+" + Temp.setNum( m_unsharpenPercent ));
       Temp2.append( "+" + Temp.setNum( m_unsharpenThreshold ));
       *proc << Temp2;
       }

    *proc << "-verbose";

    *proc << item->pathSrc();

    if ( !previewMode )   // No preview mode !
       {
       *proc << albumDest + "/" + item->nameDest();
       }

    return(extractArguments(proc));
}

}  // NameSpace KIPIBatchProcessImagesPlugin
