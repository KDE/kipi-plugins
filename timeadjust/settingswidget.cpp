/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "settingswidget.moc"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
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

#include <kapplication.h>
#include <kaboutdata.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>

// Local includes

#include "clockphotodialog.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPITimeAdjustPlugin
{

class SettingsWidget::SettingsWidgetPrivate
{

public:

    SettingsWidgetPrivate()
    {
        useGroupBox            = 0;
        adjustGroupBox         = 0;
        updateGroupBox         = 0;
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
        updFileNameCheck       = 0;
        useFileDateTypeChooser = 0;
        useMetaDateTypeChooser = 0;
        adjTypeChooser         = 0;
        adjDaysLabel           = 0;
        adjDaysInput           = 0;
        adjDetByClockPhotoBtn  = 0;
        useCustDateInput       = 0;
        useCustTimeInput       = 0;
        adjTimeInput           = 0;
        useCustomDateTodayBtn  = 0;
    }

    QGroupBox*            useGroupBox;
    QGroupBox*            adjustGroupBox;
    QGroupBox*            updateGroupBox;

    QButtonGroup*         useButtonGroup;

    QRadioButton*         useApplDateBtn;
    QRadioButton*         useFileDateBtn;
    QRadioButton*         useMetaDateBtn;
    QRadioButton*         useCustomDateBtn;

    QCheckBox*            updAppDateCheck;
    QCheckBox*            updFileModDateCheck;
    QCheckBox*            updEXIFModDateCheck;
    QCheckBox*            updEXIFOriDateCheck;
    QCheckBox*            updEXIFDigDateCheck;
    QCheckBox*            updIPTCDateCheck;
    QCheckBox*            updXMPDateCheck;
    QCheckBox*            updFileNameCheck;

    QComboBox*            useFileDateTypeChooser;
    QComboBox*            useMetaDateTypeChooser;
    QComboBox*            adjTypeChooser;

    QLabel*               adjDaysLabel;

    QSpinBox*             adjDaysInput;

    QPushButton*          adjDetByClockPhotoBtn;

    QDateEdit*            useCustDateInput;

    QTimeEdit*            useCustTimeInput;
    QTimeEdit*            adjTimeInput;

    QToolButton*          useCustomDateTodayBtn;
};

SettingsWidget::SettingsWidget(QWidget* const parent)
    : KVBox(parent), d(new SettingsWidgetPrivate)
{
    setMargin(0);
    setSpacing(KDialog::spacingHint());

    // -- Settings View Used Timestamps ---------------------------------------------------------

    d->useGroupBox              = new QGroupBox(i18n("Timestamp Used"), this);
    QGridLayout* useGBLayout    = new QGridLayout(d->useGroupBox);
    d->useButtonGroup           = new QButtonGroup(d->useGroupBox);
    d->useButtonGroup->setExclusive(true);

    QString applDateLabelString = i18n("%1 timestamp", KGlobal::mainComponent().aboutData()->programName());
    d->useApplDateBtn           = new QRadioButton("", d->useGroupBox);
    QLabel* useApplDateLbl      = new QLabel(applDateLabelString);
    useApplDateLbl->setIndent(5);

    d->useFileDateBtn           = new QRadioButton(d->useGroupBox);
    d->useFileDateTypeChooser   = new QComboBox(d->useGroupBox);
    d->useFileDateTypeChooser->insertItem(TimeAdjustSettings::FILELASTMOD, i18n("File last modified"));
    /* NOTE: not supported by Linux, although supported by Qt (read-only)
    d->useFileDateTypeChooser->insertItem(TimeAdjustSettings::FILECREATED, i18n("File created"));
    */

    d->useMetaDateBtn         = new QRadioButton(QString(), d->useGroupBox);
    d->useMetaDateTypeChooser = new QComboBox(d->useGroupBox);
    d->useMetaDateTypeChooser->insertItem(TimeAdjustSettings::EXIFIPTCXMP,   i18n("EXIF/IPTC/XMP"));
    d->useMetaDateTypeChooser->insertItem(TimeAdjustSettings::EXIFCREATED,   i18n("EXIF: created"));
    d->useMetaDateTypeChooser->insertItem(TimeAdjustSettings::EXIFORIGINAL,  i18n("EXIF: original"));
    d->useMetaDateTypeChooser->insertItem(TimeAdjustSettings::EXIFDIGITIZED, i18n("EXIF: digitized"));
    d->useMetaDateTypeChooser->insertItem(TimeAdjustSettings::IPTCCREATED,   i18n("IPTC: created"));
    d->useMetaDateTypeChooser->insertItem(TimeAdjustSettings::XMPCREATED,    i18n("XMP: created"));

    d->useCustomDateBtn       = new QRadioButton(d->useGroupBox);
    d->useCustDateInput       = new QDateEdit(d->useGroupBox);
    d->useCustDateInput->setDisplayFormat("dd MMMM yyyy");
    d->useCustDateInput->setCalendarPopup(true);
    d->useCustTimeInput       = new QTimeEdit(d->useGroupBox);
    d->useCustTimeInput->setDisplayFormat("hh:mm:ss");
    d->useCustomDateTodayBtn  = new QToolButton(d->useGroupBox);
    d->useCustomDateTodayBtn->setIcon(SmallIcon("go-jump-today"));
    d->useCustomDateTodayBtn->setToolTip(i18n("Reset to current date"));

    useGBLayout->setMargin(KDialog::spacingHint());
    useGBLayout->setSpacing(KDialog::spacingHint());
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

    // -- Settings View TimesStamp Adjustements ---------------------------------------------------

    d->adjustGroupBox           = new QGroupBox(i18n("Timestamp Adjustments"), this);
    QGridLayout* adjustGBLayout = new QGridLayout(d->adjustGroupBox);

    d->adjTypeChooser           = new QComboBox(d->adjustGroupBox);
    d->adjTypeChooser->insertItem(TimeAdjustSettings::COPYVALUE, i18nc("copy timestamp as well",             "Copy value"));
    d->adjTypeChooser->insertItem(TimeAdjustSettings::ADDVALUE,  i18nc("add a fixed timestamp to date",      "Add"));
    d->adjTypeChooser->insertItem(TimeAdjustSettings::SUBVALUE,  i18nc("subtract a fixed timestamp to date", "Subtract"));
    d->adjDaysInput             = new QSpinBox(d->adjustGroupBox);
    d->adjDaysInput->setRange(0, 9999);
    d->adjDaysInput->setSingleStep(1);
    d->adjDaysLabel             = new QLabel(i18nc("time adjust offset, days value label", "days"), d->adjustGroupBox);
    d->adjTimeInput             = new QTimeEdit(d->adjustGroupBox);
    d->adjTimeInput->setDisplayFormat("hh:mm:ss");
    d->adjDetByClockPhotoBtn    = new QPushButton(i18n("Determine difference from clock photo"));

    adjustGBLayout->setMargin(KDialog::spacingHint());
    adjustGBLayout->setSpacing(KDialog::spacingHint());
    adjustGBLayout->setColumnStretch(0, 1);
    adjustGBLayout->setColumnStretch(1, 1);
    adjustGBLayout->setColumnStretch(3, 1);
    adjustGBLayout->addWidget(d->adjTypeChooser,        0, 0, 1, 1);
    adjustGBLayout->addWidget(d->adjDaysInput,          0, 1, 1, 1);
    adjustGBLayout->addWidget(d->adjDaysLabel,          0, 2, 1, 1);
    adjustGBLayout->addWidget(d->adjTimeInput,          0, 3, 1, 1);
    adjustGBLayout->addWidget(d->adjDetByClockPhotoBtn, 1, 0, 1, 4);

    // -- Settings View Updated Timestamps -------------------------------------------------------

    d->updateGroupBox           = new QGroupBox(i18n("Timestamp Updated"), this);
    QGridLayout* updateGBLayout = new QGridLayout(d->updateGroupBox);

    d->updAppDateCheck          = new QCheckBox(applDateLabelString,        d->updateGroupBox);
    d->updFileModDateCheck      = new QCheckBox(i18n("File last modified"), d->updateGroupBox);
    d->updEXIFModDateCheck      = new QCheckBox(i18n("EXIF: created"),      d->updateGroupBox);
    d->updEXIFOriDateCheck      = new QCheckBox(i18n("EXIF: original"),     d->updateGroupBox);
    d->updEXIFDigDateCheck      = new QCheckBox(i18n("EXIF: digitized"),    d->updateGroupBox);
    d->updIPTCDateCheck         = new QCheckBox(i18n("IPTC: created"),      d->updateGroupBox);
    d->updXMPDateCheck          = new QCheckBox(i18n("XMP"),                d->updateGroupBox);
    d->updFileNameCheck         = new QCheckBox(i18n("Filename"),           d->updateGroupBox);

    updateGBLayout->setMargin(KDialog::spacingHint());
    updateGBLayout->setSpacing(KDialog::spacingHint());
    updateGBLayout->setColumnStretch(0, 1);
    updateGBLayout->setColumnStretch(1, 1);
    updateGBLayout->addWidget(d->updAppDateCheck,     0, 0, 1, 1);
    updateGBLayout->addWidget(d->updEXIFModDateCheck, 0, 1, 1, 1);
    updateGBLayout->addWidget(d->updEXIFOriDateCheck, 1, 0, 1, 1);
    updateGBLayout->addWidget(d->updEXIFDigDateCheck, 1, 1, 1, 1);
    updateGBLayout->addWidget(d->updXMPDateCheck,     2, 0, 1, 1);
    updateGBLayout->addWidget(d->updIPTCDateCheck,    2, 1, 1, 1);
    updateGBLayout->addWidget(d->updFileModDateCheck, 3, 0, 1, 1);
    updateGBLayout->addWidget(d->updFileNameCheck,    3, 1, 1, 1);

    if (!KPMetadata::supportXmp())
    {
        d->updXMPDateCheck->setEnabled(false);
    }

    // -- Settings View Slots/Signals ----------------------------------------

    connect(d->useButtonGroup, SIGNAL(buttonReleased(int)),
            this, SLOT(slotSrcTimestampChanged()));

    connect(d->useFileDateTypeChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSrcTimestampChanged()));

    connect(d->useMetaDateTypeChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSrcTimestampChanged()));

    connect(d->adjTypeChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotAdjustmentTypeChanged()));

    connect(d->useCustomDateTodayBtn, SIGNAL(clicked()),
            this, SLOT(slotResetDateToCurrent()));

    connect(d->adjDetByClockPhotoBtn, SIGNAL(clicked()),
            this, SLOT(slotDetAdjustmentByClockPhoto()));

    connect(d->updFileNameCheck, SIGNAL(clicked()),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->useCustDateInput, SIGNAL(dateChanged(QDate)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->useCustTimeInput, SIGNAL(timeChanged(QTime)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->adjDaysInput, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->adjTimeInput, SIGNAL(timeChanged(QTime)),
            this, SIGNAL(signalSettingsChanged()));
}

SettingsWidget::~SettingsWidget()
{
    delete d;
}

void SettingsWidget::readSettings(const KConfigGroup& group)
{
    int useTimestampType = group.readEntry("Use Timestamp Type", (int)TimeAdjustSettings::APPDATE);
    if      (useTimestampType == TimeAdjustSettings::APPDATE)      d->useApplDateBtn->setChecked(true);
    else if (useTimestampType == TimeAdjustSettings::FILEDATE)     d->useFileDateBtn->setChecked(true);
    else if (useTimestampType == TimeAdjustSettings::METADATADATE) d->useMetaDateBtn->setChecked(true);
    else if (useTimestampType == TimeAdjustSettings::CUSTOMDATE)   d->useCustomDateBtn->setChecked(true);

    d->useFileDateTypeChooser->setCurrentIndex(group.readEntry("File Timestamp Type",   (int)TimeAdjustSettings::FILELASTMOD));
    d->useMetaDateTypeChooser->setCurrentIndex(group.readEntry("Meta Timestamp Type",   (int)TimeAdjustSettings::EXIFIPTCXMP));
    d->useCustDateInput->setDateTime(group.readEntry("Custom Date",                     QDateTime::currentDateTime()));
    d->useCustTimeInput->setDateTime(group.readEntry("Custom Time",                     QDateTime::currentDateTime()));

    d->adjTypeChooser->setCurrentIndex(group.readEntry("Adjustment Type",               (int)TimeAdjustSettings::COPYVALUE));
    d->adjDaysInput->setValue(group.readEntry("Adjustment Days",                        0));
    d->adjTimeInput->setDateTime(group.readEntry("Adjustment Time",                     QDateTime()));

    d->updAppDateCheck->setChecked(group.readEntry("Update Application Time",           false));
    d->updFileModDateCheck->setChecked(group.readEntry("Update File Modification Time", false));
    d->updEXIFModDateCheck->setChecked(group.readEntry("Update EXIF Modification Time", false));
    d->updEXIFOriDateCheck->setChecked(group.readEntry("Update EXIF Original Time",     false));
    d->updEXIFDigDateCheck->setChecked(group.readEntry("Update EXIF Digitization Time", false));
    d->updIPTCDateCheck->setChecked(group.readEntry("Update IPTC Time",                 false));
    d->updXMPDateCheck->setChecked(group.readEntry("Update XMP Creation Time",          false));
    d->updFileNameCheck->setChecked(group.readEntry("Update File Name",                 false));

    slotSrcTimestampChanged();
    slotAdjustmentTypeChanged();
}

void SettingsWidget::saveSettings(KConfigGroup& group)
{
    group.writeEntry("Custom Date",                   d->useCustDateInput->dateTime());
    group.writeEntry("Custom Time",                   d->useCustTimeInput->dateTime());

    group.writeEntry("Adjustment Type",               d->adjTypeChooser->currentIndex());
    group.writeEntry("Adjustment Days",               d->adjDaysInput->value());
    group.writeEntry("Adjustment Time",               d->adjTimeInput->dateTime());

    TimeAdjustSettings prm = settings();

    group.writeEntry("Update Application Time",       prm.updAppDate);
    group.writeEntry("Update File Modification Time", prm.updFileModDate);
    group.writeEntry("Update EXIF Modification Time", prm.updEXIFModDate);
    group.writeEntry("Update EXIF Original Time",     prm.updEXIFOriDate);
    group.writeEntry("Update EXIF Digitization Time", prm.updEXIFDigDate);
    group.writeEntry("Update IPTC Time",              prm.updIPTCDate);
    group.writeEntry("Update XMP Creation Time",      prm.updXMPDate);
    group.writeEntry("Update File Name",              prm.updFileName);

    group.writeEntry("Use Timestamp Type",            prm.dateSource);
    group.writeEntry("Meta Timestamp Type",           prm.metadataSource);
    group.writeEntry("File Timestamp Type",           prm.fileDateSource);
}

TimeAdjustSettings SettingsWidget::settings() const
{
    TimeAdjustSettings settings;
    settings.updAppDate     = d->updAppDateCheck->isChecked();
    settings.updEXIFModDate = d->updEXIFModDateCheck->isChecked();
    settings.updEXIFOriDate = d->updEXIFOriDateCheck->isChecked();
    settings.updEXIFDigDate = d->updEXIFDigDateCheck->isChecked();
    settings.updIPTCDate    = d->updIPTCDateCheck->isChecked();
    settings.updXMPDate     = d->updXMPDateCheck->isChecked();
    settings.updFileName    = d->updFileNameCheck->isChecked();
    settings.updFileModDate = d->updFileModDateCheck->isChecked();

    settings.dateSource     = TimeAdjustSettings::APPDATE;
    if (d->useFileDateBtn->isChecked())   settings.dateSource = TimeAdjustSettings::FILEDATE;
    if (d->useMetaDateBtn->isChecked())   settings.dateSource = TimeAdjustSettings::METADATADATE;
    if (d->useCustomDateBtn->isChecked()) settings.dateSource = TimeAdjustSettings::CUSTOMDATE;

    settings.metadataSource = d->useMetaDateTypeChooser->currentIndex();
    settings.fileDateSource = d->useFileDateTypeChooser->currentIndex();

    return settings;
}

void SettingsWidget::slotSrcTimestampChanged()
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

    emit signalSettingsChanged();
}

