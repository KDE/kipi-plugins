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
#include <qimage.h>

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>
#include <kcolorbutton.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Local includes

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "resizeoptionsdialog.h"
#include "outputdialog.h"
#include "resizeimagesdialog.h"
#include "resizeimagesdialog.moc"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

ResizeImagesDialog::ResizeImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
                 : BatchProcessImagesDialog( urlList, interface, i18n("Batch Resize Images"), parent )
{
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Batch resize images"),
                                           NULL,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to batch-resize images\n"
                                                     "This plugin uses the \"convert\" program from \"ImageMagick\" package."),
                                           "(c) 2003-2004, Gilles Caulier");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at gmail dot com");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Batch Resize Images Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();

    //---------------------------------------------

    groupBox1->setTitle( i18n("Image Resizing Options") );

    m_labelType->setText( i18n("Type:") );

    m_Type->insertItem(i18n("Proportional (1 dim.)"));  // 0
    m_Type->insertItem(i18n("Proportional (2 dim.)"));  // 1
    m_Type->insertItem(i18n("Non-Proportional"));       // 2
    m_Type->insertItem(i18n("Prepare to Print"));       // 3
    m_Type->setCurrentText(i18n("Proportional (1 dim.)"));
    whatsThis = i18n("<p>Select here the image-resize type.");
    whatsThis = whatsThis + i18n("<p><b>Proportional (1 dim.)</b>: standard auto-resizing using one dimension. "
                                 "The width or the height of the images will be automatically "
                                 "selected, depending on the images' orientations. "
                                 "The images' aspect ratios are preserved.");
    whatsThis = whatsThis + i18n("<p><b>Proportional (2 dim.)</b>: auto-resizing using two dimensions. "
                                 "The images' aspect ratio are preserved. You can use this, for example, "
                                 "to adapt your images' sizes to your screen size.");
    whatsThis = whatsThis + i18n("<p><b>Non proportional</b>: non-proportional resizing using two dimensions. "
                                 "The images' aspect ratios are not preserved.");
    whatsThis = whatsThis + i18n("<p><b>Prepare to print</b>: prepare the image for photographic printing. "
                                 "The user can set the print resolution and the photographic paper size. "
                                 "The target images will be adapted to the specified dimensions "
                                 "(included the background size, margin size, and background color).");

    QWhatsThis::add( m_Type, whatsThis );

    m_previewButton->hide();
    m_smallPreview->hide();

    //---------------------------------------------

    readSettings();
    listImageFiles();
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

