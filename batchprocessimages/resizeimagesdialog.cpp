//////////////////////////////////////////////////////////////////////////////
//
//    RESIZEIMAGESDIALOG.CPP
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
#include <qimage.h>

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>
#include <kcolorbutton.h>

// Local includes

#include "resizeoptionsdialog.h"
#include "outputdialog.h"
#include "resizeimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

ResizeImagesDialog::ResizeImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
                 : BatchProcessImagesDialog( urlList, interface, parent )
{
    m_nbItem = m_selectedImageFiles.count();

    setCaption(i18n("Batch Resizing Images options"));
    setHelp("resizeimages", "kipi-plugins");

    //---------------------------------------------

    groupBox1->setTitle( i18n("Resize images options") );

    m_labelType->setText( i18n("Resizing Image type:") );

    m_Type->insertItem(i18n("Proportional (1 dim.)"));
    m_Type->insertItem(i18n("Proportional (2 dim.)"));
    m_Type->insertItem(i18n("Non proportional"));
    m_Type->insertItem(i18n("Prepare to print"));
    m_Type->setCurrentText(i18n("Proportional (1 dim.)"));
    whatsThis = i18n("<p>Select here the resizing image type.");
    whatsThis = whatsThis + i18n("<p><b>Proportional (1 dim.)</b>: standard auto-resizing using one dimension. "
                                 "The width or the height of the images will be automatically "
                                 "selected in depending of the images orientation. "
                                 "The images aspect ratio are preserved.");
    whatsThis = whatsThis + i18n("<p><b>Proportional (2 dim.)</b>: auto-resizing using two dimensions. "
                                 "The images aspect ratio are preserved. For example, you can use that for "
                                 "to adapt your images size to your screen size.");
    whatsThis = whatsThis + i18n("<p><b>Non proportional</b>: non proportional resizing using two dimensions. "
                                 "The images aspect ratio aren't preserved.");
    whatsThis = whatsThis + i18n("<p><b>Prepare to print</b>: prepare the image for a photographic printing. "
                                 "The user can set the print resolution and the photographic paper size. "
                                 "The target images will be adapted to the good dimensions "
                                 "(included the background size, marging size, and background color).");

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
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void ResizeImagesDialog::slotAbout( void )
{
    KMessageBox::about(this, i18n("A KIPI plugin for batch resize images\n\n"
                                  "Author: Gilles Caulier\n\n"
                                  "Email: caulier dot gilles at free.fr\n\n"
                                  "This plugin use the \"convert\" and \"composite\" programs "
                                  "from \"ImageMagick\" package.\n"),
                                  i18n("About KIPI batch resize images"));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ResizeImagesDialog::slotOptionsClicked(void)
{
    QString Type = m_Type->currentText();
    ResizeOptionsDialog *optionsDialog = new ResizeOptionsDialog(this, Type);

    if (Type == i18n("Proportional (1 dim.)"))
       {
       optionsDialog->m_size->setValue(m_size);
       optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
       }
    if (Type == i18n("Proportional (2 dim.)"))
       {
       optionsDialog->m_Width->setValue(m_Width);
       optionsDialog->m_Height->setValue(m_Height);
       optionsDialog->m_button_bgColor->setColor(m_bgColor);
       optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
       optionsDialog->m_Border->setValue(m_Border);
       }
    if (Type == i18n("Non proportional"))
       {
       optionsDialog->m_fixedWidth->setValue(m_fixedWidth);
       optionsDialog->m_fixedHeight->setValue(m_fixedHeight);
       optionsDialog->m_resizeFilter->setCurrentText(m_resizeFilter);
       }
    if (Type == i18n("Prepare to print"))
       {
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
       if (Type == i18n("Proportional (1 dim.)"))
          {
          m_size = optionsDialog->m_size->value();
          m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
          }
       if (Type == i18n("Proportional (2 dim.)"))
          {
          m_Width = optionsDialog->m_Width->value();
          m_Height = optionsDialog->m_Height->value();
          m_bgColor = optionsDialog->m_button_bgColor->color();
          m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
          m_Border = optionsDialog->m_Border->value();
          }
       if (Type == i18n("Non proportional"))
          {
          m_fixedWidth = optionsDialog->m_fixedWidth->value();
          m_fixedHeight = optionsDialog->m_fixedHeight->value();
          m_resizeFilter = optionsDialog->m_resizeFilter->currentText();
          }
       if (Type == i18n("Prepare to print"))
          {
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

    m_Type->setCurrentText(m_config->readEntry("ResiseType", i18n("Proportional (1 dim.)")));
    m_size = m_config->readNumEntry("Size", 640);
    m_resizeFilter = m_config->readEntry("ResizeFilter", "Lanczos");

    m_paperSize = m_config->readEntry("PaperSize", "10x15");
    m_printDpi = m_config->readEntry("PrintDpi", "300");
    m_customXSize = m_config->readNumEntry("CustomXSize", 10);
    m_customYSize = m_config->readNumEntry("CustomYSize", 15);
    m_customDpi = m_config->readNumEntry("CustomDpi", 300);
    m_backgroundColor = m_config->readColorEntry("BackgroundColor", ColorWhite);
    m_marging = m_config->readNumEntry("MargingSize", 10);

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
    m_config->writeEntry("ResiseType", m_Type->currentText());
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
                                        const QString& albumDest)
{
    QImage img;

    img.load( item->pathSrc() );

    // Get image informations.

    int w = img.width();
    int h = img.height();

    QString Type = m_Type->currentText();
    bool IncDec;
    int MargingSize;

    if (Type == i18n("Proportional (1 dim.)"))
          {
          *proc << "convert";
          IncDec = ResizeImage( w, h, m_size);

          *proc << "-geometry";
          QString Temp, Temp2;
          Temp2 = Temp.setNum( w ) + "x";
          Temp2.append(Temp.setNum( h ));
          *proc << Temp2;

          if ( IncDec == true )   // If the image is increased, enabled the filter.
             {
             *proc << "-filter" << m_resizeFilter;
             }

          *proc << "-verbose";
          *proc << item->pathSrc();
          *proc << albumDest + "/" + item->nameDest();
          }

    if (Type == i18n("Proportional (2 dim.)"))
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

          *proc << "-geometry";
          QString Temp, Temp2;
          Temp2 = Temp.setNum( w ) + "x";
          Temp2.append(Temp.setNum( h ));
          *proc << Temp2;

          if ( IncDec == true )   // If the image is increased, enabled the filter.
             {
             *proc << "-filter" << m_resizeFilter;
             }

          *proc << item->pathSrc();

          Temp2 = "xc:rgb(" + Temp.setNum(m_bgColor.red()) + ",";
          Temp2.append(Temp.setNum(m_bgColor.green()) + ",");
          Temp2.append(Temp.setNum(m_bgColor.blue()) + ")");
          *proc << Temp2;

          *proc << "-resize" << targetBackgroundSize + "!";

          *proc << albumDest + "/" + item->nameDest();
          }

    if (Type == i18n("Non proportional"))
          {
          *proc << "convert";

          *proc << "-geometry";
          QString Temp, Temp2;
          Temp2 = Temp.setNum( m_fixedWidth ) + "x";
          Temp2.append(Temp.setNum( m_fixedHeight ) + "!");
          *proc << Temp2;

          if ( m_fixedWidth > w || m_fixedHeight > h ) // If the image is increased, enabled the filter.
             {
             *proc << "-filter" << m_resizeFilter;
             }

          *proc << "-verbose";
          *proc << item->pathSrc();
          *proc << albumDest + "/" + item->nameDest();
          }

    if (Type == i18n("Prepare to print"))
          {
          if ( m_customSettings == true )
             {
             MargingSize = (int)((float)(m_marging * m_customDpi) / (float)(25.4));

             if (w < h)   // (w < h) because all paper dimensions are verticaly gived !
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

             if (w < h)   // (w < h) because all paper dimensions are verticaly given !
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

          *proc << "-geometry";
          QString Temp, Temp2;
          Temp2 = Temp.setNum( w ) + "x";
          Temp2.append(Temp.setNum( h ));
          *proc << Temp2;

          if ( IncDec == true )   // If the image is increased, enabled the filter.
             {
             *proc << "-filter" << m_resizeFilter;
             }

          *proc << item->pathSrc();

          Temp2 = "xc:rgb(" + Temp.setNum(m_backgroundColor.red()) + ",";
          Temp2.append(Temp.setNum(m_backgroundColor.green()) + ",");
          Temp2.append(Temp.setNum(m_backgroundColor.blue()) + ")");
          *proc << Temp2;

          *proc << "-resize" << targetBackgroundSize + "!";

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

#include "resizeimagesdialog.moc"
