//////////////////////////////////////////////////////////////////////////////
//
//    RESIZEOPTIONSDIALOG.CPP
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

#include <qvbox.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qcolor.h>

// Include files for KDE

#include <klocale.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <kmessagebox.h>

// Local includes

#include "resizeoptionsdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

ResizeOptionsDialog::ResizeOptionsDialog(QWidget *parent, QString ResizeType)
                   : KDialogBase( parent, "ResizeOptionsDialog", true,
                     i18n("Image-Resize options"), Ok|Cancel, Ok, false)
{
    m_Type = ResizeType;
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );
    QString whatsThis;

    if (m_Type == i18n("Proportional (1 dim.)"))
       {
       QGroupBox * groupBox1 = new QGroupBox( 1, Qt::Horizontal, i18n("Resize options"), box );

       m_size = new KIntNumInput(640, groupBox1);
       m_size->setRange(10, 10000, 1, true );
       m_size->setLabel( i18n("New size (pixels):") );
       QWhatsThis::add( m_size, i18n("<p>The new images' size in pixels.") );

       m_label_resizeFilter = new QLabel (i18n("Filter name:"), groupBox1);

       m_resizeFilter = new QComboBox( false, groupBox1 );
       m_resizeFilter->insertItem("Bessel");
       m_resizeFilter->insertItem("Blackman");
       m_resizeFilter->insertItem("Box");
       m_resizeFilter->insertItem("Catrom");
       m_resizeFilter->insertItem("Cubic");
       m_resizeFilter->insertItem("Gaussian");
       m_resizeFilter->insertItem("Hermite");
       m_resizeFilter->insertItem("Hanning");
       m_resizeFilter->insertItem("Hamming");
       m_resizeFilter->insertItem("Lanczos");
       m_resizeFilter->insertItem("Mitchell");
       m_resizeFilter->insertItem("Point");
       m_resizeFilter->insertItem("Quadratic");
       m_resizeFilter->insertItem("Sinc");
       m_resizeFilter->insertItem("Triangle");
       QWhatsThis::add( m_resizeFilter, i18n("<p>Select here the filter name for the resize-image process. "
                                             "This filter will be used like a kernel convolution process "
                                             "during the increased image size rendering. The default filter "
                                             "is 'Lanczos'.") );
       m_label_resizeFilter->setBuddy( m_resizeFilter );

       dvlay->addWidget( groupBox1 );

       m_label_size = new QLabel ( i18n("Note: the images will be resized to\n"
                                        "this size. The width or the height of the\n"
                                        "images will be automatically\n"
                                        "selected in depending of the images orientation.\n"
                                        "The images' aspect ratios are preserved."), box);
       dvlay->addWidget( m_label_size );
       }

    if (m_Type == i18n("Proportional (2 dim.)"))
       {
       QGroupBox * groupBox1 = new QGroupBox( 2, Qt::Horizontal, i18n("Size settings"), box );

       m_label_Width = new QLabel (i18n("Width (pixels):"), groupBox1);
       m_Width = new KIntNumInput(1024, groupBox1);
       m_Width->setRange(100, 10000, 1, true );
       QWhatsThis::add( m_Width, i18n("<p>The new images' width in pixels."));
       m_label_Width->setBuddy( m_Width );

       m_label_Height = new QLabel (i18n("Height (pixels):"), groupBox1);
       m_Height = new KIntNumInput(768, groupBox1);
       m_Height->setRange(100, 10000, 1, true );
       QWhatsThis::add( m_Height, i18n("<p>The new images' height in pixels."));
       m_label_Height->setBuddy( m_Height );

       dvlay->addWidget( groupBox1 );

       QGroupBox * groupBox2 = new QGroupBox( 2, Qt::Horizontal, i18n("Rendering settings"), box );

       m_label_bgColor = new QLabel(i18n("Background color:"), groupBox2);
       QColor bgColor = QColor( 0, 0, 0 );                         // Black per default.
       m_button_bgColor = new KColorButton( bgColor, groupBox2 );
       QWhatsThis::add( m_button_bgColor, i18n( "<p>You can select here the background color to "
                                                "be used when adapting the images' sizes." ));
       m_label_bgColor->setBuddy( m_button_bgColor );

       m_label_resizeFilter = new QLabel (i18n("Filter name:"), groupBox2);
       m_resizeFilter = new QComboBox( false, groupBox2 );
       m_resizeFilter->insertItem("Bessel");
       m_resizeFilter->insertItem("Blackman");
       m_resizeFilter->insertItem("Box");
       m_resizeFilter->insertItem("Catrom");
       m_resizeFilter->insertItem("Cubic");
       m_resizeFilter->insertItem("Gaussian");
       m_resizeFilter->insertItem("Hermite");
       m_resizeFilter->insertItem("Hanning");
       m_resizeFilter->insertItem("Hamming");
       m_resizeFilter->insertItem("Lanczos");
       m_resizeFilter->insertItem("Mitchell");
       m_resizeFilter->insertItem("Point");
       m_resizeFilter->insertItem("Quadratic");
       m_resizeFilter->insertItem("Sinc");
       m_resizeFilter->insertItem("Triangle");
       QWhatsThis::add( m_resizeFilter, i18n("<p>Select here the filter name for the resize-image process. "
                                             "This filter will be used like a kernel convolution process "
                                             "during the increased image size rendering. The default filter "
                                             "is 'Lanczos'.") );
       m_label_resizeFilter->setBuddy( m_resizeFilter );

       m_label_border = new QLabel (i18n("Border size (pixels):"), groupBox2);
       m_Border = new KIntNumInput(100, groupBox2);
       m_Border->setRange(0, 1000, 1, true );
       QWhatsThis::add( m_Border, i18n("<p>The border size around the images in pixels."));
       m_label_border->setBuddy( m_Border );

       dvlay->addWidget( groupBox2 );
       }

    if (m_Type == i18n("Non proportional"))
       {
       QGroupBox * groupBox1 = new QGroupBox( 1, Qt::Horizontal, i18n("Resize options"), box );

       m_fixedWidth = new KIntNumInput(640, groupBox1);
       m_fixedWidth->setRange(10, 10000, 1, true );
       m_fixedWidth->setLabel( i18n("New width (pixels):") );
       QWhatsThis::add( m_fixedWidth, i18n("<p>The new images' width in pixels.") );

       m_fixedHeight = new KIntNumInput(480, groupBox1);
       m_fixedHeight->setRange(10, 10000, 1, true );
       m_fixedHeight->setLabel( i18n("New height (pixels):") );
       QWhatsThis::add( m_fixedHeight, i18n("<p>The new images' height in pixels.") );

       m_label_resizeFilter = new QLabel (i18n("Filter name:"), groupBox1);

       m_resizeFilter = new QComboBox( false, groupBox1 );
       m_resizeFilter->insertItem("Bessel");
       m_resizeFilter->insertItem("Blackman");
       m_resizeFilter->insertItem("Box");
       m_resizeFilter->insertItem("Catrom");
       m_resizeFilter->insertItem("Cubic");
       m_resizeFilter->insertItem("Gaussian");
       m_resizeFilter->insertItem("Hermite");
       m_resizeFilter->insertItem("Hanning");
       m_resizeFilter->insertItem("Hamming");
       m_resizeFilter->insertItem("Lanczos");
       m_resizeFilter->insertItem("Mitchell");
       m_resizeFilter->insertItem("Point");
       m_resizeFilter->insertItem("Quadratic");
       m_resizeFilter->insertItem("Sinc");
       m_resizeFilter->insertItem("Triangle");
       QWhatsThis::add( m_resizeFilter, i18n("<p>Select here the filter name for the resize-image process. "
                                             "This filter will be used like a kernel convolution process "
                                             "during the increased image size rendering. The default filter "
                                             "is 'Lanczos'.") );
       m_label_resizeFilter->setBuddy( m_resizeFilter );

       dvlay->addWidget( groupBox1 );
       }

    if (m_Type == i18n("Prepare to print"))
       {
       m_customSettings = new QCheckBox( i18n("Use custom settings"), box);
       QWhatsThis::add( m_customSettings, i18n("<p>If this option is enabled, "
                                               "all printing settings can be customized."));
       dvlay->addWidget( m_customSettings );

       QGroupBox * groupBox1 = new QGroupBox( 2, Qt::Horizontal, i18n("Printing standard settings"), box );

       m_label_paperSize = new QLabel (i18n("Paper size (cm):"), groupBox1);
       m_paperSize = new QComboBox( false, groupBox1 );
       m_paperSize->insertItem("9x13");
       m_paperSize->insertItem("10x15");
       m_paperSize->insertItem("13x19");
       m_paperSize->insertItem("15x21");
       m_paperSize->insertItem("18x24");
       m_paperSize->insertItem("20x30");
       m_paperSize->insertItem("21x30");
       m_paperSize->insertItem("30x40");
       m_paperSize->insertItem("30x45");
       m_paperSize->insertItem("40x50");
       m_paperSize->insertItem("50x75");
       QWhatsThis::add( m_paperSize, i18n("<p>The standard photographic paper sizes in centimeters."));
       m_label_paperSize->setBuddy( m_paperSize );

       m_label_printDpi = new QLabel (i18n("Print resolution (dpi):"), groupBox1);
       m_printDpi = new QComboBox( false, groupBox1 );
       m_printDpi->insertItem("75");
       m_printDpi->insertItem("150");
       m_printDpi->insertItem("300");
       m_printDpi->insertItem("600");
       m_printDpi->insertItem("1200");
       m_printDpi->insertItem("2400");
       QWhatsThis::add( m_printDpi, i18n("<p>The standard print resolutions in dots per inch."));
       m_label_printDpi->setBuddy( m_printDpi );

       dvlay->addWidget( groupBox1 );

       QGroupBox * groupBox2 = new QGroupBox( 2, Qt::Horizontal, i18n("Printing custom settings"), box );

       m_label_customXSize = new QLabel (i18n("Paper width (cm):"), groupBox2);
       m_customXSize = new KIntNumInput(10, groupBox2);
       m_customXSize->setRange(1, 100, 1, true );
       QWhatsThis::add( m_customXSize, i18n("<p>The customized width of the photographic paper size in centimeters."));
       m_label_customXSize->setBuddy( m_customXSize );

       m_label_customYSize = new QLabel (i18n("Paper height (cm):"), groupBox2);
       m_customYSize = new KIntNumInput(15, groupBox2);
       m_customYSize->setRange(1, 100, 1, true );
       QWhatsThis::add( m_customYSize, i18n("<p>The customized height of the photographic paper size in centimeters."));
       m_label_customYSize->setBuddy( m_customYSize );

       m_label_customDpi = new QLabel (i18n("Print resolution (dpi):"), groupBox2);
       m_customDpi = new KIntNumInput(300, groupBox2);
       m_customDpi->setRange(10, 5000, 10, true );
       QWhatsThis::add( m_customDpi, i18n("<p>The customized print resolution in dots per inch."));
       m_label_customDpi->setBuddy( m_customDpi );

       dvlay->addWidget( groupBox2 );

       QGroupBox * groupBox3 = new QGroupBox( 2, Qt::Horizontal, i18n("Rendering settings"), box );

       m_label_backgroundColor = new QLabel(i18n("Background color:"), groupBox3);
       QColor backgroundColor = QColor( 255, 255, 255 );                         // White per default.
       m_button_backgroundColor = new KColorButton( backgroundColor, groupBox3 );
       QWhatsThis::add( m_button_backgroundColor, i18n( "<p>You can select here the background color to "
                                                "be used when adapting the images' sizes." ));
       m_label_backgroundColor->setBuddy( m_button_backgroundColor );

       m_label_resizeFilter = new QLabel (i18n("Filter name:"), groupBox3);
       m_resizeFilter = new QComboBox( false, groupBox3 );
       m_resizeFilter->insertItem("Bessel");
       m_resizeFilter->insertItem("Blackman");
       m_resizeFilter->insertItem("Box");
       m_resizeFilter->insertItem("Catrom");
       m_resizeFilter->insertItem("Cubic");
       m_resizeFilter->insertItem("Gaussian");
       m_resizeFilter->insertItem("Hermite");
       m_resizeFilter->insertItem("Hanning");
       m_resizeFilter->insertItem("Hamming");
       m_resizeFilter->insertItem("Lanczos");
       m_resizeFilter->insertItem("Mitchell");
       m_resizeFilter->insertItem("Point");
       m_resizeFilter->insertItem("Quadratic");
       m_resizeFilter->insertItem("Sinc");
       m_resizeFilter->insertItem("Triangle");
       QWhatsThis::add( m_resizeFilter, i18n("<p>Select here the filter name for the resize-image process. "
                                             "This filter will be used like a kernel convolution process "
                                             "during the increased image size rendering. The default filter "
                                             "is 'Lanczos'.") );
       m_label_resizeFilter->setBuddy( m_resizeFilter );

       m_label_marging = new QLabel (i18n("Margin size (mm):"), groupBox3);
       m_marging = new KIntNumInput(1, groupBox3);
       m_marging->setRange(0, 80, 1, true );
       QWhatsThis::add( m_marging, i18n("<p>The margin around the images in millimeters."));
       m_label_marging->setBuddy( m_marging );


       dvlay->addWidget( groupBox3 );

       connect(m_customSettings, SIGNAL( toggled(bool) ),
               this, SLOT( slotCustomSettingsEnabled(bool) ) );

       slotCustomSettingsEnabled(false);
       }
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

ResizeOptionsDialog::~ResizeOptionsDialog()
{
}

///////////////////////////////////////// SLOTS /////////////////////////////////////////////

void ResizeOptionsDialog::slotCustomSettingsEnabled(bool val)
{
    m_label_paperSize->setEnabled( !val );
    m_paperSize->setEnabled( !val );
    m_label_printDpi->setEnabled( !val );
    m_printDpi->setEnabled( !val );

    m_label_customXSize->setEnabled( val );
    m_customXSize->setEnabled( val );
    m_label_customYSize->setEnabled( val );
    m_customYSize->setEnabled( val );
    m_label_customDpi->setEnabled( val );
    m_customDpi->setEnabled( val );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ResizeOptionsDialog::slotOk()
{
    if (m_Type == i18n("Prepare to print"))
       {
       if (m_customSettings->isChecked() == true)
          {
          if (m_customXSize > m_customYSize)
             {
             KMessageBox::sorry(this, i18n("You must enter a custom height greater than the custom width: "
                                     "the photographic paper must be vertically orientated."));
             return;
             }
          }
       }

    accept();
}

}  // NameSpace KIPIBatchProcessImagesPlugin

#include "resizeoptionsdialog.moc"
