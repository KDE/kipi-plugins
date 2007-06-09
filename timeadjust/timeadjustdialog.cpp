/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// C Ansi includes.

extern "C"
{
#include <utime.h>
}

// Qt includes.

#include <qtooltip.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvbuttongroup.h>
#include <qvgroupbox.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qfile.h>
#include <qspinbox.h>
#include <qgrid.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qtoolbutton.h>

// KDE includes.

#include <kiconloader.h>
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

#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "timeadjustdialog.h"
#include "timeadjustdialog.moc"

namespace KIPITimeAdjustPlugin
{

class TimeAdjustDialogPrivate
{

public:

    TimeAdjustDialogPrivate()
    {
        add               = 0;
        subtract          = 0;
        exif              = 0;
        custom            = 0;
        syncEXIFDateCheck = 0;
        syncIPTCDateCheck = 0;
        exampleBox        = 0;
        adjustValGrp      = 0;
        adjustTypeGrp     = 0;
        infoLabel         = 0;
        exampleAdj        = 0;
        secs              = 0;
        minutes           = 0;
        hours             = 0;
        days              = 0;
        months            = 0;
        years             = 0;
        dateCreatedSel    = 0;
        interface         = 0;
        about             = 0;
        todayBtn          = 0;
    }

    QRadioButton             *add;
    QRadioButton             *subtract;
    QRadioButton             *exif;
    QRadioButton             *custom;

    QToolButton              *todayBtn;

    QCheckBox                *syncEXIFDateCheck;
    QCheckBox                *syncIPTCDateCheck;

    QVGroupBox               *exampleBox;
    QVButtonGroup            *adjustValGrp;
    QButtonGroup             *adjustTypeGrp;
    
    QLabel                   *infoLabel;
    QLabel                   *exampleAdj;
    
    QSpinBox                 *secs;
    QSpinBox                 *minutes;
    QSpinBox                 *hours;
    QSpinBox                 *days;
    QSpinBox                 *months;
    QSpinBox                 *years;
    
    QDateTime                 exampleDate;

    KDateTimeWidget          *dateCreatedSel;

    KURL::List                images;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;
};

TimeAdjustDialog::TimeAdjustDialog(KIPI::Interface* interface, QWidget* parent)
                : KDialogBase(Plain, i18n("Adjust Time & Date"), Help|Ok|Cancel, 
                              Ok, parent, 0, true, true)
{
    d = new TimeAdjustDialogPrivate;
    d->interface = interface;

    QVBoxLayout *vlay = new QVBoxLayout(plainPage(), 6);

    // -- About data and help button ----------------------------------------

    d->about = new KIPIPlugins::KPAboutData(I18N_NOOP("Time Adjust"),
                                           NULL,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin for adjusting time stamp of picture files"),
                                           "(c) 2003-2005, Jesper K. Pedersen\n"
                                           "(c) 2006, Gilles Caulier");

    d->about->addAuthor("Jesper K. Pedersen", I18N_NOOP("Author"),
                        "blackie@kde.org");

    d->about->addAuthor("Gilles Caulier", I18N_NOOP("Developper and maintainer"),
                        "caulier dot gilles at gmail dot com");

    QPushButton *helpButton = actionButton(Help);
    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Time Adjust Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    helpButton->setPopup(helpMenu->menu());

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
    d->adjustTypeGrp  = new QButtonGroup(1, Qt::Horizontal, adjGB);
    d->add            = new QRadioButton(i18n("Add"), d->adjustTypeGrp);
    d->subtract       = new QRadioButton(i18n("Subtract"), d->adjustTypeGrp);
    d->exif           = new QRadioButton(i18n("Set file date to EXIF/IPTC creation date"), d->adjustTypeGrp);
    d->custom         = new QRadioButton(i18n("Custom date"), d->adjustTypeGrp);

    d->adjustTypeGrp->setFrameStyle(QFrame::NoFrame);
    d->adjustTypeGrp->setInsideMargin(0); 
    d->adjustTypeGrp->setRadioButtonExclusive(true);

    QHBox *hbox       = new QHBox(d->adjustTypeGrp);
    QLabel *space1     = new QLabel(hbox);
    space1->setFixedWidth(15);
    d->dateCreatedSel = new KDateTimeWidget(hbox);
    QLabel *space2     = new QLabel(hbox);
    space2->setFixedWidth(15);
    d->todayBtn       = new QToolButton(hbox);   
    d->todayBtn->setIconSet(SmallIcon("today"));
    QToolTip::add(d->todayBtn, i18n("Reset to current date"));
    new QLabel(hbox);
    
    d->syncEXIFDateCheck = new QCheckBox(i18n("Update Exif creation date"), d->adjustTypeGrp);
    d->syncIPTCDateCheck = new QCheckBox(i18n("Update IPTC creation date"), d->adjustTypeGrp);

    vlay->addWidget(adjGB);

    // -- Adjustments ------------------------------------------------------------

    d->adjustValGrp = new QVButtonGroup(i18n("Adjustments"), plainPage());
    vlay->addWidget(d->adjustValGrp);

    QWidget* grid        = new QWidget(d->adjustValGrp);
    QGridLayout* gridLay = new QGridLayout(grid, 1, 7, spacingHint());
    gridLay->setColStretch( 2, 1 );
    gridLay->setColStretch( 5, 1 );

    QLabel* label = new QLabel( i18n("Hours:"), grid );
    d->hours       = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 0, 0 );
    gridLay->addWidget( d->hours, 0, 1 );