void SettingsWidget::slotResetDateToCurrent()
{
    QDateTime currentDateTime(QDateTime::currentDateTime());
    d->useCustDateInput->setDateTime(currentDateTime);
    d->useCustTimeInput->setDateTime(currentDateTime);

    emit signalSettingsChanged();
}

void SettingsWidget::slotAdjustmentTypeChanged()
{
    // If the addition or subtraction has been selected, enable the edit boxes to enter the adjustment length
    bool isAdjustment = (d->adjTypeChooser->currentIndex() > TimeAdjustSettings::COPYVALUE);
    d->adjDaysInput->setEnabled(isAdjustment);
    d->adjDaysLabel->setEnabled(isAdjustment);
    d->adjTimeInput->setEnabled(isAdjustment);

    emit signalSettingsChanged();
}

QDateTime SettingsWidget::customDate() const
{
    return QDateTime(d->useCustDateInput->date(), d->useCustTimeInput->time());
}

QDateTime SettingsWidget::calculateAdjustedDate(const QDateTime& originalTime) const
{
    int sign = 0;

    switch (d->adjTypeChooser->currentIndex())
    {
        case TimeAdjustSettings::ADDVALUE:
            sign = 1;
            break;
        case TimeAdjustSettings::SUBVALUE:
            sign = -1;
            break;
        default: // TimeAdjustSettings::COPYVALUE
            return originalTime;
    };

    const QTime& adjTime = d->adjTimeInput->time();
    int seconds          = adjTime.second();
    seconds             += 60*adjTime.minute();
    seconds             += 60*60*adjTime.hour();
    seconds             += 24*60*60*d->adjDaysInput->value();

    return originalTime.addSecs(sign * seconds);
}

void SettingsWidget::slotDetAdjustmentByClockPhoto()
{
    /* When user press the clock photo button, a dialog is displayed and set the
     * results to the proper widgets.
     */
    QPointer<ClockPhotoDialog> dlg = new ClockPhotoDialog(this);
    int result                     = dlg->exec();

    if (result == QDialog::Accepted)
    {
        if (dlg->deltaDays == 0 && dlg->deltaHours == 0 && dlg->deltaMinutes == 0 && dlg->deltaSeconds == 0)
        {
            d->adjTypeChooser->setCurrentIndex(TimeAdjustSettings::COPYVALUE);
        }
        else if (dlg->deltaNegative)
        {
            d->adjTypeChooser->setCurrentIndex(TimeAdjustSettings::SUBVALUE);
        }
        else
        {
            d->adjTypeChooser->setCurrentIndex(TimeAdjustSettings::ADDVALUE);
        }

        d->adjDaysInput->setValue(dlg->deltaDays);
        QTime deltaTime;
        deltaTime.setHMS(dlg->deltaHours, dlg->deltaMinutes, dlg->deltaSeconds);
        d->adjTimeInput->setTime(deltaTime);
    }

    delete dlg;
}

}  // namespace KIPITimeAdjustPlugin
