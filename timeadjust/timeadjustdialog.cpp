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

#include <Q3VButtonGroup>
#include <Q3ButtonGroup>
#include <Q3VGroupBox>
#include <Q3Grid>
#include <Q3HBox>
#include <QToolTip>
#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QFile>
#include <QSpinBox>
#include <QPushButton>
#include <QFrame>
#include <QToolButton>
#include <QCloseEvent>

// KDE includes.

#include <kvbox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kiconloader.h>
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
        todayBtn          = 0;
    }

    QRadioButton             *add;
    QRadioButton             *subtract;
    QRadioButton             *exif;
    QRadioButton             *custom;

    QToolButton              *todayBtn;

    QCheckBox                *syncEXIFDateCheck;
    QCheckBox                *syncIPTCDateCheck;

    Q3VGroupBox              *exampleBox;
    Q3VButtonGroup           *adjustValGrp;
    Q3ButtonGroup            *adjustTypeGrp;
    
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

    KUrl::List                images;

    KIPI::Interface          *interface;
};

TimeAdjustDialog::TimeAdjustDialog(KIPI::Interface* interface, QWidget* parent)
                : KDialog(parent)
{
    d = new TimeAdjustDialogPrivate;
    d->interface = interface;

    setButtons(KDialog::Help | KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    setCaption(i18n("Adjust Time & Date"));
    setHelp("timeadjust", "kipi-plugins");

    KVBox *box = new KVBox( this );
    setMainWidget( box );
    QVBoxLayout *vlay = new QVBoxLayout(box);

    // -- Adjustment type ------------------------------------------------------------

    Q3VGroupBox *adjGB = new Q3VGroupBox(i18n("Adjustment Type"), box);
    d->adjustTypeGrp   = new Q3ButtonGroup(1, Qt::Horizontal, adjGB);
    d->add             = new QRadioButton(i18n("Add"), d->adjustTypeGrp);
    d->subtract        = new QRadioButton(i18n("Subtract"), d->adjustTypeGrp);
    d->exif            = new QRadioButton(i18n("Set file date to EXIF/IPTC creation date"), d->adjustTypeGrp);
    d->custom          = new QRadioButton(i18n("Custom date"), d->adjustTypeGrp);

    d->adjustTypeGrp->setFrameStyle(QFrame::NoFrame);
    d->adjustTypeGrp->setInsideMargin(0); 
    d->adjustTypeGrp->setRadioButtonExclusive(true);

    Q3HBox *hbox       = new Q3HBox(d->adjustTypeGrp);
    QLabel *space1     = new QLabel(hbox);
    space1->setFixedWidth(15);
    d->dateCreatedSel  = new KDateTimeWidget(hbox);
    QLabel *space2     = new QLabel(hbox);
    space2->setFixedWidth(15);
    d->todayBtn        = new QToolButton(hbox);   
    d->todayBtn->setIcon(SmallIcon("today"));
    d->todayBtn->setToolTip(i18n("Reset to current date"));
    new QLabel(hbox);
    
    d->syncEXIFDateCheck = new QCheckBox(i18n("Update Exif creation date"), d->adjustTypeGrp);
    d->syncIPTCDateCheck = new QCheckBox(i18n("Update IPTC creation date"), d->adjustTypeGrp);

    vlay->addWidget(adjGB);

    // -- Adjustments ------------------------------------------------------------

    d->adjustValGrp = new Q3VButtonGroup(i18n("Adjustments"), box);
    vlay->addWidget(d->adjustValGrp);

    QWidget* grid        = new QWidget(d->adjustValGrp);
    QGridLayout* gridLay = new QGridLayout(grid);
    gridLay->setSpacing(spacingHint());
    gridLay->setColumnStretch( 2, 1 );
    gridLay->setColumnStretch( 5, 1 );

    QLabel* label = new QLabel( i18n("Hours:"), grid );
    d->hours      = new QSpinBox(grid);
    d->hours->setRange(0, 1000);
    d->hours->setSingleStep(1);
    gridLay->addWidget( label, 0, 0 );
    gridLay->addWidget( d->hours, 0, 1 );

    label      = new QLabel( i18n("Minutes:"), grid );
    d->minutes = new QSpinBox(grid);
    d->minutes->setRange(0, 1000);
    d->minutes->setSingleStep(1);
    gridLay->addWidget( label, 0, 3 );
    gridLay->addWidget( d->minutes, 0, 4 );

    label   = new QLabel( i18n("Seconds:"), grid );
    d->secs = new QSpinBox(grid);
    d->secs->setRange(0, 1000);
    d->secs->setSingleStep(1);
    gridLay->addWidget( label, 0, 6 );
    gridLay->addWidget( d->secs, 0, 7 );

    label   = new QLabel( i18n("Days:"), grid );
    d->days = new QSpinBox(grid);
    d->days->setRange(0, 1000);
    d->days->setSingleStep(1);
    gridLay->addWidget( label, 1, 0 );
    gridLay->addWidget( d->days, 1, 1 );

    label     = new QLabel( i18n("Months:"), grid );
    d->months = new QSpinBox(grid);
    d->months->setRange(0, 1000);
    d->months->setSingleStep(1);
    gridLay->addWidget( label, 1, 3 );
    gridLay->addWidget( d->months, 1, 4 );

    label    = new QLabel( i18n("Years:"), grid );
    d->years = new QSpinBox(grid);
    d->years->setRange(0, 1000);
    d->years->setSingleStep(1);
    gridLay->addWidget( label, 1, 6 );
    gridLay->addWidget( d->years, 1, 7 );

    // -- Example ------------------------------------------------------------

    d->exampleBox = new Q3VGroupBox(i18n("Example"), box);
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

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    // -----------------------------------------------------------------------

    readSettings();
    slotAdjustmentTypeChanged();
}

TimeAdjustDialog::~TimeAdjustDialog()
{
    delete d;
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
    reject();
}

void TimeAdjustDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Time Adjust Settings"));

    int adjType = group.readEntry("Adjustment Type", 0);   // add by default.
    if (adjType == 0) d->add->setChecked(true);
    if (adjType == 1) d->subtract->setChecked(true);
    if (adjType == 2) d->exif->setChecked(true);
    if (adjType == 3) d->custom->setChecked(true);

    d->dateCreatedSel->setDateTime(group.readEntry("Custom Date", QDateTime::currentDateTime()));

    d->syncEXIFDateCheck->setChecked(group.readEntry("Sync EXIF Date", true));
    d->syncIPTCDateCheck->setChecked(group.readEntry("Sync IPTC Date", true));

    KConfigGroup group2 = config.group(QString("Time Adjust Dialog"));
    restoreDialogSize(group2);
}

void TimeAdjustDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Time Adjust Settings"));

    int adjType = 0;              // add
    if (d->subtract->isChecked()) adjType = 1;
    if (d->exif->isChecked())     adjType = 2;
    if (d->custom->isChecked())   adjType = 3;
    group.writeEntry("Adjustment Type", adjType);

    group.writeEntry("Custom Date", d->dateCreatedSel->dateTime());

    group.writeEntry("Sync EXIF Date", d->syncEXIFDateCheck->isChecked());
    group.writeEntry("Sync IPTC Date", d->syncIPTCDateCheck->isChecked());

    KConfigGroup group2 = config.group(QString("Time Adjust Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void TimeAdjustDialog::setImages(const KUrl::List& images)
{
    d->images.clear();
    int exactCount   = 0;
    int inexactCount = 0;

    for( KUrl::List::ConstIterator it = images.begin(); it != images.end(); ++it ) 
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
        QString tmpLabel = i18np("1 image will be changed; ",
                                 "%n images will be changed; ",
                                 exactCount)
                         + i18np("1 image will be skipped due to an inexact date.",
                                 "%n images will be skipped due to inexact dates.",
                                 inexactCount );

        d->infoLabel->setText(tmpLabel);
    }
    else 
    {
        d->infoLabel->setText(i18np("1 image will be changed",
                                    "%n images will be changed",
                                    d->images.count()));
    }
    // PENDING(blackie) handle all images being inexact.

    slotUpdateExample();
}

void TimeAdjustDialog::slotUpdateExample()
{
    QString oldDate = d->exampleDate.toString(Qt::LocalDate);
    QDateTime date  = updateTime(KUrl(), d->exampleDate);
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
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::ConstIterator it = d->images.begin(); it != d->images.end(); ++it )
    {
        KUrl url             = *it;
        KIPI::ImageInfo info = d->interface->info(url);
        QDateTime dateTime   = info.time();
        dateTime             = updateTime(info.path(), info.time());
        info.setTime(dateTime);

        // See B.K.O #138880: set the file acess and modification time.
        struct utimbuf ut;
        ut.modtime = dateTime.toTime_t();
        ut.actime  = dateTime.toTime_t();
        ::utime(QFile::encodeName(url.path()), &ut);

        if (d->syncEXIFDateCheck->isChecked() || d->syncIPTCDateCheck->isChecked())
        {
            bool ret = true;
            if (!KExiv2Iface::KExiv2::isReadOnly(url.path()))
            {
                KExiv2Iface::KExiv2 exiv2Iface;

                ret &= exiv2Iface.load(url.path());
                if (ret)
                {
                    if (d->syncEXIFDateCheck->isChecked())
                    {
                        ret &= exiv2Iface.setExifTagString("Exif.Image.DateTime",
                            dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    }
        
                    if (d->syncIPTCDateCheck->isChecked())
                    {
                        ret &= exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                            dateTime.date().toString(Qt::ISODate));
                        ret &= exiv2Iface.setIptcTagString("Iptc.Application2.TimeCreated",
                            dateTime.time().toString(Qt::ISODate));
                    }
    
                    ret &= exiv2Iface.save(url.path());

                    if (!ret)
                    {
                        kDebug() << "Failed to save metadata to file " << url.fileName() << endl;
                    }
                }
                else
                {
                    kDebug() << "Failed to load metadata from file " << url.fileName() << endl;
                }
            }
    
            if (!ret)
                errorFiles.append(url.fileName());
            else 
                updatedURLs.append(url);
        }
    }

    // We use kipi interface refreshImages() method to tell to host than 
    // metadata from pictures have changed and need to be re-read.
    d->interface->refreshImages(d->images);

    if (!errorFiles.isEmpty() && (d->syncEXIFDateCheck->isChecked() || d->syncIPTCDateCheck->isChecked()))
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

QDateTime TimeAdjustDialog::updateTime(const KUrl& url, const QDateTime& time) const
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