ResizeImagesDialog::~ResizeImagesDialog()
{
    delete m_about;
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void ResizeImagesDialog::slotHelp( void )
{
    KApplication::kApplication()->invokeHelp("resizeimages",
                                             "kipi-plugins");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ResizeImagesDialog::slotOptionsClicked(void)
{
    int Type = m_Type->currentItem();
    ResizeOptionsDialog *optionsDialog = new ResizeOptionsDialog(this, Type);

    if (Type == 0) // Proportional (1 dim.)
       {
       optionsDialog->m_quality->setValue(m_quality);
       optionsDialog->m_size->setValue(m_size);
       optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
       }
    if (Type == 1) // Proportional (2 dim.)
       {
       optionsDialog->m_quality->setValue(m_quality);
       optionsDialog->m_Width->setValue(m_Width);
       optionsDialog->m_Height->setValue(m_Height);
       optionsDialog->m_button_bgColor->setColor(m_bgColor);
       optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
       optionsDialog->m_Border->setValue(m_Border);
       }
    if (Type == 2) // Non-proportional
       {
       optionsDialog->m_quality->setValue(m_quality);
       optionsDialog->m_fixedWidth->setValue(m_fixedWidth);
       optionsDialog->m_fixedHeight->setValue(m_fixedHeight);
       optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
       }
    if (Type == 3) // Prepare to print
       {
       optionsDialog->m_quality->setValue(m_quality);
       optionsDialog->m_paperSize->setCurrentText(m_paperSize);
       optionsDialog->m_printDpi->setCurrentText(m_printDpi);
       optionsDialog->m_customXSize->setValue(m_customXSize);
       optionsDialog->m_customYSize->setValue(m_customYSize);
       optionsDialog->m_customDpi->setValue(m_customDpi);
       optionsDialog->m_button_backgroundColor->setColor(m_backgroundColor);
       optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
       optionsDialog->m_marging->setValue(m_marging);
       optionsDialog->m_customSettings->setChecked(m_customSettings);
       }

    if ( optionsDialog->exec() == KMessageBox::Ok )
       {
       if (Type == 0) // Proportional (1 dim.)
          {
	  m_quality = optionsDialog->m_quality->value();	  
          m_size = optionsDialog->m_size->value();
          m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
          }
       if (Type == 1) // Proportional (2 dim.)
          {
	  m_quality = optionsDialog->m_quality->value();	  
          m_Width = optionsDialog->m_Width->value();
          m_Height = optionsDialog->m_Height->value();
          m_bgColor = optionsDialog->m_button_bgColor->color();
          m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
          m_Border = optionsDialog->m_Border->value();
          }
       if (Type == 2) // Non-proportional
          {
	  m_quality = optionsDialog->m_quality->value();	  
          m_fixedWidth = optionsDialog->m_fixedWidth->value();
          m_fixedHeight = optionsDialog->m_fixedHeight->value();
          m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
          }
       if (Type == 3) // Prepare to print
          {
	  m_quality = optionsDialog->m_quality->value();	  
          m_paperSize = optionsDialog->m_paperSize->currentText();
          m_printDpi = optionsDialog->m_printDpi->currentText();
          m_customXSize = optionsDialog->m_customXSize->value();
          m_customYSize = optionsDialog->m_customYSize->value();
          m_customDpi = optionsDialog->m_customDpi->value();
          m_backgroundColor = optionsDialog->m_button_backgroundColor->color();
          m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
          m_marging = optionsDialog->m_marging->value();
          m_customSettings = optionsDialog->m_customSettings->isChecked();
          }
       }

    delete optionsDialog;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ResizeImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    QColor *ColorWhite = new QColor( 255, 255, 255 );
    QColor *ColorBlack = new QColor( 0, 0, 0 );
    m_config = new KConfig("kipirc");
    m_config->setGroup("ResizeImages Settings");

    m_Type->setCurrentItem(m_config->readNumEntry("ResiseType", 3)); // Prepare to print per default.
    m_size = m_config->readNumEntry("Size", 640);
    m_resizeFilter = m_config->readEntry("ResizeFilter", "Lanczos");

    m_paperSize = m_config->readEntry("PaperSize", "10x15");
    m_printDpi = m_config->readEntry("PrintDpi", "300");
    m_customXSize = m_config->readNumEntry("CustomXSize", 10);
    m_customYSize = m_config->readNumEntry("CustomYSize", 15);
    m_customDpi = m_config->readNumEntry("CustomDpi", 300);
    m_backgroundColor = m_config->readColorEntry("BackgroundColor", ColorWhite);
    m_marging = m_config->readNumEntry("MargingSize", 10);


    m_quality = m_config->readNumEntry("Quality", 75);
    m_Width = m_config->readNumEntry("Width", 1024);
    m_Height = m_config->readNumEntry("Height", 768);
    m_Border = m_config->readNumEntry("Border", 100);
    m_bgColor = m_config->readColorEntry("BgColor", ColorBlack);

    m_fixedWidth = m_config->readNumEntry("FixedWidth", 640);
    m_fixedHeight = m_config->readNumEntry("FixedHeight", 480);

    if ( m_config->readEntry("CustomSettings", "false") == "true")
       m_customSettings = true;
    else
       m_customSettings = false;

    m_overWriteMode->setCurrentItem(m_config->readNumEntry("OverWriteMode", 2));  // 'Rename' per default...

    if (m_config->readEntry("RemoveOriginal", "false") == "true")
        m_removeOriginal->setChecked( true );
    else
        m_removeOriginal->setChecked( false );

    delete ColorWhite;
    delete ColorBlack;
    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ResizeImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("ResizeImages Settings");
    m_config->writeEntry("ResiseType", m_Type->currentItem());
    m_config->writeEntry("Size", m_size);
    m_config->writeEntry("ResizeFilter", m_resizeFilter);

    m_config->writeEntry("PaperSize", m_paperSize);
    m_config->writeEntry("PrintDpi", m_printDpi);
    m_config->writeEntry("CustomXSize", m_customXSize);
    m_config->writeEntry("CustomYSize", m_customYSize);
    m_config->writeEntry("CustomDpi", m_customDpi);
    m_config->writeEntry("BackgroundColor", m_backgroundColor);
    m_config->writeEntry("MargingSize", m_marging);
    m_config->writeEntry("CustomSettings", m_customSettings);

    m_config->writeEntry("Quality", m_quality);
    m_config->writeEntry("Width", m_Width);
    m_config->writeEntry("Height", m_Height);
    m_config->writeEntry("Border", m_Border);
    m_config->writeEntry("BgColor", m_bgColor);

    m_config->writeEntry("FixedWidth", m_fixedWidth);
    m_config->writeEntry("FixedHeight", m_fixedHeight);

    m_config->writeEntry("OverWriteMode", m_overWriteMode->currentItem());
    m_config->writeEntry("RemoveOriginal", m_removeOriginal->isChecked());

    m_config->sync();

    delete m_config;
}


////////////////////////////////////////////// FONCTIONS ////////////////////////////////////////////

QString ResizeImagesDialog::makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                                        const QString& albumDest, bool )
{
    QImage img;

    img.load( item->pathSrc() );

    // Get image information.

    int w = img.width();
    int h = img.height();

    int Type = m_Type->currentItem();
    bool IncDec;
    int MargingSize;

    if (Type == 0) // Proportional (1 dim.)
          {
          *proc << "convert";
          IncDec = ResizeImage( w, h, m_size);

          *proc << "-resize";
          QString Temp, Temp2;
          Temp2 = Temp.setNum( w ) + "x";
          Temp2.append(Temp.setNum( h ));
          *proc << Temp2;

          *proc << "-quality";
          QString Temp3;
          Temp3.setNum(m_quality);
          *proc << Temp3;

          if ( IncDec == true )   // If the image is increased, enabled the filter.
             {
             *proc << "-filter" << m_resizeFilter;
             }

          *proc << "-verbose";
          *proc << item->pathSrc() + "[0]";
          *proc << albumDest + "/" + item->nameDest();
          }

    if (Type == 1) // Proportional (2 dim.)
          {
          QString targetBackgroundSize;
          int ResizeCoeff;
          *proc << "composite";

          // Get the target image resizing dimensions with using the target size.

          if ( m_Width < m_Height )  // Vertically resizing
             {
             if ( w < h )                // Original size vertically oriented.
                ResizeCoeff = m_Height;
             else                        // Original size horizontally oriented.
                ResizeCoeff = m_Width;
             }
          else                       // Horizontally resizing
             {
             if ( w < h )                // Original size vertically oriented.
                ResizeCoeff = m_Height;
             else                        // Original size horizontally oriented.
                ResizeCoeff = m_Width;
             }

          IncDec = ResizeImage( w, h, ResizeCoeff - m_Border);
          targetBackgroundSize = QString::number(m_Width) + "x" + QString::number(m_Height);

          *proc << "-verbose" << "-gravity" << "Center";

          *proc << "-resize";
          QString Temp, Temp2;
          Temp2 = Temp.setNum( w ) + "x";
          Temp2.append(Temp.setNum( h ));
          *proc << Temp2;

          *proc << "-quality";
          QString Temp3;
          Temp3.setNum(m_quality);
          *proc << Temp3;

          if ( IncDec == true )   // If the image is increased, enabled the filter.
             {
             *proc << "-filter" << m_resizeFilter;
             }

          *proc << item->pathSrc() + "[0]";

          // ImageMagick composite program do not preserve exif data from original. 
          // Need to use "-profile" option for that.
          
          *proc << "-profile" << item->pathSrc();
          
          Temp2 = "xc:rgb(" + Temp.setNum(m_bgColor.red()) + ",";
          Temp2.append(Temp.setNum(m_bgColor.green()) + ",");
          Temp2.append(Temp.setNum(m_bgColor.blue()) + ")");
          *proc << Temp2;

          *proc << "-resize" << targetBackgroundSize + "!";

          *proc << albumDest + "/" + item->nameDest();
          }

    if (Type == 2) // Non-proportional
          {
          *proc << "convert";

          *proc << "-resize";
          QString Temp, Temp2;
          Temp2 = Temp.setNum( m_fixedWidth ) + "x";
          Temp2.append(Temp.setNum( m_fixedHeight ) + "!");
          *proc << Temp2;

          if ( m_fixedWidth > w || m_fixedHeight > h ) // If the image is increased, enabled the filter.
             {
             *proc << "-filter" << m_resizeFilter;
             }
            
          *proc << "-quality";
          QString Temp3;
          Temp3.setNum(m_quality);
          *proc << Temp3;

          *proc << "-verbose";
          *proc << item->pathSrc() + "[0]";
          *proc << albumDest + "/" + item->nameDest();
          }

    if (Type == 3) // Prepare to print
          {
          if ( m_customSettings == true )
             {
             MargingSize = (int)((float)(m_marging * m_customDpi) / (float)(25.4));

             if (w < h)   // (w < h) because all paper dimensions are vertically gived !
                {
                m_xPixels = (int)( (float)(m_customXSize * m_customDpi) / (float)(2.54) );
                m_yPixels = (int)( (float)(m_customYSize * m_customDpi) / (float)(2.54) );
                }
             else
                {
                m_yPixels = (int)( (float)(m_customXSize * m_customDpi) / (float)(2.54) );
                m_xPixels = (int)( (float)(m_customYSize * m_customDpi) / (float)(2.54) );
                }
             }
          else
             {
             QString Temp = m_printDpi;
             int Dpi = Temp.toInt();
             MargingSize = (int)((float)(m_marging * Dpi) / (float)(25.4));

             if (w < h)   // (w < h) because all paper dimensions are vertically given !
                {
                Temp = m_paperSize.left(m_paperSize.find('x'));
                m_xPixels = (int)( (float)(Temp.toInt() * Dpi) / (float)(2.54) );
                Temp = m_paperSize.right(m_paperSize.find('x'));
                m_yPixels = (int)( (float)(Temp.toInt() * Dpi) / (float)(2.54) );
                }
             else
                {
                Temp = m_paperSize.left(m_paperSize.find('x'));
                m_yPixels = (int)( (float)(Temp.toInt() * Dpi) / (float)(2.54) );
                Temp = m_paperSize.right(m_paperSize.find('x'));
                m_xPixels = (int)( (float)(Temp.toInt() * Dpi) / (float)(2.54) );
                }
             }

          QString targetBackgroundSize;
          int ResizeCoeff;
          float RFactor;
          *proc << "composite";

          // Get the target image resizing dimensions with using the target paper size.

          if (m_xPixels < m_yPixels)
             {
             RFactor = (float)m_xPixels / (float)w;
             if (RFactor > 1.0) RFactor = (float)m_yPixels / (float)h;
             ResizeCoeff = (int)((float)h * RFactor);
             }
          else
             {
             RFactor = (float)m_yPixels / (float)h;
             if (RFactor > 1.0) RFactor = (float)m_xPixels / (float)w;
             ResizeCoeff = (int)((float)w * RFactor);
             }

          IncDec = ResizeImage( w, h, ResizeCoeff - MargingSize);
          targetBackgroundSize = QString::number(m_xPixels) + "x" + QString::number(m_yPixels);

          *proc << "-verbose" << "-gravity" << "Center";

          *proc << "-resize";
          QString Temp, Temp2;
          Temp2 = Temp.setNum( w ) + "x";
          Temp2.append(Temp.setNum( h ));
          *proc << Temp2;

          *proc << "-quality";
          QString Temp3;
          Temp3.setNum(m_quality);
          *proc << Temp3;

          if ( IncDec == true )   // If the image is increased, enabled the filter.
             {
             *proc << "-filter" << m_resizeFilter;
             }

          *proc << item->pathSrc();

          Temp2 = "xc:rgb(" + Temp.setNum(m_backgroundColor.red()) + ",";
          Temp2.append(Temp.setNum(m_backgroundColor.green()) + ",");
          Temp2.append(Temp.setNum(m_backgroundColor.blue()) + ")");
          *proc << Temp2;

          // ImageMagick composite program do not preserve exif data from original. 
          // Need to use "-profile" option for that.
          
          *proc << "-profile" << item->pathSrc();

          *proc << "-resize" << targetBackgroundSize + "!";

          *proc << "-quality";
          QString Temp4;
          Temp4.setNum(m_quality);
          *proc << Temp4;

          *proc << albumDest + "/" + item->nameDest();
          }

    return(extractArguments(proc));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool ResizeImagesDialog::prepareStartProcess(BatchProcessImagesItem *item,
                                             const QString& /*albumDest*/)
{
    QImage img;

    if ( img.load( item->pathSrc() ) == false )
       {
       item->changeResult(i18n("Skipped."));
       item->changeError(i18n("image file format unsupported."));
       return false;
       }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool ResizeImagesDialog::ResizeImage( int &w, int &h, int SizeFactor)
{
    bool valRet;

    if ( w > h )
       {
       h = (int)( (double)( h * SizeFactor ) / w );

       if ( h == 0 ) h = 1;

       if ( w < SizeFactor ) valRet = true;
       else valRet = false;

       w = SizeFactor;
       }
    else
       {
       w = (int)( (double)( w * SizeFactor ) / h );

       if ( w == 0 ) w = 1;

       if ( h < SizeFactor ) valRet = true;
       else valRet = false;

       h = SizeFactor;
       }

    return (valRet);  // Return true is image increased, else true.
}

}  // NameSpace KIPIBatchProcessImagesPlugin
