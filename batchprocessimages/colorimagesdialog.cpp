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

#include "pluginsversion.h"
#include "kpaboutdata.h"
#include "coloroptionsdialog.h"
#include "outputdialog.h"
#include "imagepreview.h"
#include "colorimagesdialog.h"
#include "colorimagesdialog.moc"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

ColorImagesDialog::ColorImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
                 : BatchProcessImagesDialog( urlList, interface, i18n("Batch Image-Color Processing"), parent )
{
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Batch image-color enhancement"),
                                           NULL,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin for batch image-color enhancement\n"
                                                     "This plugin use the \"convert\" program from \"ImageMagick\" package."),
                                           "(c) 2003-2004, Gilles Caulier");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at gmail dot com");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Batch Image-Color Enhancement Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );
    
    //---------------------------------------------
    
    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    groupBox1->setTitle( i18n("Image Coloring Options") );

    m_labelType->setText( i18n("Filter:") );

    m_Type->insertItem(i18n("Decrease Contrast"));
    m_Type->insertItem(i18n("Depth"));              // 1
    m_Type->insertItem(i18n("Equalize"));
    m_Type->insertItem(i18n("Fuzz"));               // 3
    m_Type->insertItem(i18n("Gray Scales"));
    m_Type->insertItem(i18n("Increase Contrast"));
    m_Type->insertItem(i18n("Monochrome"));
    m_Type->insertItem(i18n("Negate"));
    m_Type->insertItem(i18n("Normalize"));
    m_Type->insertItem(i18n("Segment"));            // 9
    m_Type->insertItem(i18n("Trim"));
    m_Type->setCurrentText(i18n("Normalize"));
    whatsThis = i18n("<p>Select here the color enhancement type for your images:<p>"
                     "<b>Decrease contrast</b>: reduce the image contrast. The algorithm "
                     "reduces the intensity difference between the lighter and darker elements "
                     "of the image.<p>"
                     "<b>Depth</b>: change the color depth of the image.<p>"
                     "<b>Equalize</b>: perform histogram equalization to the image.<p>"
                     "<b>Fuzz</b>: merging colors within a distance, i.e. consider them to be equal.<p>"
                     "<b>Gray scales</b>: convert color images to grayscale images.<p>"
                     "<b>Increase contrast</b>: enhance the image contrast. The algorithm enhances "
                     "the intensity differences between the lighter and darker elements of the image.<p>"
                     "<b>Monochrome</b>: transform the image to black and white.<p>"
                     "<b>Negate</b>: replace every pixel with its complementary color. The red, green, "
                     "and blue intensities of an image are negated. White becomes black, yellow becomes "
                     "blue, etc.<p>"
                     "<b>Normalize</b>: transform image to span the full range of color values. "
                     "This is a contrast enhancement technique. The algorithm enhances the contrast "
                     "of a colored image by adjusting the pixels color to span the entire range of "
                     "colors available.<p>"
                     "<b>Segment</b>: segment an image by analyzing the histograms of the color "
                     "components and identifying units that are homogeneous with the fuzzy c-means "
                     "technique.<p>"
                     "<b>Trim</b>: trim an image (fuzz reverse technic). The algorithm remove edges "
                     "that are the background color from the image.<p>");

    QWhatsThis::add( m_Type, whatsThis );

    //---------------------------------------------

    readSettings();
    listImageFiles();
    slotTypeChanged(m_Type->currentItem());
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

