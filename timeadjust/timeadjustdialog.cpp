/* ============================================================
 * Authors: Jesper K. Pedersen <blackie@kde.org>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright 2003-2005 by Jesper Pedersen
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
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
#include <qvgroupbox.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qgrid.h>
#include <qpushbutton.h>
#include <qframe.h>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <kdatetimewidget.h>
#include <kconfig.h>
#include <kmessagebox.h>

// LibKipi includes.

#include <libkipi/imageinfo.h>

// Local includes.

#include "pluginsversion.h"
#include "exiv2iface.h"
#include "timeadjustdialog.h"
#include "timeadjustdialog.moc"

namespace KIPITimeAdjustPlugin
{

TimeAdjustDialog::TimeAdjustDialog(KIPI::Interface* interface, QWidget* parent)
                : KDialogBase(Plain, i18n("Adjust Time & Date"), Help|Ok|Cancel, 
                              Ok, parent, 0, true, true),
                  m_interface(interface)
{
    QVBoxLayout *vlay = new QVBoxLayout(plainPage(), 6);

    // -- About data and help button ----------------------------------------

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Time Adjust"),
                                           NULL,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin for adjusting time stamp of picture files"),
                                           "(c) 2003-2005, Jesper K. Pedersen\n"
                                           "(c) 2006, Gilles Caulier");

    m_about->addAuthor("Jesper K. Pedersen", I18N_NOOP("Author and maintainer"),
                       "blackie@kde.org");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Developper"),
                       "caulier dot gilles at kdemail dot net");

    m_helpButton        = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Time Adjust Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    // -- Banner ------------------------------------------------------------

    QFrame *headerFrame = new QFrame(plainPage());
    headerFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QHBoxLayout* layout = new QHBoxLayout(headerFrame);
    layout->setMargin( 2 ); // to make sure the frame gets displayed
    layout->setSpacing( 0 );
    QLabel *pixmapLabelLeft = new QLabel(headerFrame, "pixmapLabelLeft");
    pixmapLabelLeft->setScaledContents( false );
    layout->addWidget( pixmapLabelLeft );
    QLabel *labelTitle = new QLabel( i18n("Adjust Time Stamp of Picture Files"), headerFrame, "labelTitle" );
    layout->addWidget( labelTitle );
    layout->setStretchFactor( labelTitle, 1 );
    vlay->addWidget( headerFrame );

    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");

    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    // -- Adjustment type ------------------------------------------------------------

    QVGroupBox *adjGB = new QVGroupBox(i18n("Adjustment Type"), plainPage());
    m_adjustTypeGrp   = new QButtonGroup(1, Qt::Horizontal, adjGB);
    m_add             = new QRadioButton(i18n("Add"), m_adjustTypeGrp);
    m_subtract        = new QRadioButton(i18n("Subtract"), m_adjustTypeGrp);
    m_exif            = new QRadioButton(i18n("Set file date to EXIF/IPTC creation date"), m_adjustTypeGrp);
    m_custom          = new QRadioButton(i18n("Custom date"), m_adjustTypeGrp);

    m_adjustTypeGrp->setFrameStyle(QFrame::NoFrame);
    m_adjustTypeGrp->setInsideMargin(0); 
    m_adjustTypeGrp->setRadioButtonExclusive(true);

    QHBox *hbox      = new QHBox(m_adjustTypeGrp);
    QLabel *space    = new QLabel(hbox);
    m_dateCreatedSel = new KDateTimeWidget(hbox);
    new QLabel(hbox);
    space->setFixedWidth(15);
    m_dateCreatedSel->setDateTime(QDateTime::currentDateTime());
    
    m_syncEXIFDateCheck = new QCheckBox(i18n("Sync EXIF creation date"), m_adjustTypeGrp);
    m_syncIPTCDateCheck = new QCheckBox(i18n("Sync IPTC creation date"), m_adjustTypeGrp);

    m_add->setChecked(true);
    vlay->addWidget(adjGB);

    // -- Adjustments ------------------------------------------------------------

    m_adjustValGrp = new QVButtonGroup(i18n("Adjustments"), plainPage());
    vlay->addWidget(m_adjustValGrp);

    QWidget* grid        = new QWidget(m_adjustValGrp);
    QGridLayout* gridLay = new QGridLayout(grid, 1, 7, spacingHint());
    gridLay->setColStretch( 2, 1 );
    gridLay->setColStretch( 5, 1 );

    QLabel* label = new QLabel( i18n("Hours:"), grid );
    m_hours       = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 0, 0 );
    gridLay->addWidget( m_hours, 0, 1 );

    label     = new QLabel( i18n("Minutes:"), grid );
    m_minutes = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 0, 3 );
    gridLay->addWidget( m_minutes, 0, 4 );

    label  = new QLabel( i18n("Seconds:"), grid );
    m_secs = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 0, 6 );
    gridLay->addWidget( m_secs, 0, 7 );

    label  = new QLabel( i18n("Days:"), grid );
    m_days = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 1, 0 );
    gridLay->addWidget( m_days, 1, 1 );

    label    = new QLabel( i18n("Months:"), grid );
    m_months = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 1, 3 );
    gridLay->addWidget( m_months, 1, 4 );

    label   = new QLabel( i18n("Years:"), grid );
    m_years = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 1, 6 );
    gridLay->addWidget( m_years, 1, 7 );

    // -- Example ------------------------------------------------------------

    m_exampleBox = new QVGroupBox(i18n("Example"), plainPage());
    vlay->addWidget(m_exampleBox);

    m_infoLabel  = new QLabel(m_exampleBox);
    m_exampleAdj = new QLabel(m_exampleBox);
    m_exampleAdj->setAlignment(Qt::AlignCenter);

    // -- Slots/Signals ------------------------------------------------------

    connect(m_adjustTypeGrp, SIGNAL( clicked(int) ),
            this, SLOT( slotAdjustmentTypeChanged() ));

    connect(m_secs, SIGNAL( valueChanged( int ) ), 
            this, SLOT( slotUpdateExample() ));

    connect(m_minutes, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    connect(m_hours, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    connect(m_days, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    connect(m_months, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    connect(m_years, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    // -----------------------------------------------------------------------

    readSettings();
    slotAdjustmentTypeChanged();
}

TimeAdjustDialog::~TimeAdjustDialog()
{
    delete m_about;
}

void TimeAdjustDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("timeadjust", "kipi-plugins");
}

void TimeAdjustDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void TimeAdjustDialog::slotCancel()
{
    saveSettings();
    KDialogBase::slotCancel();
}

void TimeAdjustDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("Time Adjust Settings");
    m_syncEXIFDateCheck->setChecked(config.readBoolEntry("Sync EXIF Date", true));
    m_syncIPTCDateCheck->setChecked(config.readBoolEntry("Sync IPTC Date", true));
    resize(configDialogSize(config, QString("Time Adjust Dialog")));
}

void TimeAdjustDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("Time Adjust Settings");
    config.writeEntry("Sync EXIF Date", m_syncEXIFDateCheck->isChecked());
    config.writeEntry("Sync IPTC Date", m_syncIPTCDateCheck->isChecked());
    saveDialogSize(config, QString("Time Adjust Dialog"));
    config.sync();
}

void TimeAdjustDialog::setImages(const KURL::List& images)
{
    m_images.clear();
    int exactCount=0;
    int inexactCount=0;

    for( KURL::List::ConstIterator it = images.begin(); it != images.end(); ++it ) 
    {
        KIPI::ImageInfo info = m_interface->info( *it );
        if (info.isTimeExact()) 
        {
            exactCount++;
            m_exampleDate = info.time();
            m_images.append(*it);
        }
        else
            inexactCount++;
    }

    if ( inexactCount > 0 ) 
    {
        QString tmpLabel = i18n("1 image will be changed; ",
                                "%n images will be changed; ",
                                exactCount)
                         + i18n("1 image will be skipped due to an inexact date.",
                                "%n images will be skipped due to inexact dates.",
                                inexactCount );

        m_infoLabel->setText(tmpLabel);
    }
    else 
    {
        m_infoLabel->setText(i18n("1 image will be changed",
                                  "%n images will be changed",
                                  m_images.count()));
    }
    // PENDING(blackie) handle all images being inexact.

    slotUpdateExample();
}

void TimeAdjustDialog::slotUpdateExample()
{
    QString oldDate = m_exampleDate.toString();
    QDateTime date  = updateTime(KURL(), m_exampleDate);
    QString newDate = date.toString();
    m_exampleAdj->setText(i18n("<b>%1</b><br>would, for example, "
                               "change into<br><b>%2</b>")
                          .arg(oldDate).arg(newDate));
}

void TimeAdjustDialog::slotAdjustmentTypeChanged()
{
    m_exampleBox->setEnabled(false);
    m_adjustValGrp->setEnabled(false);
    m_dateCreatedSel->setEnabled(false);
    m_syncEXIFDateCheck->setEnabled(false);
    m_syncIPTCDateCheck->setEnabled(false);

    if (m_add->isChecked() || m_subtract->isChecked())
    {
        m_exampleBox->setEnabled(true);
        m_adjustValGrp->setEnabled(true);
        m_syncEXIFDateCheck->setEnabled(true);
        m_syncIPTCDateCheck->setEnabled(true);
    }
    else if (m_custom->isChecked())
    {
        m_dateCreatedSel->setEnabled(true);
        m_syncEXIFDateCheck->setEnabled(true);
        m_syncIPTCDateCheck->setEnabled(true);
    }
}

void TimeAdjustDialog::slotOk()
{
    KURL::List  updatedURLs;
    QStringList errorFiles;

    for( KURL::List::ConstIterator it = m_images.begin(); it != m_images.end(); ++it )
    {
        KURL url = *it;
        bool ret = false;

        KIPI::ImageInfo info = m_interface->info(url);
        QDateTime dateTime   = info.time();
        dateTime             = updateTime(info.path(), info.time());
        info.setTime(dateTime);

        if (!m_exif->isChecked())
        {
            if (m_syncEXIFDateCheck->isChecked() || m_syncIPTCDateCheck->isChecked())
            {
                if (!KIPIPlugins::Exiv2Iface::isReadOnly(url.path()))
                {
                    ret = true;
                    KIPIPlugins::Exiv2Iface exiv2Iface;
    
                    ret &= exiv2Iface.load(url.path());
                    if (ret)
                    {
                        if (m_syncEXIFDateCheck->isChecked())
                        {
                            ret &= exiv2Iface.setExifTagString("Exif.Image.DateTime",
                                dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());
                        }
            
                        if (m_syncIPTCDateCheck->isChecked())
                        {
                            ret &= exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                                dateTime.date().toString(Qt::ISODate));
                            ret &= exiv2Iface.setIptcTagString("Iptc.Application2.TimeCreated",
                                dateTime.time().toString(Qt::ISODate));
                        }
        
                        ret &= exiv2Iface.save(url.path());
                    }
                }
        
                if (!ret)
                    errorFiles.append(url.fileName());
                else 
                    updatedURLs.append(url);
            }
        }        
    }

    // We use kipi interface refreshImages() method to tell to host than 
    // metadata from pictures have changed and need to be re-read.
    m_interface->refreshImages(m_images);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::informationList(
                     kapp->activeWindow(),
                     i18n("Unable to set date and time like picture metadata from:"),
                     errorFiles,
                     i18n("Adjust Time & Date"));  
    }

    saveSettings();
    accept();
}

QDateTime TimeAdjustDialog::updateTime(const KURL& url, const QDateTime& time) const
{
    if (m_custom->isChecked())
    {
        return m_dateCreatedSel->dateTime();
    }
    else if (m_exif->isChecked())
    {
        KIPIPlugins::Exiv2Iface exiv2Iface;
        if ( !exiv2Iface.load(url.path()) )
            return time;

        QDateTime newTime = exiv2Iface.getImageDateTime();
        if (newTime.isValid())
            return newTime;
        else
            return time;
    }
    else
    {
        int sign = -1;
        if (m_add->isChecked())
            sign = 1;

        QDateTime newTime = time.addSecs( sign * ( m_secs->value()
                                                   + 60*m_minutes->value()
                                                   + 60*60*m_hours->value()
                                                   + 24*60*60*m_days->value() ) );
        newTime = newTime.addMonths( sign * m_months->value() );
        newTime = newTime.addYears( sign * m_years->value() );
        return newTime;
    }
}

}  // NameSpace KIPITimeAdjustPlugin
