//////////////////////////////////////////////////////////////////////////////
//
//    BORDERIMAGESDIALOG.CPP
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

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprocess.h>
#include <kcolorbutton.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Include files for libKipi.

#include <libkipi/version.h>

// Local includes

#include "borderoptionsdialog.h"
#include "outputdialog.h"
#include "imagepreview.h"
#include "borderimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

BorderImagesDialog::BorderImagesDialog( KURL::List urlList, KIPI::Interface* interface, QWidget *parent )
                  : BatchProcessImagesDialog( urlList, interface, i18n("Batch-Bordering Images"), parent )
{
    // About data and help button.
    
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Batch Image-bordering"), 
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin for batch bordering images\n"
                                                 "This plugin use the \"convert\" program from \"ImageMagick\" package."),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Gilles Caulier", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi");
    
    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at free.fr");
                        
    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Batch Image-Bordering Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );
    
    //---------------------------------------------

    m_nbItem = m_selectedImageFiles.count();
    
    //---------------------------------------------

    groupBox1->setTitle( i18n("Images Bordering Options") );

    m_labelType->setText( i18n("Border:") );

    m_Type->insertItem(i18n("Solid"));
    m_Type->insertItem(i18n("Niepce"));
    m_Type->insertItem(i18n("Raise"));
    m_Type->insertItem(i18n("Frame"));
    m_Type->setCurrentText(i18n("Niepce"));
    whatsThis = i18n("<p>Select here the border type for your images:<p>"
                     "<b>Solid</b>: just surround the images with a line.<p>"
                     "<b>Niepce</b>: surround the images with a fine line and a large border "
                     "(ideal for black and white pictures).<p>"
                     "<b>Raise</b>: creating a 3D effect around the images.<p>"
                     "<b>Frame</b>: surround the images with an ornamental border.<p>");

    QWhatsThis::add( m_Type, whatsThis );

    //---------------------------------------------

    readSettings();
    listImageFiles();
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

