/* ============================================================
 * File  : timeadjustdialog.cpp
 * Author: Jesper K. Pedersen <blackie@kde.org>
 * Date  : 2004-05-16
 * Description :
 *
 * Copyright 2003 by Jesper Pedersen
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */
 
// Qt includes.

#include <qlayout.h>
#include <qlabel.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qgrid.h>
#include <qpushbutton.h>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// LibKIPI includes.

#include <libkipi/imageinfo.h>

// Local includes.

#include "timeadjustdialog.h"

namespace KIPITimeAdjustPlugin
{

TimeAdjustDialog::TimeAdjustDialog( KIPI::Interface* interface, QWidget* parent, const char* name )
                : KDialogBase( IconList, i18n("Adjust Time & Date"), Help|Ok|Cancel, Ok, parent, name ),
                  m_interface( interface )
{
    // About data and help button.
    
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Time Adjust"), 
                                       "0.1.0-cvs",
                                       I18N_NOOP("A Kipi plugin for adjusting dates and times"),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Jesper K. Pedersen", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi.php");
    
    about->addAuthor("Jesper K. Pedersen", I18N_NOOP("Author and maintainer"),
                     "blackie@kde.org");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Time Adjust handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    // ------------------------------------------------------------------
    
    addInfoPage();
    addConfigPage();
    
    connect( this, SIGNAL( okClicked() ),
             this, SLOT( slotOK() ) );
}

void TimeAdjustDialog::setImages( const KURL::List& images )
{
    m_images.clear();
    int exactCount=0;
    int inexactCount=0;

    for( KURL::List::ConstIterator it = images.begin(); it != images.end(); ++it ) {
        KIPI::ImageInfo info = m_interface->info( *it );
        if ( info.isTimeExact() ) {
            exactCount++;
            m_exampleDate = info.time();
            m_images.append( *it );
        }
        else
            inexactCount++;
    }

    if ( inexactCount > 0 ) {
    
        QString tmpLabel = i18n("1 images will be changed; ", "%n images will be changed; ", exactCount)
                         + i18n("1 image will be skipped due an inexact date.", "%n images will be skipped due to inexact dates.", inexactCount );
        m_infoLabel->setText( tmpLabel );
    } else {
        m_infoLabel->setText( i18n("1 image will be changed", "%n images will be changed", m_images.count() ) );
        }
    // PENDING(blackie) handle all images being inexact.

    updateExample();
}

void TimeAdjustDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("timeadjust",
                                             "kipi-plugins");
}

void TimeAdjustDialog::addConfigPage()
{
    QWidget* page = addPage( i18n("Configure"), i18n("Adjust Time and Dates"),
                             BarIcon("config", KIcon::SizeMedium ) );
    QVBoxLayout *vlay = new QVBoxLayout( page, 6 );

    // Adjustment type
    QVButtonGroup* grp = new QVButtonGroup( i18n("Adjustment Type"), page, "adjustment type" );
    m_add = new QRadioButton( i18n("Add"), grp );
    new QRadioButton( i18n("Subtract" ), grp );
    vlay->addWidget( grp );
    m_add->setChecked( true );
    connect( grp, SIGNAL( clicked(int) ), this, SLOT( updateExample() ) );

    // Adjustment
    grp = new QVButtonGroup( i18n("Adjustment"), page, "adjustment" );
    vlay->addWidget( grp );
    QWidget* grid = new QWidget( grp );
    QGridLayout* gridLay = new QGridLayout( grid, 0, 3 );
    gridLay->setColStretch( 2, 1 );

    QLabel* label = new QLabel( i18n("Seconds:"), grid );
    m_secs = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 0, 0 );
    gridLay->addWidget( m_secs, 0, 1 );

    label = new QLabel( i18n("Minutes:"), grid );
    m_minutes = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 1, 0 );
    gridLay->addWidget( m_minutes, 1, 1 );

    label = new QLabel( i18n("Hours:"), grid );
    m_hours = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 2, 0 );
    gridLay->addWidget( m_hours, 2, 1 );

    label = new QLabel( i18n("Days:"), grid );
    m_days = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 3, 0 );
    gridLay->addWidget( m_days, 3, 1 );

    label = new QLabel( i18n("Month:"), grid );
    m_months = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 4, 0 );
    gridLay->addWidget( m_months, 4, 1 );

    label = new QLabel( i18n("Years:"), grid );
    m_years = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 5, 0 );
    gridLay->addWidget( m_years, 5, 1 );

    // Example page
    grp = new QVButtonGroup( i18n( "Example" ), page, "example" );
    vlay->addWidget( grp );

    m_infoLabel = new QLabel( grp );
    m_exampleAdj = new QLabel( grp );

    connect( m_secs, SIGNAL( valueChanged( int ) ), this,
             SLOT( updateExample() ) );
             
    connect( m_minutes, SIGNAL( valueChanged( int ) ),
             this, SLOT( updateExample() ) );
             
    connect( m_hours, SIGNAL( valueChanged( int ) ), 
             this, SLOT( updateExample() ) );
             
    connect( m_days, SIGNAL( valueChanged( int ) ), 
             this, SLOT( updateExample() ) );
             
    connect( m_months, SIGNAL( valueChanged( int ) ), 
             this, SLOT( updateExample() ) );
             
    connect( m_years, SIGNAL( valueChanged( int ) ), 
             this, SLOT( updateExample() ) );
}

void TimeAdjustDialog::addInfoPage()
{
    QWidget* pageAbout = addPage( i18n("Description"), i18n("Description"),
                                  BarIcon("contents", KIcon::SizeMedium ) );
    QVBoxLayout *vlay = new QVBoxLayout( pageAbout, 6 );

    QLabel *label = new QLabel( i18n("<qt><p>Sometimes it happens that the "
                                     "time on your digital cameral is set incorrectly, and the dates of all "
                                     "your images are therefore incorrect.</p>"
                                     "<p>Using this plugin you can adjust the time of several images at once, "
                                     "i.e. add or substract a certain amount of time (in minutes, hours, or days) to each image.</p></qt>"),
                                pageAbout);
    vlay->addWidget(label); vlay->addStretch(1);


}

void TimeAdjustDialog::updateExample()
{
    QString oldDate = m_exampleDate.toString();
    QDateTime date = updateTime( m_exampleDate );
    QString newDate = date.toString();
    m_exampleAdj->setText( i18n( "%1 would, for example, change into %2").arg(oldDate).arg(newDate) );
}

void TimeAdjustDialog::slotOK()
{
    for( KURL::List::ConstIterator it = m_images.begin(); it != m_images.end(); ++it ) {
        KIPI::ImageInfo info = m_interface->info( *it );
        QDateTime time = info.time();
        time = updateTime( time );
        info.setTime( time );
    }
}

QDateTime TimeAdjustDialog::updateTime( QDateTime time ) const
{
    int sign = -1;
    if ( m_add->isChecked() )
        sign = 1;

    time = time.addSecs( sign * ( m_secs->value() + 60*m_minutes->value() + 60*60*m_hours->value() + 24*60*60*m_days->value() ) );
    time = time.addMonths( sign * m_months->value() );
    time = time.addYears( sign * m_years->value() );
    return time;
}

}  // NameSpace KIPITimeAdjustPlugin

#include "timeadjustdialog.moc"