ColorImagesDialog::~ColorImagesDialog()
{
    delete m_about;
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void ColorImagesDialog::slotHelp( void )
{
    KApplication::kApplication()->invokeHelp("colorimages",
                                             "kipi-plugins");
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ColorImagesDialog::slotTypeChanged(int type)
{
    if ( type == 1 || // Depth
         type == 3 || // Fuzz
         type == 9    // Segment
       )
       m_optionsButton->setEnabled(true);
    else
       m_optionsButton->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ColorImagesDialog::slotOptionsClicked(void)
{
    int Type = m_Type->currentItem();
    ColorOptionsDialog *optionsDialog = new ColorOptionsDialog(this, Type);

    if ( Type == 1) // Depth
       optionsDialog->m_depthValue->setCurrentText(m_depthValue);

    if ( Type == 3) // Fuzz
       optionsDialog->m_fuzzDistance->setValue(m_fuzzDistance);

    if ( Type == 9) // Segment
       {
       optionsDialog->m_segmentCluster->setValue(m_segmentCluster);
       optionsDialog->m_segmentSmooth->setValue(m_segmentSmooth);
       }

    if ( optionsDialog->exec() == KMessageBox::Ok )
       {
       if ( Type == 1) // Depth
          m_depthValue = optionsDialog->m_depthValue->currentText();

       if ( Type == 3) // Fuzz
          m_fuzzDistance = optionsDialog->m_fuzzDistance->value();

       if ( Type == 9) // Segment
          {
          m_segmentCluster = optionsDialog->m_segmentCluster->value();
          m_segmentSmooth = optionsDialog->m_segmentSmooth->value();
          }
       }

    delete optionsDialog;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ColorImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("ColorImages Settings");

    m_Type->setCurrentItem(m_config->readNumEntry("ColorType", 8)); // Normalize per default.
    m_depthValue = m_config->readEntry("DepthValue", "32");
    m_fuzzDistance = m_config->readNumEntry("FuzzDistance", 3);
    m_segmentCluster = m_config->readNumEntry("SegmentCluster", 3);
    m_segmentSmooth = m_config->readNumEntry("SegmentSmooth", 3);

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


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ColorImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("ColorImages Settings");

    m_config->writeEntry("ColorType", m_Type->currentItem());
    m_config->writeEntry("DepthValue", m_depthValue);
    m_config->writeEntry("FuzzDistance", m_fuzzDistance);
    m_config->writeEntry("SegmentCluster", m_segmentCluster);
    m_config->writeEntry("SegmentSmooth", m_segmentSmooth);

    m_config->writeEntry("SmallPreview", m_smallPreview->isChecked());
    m_config->writeEntry("OverWriteMode", m_overWriteMode->currentItem());
    m_config->writeEntry("RemoveOriginal", m_removeOriginal->isChecked());

    m_config->sync();

    delete m_config;
}


////////////////////////////////////////////// FONCTIONS ////////////////////////////////////////////

QString ColorImagesDialog::makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                                       const QString& albumDest, bool previewMode)
{
    *proc << "convert";

    if ( previewMode && m_smallPreview->isChecked() )    // Preview mode and small preview enabled !
       {
       *m_PreviewProc << "-crop" << "300x300+0+0";
       m_previewOutput.append( " -crop 300x300+0+0 ");
       }

    if (m_Type->currentItem() == 0) // Decrease contrast"
       {
       *proc << "-contrast";
       }

    if (m_Type->currentItem() == 1) // Depth
       {
       *proc << "-depth" << m_depthValue;
       }

    if (m_Type->currentItem() == 2) // Equalize
       {
       *proc << "-equalize";
       }

    if (m_Type->currentItem() == 3) // Fuzz
       {
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_fuzzDistance );
       *proc << "-fuzz" << Temp2;
       }

    if (m_Type->currentItem() == 4) // Gray scales
       {
       *proc << "-type";
       *proc << "Grayscale";
       }

    if (m_Type->currentItem() == 5) // Increase contrast
       {
       *proc << "+contrast";
       }

    if (m_Type->currentItem() == 6) // Monochrome
       {
       *proc << "-monochrome";
       }

    if (m_Type->currentItem() == 7) // Negate
       {
       *proc << "-negate";
       }

    if (m_Type->currentItem() == 8) // Normalize
       {
       *proc << "-normalize";
       }

    if (m_Type->currentItem() == 9) // Segment
       {
       *proc << "-segment";
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_segmentCluster ) + "x";
       Temp2.append (Temp.setNum( m_segmentSmooth ));
       *proc << Temp2;
       }

    if (m_Type->currentItem() == 10) // Trim
       {
       *proc << "-trim";
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
