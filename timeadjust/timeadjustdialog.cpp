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
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QTimeEdit>
#include <QComboBox>
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
        useGroupBox            = 0;
        adjustGroupBox         = 0;
        updateGroupBox         = 0;
        exampleGroupBox        = 0;
        useButtonGroup         = 0;
        useApplDateBtn         = 0;
        useFileDateBtn         = 0;
        useMetaDateBtn         = 0;
        useCustomDateBtn       = 0;
        updAppDateCheck        = 0;
        updFileModDateCheck    = 0;
        updEXIFModDateCheck    = 0;
        updEXIFOriDateCheck    = 0;
        updEXIFDigDateCheck    = 0;
        updIPTCDateCheck       = 0;
        updXMPDateCheck        = 0;
        useFileDateTypeChooser = 0;
        useMetaDateTypeChooser = 0;
        adjTypeChooser         = 0;
        exampleFileChooser     = 0;
        adjDaysLabel           = 0;
        exampleSummaryLabel    = 0;
        exampleTimeChangeLabel = 0;
        adjDaysInput           = 0;
        adjDetByClockPhotoBtn  = 0;
        useCustDateInput       = 0;
        useCustTimeInput       = 0;
        adjTimeInput           = 0;
        useCustomDateTodayBtn  = 0;
        interface              = 0;
        about                  = 0;
    }

    QGroupBox                *useGroupBox;
    QGroupBox                *adjustGroupBox;
    QGroupBox                *updateGroupBox;
    QGroupBox                *exampleGroupBox;

    QButtonGroup             *useButtonGroup;

    QRadioButton             *useApplDateBtn;
    QRadioButton             *useFileDateBtn;
    QRadioButton             *useMetaDateBtn;
    QRadioButton             *useCustomDateBtn;

    QCheckBox                *updAppDateCheck;
    QCheckBox                *updFileModDateCheck;
    QCheckBox                *updEXIFModDateCheck;
    QCheckBox                *updEXIFOriDateCheck;
    QCheckBox                *updEXIFDigDateCheck;
    QCheckBox                *updIPTCDateCheck;
    QCheckBox                *updXMPDateCheck;

    QComboBox                *useFileDateTypeChooser;
    QComboBox                *useMetaDateTypeChooser;
    QComboBox                *adjTypeChooser;
    QComboBox                *exampleFileChooser;

    QLabel                   *adjDaysLabel;
    QLabel                   *exampleSummaryLabel;
    QLabel                   *exampleTimeChangeLabel;

    QSpinBox                 *adjDaysInput;

    QPushButton              *adjDetByClockPhotoBtn;

    QDateEdit                *useCustDateInput;

    QTimeEdit                *useCustTimeInput;
    QTimeEdit                *adjTimeInput;

    QToolButton              *useCustomDateTodayBtn;

    KUrl::List                imageURLs;
    QList<QDateTime>          imageOriginalDates;

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
    QVBoxLayout *mainWidgetLayout = new QVBoxLayout(mainWidget());

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
    QAction *handbook = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // -- Use ------------------------------------------------------------

    d->useGroupBox           = new QGroupBox(i18n("Use Time && Date"), mainWidget());
    QGridLayout* useGBLayout = new QGridLayout(d->useGroupBox);

    d->useButtonGroup = new QButtonGroup(d->useGroupBox);
    d->useButtonGroup->setExclusive(true);

    QString applDateLabelString = i18n("%1 timestamp", KGlobal::mainComponent().aboutData()->programName());
    d->useApplDateBtn           = new QRadioButton("", d->useGroupBox);
    QLabel *useApplDateLbl      = new QLabel(applDateLabelString);
    useApplDateLbl->setIndent(5);

    d->useFileDateBtn         = new QRadioButton("", d->useGroupBox);
    d->useFileDateTypeChooser = new QComboBox(d->useGroupBox);
    //d->useFileDateTypeChooser->addItem(i18n("File created")); // not supported by Linux, although supported by Qt (read-only)
    d->useFileDateTypeChooser->addItem(i18n("File last modified"));

    d->useMetaDateBtn = new QRadioButton("", d->useGroupBox);
    d->useMetaDateTypeChooser = new QComboBox(d->useGroupBox);
    d->useMetaDateTypeChooser->addItem(i18n("EXIF/IPTC/XMP"));
    d->useMetaDateTypeChooser->addItem(i18n("EXIF: created"));
    d->useMetaDateTypeChooser->addItem(i18n("EXIF: original"));
    d->useMetaDateTypeChooser->addItem(i18n("EXIF: digitized"));
    d->useMetaDateTypeChooser->addItem(i18n("IPTC: created"));
    d->useMetaDateTypeChooser->addItem(i18n("XMP: created"));

    d->useCustomDateBtn = new QRadioButton(d->useGroupBox);
    d->useCustDateInput = new QDateEdit(d->useGroupBox);
    d->useCustDateInput->setDisplayFormat("dd MMMM yyyy");
    d->useCustDateInput->setCalendarPopup(true);
    d->useCustTimeInput = new QTimeEdit(d->useGroupBox);
    d->useCustTimeInput->setDisplayFormat("hh:mm:ss");
    d->useCustomDateTodayBtn = new QToolButton(d->useGroupBox);
    d->useCustomDateTodayBtn->setIcon(SmallIcon("go-jump-today"));
    d->useCustomDateTodayBtn->setToolTip(i18n("Reset to current date"));

    useGBLayout->setMargin(spacingHint());
    useGBLayout->setSpacing(spacingHint());
    useGBLayout->setColumnStretch(1, 1);
    useGBLayout->setColumnStretch(2, 1);
    useGBLayout->addWidget(d->useApplDateBtn,         0, 0, 1, 1);
    useGBLayout->addWidget(useApplDateLbl,            0, 1, 1, 1);
    useGBLayout->addWidget(d->useFileDateBtn,         1, 0, 1, 1);
    useGBLayout->addWidget(d->useFileDateTypeChooser, 1, 1, 1, 1);
    useGBLayout->addWidget(d->useMetaDateBtn,         2, 0, 1, 1);
    useGBLayout->addWidget(d->useMetaDateTypeChooser, 2, 1, 1, 1);
    useGBLayout->addWidget(d->useCustomDateBtn,       3, 0, 1, 1);
    useGBLayout->addWidget(d->useCustDateInput,       3, 1, 1, 1);
    useGBLayout->addWidget(d->useCustTimeInput,       3, 2, 1, 1);
    useGBLayout->addWidget(d->useCustomDateTodayBtn,  3, 3, 1, 1);

    d->useButtonGroup->addButton(d->useApplDateBtn,   0);
    d->useButtonGroup->addButton(d->useFileDateBtn,   1);
    d->useButtonGroup->addButton(d->useMetaDateBtn,   2);
    d->useButtonGroup->addButton(d->useCustomDateBtn, 3);
    d->useApplDateBtn->setChecked(true);


    // -- Adjust-----------------------------------------------------------

    d->adjustGroupBox           = new QGroupBox(i18n("Adjust"), mainWidget());
    QGridLayout* adjustGBLayout = new QGridLayout(d->adjustGroupBox);

    d->adjTypeChooser = new QComboBox(d->adjustGroupBox);
    d->adjTypeChooser->addItem(i18n("Copy value"));
    d->adjTypeChooser->addItem(i18n("Add"));
    d->adjTypeChooser->addItem(i18n("Subtract"));
    d->adjDaysInput = new QSpinBox(d->adjustGroupBox);
    d->adjDaysInput->setRange(0, 9999);
    d->adjDaysInput->setSingleStep(1);
    d->adjDaysLabel = new QLabel(i18n("days"), d->adjustGroupBox);
    d->adjTimeInput = new QTimeEdit(d->adjustGroupBox);
    d->adjTimeInput->setDisplayFormat("hh:mm:ss");
    d->adjDetByClockPhotoBtn = new QPushButton(i18n("Determine from clock photo"));

    adjustGBLayout->setMargin(spacingHint());
    adjustGBLayout->setSpacing(spacingHint());
    adjustGBLayout->setColumnStretch(0, 1);
    adjustGBLayout->setColumnStretch(1, 1);
    adjustGBLayout->setColumnStretch(3, 1);
    adjustGBLayout->addWidget(d->adjTypeChooser,        0, 0, 1, 1);
    adjustGBLayout->addWidget(d->adjDaysInput,          0, 1, 1, 1);
    adjustGBLayout->addWidget(d->adjDaysLabel,          0, 2, 1, 1);
    adjustGBLayout->addWidget(d->adjTimeInput,          0, 3, 1, 1);
    adjustGBLayout->addWidget(d->adjDetByClockPhotoBtn, 1, 0, 1, 4);

    // -- Update ------------------------------------------------------------

    d->updateGroupBox           = new QGroupBox(i18n("Update Time && Date"), mainWidget());
    QGridLayout* updateGBLayout = new QGridLayout(d->updateGroupBox);

    d->updAppDateCheck     = new QCheckBox(applDateLabelString, d->updateGroupBox);
    d->updFileModDateCheck = new QCheckBox(i18n("File last modified"), d->updateGroupBox);
    d->updEXIFModDateCheck = new QCheckBox(i18n("EXIF: created"), d->updateGroupBox);
    d->updEXIFOriDateCheck = new QCheckBox(i18n("EXIF: original"), d->updateGroupBox);
    d->updEXIFDigDateCheck = new QCheckBox(i18n("EXIF: digitized"), d->updateGroupBox);
    d->updIPTCDateCheck    = new QCheckBox(i18n("IPTC: created"), d->updateGroupBox);
    d->updXMPDateCheck     = new QCheckBox(i18n("XMP"), d->updateGroupBox);

    updateGBLayout->setMargin(spacingHint());
    updateGBLayout->setSpacing(spacingHint());
    updateGBLayout->setColumnStretch(0, 1);
    updateGBLayout->setColumnStretch(1, 1);
    updateGBLayout->setColumnStretch(2, 1);
    updateGBLayout->addWidget(d->updAppDateCheck,     0, 0, 1, 1);
    updateGBLayout->addWidget(d->updFileModDateCheck, 1, 0, 1, 1);
    updateGBLayout->addWidget(d->updEXIFModDateCheck, 0, 1, 1, 1);
    updateGBLayout->addWidget(d->updEXIFOriDateCheck, 1, 1, 1, 1);
    updateGBLayout->addWidget(d->updEXIFDigDateCheck, 2, 1, 1, 1);
    updateGBLayout->addWidget(d->updIPTCDateCheck,    2, 0, 1, 1);
    updateGBLayout->addWidget(d->updXMPDateCheck,     0, 2, 1, 1);

    if (!KExiv2Iface::KExiv2::supportXmp())
    {
        d->updXMPDateCheck->setEnabled(false);
    }

    // -- Example ------------------------------------------------------------

    d->exampleGroupBox           = new QGroupBox(i18n("Example"), mainWidget());
    QVBoxLayout *exampleGBLayout = new QVBoxLayout(d->exampleGroupBox);

    d->exampleSummaryLabel    = new QLabel(d->exampleGroupBox);
    d->exampleFileChooser     = new QComboBox(d->exampleGroupBox);
    d->exampleTimeChangeLabel = new QLabel(d->exampleGroupBox);
    //d->exampleTimeChangeLabel->setAlignment(Qt::AlignCenter);

    exampleGBLayout->setMargin(spacingHint());
    exampleGBLayout->setSpacing(spacingHint());
    exampleGBLayout->addWidget(d->exampleSummaryLabel);
    exampleGBLayout->addWidget(d->exampleFileChooser);
    exampleGBLayout->addWidget(d->exampleTimeChangeLabel);

    // -----------------------------------------------------------------------

    mainWidgetLayout->setMargin(0);
    mainWidgetLayout->setSpacing(spacingHint());
    mainWidgetLayout->addWidget(d->useGroupBox);
    mainWidgetLayout->addWidget(d->adjustGroupBox);
    mainWidgetLayout->addWidget(d->updateGroupBox);
    mainWidgetLayout->addWidget(d->exampleGroupBox);
    mainWidgetLayout->addStretch();

    // -- Slots/Signals ------------------------------------------------------

    connect(d->useButtonGroup, SIGNAL(buttonReleased(int)),
            this, SLOT(slotSrcTimestampChanged()));

    connect(d->useFileDateTypeChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSrcTimestampChanged()));

    connect(d->useMetaDateTypeChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSrcTimestampChanged()));

    connect(d->useCustDateInput, SIGNAL(dateChanged(const QDate&)),
            this, SLOT(slotUpdateExample()));

    connect(d->useCustTimeInput, SIGNAL(timeChanged (const QTime&)),
            this, SLOT(slotUpdateExample()));

    connect(d->adjTypeChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotAdjustmentTypeChanged()));

    connect(d->adjDaysInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotUpdateExample()));

    connect(d->adjTimeInput, SIGNAL(timeChanged(const QTime&)),
            this, SLOT(slotUpdateExample()));

    connect(d->useCustomDateTodayBtn, SIGNAL(clicked()),
            this, SLOT(slotResetDateToCurrent()));

    connect(d->adjDetByClockPhotoBtn, SIGNAL(clicked()),
            this, SLOT(slotDetAdjustmentByClockPhoto()));

    connect(d->exampleFileChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotUpdateExample()));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    // -----------------------------------------------------------------------

    readSettings();
    slotSrcTimestampChanged();
    slotAdjustmentTypeChanged();
}

