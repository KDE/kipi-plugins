//////////////////////////////////////////////////////////////////////////////
//
//    COLORIMAGESDIALOG.CPP
//
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
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

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>

// Local includes

#include "colorimagesdialog.h"
#include "coloroptionsdialog.h"
#include "outputdialog.h"
#include "imagepreview.h"


//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

ColorImagesDialog::ColorImagesDialog(QWidget *parent, QStringList filesList)
                 : BatchProcessImagesDialog( parent )
{
    m_selectedImageFiles = filesList;
    m_nbItem = m_selectedImageFiles.count();

    setCaption(i18n("Batch Coloring Images options"));
    setHelp("plugin-colorimages.anchor", "digikam");

    //---------------------------------------------

    groupBox1->setTitle( i18n("Color images options") );

    m_labelType->setText( i18n("Coloring enhancement type:") );

    m_Type->insertItem(i18n("Decrease contrast"));
    m_Type->insertItem(i18n("Depth"));
    m_Type->insertItem(i18n("Equalize"));
    m_Type->insertItem(i18n("Fuzz"));
    m_Type->insertItem(i18n("Gray scales"));
    m_Type->insertItem(i18n("Increase contrast"));
    m_Type->insertItem(i18n("Monochrome"));
    m_Type->insertItem(i18n("Negate"));
    m_Type->insertItem(i18n("Normalize"));
    m_Type->insertItem(i18n("Segment"));
    m_Type->insertItem(i18n("Trim"));
    m_Type->setCurrentText(i18n("Normalize"));
    whatsThis = i18n("<p>Select here the colors enhancement type for your images:<p>"
                     "<b>Decrease contrast</b>: reduce the image contrast. The algorithm "
                     "reduce the intensity differences between the lighter and darker elements "
                     "of the image.<p>"
                     "<b>Depth</b>: change the color depth of the image.<p>"
                     "<b>Equalize</b>: perform histogram equalization to the image.<p>"
                     "<b>Fuzz</b>: merging colors within a distance are considered equal.<p>"
                     "<b>Gray scales</b>: convert colors image to gray scales image.<p>"
                     "<b>Increase contrast</b>: enhance the image contrast. The algorithm enhance "
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
    slotTypeChanged(m_Type->currentText());
    slotOverWriteModeActived(m_overWriteMode->currentText());
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

ColorImagesDialog::~ColorImagesDialog()
{
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void ColorImagesDialog::slotAbout( void )
{
    KMessageBox::about(this, i18n("A Digikam plugin for batch colors images enhancement\n\n"
                                  "Author: Gilles Caulier\n\n"
                                  "Email: caulier dot gilles at free.fr\n\n"
                                  "This plugin use the \"convert\" program from \"ImageMagick\" package.\n"),
                                  i18n("About Digikam batch colors images enhancement"));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ColorImagesDialog::slotTypeChanged(const QString &string)
{
    if ( string == i18n("Depth") || string == i18n("Fuzz") || string == i18n("Segment") )
       m_optionsButton->setEnabled(true);
    else
       m_optionsButton->setEnabled(false);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ColorImagesDialog::slotOptionsClicked(void)
{
    QString Type = m_Type->currentText();
    ColorOptionsDialog *optionsDialog = new ColorOptionsDialog(this, Type);

    if ( Type == i18n("Depth") )
       optionsDialog->m_depthValue->setCurrentText(m_depthValue);

    if ( Type == i18n("Fuzz") )
       optionsDialog->m_fuzzDistance->setValue(m_fuzzDistance);

    if ( Type == i18n("Segment") )
       {
       optionsDialog->m_segmentCluster->setValue(m_segmentCluster);
       optionsDialog->m_segmentSmooth->setValue(m_segmentSmooth);
       }

    if ( optionsDialog->exec() == KMessageBox::Ok )
       {
       if ( Type == i18n("Depth") )
          m_depthValue = optionsDialog->m_depthValue->currentText();

       if ( Type == i18n("Fuzz") )
          m_fuzzDistance = optionsDialog->m_fuzzDistance->value();

       if ( Type == i18n("Segment") )
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

    m_config = new KConfig("digikamrc");
    m_config->setGroup("ColorImages Settings");

    m_Type->setCurrentText(m_config->readEntry("ColorType", i18n("Normalize")));
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

    m_config = new KConfig("digikamrc");
    m_config->setGroup("ColorImages Settings");

    m_config->writeEntry("ColorType", m_Type->currentText());
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
                                       Digikam::AlbumInfo *albumDest)
{
    *proc << "convert";

    if ( albumDest == 0L && m_smallPreview->isChecked() )    // Preview mode and small preview enabled !
       {
       *m_PreviewProc << "-crop" << "300x300+0+0";
       m_previewOutput.append( " -crop 300x300+0+0 ");
       }

    if (m_Type->currentText() == i18n("Decrease contrast"))
       {
       *proc << "-contrast";
       }

    if (m_Type->currentText() == i18n("Depth"))
       {
       *proc << "-depth" << m_depthValue;
       }

    if (m_Type->currentText() == i18n("Equalize"))
       {
       *proc << "-equalize";
       }

    if (m_Type->currentText() == i18n("Fuzz"))
       {
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_fuzzDistance );
       *proc << "-fuzz" << Temp2;
       }

    if (m_Type->currentText() == i18n("Gray scales"))
       {
       *proc << "-type";
       *proc << "Grayscale";
       }

    if (m_Type->currentText() == i18n("Increase contrast"))
       {
       *proc << "+contrast";
       }

    if (m_Type->currentText() == i18n("Monochrome"))
       {
       *proc << "-monochrome";
       }

    if (m_Type->currentText() == i18n("Negate"))
       {
       *proc << "-negate";
       }

    if (m_Type->currentText() == i18n("Normalize"))
       {
       *proc << "-normalize";
       }

    if (m_Type->currentText() == i18n("Segment"))
       {
       *proc << "-segment";
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_segmentCluster ) + "x";
       Temp2.append (Temp.setNum( m_segmentSmooth ));
       *proc << Temp2;
       }

    if (m_Type->currentText() == i18n("Trim"))
       {
       *proc << "-trim";
       }

    *proc << "-verbose";

    *proc << item->pathSrc();

    if ( albumDest != 0L )   // No preview mode !
       {
       *proc << albumDest->getPath() + "/" + item->nameDest();
       }

    return(extractArguments(proc));
}
