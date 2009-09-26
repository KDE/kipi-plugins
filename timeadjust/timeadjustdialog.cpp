/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "timeadjustdialog.h"
#include "timeadjustdialog.moc"

// C ANSI includes

extern "C"
{
#include <utime.h>
}

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QPointer>

// KDE includes

#include <kaboutdata.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdatetimewidget.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kvbox.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// Local includes

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "clockphotodialog.h"

namespace KIPITimeAdjustPlugin
{

class TimeAdjustDialogPrivate
{

public:

    TimeAdjustDialogPrivate()
    {
        adjGB             = 0;
        adjTypeGB         = 0;
        add               = 0;
        subtract          = 0;
        exif              = 0;
        custom            = 0;
        syncEXIFDateCheck = 0;
        syncIPTCDateCheck = 0;
        syncXMPDateCheck  = 0;
        exampleBox        = 0;
        adjustTypeGrp     = 0;
        infoLabel         = 0;
        exampleAdj        = 0;
        secs              = 0;
        minutes           = 0;
        hours             = 0;
        days              = 0;
        months            = 0;
        years             = 0;
        clockPhotoBtn     = 0;
        dateCreatedSel    = 0;
        interface         = 0;
        todayBtn          = 0;
        about             = 0;
    }

    QRadioButton             *add;
    QRadioButton             *subtract;
    QRadioButton             *exif;
    QRadioButton             *custom;

    QToolButton              *todayBtn;

    QCheckBox                *syncEXIFDateCheck;
    QCheckBox                *syncIPTCDateCheck;
    QCheckBox                *syncXMPDateCheck;

    QGroupBox                *exampleBox;
    QGroupBox                *adjGB;
    QGroupBox                *adjTypeGB;

    QButtonGroup             *adjustTypeGrp;

    QLabel                   *infoLabel;
    QLabel                   *exampleAdj;

    QSpinBox                 *secs;
    QSpinBox                 *minutes;
    QSpinBox                 *hours;
    QSpinBox                 *days;
    QSpinBox                 *months;
    QSpinBox                 *years;
    QPushButton              *clockPhotoBtn;

    QDateTime                 exampleDate;

    KDateTimeWidget          *dateCreatedSel;

    KUrl::List                images;

    KIPI::Interface          *interface;

