//////////////////////////////////////////////////////////////////////////////
//
//    BORDEROPTIONSDIALOG.CPP
//
//    Copyright (C) 2003 Gilles CAULIER <caulier dot gilles at free.fr>
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
#include <qstring.h>
#include <qcombobox.h>
#include <qcolor.h>

// Include files for KDE

#include <klocale.h>
#include <knuminput.h>
#include <kcolorbutton.h>

// Local includes

#include "borderoptionsdialog.h"


//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

BorderOptionsDialog::BorderOptionsDialog(QWidget *parent, QString BorderType)
                        : KDialogBase( parent, "BorderOptionsDialog", true,
                          i18n("Border options"), Ok|Cancel, Ok, false)
{
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );
    QString whatsThis;

    if (BorderType == i18n("Solid"))
       {
       QLabel *m_label_solidBorderWidth = new QLabel (i18n("Border width:"), box);
       dvlay->addWidget( m_label_solidBorderWidth );
       m_solidBorderWidth = new KIntNumInput(25, box);
       m_solidBorderWidth->setRange(1, 1000, 1, true );
       QWhatsThis::add( m_solidBorderWidth, i18n("<p>Select here the border width in pixels.") );
       m_label_solidBorderWidth->setBuddy( m_solidBorderWidth );
       dvlay->addWidget( m_solidBorderWidth );

       QLabel *m_label_solidColor = new QLabel(i18n("Border color:"), box);
       dvlay->addWidget( m_label_solidColor );
       QColor solidBorderColor = QColor( 0, 0, 0 );                         // Black per default.
       m_button_solidBorderColor = new KColorButton( solidBorderColor, box );
       QWhatsThis::add( m_button_solidBorderColor, i18n( "<p>You can select here the border color." ));
       dvlay->addWidget( m_button_solidBorderColor );
       }

    if (BorderType == i18n("Niepce"))
       {
       QLabel *m_label_lineNiepceBorderWidth = new QLabel (i18n("Line border width:"), box);
       dvlay->addWidget( m_label_lineNiepceBorderWidth );
       m_lineNiepceBorderWidth = new KIntNumInput(10, box);
       m_lineNiepceBorderWidth->setRange(1, 500, 1, true );
       QWhatsThis::add( m_lineNiepceBorderWidth, i18n("<p>Select here the line border width in pixels.") );
       m_label_lineNiepceBorderWidth->setBuddy( m_lineNiepceBorderWidth );
       dvlay->addWidget( m_lineNiepceBorderWidth );

       QLabel *m_label_lineNiepceColor = new QLabel(i18n("Line border color:"), box);
       dvlay->addWidget( m_label_lineNiepceColor );
       QColor lineNiepceBorderColor = QColor( 0, 0, 0 );                         // Black per default.
       m_button_lineNiepceBorderColor = new KColorButton( lineNiepceBorderColor, box );
       QWhatsThis::add( m_button_lineNiepceBorderColor, i18n( "<p>You can select here the line border color." ));
       dvlay->addWidget( m_button_lineNiepceBorderColor );

       QLabel *m_label_NiepceBorderWidth = new QLabel (i18n("Border width:"), box);
       dvlay->addWidget( m_label_NiepceBorderWidth );
       m_NiepceBorderWidth = new KIntNumInput(100, box);
       m_NiepceBorderWidth->setRange(1, 500, 1, true );
       QWhatsThis::add( m_NiepceBorderWidth, i18n("<p>Select here the border width in pixels.") );
       m_label_NiepceBorderWidth->setBuddy( m_NiepceBorderWidth );
       dvlay->addWidget( m_NiepceBorderWidth );

       QLabel *m_label_NiepceColor = new QLabel(i18n("Border color:"), box);
       dvlay->addWidget( m_label_NiepceColor );
       QColor NiepceBorderColor = QColor( 255, 255, 255 );                         // White per default.
       m_button_NiepceBorderColor = new KColorButton( NiepceBorderColor, box );
       QWhatsThis::add( m_button_NiepceBorderColor, i18n( "<p>You can select here the border color." ));
       dvlay->addWidget( m_button_NiepceBorderColor );
       }

    if (BorderType == i18n("Raise"))
       {
       QLabel *m_label_raiseBorderWidth = new QLabel (i18n("Border width:"), box);
       dvlay->addWidget( m_label_raiseBorderWidth );
       m_raiseBorderWidth = new KIntNumInput(50, box);
       m_raiseBorderWidth->setRange(1, 500, 1, true );
       QWhatsThis::add( m_raiseBorderWidth, i18n("<p>Select here the border width in pixels.") );
       m_label_raiseBorderWidth->setBuddy( m_raiseBorderWidth );
       dvlay->addWidget( m_raiseBorderWidth );
       }

    if (BorderType == i18n("Frame"))
       {
       QLabel *m_label_frameBorderWidth = new QLabel (i18n("Border width:"), box);
       dvlay->addWidget( m_label_frameBorderWidth );
       m_frameBorderWidth = new KIntNumInput(25, box);
       m_frameBorderWidth->setRange(0, 500, 1, true );
       QWhatsThis::add( m_frameBorderWidth, i18n("<p>Select here the border width in pixels.") );
       m_label_frameBorderWidth->setBuddy( m_frameBorderWidth );
       dvlay->addWidget( m_frameBorderWidth );

       QLabel *m_label_frameBevelBorderWidth = new QLabel (i18n("Bevel width:"), box);
       dvlay->addWidget( m_label_frameBevelBorderWidth );
       m_frameBevelBorderWidth = new KIntNumInput(10, box);
       m_frameBevelBorderWidth->setRange(0, 250, 1, true );
       QWhatsThis::add( m_frameBevelBorderWidth, i18n("<p>Select here the bevel width in pixels. "
                                                      "This value must be <= Border width / 2") );
       m_label_frameBevelBorderWidth->setBuddy( m_frameBevelBorderWidth );
       dvlay->addWidget( m_frameBevelBorderWidth );

       QLabel *m_label_frameColor = new QLabel(i18n("Border color:"), box);
       dvlay->addWidget( m_label_frameColor );
       QColor frameBorderColor = QColor( 0, 0, 0 );                         // Black per default.
       m_button_frameBorderColor = new KColorButton( frameBorderColor, box );
       QWhatsThis::add( m_button_frameBorderColor, i18n( "<p>You can select here the border color." ));
       dvlay->addWidget( m_button_frameBorderColor );

       connect(m_frameBorderWidth, SIGNAL(valueChanged(int)), this, SLOT(slotFrameBorderWidthChanged(int)));
       }
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

BorderOptionsDialog::~BorderOptionsDialog()
{
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void BorderOptionsDialog::slotFrameBorderWidthChanged (int value)
{
   m_frameBevelBorderWidth->setMaxValue((int)(value / 2));
}