BorderImagesDialog::~BorderImagesDialog()
{
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void BorderImagesDialog::slotHelp( void )
{
    KApplication::kApplication()->invokeHelp("borderimages",
                                             "kipi-plugins");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BorderImagesDialog::slotOptionsClicked(void)
{
    int Type = m_Type->currentItem();
    BorderOptionsDialog *optionsDialog = new BorderOptionsDialog(this, Type);

    if ( Type == 0 )  // Solid
       {
       optionsDialog->m_solidBorderWidth->setValue(m_solidWidth);
       optionsDialog->m_button_solidBorderColor->setColor(m_solidColor);
       }
    if ( Type == 1 ) // Niepce
       {
       optionsDialog->m_lineNiepceBorderWidth->setValue(m_lineNiepceWidth);
       optionsDialog->m_button_lineNiepceBorderColor->setColor(m_lineNiepceColor);
       optionsDialog->m_NiepceBorderWidth->setValue(m_NiepceWidth);
       optionsDialog->m_button_NiepceBorderColor->setColor(m_NiepceColor);
       }
    if ( Type == 2 ) // Raise
       {
       optionsDialog->m_raiseBorderWidth->setValue(m_raiseWidth);
       }
    if ( Type == 3 ) // Frame
       {
       optionsDialog->m_frameBorderWidth->setValue(m_frameWidth);
       optionsDialog->m_frameBevelBorderWidth->setValue(m_bevelWidth);
       optionsDialog->m_button_frameBorderColor->setColor(m_frameColor);
       }

    if ( optionsDialog->exec() == KMessageBox::Ok )
       {
       if ( Type == 0 ) // Solid
          {
          m_solidWidth = optionsDialog->m_solidBorderWidth->value();
          m_solidColor = optionsDialog->m_button_solidBorderColor->color();
          }
       if ( Type == 1 ) // Niepce
          {
          m_lineNiepceWidth = optionsDialog->m_lineNiepceBorderWidth->value();
          m_lineNiepceColor = optionsDialog->m_button_lineNiepceBorderColor->color();
          m_NiepceWidth = optionsDialog->m_NiepceBorderWidth->value();
          m_NiepceColor = optionsDialog->m_button_NiepceBorderColor->color();
          }
       if ( Type == 2 ) // Raise
          {
          m_raiseWidth = optionsDialog->m_raiseBorderWidth->value();
          }
       if ( Type == 3 ) // Frame
          {
          m_frameWidth = optionsDialog->m_frameBorderWidth->value();
          m_bevelWidth = optionsDialog->m_frameBevelBorderWidth->value();
          m_frameColor = optionsDialog->m_button_frameBorderColor->color();
          }
       }

   delete optionsDialog;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void BorderImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("BorderImages Settings");

    m_Type->setCurrentItem(m_config->readNumEntry("BorderType", 1));     // Niepce per default.
    QColor *ColorBlack = new QColor( 0, 0, 0 );
    QColor *ColorWhite = new QColor( 255, 255, 255 );
    m_solidWidth = m_config->readNumEntry("SolidWidth", 25);
    m_solidColor = m_config->readColorEntry("SolidColor", ColorBlack);

    m_lineNiepceWidth = m_config->readNumEntry("LineNiepceWidth", 10);
    m_lineNiepceColor = m_config->readColorEntry("LineNiepceColor", ColorBlack);
    m_NiepceWidth = m_config->readNumEntry("NiepceWidth", 100);
    m_NiepceColor = m_config->readColorEntry("NiepceColor", ColorWhite);

    m_raiseWidth = m_config->readNumEntry("RaiseWidth", 50);

    m_frameWidth = m_config->readNumEntry("FrameWidth", 25);
    m_bevelWidth = m_config->readNumEntry("BevelWidth", 10);
    m_frameColor = m_config->readColorEntry("FrameColor", ColorBlack);

    if (m_config->readEntry("SmallPreview", "true") == "true")
        m_smallPreview->setChecked( true );
    else
        m_smallPreview->setChecked( false );

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

void BorderImagesDialog::saveSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("BorderImages Settings");
    m_config->writeEntry("BorderType", m_Type->currentItem());

    m_config->writeEntry("SolidWidth", m_solidWidth);
    m_config->writeEntry("SolidColor", m_solidColor);

    m_config->writeEntry("LineNiepceWidth", m_lineNiepceWidth);
    m_config->writeEntry("LineNiepceColor", m_lineNiepceColor);
    m_config->writeEntry("NiepceWidth", m_NiepceWidth);
    m_config->writeEntry("NiepceColor", m_NiepceColor);

    m_config->writeEntry("RaiseWidth", m_raiseWidth);

    m_config->writeEntry("FrameWidth", m_frameWidth);
    m_config->writeEntry("BevelWidth", m_bevelWidth);
    m_config->writeEntry("FrameColor", m_frameColor);

    m_config->writeEntry("SmallPreview", m_smallPreview->isChecked());
    m_config->writeEntry("OverWriteMode", m_overWriteMode->currentItem());
    m_config->writeEntry("RemoveOriginal", m_removeOriginal->isChecked());

    m_config->sync();

    delete m_config;
}


////////////////////////////////////////////// FONCTIONS ////////////////////////////////////////////

QString BorderImagesDialog::makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                                        const QString& albumDest, bool previewMode)
{
    *proc << "convert";

    if ( previewMode && m_smallPreview->isChecked() )    // Preview mode and small preview enabled !
       {
       *m_PreviewProc << "-crop" << "300x300+0+0";
       m_previewOutput.append( " -crop 300x300+0+0 ");
       }

    if (m_Type->currentItem() == 0) // Solid
       {
       *proc << "-border";
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_solidWidth ) + "x";
       Temp2.append(Temp.setNum( m_solidWidth ));
       *proc << Temp2;
       *proc << "-bordercolor";
       Temp2 = "rgb(" + Temp.setNum(m_solidColor.red()) + ",";
       Temp2.append(Temp.setNum(m_solidColor.green()) + ",");
       Temp2.append(Temp.setNum(m_solidColor.blue()) + ")");
       *proc << Temp2;
       }

    if (m_Type->currentItem() == 1) // Niepce
       {
       QString Temp, Temp2;

       *proc << "-border";
       Temp2 = Temp.setNum( m_lineNiepceWidth ) + "x";
       Temp2.append(Temp.setNum( m_lineNiepceWidth ));
       *proc << Temp2;

       *proc << "-bordercolor";
       Temp2 = "rgb(" + Temp.setNum(m_NiepceColor.red()) + ",";
       Temp2.append(Temp.setNum(m_NiepceColor.green()) + ",");
       Temp2.append(Temp.setNum(m_NiepceColor.blue()) + ")");
       *proc << Temp2;

       *proc << "-border";
       Temp2 = Temp.setNum( m_NiepceWidth ) + "x";
       Temp2.append(Temp.setNum( m_NiepceWidth ));
       *proc << Temp2;

       *proc << "-bordercolor";
       Temp2 = "rgb(" + Temp.setNum(m_lineNiepceColor.red()) + ",";
       Temp2.append(Temp.setNum(m_lineNiepceColor.green()) + ",");
       Temp2.append(Temp.setNum(m_lineNiepceColor.blue()) + ")");
       *proc << Temp2;
       }

    if (m_Type->currentItem() == 2) // Raise
       {
       *proc << "-raise";
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_raiseWidth ) + "x";
       Temp2.append(Temp.setNum( m_raiseWidth ));
       *proc << Temp2;
       }

    if (m_Type->currentItem() == 3) // Frame
       {
       *proc << "-frame";
       QString Temp, Temp2;
       Temp2 = Temp.setNum( m_frameWidth ) + "x";
       Temp2.append(Temp.setNum( m_frameWidth ) + "+");
       Temp2.append(Temp.setNum( m_bevelWidth ) + "+");
       Temp2.append(Temp.setNum( m_bevelWidth ) );
       *proc << Temp2;
       *proc << "-mattecolor";
       Temp2 = "rgb(" + Temp.setNum(m_frameColor.red()) + ",";
       Temp2.append(Temp.setNum(m_frameColor.green()) + ",");
       Temp2.append(Temp.setNum(m_frameColor.blue()) + ")");
       *proc << Temp2;
       }

    *proc << "-verbose";

    *proc << item->pathSrc();

    if ( !previewMode )   // No preview mode !
       *proc << albumDest + "/" + item->nameDest();

    return(extractArguments(proc));
}

}  // NameSpace KIPIBatchProcessImagesPlugin

#include "borderimagesdialog.moc"
