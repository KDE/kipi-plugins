//////////////////////////////////////////////////////////////////////////////
//
//    COLOROPTIONSDIALOG.CPP
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
#include <qcolor.h>

// Include files for KDE

#include <kdialogbase.h>
#include <klocale.h>
#include <knuminput.h>
#include <kcolorbutton.h>

// Local includes

#include "coloroptionsdialog.h"


//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

ColorOptionsDialog::ColorOptionsDialog(QWidget *parent, QString ColorType)
                        : KDialogBase( parent, "ColorOptionsDialog", true,
                          i18n("Color options"), Ok|Cancel, Ok, false)
{
    QWidget* box = new QWidget( this );
    setMainWidget(box);
    QVBoxLayout *dvlay = new QVBoxLayout( box, 10, spacingHint() );
    QString whatsThis;

    if (ColorType == i18n("Depth"))
       {
       QLabel *m_label_depthValue = new QLabel (i18n("Depth value:"), box);
       dvlay->addWidget( m_label_depthValue );
       m_depthValue = new QComboBox( false, box );
       m_depthValue->insertItem("8");
       m_depthValue->insertItem("16");
       m_depthValue->insertItem("32");
       QWhatsThis::add( m_depthValue, i18n("<p>Select here the image depth in bits.") );
       m_label_depthValue->setBuddy( m_depthValue );
       dvlay->addWidget( m_depthValue );
       }

    if (ColorType == i18n("Fuzz"))
       {
       QLabel *m_label_fuzzDistance = new QLabel (i18n("Distance:"), box);
       dvlay->addWidget( m_label_fuzzDistance );
       m_fuzzDistance = new KIntNumInput(3, box);
       m_fuzzDistance->setRange(0, 20, 1, true );
       QWhatsThis::add( m_fuzzDistance, i18n("<p>Select here the fuzz distance in "
                                             "absolute intensity units.") );
       m_label_fuzzDistance->setBuddy( m_fuzzDistance );
       dvlay->addWidget( m_fuzzDistance );
       }

    if (ColorType == i18n("Segment"))
       {
       QLabel *m_label_segmentCluster = new QLabel (i18n("Cluster threshold:"), box);
       dvlay->addWidget( m_label_segmentCluster );
       m_segmentCluster = new KIntNumInput(3, box);
       m_segmentCluster->setRange(0, 20, 1, true );
       QWhatsThis::add( m_segmentCluster, i18n("<p>Select here the value who represents the minimum "
                                               "number of pixels contained in a hexahedra before it can "
                                               "be considered valid.") );
       m_label_segmentCluster->setBuddy( m_segmentCluster );
       dvlay->addWidget( m_segmentCluster );

       QLabel *m_label_segmentSmooth = new QLabel (i18n("Smooth threshold:"), box);
       dvlay->addWidget( m_label_segmentSmooth );
       m_segmentSmooth = new KIntNumInput(3, box);
       m_segmentSmooth->setRange(0, 20, 1, true );
       QWhatsThis::add( m_segmentSmooth, i18n("<p>Select here the value who eliminates noise in the "
                                              "second derivative of the histogram. As the value is increased, "
                                              "you can expect a smoother second derivative.") );
       m_label_segmentSmooth->setBuddy( m_segmentSmooth );
       dvlay->addWidget( m_segmentSmooth );
       }
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

ColorOptionsDialog::~ColorOptionsDialog()
{
}




#include "coloroptionsdialog.moc"