    label      = new QLabel( i18n("Minutes:"), grid );
    d->minutes = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 0, 3 );
    gridLay->addWidget( d->minutes, 0, 4 );

    label   = new QLabel( i18n("Seconds:"), grid );
    d->secs = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 0, 6 );
    gridLay->addWidget( d->secs, 0, 7 );

    label   = new QLabel( i18n("Days:"), grid );
    d->days = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 1, 0 );
    gridLay->addWidget( d->days, 1, 1 );

    label     = new QLabel( i18n("Months:"), grid );
    d->months = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 1, 3 );
    gridLay->addWidget( d->months, 1, 4 );

    label    = new QLabel( i18n("Years:"), grid );
    d->years = new QSpinBox( 0, 1000, 1, grid );
    gridLay->addWidget( label, 1, 6 );
    gridLay->addWidget( d->years, 1, 7 );

    // -- Example ------------------------------------------------------------

    d->exampleBox = new QVGroupBox(i18n("Example"), plainPage());
    vlay->addWidget(d->exampleBox);

    d->infoLabel  = new QLabel(d->exampleBox);
    d->exampleAdj = new QLabel(d->exampleBox);
    d->exampleAdj->setAlignment(Qt::AlignCenter);

    vlay->addStretch();

    // -- Slots/Signals ------------------------------------------------------

    connect(d->adjustTypeGrp, SIGNAL( clicked(int) ),
            this, SLOT( slotAdjustmentTypeChanged() ));

    connect(d->secs, SIGNAL( valueChanged( int ) ), 
            this, SLOT( slotUpdateExample() ));

    connect(d->minutes, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    connect(d->hours, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    connect(d->days, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    connect(d->months, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    connect(d->years, SIGNAL( valueChanged( int ) ),
            this, SLOT( slotUpdateExample() ));

    connect(d->todayBtn, SIGNAL(clicked()),
            this, SLOT(slotResetDateToCurrent()));

    // -----------------------------------------------------------------------

    readSettings();
    slotAdjustmentTypeChanged();
}

TimeAdjustDialog::~TimeAdjustDialog()
{
    delete d->about;
    delete d;
}

void TimeAdjustDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("timeadjust", "kipi-plugins");
}

void TimeAdjustDialog::slotResetDateToCurrent()
{
    d->dateCreatedSel->setDateTime(QDateTime::currentDateTime());
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

    int adjType = config.readNumEntry("Adjustment Type", 0);   // add by default.
    if (adjType == 0) d->add->setChecked(true);
    if (adjType == 1) d->subtract->setChecked(true);
    if (adjType == 2) d->exif->setChecked(true);
    if (adjType == 3) d->custom->setChecked(true);

    QDateTime current = QDateTime::currentDateTime();
    d->dateCreatedSel->setDateTime(config.readDateTimeEntry("Custom Date", &current));

    d->syncEXIFDateCheck->setChecked(config.readBoolEntry("Sync EXIF Date", true));
    d->syncIPTCDateCheck->setChecked(config.readBoolEntry("Sync IPTC Date", true));
    resize(configDialogSize(config, QString("Time Adjust Dialog")));
}

void TimeAdjustDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("Time Adjust Settings");

    int adjType = 0;              // add
    if (d->subtract->isChecked()) adjType = 1;
    if (d->exif->isChecked())     adjType = 2;
    if (d->custom->isChecked())   adjType = 3;
    config.writeEntry("Adjustment Type", adjType);

    config.writeEntry("Custom Date", d->dateCreatedSel->dateTime());

    config.writeEntry("Sync EXIF Date", d->syncEXIFDateCheck->isChecked());
    config.writeEntry("Sync IPTC Date", d->syncIPTCDateCheck->isChecked());
    saveDialogSize(config, QString("Time Adjust Dialog"));
    config.sync();
}

void TimeAdjustDialog::setImages(const KURL::List& images)
{
    d->images.clear();
    int exactCount   = 0;
    int inexactCount = 0;

    for( KURL::List::ConstIterator it = images.begin(); it != images.end(); ++it ) 
    {
        KIPI::ImageInfo info = d->interface->info( *it );
        if (info.isTimeExact()) 
        {
            exactCount++;
            d->exampleDate = info.time();
            d->images.append(*it);
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

        d->infoLabel->setText(tmpLabel);
    }
    else 
    {
        d->infoLabel->setText(i18n("1 image will be changed",
                                  "%n images will be changed",
                                  d->images.count()));
    }
    // PENDING(blackie) handle all images being inexact.

    slotUpdateExample();
}

void TimeAdjustDialog::slotUpdateExample()
{
    QString oldDate = d->exampleDate.toString(Qt::LocalDate);
    QDateTime date  = updateTime(KURL(), d->exampleDate);
    QString newDate = date.toString(Qt::LocalDate);
    d->exampleAdj->setText(i18n("<b>%1</b><br>would, for example, "
                               "change into<br><b>%2</b>")
                           .arg(oldDate).arg(newDate));
}

void TimeAdjustDialog::slotAdjustmentTypeChanged()
{
    d->exampleBox->setEnabled(false);
    d->adjustValGrp->setEnabled(false);
    d->dateCreatedSel->setEnabled(false);
    d->todayBtn->setEnabled(false);
    d->syncEXIFDateCheck->setEnabled(false);
    d->syncIPTCDateCheck->setEnabled(false);

    if (d->add->isChecked() || d->subtract->isChecked())
    {
        d->exampleBox->setEnabled(true);
        d->adjustValGrp->setEnabled(true);
        d->syncEXIFDateCheck->setEnabled(true);
        d->syncIPTCDateCheck->setEnabled(true);
    }
    else if (d->custom->isChecked())
    {
        d->dateCreatedSel->setEnabled(true);
        d->todayBtn->setEnabled(true);
        d->syncEXIFDateCheck->setEnabled(true);
        d->syncIPTCDateCheck->setEnabled(true);
    }
}

void TimeAdjustDialog::slotOk()
{
    KURL::List  updatedURLs;
    QStringList errorFiles;

    for( KURL::List::ConstIterator it = d->images.begin(); it != d->images.end(); ++it )
    {
        KURL url             = *it;
        KIPI::ImageInfo info = d->interface->info(url);
        QDateTime dateTime   = info.time();
        dateTime             = updateTime(info.path(), info.time());
        info.setTime(dateTime);

        // See B.K.O #138880: set the file acess and modification time.
        struct utimbuf ut;
        ut.modtime = dateTime.toTime_t();
        ut.actime  = dateTime.toTime_t();
        ::utime(QFile::encodeName(url.path()), &ut);

        if (!d->exif->isChecked())
        {
            if (d->syncEXIFDateCheck->isChecked() || d->syncIPTCDateCheck->isChecked())
            {
                bool ret = false;
                if (!KExiv2Iface::KExiv2::isReadOnly(url.path()))
                {
                    KExiv2Iface::KExiv2 exiv2Iface;
    
                    ret &= exiv2Iface.load(url.path());
                    if (ret)
                    {
                        if (d->syncEXIFDateCheck->isChecked())
                        {
                            ret &= exiv2Iface.setExifTagString("Exif.Image.DateTime",
                                dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).ascii());
                        }
            
                        if (d->syncIPTCDateCheck->isChecked())
                        {
                            ret &= exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                                dateTime.date().toString(Qt::ISODate));
                            ret &= exiv2Iface.setIptcTagString("Iptc.Application2.TimeCreated",
                                dateTime.time().toString(Qt::ISODate));
                        }
        
                        ret &= exiv2Iface.save(url.path());
                    }
                    else
                    {
                       kdDebug() << "Failed to load metadata from file " << url.fileName() << endl;
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
    d->interface->refreshImages(d->images);

    if (!errorFiles.isEmpty() && !d->exif->isChecked())
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
    if (d->custom->isChecked())
    {
        return d->dateCreatedSel->dateTime();
    }
    else if (d->exif->isChecked())
    {
        KExiv2Iface::KExiv2 exiv2Iface;
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
        if (d->add->isChecked())
            sign = 1;

        QDateTime newTime = time.addSecs( sign * ( d->secs->value()
                                                   + 60*d->minutes->value()
                                                   + 60*60*d->hours->value()
                                                   + 24*60*60*d->days->value() ) );
        newTime = newTime.addMonths( sign * d->months->value() );
        newTime = newTime.addYears( sign * d->years->value() );
        return newTime;
    }
}

}  // NameSpace KIPITimeAdjustPlugin