    KIPIPlugins::KPAboutData *about;
};

TimeAdjustDialog::TimeAdjustDialog(KIPI::Interface* interface, QWidget* parent)
                : KDialog(parent), d(new TimeAdjustDialogPrivate)
{
    d->interface = interface;

    setButtons(Help | Ok | Cancel);
    setDefaultButton(Ok);
    setCaption(i18n("Adjust Time & Date"));
    setModal(true);

    setMainWidget(new QWidget(this));
    QVBoxLayout *vlay = new QVBoxLayout(mainWidget());

    // -- About data and help button ----------------------------------------

    d->about = new KIPIPlugins::KPAboutData(ki18n("Time Adjust"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin for adjusting the timestamp of picture files"),
                   ki18n("(c) 2003-2005, Jesper K. Pedersen\n"
                         "(c) 2006-2009, Gilles Caulier"));

    d->about->addAuthor(ki18n("Jesper K. Pedersen"),
                        ki18n("Author"),
                        "blackie at kde dot org");

    d->about->addAuthor(ki18n("Gilles Caulier"),
                        ki18n("Developer and maintainer"),
                        "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // -- Adjustment type ------------------------------------------------------------

    d->adjTypeGB       = new QGroupBox(i18n("Adjustment Type"), mainWidget());
    QVBoxLayout *vlay2 = new QVBoxLayout(d->adjTypeGB);
    d->adjustTypeGrp   = new QButtonGroup(d->adjTypeGB);
    d->adjustTypeGrp->setExclusive(true);

    d->add            = new QRadioButton(i18n("Add"), d->adjTypeGB);
    d->subtract       = new QRadioButton(i18n("Subtract"), d->adjTypeGB);
    d->exif           = new QRadioButton(i18n("Set file date to EXIF/IPTC/XMP creation date"), d->adjTypeGB);
    d->custom         = new QRadioButton(i18n("Custom date"), d->adjTypeGB);

    d->adjustTypeGrp->addButton(d->add, 0);
    d->adjustTypeGrp->addButton(d->subtract, 1);
    d->adjustTypeGrp->addButton(d->exif, 2);
    d->adjustTypeGrp->addButton(d->custom, 3);

    KHBox *hbox       = new KHBox(d->adjTypeGB);
    QLabel *space1    = new QLabel(hbox);
    space1->setFixedWidth(15);
    d->dateCreatedSel  = new KDateTimeWidget(hbox);
    QLabel *space2     = new QLabel(hbox);
    space2->setFixedWidth(15);
    d->todayBtn        = new QToolButton(hbox);
    d->todayBtn->setIcon(SmallIcon("go-jump-today"));
    d->todayBtn->setToolTip(i18n("Reset to current date"));
    new QLabel(hbox);

    d->syncEXIFDateCheck = new QCheckBox(i18n("Update EXIF creation date"), d->adjTypeGB);
    d->syncIPTCDateCheck = new QCheckBox(i18n("Update IPTC creation date"), d->adjTypeGB);
    d->syncXMPDateCheck  = new QCheckBox(i18n("Update XMP creation date"), d->adjTypeGB);

    if (!KExiv2Iface::KExiv2::supportXmp())
        d->syncXMPDateCheck->setEnabled(false);

    vlay2->setMargin(spacingHint());
    vlay2->setSpacing(spacingHint());
    vlay2->addWidget(d->clockPhotoBtn);
    vlay2->addWidget(d->add);
    vlay2->addWidget(d->subtract);
    vlay2->addWidget(d->exif);
    vlay2->addWidget(d->custom);
    vlay2->addWidget(hbox);
    vlay2->addWidget(d->syncEXIFDateCheck);
    vlay2->addWidget(d->syncIPTCDateCheck);
    vlay2->addWidget(d->syncXMPDateCheck);

    // -- Adjustments ------------------------------------------------------------

    d->adjGB             = new QGroupBox(i18n("Adjustments"), mainWidget());
    QGridLayout* gridLay = new QGridLayout(d->adjGB);

    QLabel* label1 = new QLabel( i18n("Hours:"), d->adjGB);
    d->hours       = new QSpinBox(d->adjGB);
    d->hours->setRange(0, 1000);
    d->hours->setSingleStep(1);

    QLabel* label2 = new QLabel( i18n("Minutes:"), d->adjGB);
    d->minutes     = new QSpinBox(d->adjGB);
    d->minutes->setRange(0, 1000);
    d->minutes->setSingleStep(1);

    QLabel* label3 = new QLabel( i18n("Seconds:"), d->adjGB);
    d->secs        = new QSpinBox(d->adjGB);
    d->secs->setRange(0, 1000);
    d->secs->setSingleStep(1);

    QLabel* label4 = new QLabel( i18n("Days:"), d->adjGB);
    d->days        = new QSpinBox(d->adjGB);
    d->days->setRange(0, 1000);
    d->days->setSingleStep(1);

    QLabel* label5 = new QLabel( i18n("Months:"), d->adjGB);
    d->months      = new QSpinBox(d->adjGB);
    d->months->setRange(0, 1000);
    d->months->setSingleStep(1);

    QLabel* label6 = new QLabel( i18n("Years:"), d->adjGB );
    d->years       = new QSpinBox(d->adjGB);
    d->years->setRange(0, 1000);
    d->years->setSingleStep(1);

    d->clockPhotoBtn = new QPushButton(i18n("Determine from clock photo"));

    gridLay->setMargin(spacingHint());
    gridLay->setSpacing(spacingHint());
    gridLay->setColumnStretch(2, 1);
    gridLay->setColumnStretch(5, 1);
    gridLay->addWidget(label1,           0, 0, 1, 1);
    gridLay->addWidget(d->hours,         0, 1, 1, 1);
    gridLay->addWidget(label2,           0, 3, 1, 1);
    gridLay->addWidget(d->minutes,       0, 4, 1, 1);
    gridLay->addWidget(label3,           0, 6, 1, 1);
    gridLay->addWidget(d->secs,          0, 7, 1, 1);
    gridLay->addWidget(label4,           1, 0, 1, 1);
    gridLay->addWidget(d->days,          1, 1, 1, 1);
    gridLay->addWidget(label5,           1, 3, 1, 1);
    gridLay->addWidget(d->months,        1, 4, 1, 1);
    gridLay->addWidget(label6,           1, 6, 1, 1);
    gridLay->addWidget(d->years,         1, 7, 1, 1);
    gridLay->addWidget(d->clockPhotoBtn, 2, 0, 1, 8);

    // -- Example ------------------------------------------------------------

    d->exampleBox      = new QGroupBox(i18n("Example"), mainWidget());
    QVBoxLayout *vlay3 = new QVBoxLayout(d->exampleBox);

    d->infoLabel  = new QLabel(d->exampleBox);
    d->exampleAdj = new QLabel(d->exampleBox);
    d->exampleAdj->setAlignment(Qt::AlignCenter);

    vlay3->setMargin(spacingHint());
    vlay3->setSpacing(spacingHint());
    vlay3->addWidget(d->infoLabel);
    vlay3->addWidget(d->exampleAdj);

    // -----------------------------------------------------------------------

    vlay->setMargin(0);
    vlay->setSpacing(spacingHint());
    vlay->addWidget(d->adjTypeGB);
    vlay->addWidget(d->adjGB);
    vlay->addWidget(d->exampleBox);
    vlay->addStretch();

    // -- Slots/Signals ------------------------------------------------------

    connect(d->adjustTypeGrp, SIGNAL( buttonReleased(int) ),
            this, SLOT( slotAdjustmentTypeChanged() ));

    connect(d->adjustTypeGrp, SIGNAL( buttonClicked( int ) ),
            this, SLOT( slotUpdateExample() ));

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

    connect(d->clockPhotoBtn, SIGNAL( clicked() ),
            this, SLOT( slotClockPhoto() ));

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
    delete d->about;
    delete d;
}

void TimeAdjustDialog::slotHelp()
{
    KToolInvocation::invokeHelp("timeadjust", "kipi-plugins");
}

void TimeAdjustDialog::slotClockPhoto()
{
    /* When the use presses the clock photo button, present a dialog and set the
     * results to the proper widgets. */

    QPointer<ClockPhotoDialog> dilg = new ClockPhotoDialog(d->interface, this);
    int result = dilg->exec();
    if (result == QDialog::Accepted)
    {
        if (dilg->deltaNegative)
        {
            d->subtract->setChecked(true);
        } else
        {
            d->add->setChecked(true);
        }
        d->days->setValue(dilg->deltaDays);
        d->hours->setValue(dilg->deltaHours);
        d->minutes->setValue(dilg->deltaMinutes);
        d->secs->setValue(dilg->deltaSeconds);
    }

    delete dilg;
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
    d->syncXMPDateCheck->setChecked(group.readEntry("Sync XMP Date", true));

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
    group.writeEntry("Sync XMP Date", d->syncXMPDateCheck->isChecked());

    KConfigGroup group2 = config.group(QString("Time Adjust Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void TimeAdjustDialog::setImages(const KUrl::List& images)
{
    d->images.clear();
    int exactCount   = 0;
    int inexactCount = 0;

    for( KUrl::List::ConstIterator it = images.constBegin(); it != images.constEnd(); ++it )
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
                                 "%1 images will be changed; ",
                                 exactCount)
                         + i18np("1 image will be skipped due to an inexact date.",
                                 "%1 images will be skipped due to inexact dates.",
                                 inexactCount);

        d->infoLabel->setText(tmpLabel);
    }
    else
    {
        d->infoLabel->setText(i18np("1 image will be changed",
                                    "%1 images will be changed",
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
    d->exampleAdj->setText(i18n("<b>%1</b><br/>would, for example, "
                                "change into<br/><b>%2</b>",
                                oldDate, newDate));
}

void TimeAdjustDialog::slotAdjustmentTypeChanged()
{
    d->exampleBox->setEnabled(false);
    d->adjGB->setEnabled(false);
    d->dateCreatedSel->setEnabled(false);
    d->todayBtn->setEnabled(false);
    d->syncEXIFDateCheck->setEnabled(false);
    d->syncIPTCDateCheck->setEnabled(false);
    d->syncXMPDateCheck->setEnabled(false);

    if (d->add->isChecked() || d->subtract->isChecked())
    {
        d->exampleBox->setEnabled(true);
        d->adjGB->setEnabled(true);
        d->syncEXIFDateCheck->setEnabled(true);
        d->syncIPTCDateCheck->setEnabled(true);
        d->syncXMPDateCheck->setEnabled(true);
    }
    else if (d->custom->isChecked())
    {
        d->dateCreatedSel->setEnabled(true);
        d->todayBtn->setEnabled(true);
        d->syncEXIFDateCheck->setEnabled(true);
        d->syncIPTCDateCheck->setEnabled(true);
        d->syncXMPDateCheck->setEnabled(true);
    }
}

void TimeAdjustDialog::slotOk()
{
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::ConstIterator it = d->images.constBegin(); it != d->images.constEnd(); ++it )
    {
        KUrl url             = *it;
        KIPI::ImageInfo info = d->interface->info(url);
        QDateTime dateTime   = info.time();
        dateTime             = updateTime(info.path(), info.time());
        info.setTime(dateTime);

        if (d->syncEXIFDateCheck->isChecked() || d->syncIPTCDateCheck->isChecked() ||
            d->syncXMPDateCheck->isChecked())
        {
            bool ret = true;

            KExiv2Iface::KExiv2 exiv2Iface;
            exiv2Iface.setWriteRawFiles(d->interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
            exiv2Iface.setUpdateFileTimeStamp(d->interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

            ret &= exiv2Iface.load(url.path());
            if (ret)
            {
                if (d->syncEXIFDateCheck->isChecked() && exiv2Iface.canWriteExif(url.path()))
                {
                    ret &= exiv2Iface.setExifTagString("Exif.Image.DateTime",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= exiv2Iface.setExifTagString("Exif.Photo.DateTimeOriginal",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= exiv2Iface.setExifTagString("Exif.Photo.DateTimeDigitized",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                if (d->syncIPTCDateCheck->isChecked() && exiv2Iface.canWriteIptc(url.path()))
                {
                    ret &= exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                        dateTime.date().toString(Qt::ISODate));
                    ret &= exiv2Iface.setIptcTagString("Iptc.Application2.TimeCreated",
                        dateTime.time().toString(Qt::ISODate));
                }

                if (d->syncXMPDateCheck->isChecked() && exiv2Iface.supportXmp() &&
                    exiv2Iface.canWriteXmp(url.path()))
                {
                    ret &= exiv2Iface.setXmpTagString("Xmp.exif.DateTimeOriginal",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= exiv2Iface.setXmpTagString("Xmp.photoshop.DateCreated",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= exiv2Iface.setXmpTagString("Xmp.tiff.DateTime",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= exiv2Iface.setXmpTagString("Xmp.xmp.CreateDate",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= exiv2Iface.setXmpTagString("Xmp.xmp.MetadataDate",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    ret &= exiv2Iface.setXmpTagString("Xmp.xmp.ModifyDate",
                        dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                }

                ret &= exiv2Iface.save(url.path());

                if (!ret)
                {
                    kDebug(51000) << "Failed to save metadata to file " << url.fileName();
                }
            }
            else
            {
                kDebug(51000) << "Failed to load metadata from file " << url.fileName();
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

}  // namespace KIPITimeAdjustPlugin
