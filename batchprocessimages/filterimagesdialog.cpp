//////////////////////////////////////////////////////////////////////////////
//
//    FILTERIMAGESDIALOG.CPP
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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////


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
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Include files for libKipi.

#include <libkipi/version.h>

// Local includes

#include "filteroptionsdialog.h"
#include "outputdialog.h"
#include "imagepreview.h"
#include "filterimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

FilterImagesDialog::FilterImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
                  : BatchProcessImagesDialog( urlList, interface, i18n("Batch Image Filtering"), parent )
{
    // About data and help button.

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Batch image filtering"),
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin to batch filter images\n"
                                                 "This plugin uses the \"convert\" program from \"ImageMagick\" package."),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Gilles Caulier",
                                       0,
                                       "http://extragear.kde.org/apps/kipi");

    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at free.fr");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Batch Image Filtering Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

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

    QWhatsThis::add( m_Type, whatsThis );

    //---------------------------------------------

    readSettings();
    listImageFiles();
    slotTypeChanged(m_Type->currentItem());
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

FilterImagesDialog::~FilterImagesDialog()
{
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void FilterImagesDialog::slotHelp( void )
{
    KApplication::kApplication()->invokeHelp("filterimages",
                                             "kipi-plugins");
}


/////////////////////////////////////////////////////////////////////////////////////////////

void FilterImagesDialog::slotTypeChanged(int type)
{
    if ( type == 1 ||      // Antialias
         type == 3 ||      // Despeckle
         type == 4 )       // Enhance
       m_optionsButton->setEnabled(false);
    else
       m_optionsButton->setEnabled(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

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


/////////////////////////////////////////////////////////////////////////////////////////////////////

void FilterImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    KConfig* config = kapp->config();
    config->setGroup("FilterImages Settings");

    m_Type->setCurrentItem(config->readNumEntry("FilterType", 7));      // Sharpen per default
    m_noiseType = config->readEntry("NoiseType", i18n("Gaussian"));
    m_blurRadius = config->readNumEntry("BlurRadius", 3);
    m_blurDeviation = config->readNumEntry("BlurDeviation", 1);
    m_medianRadius = config->readNumEntry("MedianRadius", 3);
    m_noiseRadius = config->readNumEntry("NoiseRadius", 3);
    m_sharpenRadius = config->readNumEntry("SharpenRadius", 3);
    m_sharpenDeviation = config->readNumEntry("SharpenDeviation", 1);
    m_unsharpenRadius = config->readNumEntry("UnsharpenRadius", 3);
    m_unsharpenDeviation = config->readNumEntry("UnsharpenDeviation", 1);
    m_unsharpenPercent = config->readNumEntry("UnsharpenPercent", 3);
    m_unsharpenThreshold = config->readNumEntry("UnsharpenThreshold", 1);

    if (config->readEntry("SmallPreview", "true") == "true")
        m_smallPreview->setChecked( true );
    else
        m_smallPreview->setChecked( false );

    m_overWriteMode->setCurrentItem(config->readNumEntry("OverWriteMode", 2));  // 'Rename' per default...

    if (config->readEntry("RemoveOriginal", "false") == "true")
        m_removeOriginal->setChecked( true );
    else
        m_removeOriginal->setChecked( false );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void FilterImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    KConfig* config = kapp->config();
    config->setGroup("FilterImages Settings");

    config->writeEntry("FilterType", m_Type->currentItem());
    config->writeEntry("NoiseType", m_noiseType);
    config->writeEntry("BlurRadius", m_blurRadius);
    config->writeEntry("BlurDeviation", m_blurDeviation);
    config->writeEntry("MedianRadius", m_medianRadius);
    config->writeEntry("NoiseRadius", m_noiseRadius);
    config->writeEntry("SharpenRadius", m_sharpenRadius);
    config->writeEntry("SharpenDeviation", m_sharpenDeviation);
    config->writeEntry("UnsharpenRadius", m_unsharpenRadius);
    config->writeEntry("UnsharpenDeviation", m_unsharpenDeviation);
    config->writeEntry("UnsharpenPercent", m_unsharpenPercent);
    config->writeEntry("UnsharpenThreshold", m_unsharpenThreshold);

    config->writeEntry("SmallPreview", m_smallPreview->isChecked());
    config->writeEntry("OverWriteMode", m_overWriteMode->currentItem());
    config->writeEntry("RemoveOriginal", m_removeOriginal->isChecked());

    config->sync();
}


////////////////////////////////////////////// FONCTIONS ////////////////////////////////////////////

QString FilterImagesDialog::makeProcess(KProcess* proc, BatchProcessImagesItem *item,
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

#include "filterimagesdialog.moc"