TimeAdjustDialog::~TimeAdjustDialog()
{
    delete d->about;
    delete d;
}

void TimeAdjustDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void TimeAdjustDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Time Adjust Settings"));

    int useTimestampType = group.readEntry("Use Timestamp Type", 0);
    if (useTimestampType == 0) d->useApplDateBtn->setChecked(true);
    else if (useTimestampType == 1) d->useFileDateBtn->setChecked(true);
    else if (useTimestampType == 2) d->useMetaDateBtn->setChecked(true);
    else if (useTimestampType == 3) d->useCustomDateBtn->setChecked(true);

    d->useFileDateTypeChooser->setCurrentIndex(group.readEntry("File Timestamp Type", 0));
    d->useMetaDateTypeChooser->setCurrentIndex(group.readEntry("Meta Timestamp Type", 0));
    d->useCustDateInput->setDateTime(group.readEntry("Custom Date", QDateTime::currentDateTime()));
    d->useCustTimeInput->setDateTime(group.readEntry("Custom Time", QDateTime::currentDateTime()));

    d->adjTypeChooser->setCurrentIndex(group.readEntry("Adjustment Type", 0));
    d->adjDaysInput->setValue(group.readEntry("Adjustment Days", 0));
    d->adjTimeInput->setDateTime(group.readEntry("Adjustment Time", QDateTime()));

    d->updAppDateCheck->setChecked(group.readEntry("Update Application Time", false));
    d->updFileModDateCheck->setChecked(group.readEntry("Update File Modification Time", false));
    d->updEXIFModDateCheck->setChecked(group.readEntry("Update EXIF Modification Time", false));
    d->updEXIFOriDateCheck->setChecked(group.readEntry("Update EXIF Original Time", false));
    d->updEXIFDigDateCheck->setChecked(group.readEntry("Update EXIF Digitization Time", false));
    d->updIPTCDateCheck->setChecked(group.readEntry("Update IPTC Time", false));
    d->updXMPDateCheck->setChecked(group.readEntry("Update XMP Creation Time", false));

    KConfigGroup group2 = config.group(QString("Time Adjust Dialog"));
    restoreDialogSize(group2);
}

void TimeAdjustDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Time Adjust Settings"));

    int useTimestampType = 0; // default if d->useApplDateBtn->isChecked()
    if (d->useFileDateBtn->isChecked()) useTimestampType = 1;
    if (d->useMetaDateBtn->isChecked()) useTimestampType = 2;
    if (d->useCustomDateBtn->isChecked()) useTimestampType = 3;
    group.writeEntry("Use Timestamp Type", useTimestampType);

    group.writeEntry("File Timestamp Type", d->useFileDateTypeChooser->currentIndex());
    group.writeEntry("Meta Timestamp Type", d->useMetaDateTypeChooser->currentIndex());
    group.writeEntry("Custom Date", d->useCustDateInput->dateTime());
    group.writeEntry("Custom Time", d->useCustTimeInput->dateTime());

    group.writeEntry("Adjustment Type", d->adjTypeChooser->currentIndex());
    group.writeEntry("Adjustment Days", d->adjDaysInput->value());
    group.writeEntry("Adjustment Time", d->adjTimeInput->dateTime());

    group.writeEntry("Update Application Time", d->updAppDateCheck->isChecked());
    group.writeEntry("Update File Modification Time", d->updFileModDateCheck->isChecked());
    group.writeEntry("Update EXIF Modification Time", d->updEXIFModDateCheck->isChecked());
    group.writeEntry("Update EXIF Original Time", d->updEXIFOriDateCheck->isChecked());
    group.writeEntry("Update EXIF Digitization Time", d->updEXIFDigDateCheck->isChecked());
    group.writeEntry("Update IPTC Time", d->updIPTCDateCheck->isChecked());
    group.writeEntry("Update XMP Creation Time", d->updXMPDateCheck->isChecked());

    KConfigGroup group2 = config.group(QString("Time Adjust Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void TimeAdjustDialog::setImages(const KUrl::List& imageURLs)
{
    d->imageURLs.clear();

    for (KUrl::List::ConstIterator it = imageURLs.constBegin(); it != imageURLs.constEnd(); ++it)
    {
        d->imageURLs.append(*it);
        d->exampleFileChooser->addItem((*it).fileName());
    }

    readExampleTimestamps();
}

void TimeAdjustDialog::readExampleTimestamps()
{
    d->imageOriginalDates.clear();

    if (d->useApplDateBtn->isChecked())
        readApplicationTimestamps();

    else if (d->useFileDateBtn->isChecked())
        readFileTimestamps();

    else if (d->useMetaDateBtn->isChecked())
        readMetadataTimestamps();

    else if (d->useCustomDateBtn->isChecked())
    {
        d->exampleSummaryLabel->setText(i18np("1 image will be changed",
                                        "%1 images will be changed",
                                        d->imageURLs.count()));
    }

    slotUpdateExample();
}

void TimeAdjustDialog::readApplicationTimestamps()
{
    int       exactCount   = 0;
    int       inexactCount = 0;
    QDateTime nullDateTime;

    for (KUrl::List::ConstIterator it = d->imageURLs.constBegin(); it != d->imageURLs.constEnd(); ++it)
    {
        KIPI::ImageInfo info = d->interface->info(*it);
        if (info.isTimeExact())
        {
            exactCount++;
            d->imageOriginalDates.append(info.time());
        }
        else
        {
            inexactCount++;
            d->imageOriginalDates.append(nullDateTime);
        }
    }

    if (inexactCount == 0)
    {
        d->exampleSummaryLabel->setText(i18np("1 image will be changed",
                                    "%1 images will be changed",
                                    d->imageURLs.count()));
    }
    else
    {
        d->exampleSummaryLabel->setText(i18np("1 image will be changed; ",
                                    "%1 images will be changed; ",
                                    exactCount)
                                + "<br>"
                                + i18np("1 image will be skipped due to an inexact date.",
                                        "%1 images will be skipped due to inexact dates.",
                                        inexactCount));
    }
    // PENDING(blackie) handle all images being inexact.
}

void TimeAdjustDialog::readFileTimestamps()
{
    for (KUrl::List::ConstIterator it = d->imageURLs.constBegin(); it != d->imageURLs.constEnd(); ++it)
    {
        KIPI::ImageInfo info = d->interface->info(*it);
        QFileInfo fileInfo(info.path().toLocalFile());
        d->imageOriginalDates.append(fileInfo.lastModified());
    }

    d->exampleSummaryLabel->setText(i18np("1 image will be changed",
                                          "%1 images will be changed",
                                          d->imageURLs.count()));
}

void TimeAdjustDialog::readMetadataTimestamps()
{
    int okCount      = 0;
    int missingCount = 0;
    QDateTime nullDateTime;

    for (KUrl::List::ConstIterator it = d->imageURLs.constBegin(); it != d->imageURLs.constEnd(); ++it)
    {
        KIPI::ImageInfo info = d->interface->info( *it );
        KExiv2Iface::KExiv2 exiv2Iface;
        if (!exiv2Iface.load(info.path().path()))
        {
            missingCount++;
            d->imageOriginalDates.append(nullDateTime);
            continue;
        }

        QDateTime curImageDateTime;
        switch (d->useMetaDateTypeChooser->currentIndex()) {
        case 0:
            curImageDateTime = exiv2Iface.getImageDateTime();
            break;
        case 1:
            curImageDateTime = QDateTime::fromString(exiv2Iface.getExifTagString("Exif.Image.DateTime"), "yyyy:MM:dd hh:mm:ss");
            break;
        case 2:
            curImageDateTime = QDateTime::fromString(exiv2Iface.getExifTagString("Exif.Photo.DateTimeOriginal"), "yyyy:MM:dd hh:mm:ss");
            break;
        case 3:
            curImageDateTime = QDateTime::fromString(exiv2Iface.getExifTagString("Exif.Photo.DateTimeDigitized"), "yyyy:MM:dd hh:mm:ss");
            break;
        case 4:
            // we have to truncate the timezone from the time, otherwise it cannot be converted to a QTime
            curImageDateTime = QDateTime(QDate::fromString(exiv2Iface.getIptcTagString("Iptc.Application2.DateCreated"), Qt::ISODate),
                                         QTime::fromString(exiv2Iface.getIptcTagString("Iptc.Application2.TimeCreated").left(8), Qt::ISODate));
            //kDebug() << "IPTC for " << info.path().path() << ": " << exiv2Iface.getIptcTagString("Iptc.Application2.DateCreated") << ", " << exiv2Iface.getIptcTagString("Iptc.Application2.TimeCreated") << endl;
            //kDebug() << "converted: " << QDate::fromString(exiv2Iface.getIptcTagString("Iptc.Application2.DateCreated"), Qt::ISODate) << ", " << QTime::fromString(exiv2Iface.getIptcTagString("Iptc.Application2.TimeCreated").left(8), Qt::ISODate) << endl;
            break;
        case 5:
            curImageDateTime = QDateTime::fromString(exiv2Iface.getXmpTagString("Xmp.xmp.CreateDate"), "yyyy:MM:dd hh:mm:ss");
            break;
        default:
            // curImageDateTime stays invalid
            break;
        };

        d->imageOriginalDates.append(curImageDateTime);
        if (curImageDateTime.isValid())
            okCount++;
        else
            missingCount++;
    }

    if (missingCount == 0)
    {
        d->exampleSummaryLabel->setText(i18np("1 image will be changed",
                                    "%1 images will be changed",
                                    d->imageURLs.count()));
    }
    else
    {
        d->exampleSummaryLabel->setText(i18np("1 image will be changed; ",
                                    "%1 images will be changed; ",
                                    okCount)
                            + "<br>"
                            + i18np("1 image will be skipped due to a missing source timestamp.",
                                    "%1 images will be skipped due to missing source timestamps.",
                                    missingCount));
    }
}

void TimeAdjustDialog::slotSrcTimestampChanged()
{
    d->useFileDateTypeChooser->setEnabled(false);
    d->useMetaDateTypeChooser->setEnabled(false);
    d->useCustDateInput->setEnabled(false);
    d->useCustTimeInput->setEnabled(false);
    d->useCustomDateTodayBtn->setEnabled(false);

    if (d->useFileDateBtn->isChecked())
    {
        d->useFileDateTypeChooser->setEnabled(true);
    }
    else if (d->useMetaDateBtn->isChecked())
    {
        d->useMetaDateTypeChooser->setEnabled(true);
    }
    else if (d->useCustomDateBtn->isChecked())
    {
        d->useCustDateInput->setEnabled(true);
        d->useCustTimeInput->setEnabled(true);
        d->useCustomDateTodayBtn->setEnabled(true);
    }

    // read the original timestamps for all selected files
    // (according to the newly selected source timestamp type),
    // this will also implicitly update the example
    readExampleTimestamps();
}

void TimeAdjustDialog::slotResetDateToCurrent()
{
    QDateTime currentDateTime(QDateTime::currentDateTime());
    d->useCustDateInput->setDateTime(currentDateTime);
    d->useCustTimeInput->setDateTime(currentDateTime);
}

void TimeAdjustDialog::slotAdjustmentTypeChanged()
{
    // if the addition or subtraction has been selected, enable the edit boxes to enter the adjustment length
    bool isAdjustment = (d->adjTypeChooser->currentIndex() > 0);
    d->adjDaysInput->setEnabled(isAdjustment);
    d->adjDaysLabel->setEnabled(isAdjustment);
    d->adjTimeInput->setEnabled(isAdjustment);

    // update the examples (with adjustment enabled also the adjusted time is shown)
    slotUpdateExample();
}

void TimeAdjustDialog::slotDetAdjustmentByClockPhoto()
{
    /* When the use presses the clock photo button, present a dialog and set the
     * results to the proper widgets. */

    QPointer<ClockPhotoDialog> dilg = new ClockPhotoDialog(d->interface, this);
    int result = dilg->exec();
    if (result == QDialog::Accepted)
    {
        if (dilg->deltaDays == 0 && dilg->deltaHours == 0 && dilg->deltaMinutes == 0 && dilg->deltaSeconds == 0)
        {
            d->adjTypeChooser->setCurrentIndex(0);
        }
        else if (dilg->deltaNegative)
        {
            d->adjTypeChooser->setCurrentIndex(2);
        }
        else
        {
            d->adjTypeChooser->setCurrentIndex(1);
        }

        d->adjDaysInput->setValue(dilg->deltaDays);
        QTime deltaTime;
        deltaTime.setHMS(dilg->deltaHours, dilg->deltaMinutes, dilg->deltaSeconds);
        d->adjTimeInput->setTime(deltaTime);
    }

    delete dilg;
}

void TimeAdjustDialog::slotUpdateExample()
{
    static const QString exampleDateTimeFormat("dd.MM.yyyy hh:mm:ss");

    // When the custom timestamp is to be used, do not show any file original timestamps
    if (d->useCustomDateBtn->isChecked())
    {
        QDateTime customTime(d->useCustDateInput->date(), d->useCustTimeInput->time());
        if (d->adjTypeChooser->currentIndex() > 0)
            customTime = calculateAdjustedTime(customTime);

        QString customTimeStr = customTime.toString(exampleDateTimeFormat);
        d->exampleTimeChangeLabel->setText(i18n("Custom: <b>%1</b>", customTimeStr));
        return;
    }

    // If the file timestamp structures are not ready yet, do not show any information
    // (this may happen during initialization)
    if (d->imageOriginalDates.size() == 0 || d->exampleFileChooser->currentIndex() >= d->imageOriginalDates.size()) 
    {
        d->exampleTimeChangeLabel->setText("");
        return;
    }

    // Get the file original timestamp according to the selection, if it is not available inform the user
    QDateTime originalTime = d->imageOriginalDates.at(d->exampleFileChooser->currentIndex());
    if (!originalTime.isValid())
    {
        d->exampleTimeChangeLabel->setText(i18n("Original: <b>N/A</b><br/>"
                                                "Image will be skipped"));
        return;
    }

    // Show the file original timestamp (and adjusted one if needed)
    QString originalTimeStr = originalTime.toString(exampleDateTimeFormat);
    if (d->adjTypeChooser->currentIndex() == 0)
    {
        d->exampleTimeChangeLabel->setText(i18n("Original: <b>%1</b>", originalTimeStr));
        return;
    }
    else
    {
        QDateTime adjustedTime  = calculateAdjustedTime(originalTime);
        QString adjustedTimeStr = adjustedTime.toString(exampleDateTimeFormat);
        d->exampleTimeChangeLabel->setText(i18n("Original: <b>%1</b><br/>"
                                                "Adjusted: <b>%2</b>",
                                           originalTimeStr, adjustedTimeStr));
    }
}

void TimeAdjustDialog::slotHelp()
{
    KToolInvocation::invokeHelp("timeadjust", "kipi-plugins");
}

void TimeAdjustDialog::slotOk()
{
    QStringList fileTimeErrorFiles;
    QStringList metaTimeErrorFiles;
    QDateTime   dateTime;
    QDateTime   customTime(d->useCustDateInput->date(), d->useCustTimeInput->time());
    bool metadataChanged = d->updEXIFModDateCheck->isChecked() ||
                           d->updEXIFOriDateCheck->isChecked() ||
                           d->updEXIFDigDateCheck->isChecked() ||
                           d->updIPTCDateCheck->isChecked()    ||
                           d->updXMPDateCheck->isChecked();

    for (int i=0; i<d->imageURLs.size(); ++i)
    {
        KUrl url = d->imageURLs[i];
        if (d->useCustomDateBtn->isChecked()) dateTime = customTime;
        else dateTime = d->imageOriginalDates[i];
        if (!dateTime.isValid()) continue;

        dateTime = calculateAdjustedTime(dateTime);

        if (d->updAppDateCheck->isChecked())
        {
            KIPI::ImageInfo info = d->interface->info(url);
            info.setTime(dateTime);
        }

        if (metadataChanged)
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
                if (exiv2Iface.canWriteExif(url.path()))
                {
                    if (d->updEXIFModDateCheck->isChecked())
                    {
                        ret &= exiv2Iface.setExifTagString("Exif.Image.DateTime",
                            dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    }

                    if (d->updEXIFOriDateCheck->isChecked())
                    {
                        ret &= exiv2Iface.setExifTagString("Exif.Photo.DateTimeOriginal",
                            dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    }

                    if (d->updEXIFDigDateCheck->isChecked())
                    {
                        ret &= exiv2Iface.setExifTagString("Exif.Photo.DateTimeDigitized",
                            dateTime.toString(QString("yyyy:MM:dd hh:mm:ss")).toAscii());
                    }
                }
                else if (d->updEXIFModDateCheck->isChecked() || d->updEXIFOriDateCheck->isChecked() ||
                         d->updEXIFDigDateCheck->isChecked())
                {
                    ret = false;
                }

                if (d->updIPTCDateCheck->isChecked())
                {
                    if (exiv2Iface.canWriteIptc(url.path()))
                    {
                        ret &= exiv2Iface.setIptcTagString("Iptc.Application2.DateCreated",
                            dateTime.date().toString(Qt::ISODate));
                        ret &= exiv2Iface.setIptcTagString("Iptc.Application2.TimeCreated",
                            dateTime.time().toString(Qt::ISODate));
                    }
                    else ret = false;
                }

                if (d->updXMPDateCheck->isChecked())
                {
                    if (exiv2Iface.supportXmp() && exiv2Iface.canWriteXmp(url.path()))
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
                    else ret = false;
                }

                ret &= exiv2Iface.save(url.path());

                if (!ret)
                {
                    kDebug() << "Failed to update metadata in file " << url.fileName();
                }
            }
            else
            {
                kDebug() << "Failed to load metadata from file " << url.fileName();
            }

            if (!ret) metaTimeErrorFiles.append(url.fileName());
        }

        if (d->updFileModDateCheck->isChecked())
        {
            // since QFileInfo does not support timestamp updates, see Qt suggestion #79427 at
            // http://www.qtsoftware.com/developer/task-tracker/index_html?id=79427&method=entry
            // we have to use the utime() system call

            utimbuf times;
            times.actime = QDateTime::currentDateTime().toTime_t();
            times.modtime = dateTime.toTime_t();

            if (0 != utime(url.path().toLatin1().constData(), &times)) 
                fileTimeErrorFiles.append(url.fileName());
        }
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.
    d->interface->refreshImages(d->imageURLs);

    if (!metaTimeErrorFiles.isEmpty())
    {
        KMessageBox::informationList(
                     kapp->activeWindow(),
                     i18n("Unable to update metadata in:"),
                     metaTimeErrorFiles,
                     i18n("Adjust Time & Date"));
    }

    if (!fileTimeErrorFiles.isEmpty())
    {
        KMessageBox::informationList(
                     kapp->activeWindow(),
                     i18n("Unable to update file modification time in:"),
                     fileTimeErrorFiles,
                     i18n("Adjust Time & Date"));
    }

    saveSettings();
    accept();
}

void TimeAdjustDialog::slotCancel()
{
    saveSettings();
    reject();
}

QDateTime TimeAdjustDialog::calculateAdjustedTime(const QDateTime& originalTime) const
{
    int sign = 0;
    switch (d->adjTypeChooser->currentIndex())
    {
        case 1:
            sign = 1;
            break;
        case 2:
            sign = -1;
            break;
        default:
            return originalTime;
    };

    const QTime &adjTime = d->adjTimeInput->time();
    int seconds          = adjTime.second();
    seconds += 60*adjTime.minute();
    seconds += 60*60*adjTime.hour();
    seconds += 24*60*60*d->adjDaysInput->value();

    return originalTime.addSecs(sign * seconds);
}

}  // namespace KIPITimeAdjustPlugin
