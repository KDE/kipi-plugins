/* ============================================================
 *
 * Date        : 2010-03-26
 * Description : A widget to configure the GPS correlation
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#include "gpscorrelatorwidget.moc"


// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QPointer>
#include <QRadioButton>
#include <QTreeWidget>
#include <QVBoxLayout>

// KDE includes

#include <kapplication.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobalsettings.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kseparator.h>
#include <ksqueezedtextlabel.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kurl.h>

// local includes

#include "gpsdataparser.h"
#include "kipiimagemodel.h"
#include "gpsimageitem.h"
#include "gpsundocommand.h"

namespace KIPIGPSSyncPlugin
{

class GPSCorrelatorWidgetPrivate
{
public:
    GPSCorrelatorWidgetPrivate(const int pMarginHint, const int pSpacingHint)
    : marginHint(pMarginHint),
      spacingHint(pSpacingHint),
      gpxFileList(0),
      maxTimeLabel(0),
      timeZoneGroup(0),
      timeZoneSystem(0),
      timeZoneManual(0),
      timeZoneCB(0),
      offsetEnabled(0),
      offsetSign(0),
      offsetMin(0),
      offsetSec(0),
      interpolateBox(0),
      maxGapInput(0),
      maxTimeInput(0),
      correlateButton(0),
      uiEnabledInternal(true),
      uiEnabledExternal(true),
      correlationTotalCount(0),
      correlationCorrelatedCount(0),
      correlationTriedCount(0),
      correlationUndoCommand(0)
    {
    }

    int marginHint;
    int spacingHint;
    KUrl gpxFileOpenLastDirectory;
    QPushButton              *gpxLoadFilesButton;
    QTreeWidget              *gpxFileList;
    QLabel                   *maxTimeLabel;

    QButtonGroup             *timeZoneGroup;
    QRadioButton             *timeZoneSystem;
    QRadioButton             *timeZoneManual;
    KComboBox                *timeZoneCB;
    QCheckBox                *offsetEnabled;
    KComboBox                *offsetSign;
    KIntSpinBox              *offsetMin;
    KIntSpinBox              *offsetSec;

    QCheckBox                *interpolateBox;

    KIntSpinBox              *maxGapInput;
    KIntSpinBox              *maxTimeInput;

    QPushButton              *correlateButton;

    GPSDataParser            *gpsDataParser;
    bool uiEnabledInternal;
    bool uiEnabledExternal;
    KipiImageModel           *imageModel;

    int correlationTotalCount;
    int correlationCorrelatedCount;
    int correlationTriedCount;
    GPSUndoCommand *correlationUndoCommand;
};

GPSCorrelatorWidget::GPSCorrelatorWidget(QWidget* const parent, KipiImageModel* const imageModel, const int marginHint, const int spacingHint)
: QWidget(parent), d(new GPSCorrelatorWidgetPrivate(marginHint, spacingHint))
{
    d->imageModel = imageModel;
    d->gpsDataParser = new GPSDataParser(this);

    connect(d->gpsDataParser, SIGNAL(signalGPXFilesReadyAt(int, int)),
            this, SLOT(slotGPXFilesReadyAt(int, int)));

    connect(d->gpsDataParser, SIGNAL(signalAllGPXFilesReady()),
            this, SLOT(slotAllGPXFilesReady()));

    connect(d->gpsDataParser, SIGNAL(signalItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List&)),
            this, SLOT(slotItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List&)));

    connect(d->gpsDataParser, SIGNAL(signalAllItemsCorrelated()),
            this, SLOT(slotAllItemsCorrelated()));

    connect(d->gpsDataParser, SIGNAL(signalCorrelationCanceled()),
            this, SLOT(slotCorrelationCanceled()));

    QVBoxLayout* const vboxlayout = new QVBoxLayout(this);
    setLayout(vboxlayout);

    QGridLayout* const settingsLayout = new QGridLayout(this);
    vboxlayout->addLayout(settingsLayout);
    vboxlayout->addStretch(10);

    d->gpxLoadFilesButton = new QPushButton(i18n("Load GPX files..."), this);

    d->gpxFileList = new QTreeWidget(this);
    d->gpxFileList->setColumnCount(2);
    QStringList gpxHeaderLabels;
    gpxHeaderLabels << i18n("Filename") << i18n("#points");
    d->gpxFileList->setHeaderLabels(gpxHeaderLabels);

    KSeparator* const line = new KSeparator(Qt::Horizontal, this);

    QLabel *maxGapLabel = new QLabel(i18n("Max. time gap (sec.):"), this);
    d->maxGapInput = new KIntSpinBox(0, 1000000, 1, 30, this);
    d->maxGapInput->setWhatsThis(i18n("Sets the maximum difference in "
                    "seconds from a GPS track point to the image time to be matched. "
                    "If the time difference exceeds this setting, no match will be attempted."));

    QLabel* const timeZoneLabel = new QLabel(i18n("Camera time zone:"), this);
    d->timeZoneSystem = new QRadioButton(i18n("Same as system"), this);
    d->timeZoneSystem->setWhatsThis(i18n(
                    "Use this option if the timezone of the camera "
                    "is the same as the timezone of this system. "
                    "The conversion to GMT will be done automatically."));
    d->timeZoneManual = new QRadioButton(i18nc("manual time zone selection for gps syncing", "Manual:"), this);
    d->timeZoneManual->setWhatsThis(i18n(
                    "Use this option if the timezone of the camera "
                    "is different from this system and you have to "
                    "specify the difference to GMT manually."));
    d->timeZoneGroup = new QButtonGroup(this);
    d->timeZoneGroup->addButton(d->timeZoneSystem, 1);
    d->timeZoneGroup->addButton(d->timeZoneManual, 2);

    d->timeZoneCB         = new KComboBox(this);

    // See list of time zones over the world :
    // http://en.wikipedia.org/wiki/List_of_time_zones
    // NOTE: Combobox strings are not i18n.
    d->timeZoneCB->addItem("GMT-12:00");
    d->timeZoneCB->addItem("GMT-11:00");
    d->timeZoneCB->addItem("GMT-10:00");
    d->timeZoneCB->addItem("GMT-09:30");
    d->timeZoneCB->addItem("GMT-09:00");
    d->timeZoneCB->addItem("GMT-08:00");
    d->timeZoneCB->addItem("GMT-07:00");
    d->timeZoneCB->addItem("GMT-06:00");
    d->timeZoneCB->addItem("GMT-05:30");
    d->timeZoneCB->addItem("GMT-05:00");
    d->timeZoneCB->addItem("GMT-04:30");
    d->timeZoneCB->addItem("GMT-04:00");
    d->timeZoneCB->addItem("GMT-03:30");
    d->timeZoneCB->addItem("GMT-03:00");
    d->timeZoneCB->addItem("GMT-02:00");
    d->timeZoneCB->addItem("GMT-01:00");
    d->timeZoneCB->addItem("GMT+00:00");
    d->timeZoneCB->addItem("GMT+01:00");
    d->timeZoneCB->addItem("GMT+02:00");
    d->timeZoneCB->addItem("GMT+03:00");
    d->timeZoneCB->addItem("GMT+03:30");
    d->timeZoneCB->addItem("GMT+04:00");
    d->timeZoneCB->addItem("GMT+05:00");
    d->timeZoneCB->addItem("GMT+05:30");    // See B.K.O # 149491
    d->timeZoneCB->addItem("GMT+05:45");
    d->timeZoneCB->addItem("GMT+06:00");
    d->timeZoneCB->addItem("GMT+06:30");
    d->timeZoneCB->addItem("GMT+07:00");
    d->timeZoneCB->addItem("GMT+08:00");
    d->timeZoneCB->addItem("GMT+08:45");
    d->timeZoneCB->addItem("GMT+09:00");
    d->timeZoneCB->addItem("GMT+09:30");
    d->timeZoneCB->addItem("GMT+10:00");
    d->timeZoneCB->addItem("GMT+10:30");
    d->timeZoneCB->addItem("GMT+11:00");
    d->timeZoneCB->addItem("GMT+11:30");
    d->timeZoneCB->addItem("GMT+12:00");
    d->timeZoneCB->addItem("GMT+12:45");
    d->timeZoneCB->addItem("GMT+13:00");
    d->timeZoneCB->addItem("GMT+14:00");
    d->timeZoneCB->setWhatsThis(i18n("<p>Sets the time zone the camera was set to "
                    "during photo shooting, so that the time stamps of the images "
                    "can be converted to GMT to match the GPS time reference.</p>"
                    "<p>Note: positive offsets count eastwards from zero longitude (GMT), "
                    "they are 'ahead of time'.</p>"));

    // additional camera offset to respect
    d->offsetEnabled = new QCheckBox(i18n("Fine offset (mm:ss):"), this);
    d->offsetEnabled->setWhatsThis(i18n(
                        "Sets an additional offset in minutes and "
                        "seconds that is used to correlate the photos "
                        "to the GPS track. "
                        "This can be used for fine tuning to adjust a "
                        "wrong camera clock."));

    QWidget* const offsetWidget = new QWidget(this);
    d->offsetSign = new KComboBox(offsetWidget);
    d->offsetSign->addItem("+");
    d->offsetSign->addItem("-");
    d->offsetSign->setWhatsThis(i18n("Set whether the camera offset "
        "is negative or positive."));

    d->offsetMin = new KIntSpinBox(0, 59, 1, 0, offsetWidget);
    d->offsetMin->setWhatsThis(i18n("Minutes to fine tune camera offset."));

    d->offsetSec = new KIntSpinBox(0, 59, 1, 0, offsetWidget);
    d->offsetSec->setWhatsThis(i18n("Seconds to fine tune camera offset."));

    QGridLayout* const offsetLayout = new QGridLayout(offsetWidget);
    offsetLayout->addWidget(d->offsetSign, 0, 0, 1, 1);
    offsetLayout->addWidget(d->offsetMin, 0, 1, 1, 1);
    offsetLayout->addWidget(d->offsetSec, 0, 2, 1, 1);
    offsetLayout->setSpacing(d->spacingHint);
    offsetLayout->setMargin(d->marginHint);

    // interpolation options
    d->interpolateBox = new QCheckBox(i18n("Interpolate"), this);
    d->interpolateBox->setWhatsThis(i18n("Set this option to interpolate GPS track points "
                    "which are not closely matched to the GPX data file."));

    connect(d->interpolateBox, SIGNAL(stateChanged(int)),
            this, SLOT(updateUIState()));

    d->maxTimeLabel = new QLabel(i18n("Difference in min.:"), this);
    d->maxTimeInput = new KIntSpinBox(0, 240, 1, 15, this);
    d->maxTimeInput->setWhatsThis(i18n("Sets the maximum time difference in minutes (240 max.)"
                    " to interpolate GPX file points to image time data."));

    d->correlateButton = new QPushButton(i18n("Correlate"), this);

    // layout form
    int row = 0;
    settingsLayout->addWidget(d->gpxLoadFilesButton,     row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->gpxFileList,    row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(line,              row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(maxGapLabel,       row, 0, 1, 1);
    settingsLayout->addWidget(d->maxGapInput,    row, 1, 1, 1);
    row++;
    settingsLayout->addWidget(timeZoneLabel,     row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->timeZoneSystem, row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->timeZoneManual, row, 0, 1, 1);
    settingsLayout->addWidget(d->timeZoneCB,     row, 1, 1, 1);
    row++;
    settingsLayout->addWidget(d->offsetEnabled,  row, 0, 1, 1);
    settingsLayout->addWidget(offsetWidget,      row, 1, 1, 1);
    row++;
    settingsLayout->addWidget(d->interpolateBox, row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->maxTimeLabel,   row, 0, 1, 1);
    settingsLayout->addWidget(d->maxTimeInput,   row, 1, 1, 1);
    row++;
    settingsLayout->addWidget(d->correlateButton,row, 0, 1, 1);
    settingsLayout->setSpacing(d->spacingHint);
    settingsLayout->setMargin(d->marginHint);

    settingsLayout->setRowStretch(row, 100);

    connect(d->gpxLoadFilesButton, SIGNAL(clicked()),
            this, SLOT(slotLoadGPXFiles()));

    connect(d->correlateButton, SIGNAL(clicked()),
            this, SLOT(slotCorrelate()));

    connect(d->offsetEnabled, SIGNAL(stateChanged(int)),
            this, SLOT(updateUIState()));

    connect(d->timeZoneGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateUIState()));

    updateUIState();
}

GPSCorrelatorWidget::~GPSCorrelatorWidget()
{
    delete d;
}


void GPSCorrelatorWidget::slotLoadGPXFiles()
{
    const KUrl::List gpxFiles = KFileDialog::getOpenUrls(d->gpxFileOpenLastDirectory,
                                                        i18n("%1|GPS Exchange Format", QString("*.gpx")), this,
                                                        i18n("Select GPX File to Load") );

    if (gpxFiles.isEmpty())
        return;

    d->gpxFileOpenLastDirectory = gpxFiles.first().upUrl();

    setUIEnabledInternal(false);

    d->gpsDataParser->loadGPXFiles(gpxFiles);
}

void GPSCorrelatorWidget::slotGPXFilesReadyAt(int beginIndex, int endIndex)
{
    // note that endIndex is exclusive!
    for (int i=beginIndex; i<endIndex; ++i)
    {
        const GPSDataParser::GPXFileData& gpxData = d->gpsDataParser->fileData(i);

        if (!gpxData.isValid)
            continue;

        QTreeWidgetItem* const treeItem = new QTreeWidgetItem(d->gpxFileList);
        treeItem->setText(0, gpxData.url.fileName());
        // TODO: use KDE number formatting
        treeItem->setText(1, QString::number(gpxData.nPoints));
    }
}

void GPSCorrelatorWidget::slotAllGPXFilesReady()
{
    // are there any invalid files?
    QStringList invalidFiles;
    const QList<QPair<KUrl, QString> > loadErrorFiles = d->gpsDataParser->readLoadErrors();
    for (int i=0; i<loadErrorFiles.count(); ++i)
    {
        const QPair<KUrl, QString> currentError = loadErrorFiles.at(i);
        const QString fileErrorString = QString("%1: %2").arg(currentError.first.toLocalFile()).arg(currentError.second);

        invalidFiles << fileErrorString;
    }

    if (!invalidFiles.isEmpty())
    {
        const QString errorString = i18np(
                "The following GPX file could not be loaded:",
                "The following %1 GPX files could not be loaded:",
                invalidFiles.count()
            );

        const QString errorTitleString = i18np(
                "Error loading GPX file",
                "Error loading GPX files",
                invalidFiles.count()
            );

        KMessageBox::errorList(this, errorString, invalidFiles, errorTitleString);
    }

    setUIEnabledInternal(true);
}

void GPSCorrelatorWidget::setUIEnabledInternal(const bool state)
{
    d->uiEnabledInternal = state;
    updateUIState();
}

void GPSCorrelatorWidget::setUIEnabledExternal(const bool state)
{
    d->uiEnabledExternal = state;
    updateUIState();
}

void GPSCorrelatorWidget::updateUIState()
{
    const bool state = d->uiEnabledInternal && d->uiEnabledExternal;

    d->gpxLoadFilesButton->setEnabled(state);
    d->timeZoneSystem->setEnabled(state);
    d->timeZoneManual->setEnabled(state);
    d->timeZoneCB->setEnabled(state && d->timeZoneManual->isChecked());
    d->offsetEnabled->setEnabled(state);
    const bool offsetEnabled = d->offsetEnabled->isChecked();
    d->offsetSign->setEnabled(state && offsetEnabled);
    d->offsetMin->setEnabled(state && offsetEnabled);
    d->offsetSec->setEnabled(state && offsetEnabled);
    d->maxGapInput->setEnabled(state);
    d->interpolateBox->setEnabled(state);
    d->maxTimeInput->setEnabled(state && d->interpolateBox->isChecked());

    bool haveValidGpxFiles = false;
    for (int i=0; i<d->gpsDataParser->fileCount(); ++i)
    {
        haveValidGpxFiles = d->gpsDataParser->fileData(i).isValid;
        if (haveValidGpxFiles)
            break;
    }
    d->correlateButton->setEnabled(state && haveValidGpxFiles);
}

void GPSCorrelatorWidget::slotCorrelate()
{
    // disable the UI of the entire dialog:
    emit(signalSetUIEnabled(false, this, SLOT(slotCancelCorrelation())));

    // store the options:
    GPSDataParser::GPXCorrelationOptions options;
    options.maxGapTime = d->maxGapInput->value();
    options.photosHaveSystemTimeZone = (d->timeZoneGroup->checkedId() == 1);
    if (!options.photosHaveSystemTimeZone)
    {
        const QString tz = d->timeZoneCB->currentText();
        const int hh     = QString(QString(tz[4])+QString(tz[5])).toInt();
        const int mm     = QString(QString(tz[7])+QString(tz[8])).toInt();
        int timeZoneOffset = hh*3600 + mm*60;
        if (tz[3] == QChar('-')) {
            timeZoneOffset = (-1) * timeZoneOffset;
        }

        options.secondsOffset+= timeZoneOffset;
    }

    if (d->offsetEnabled->isChecked())
    {
        int userOffset = d->offsetMin->value() * 60 + d->offsetSec->value();
        if (d->offsetSign->currentText() == "-") {
            userOffset = (-1) * userOffset;
        }
        options.secondsOffset+=userOffset;
    }
    options.interpolate = d->interpolateBox->isChecked();
    options.interpolationDstTime = d->maxTimeInput->value()*60;

    // create a list of items to be correlated
    GPSDataParser::GPXCorrelation::List itemList;

    const int imageCount = d->imageModel->rowCount();
    for (int i = 0; i<imageCount; ++i)
    {
        QPersistentModelIndex imageIndex = d->imageModel->index(i, 0);
        GPSImageItem* const imageItem = reinterpret_cast<GPSImageItem*>(d->imageModel->itemFromIndex(imageIndex));
        if (!imageItem)
            continue;

        GPSDataParser::GPXCorrelation correlationItem;
        correlationItem.userData = QVariant::fromValue(imageIndex);
        correlationItem.dateTime = imageItem->dateTime();

        itemList << correlationItem;
    }

    d->correlationTotalCount = imageCount;
    d->correlationCorrelatedCount = 0;
    d->correlationTriedCount = 0;
    d->correlationUndoCommand = new GPSUndoCommand;
    emit(signalProgressSetup(imageCount, i18n("Correlating images - %p%")));

    d->gpsDataParser->correlate(itemList, options);

    // results will be sent to slotItemsCorrelated and slotAllItemsCorrelated
}

void GPSCorrelatorWidget::slotItemsCorrelated(const KIPIGPSSyncPlugin::GPSDataParser::GPXCorrelation::List& correlatedItems)
{
    kDebug()<<correlatedItems.count();
    d->correlationTriedCount+=correlatedItems.count();
    for (int i=0; i<correlatedItems.count(); ++i)
    {
        const GPSDataParser::GPXCorrelation& itemCorrelation = correlatedItems.at(i);

        const QPersistentModelIndex itemIndex = itemCorrelation.userData.value<QPersistentModelIndex>();
        if (!itemIndex.isValid())
            continue;

        GPSImageItem* const imageItem = reinterpret_cast<GPSImageItem*>(d->imageModel->itemFromIndex(itemIndex));
        if (!imageItem)
            continue;

        if (itemCorrelation.flags&GPSDataParser::GPXFlagCoordinates)
        {
            d->correlationCorrelatedCount++;

            const GPSDataContainer oldData = imageItem->gpsData();
            GPSDataContainer newData = oldData;
            newData.setCoordinates(itemCorrelation.coordinates);
            imageItem->setGPSData(newData);

            d->correlationUndoCommand->addUndoInfo(GPSUndoCommand::UndoInfo(itemIndex, oldData, newData));
        }
    }

    emit(signalProgressChanged(d->correlationTriedCount));
}

void GPSCorrelatorWidget::slotAllItemsCorrelated()
{
    if (d->correlationCorrelatedCount==0)
    {
        KMessageBox::sorry(this,
                           i18n("Could not correlate any image - please make sure the timezone and gap settings are correct."),
                           i18n("Correlation failed"));
    }
    else if (d->correlationCorrelatedCount==d->correlationTotalCount)
    {
        KMessageBox::information(this,
                                 i18n("All images have been correlated. You can now check their position on the map."),
                                 i18n("Correlation succeeded"));
    }
    else
    {
        // note: no need for i18np here, because the case of correlationTotalCount==1 is covered in the other two cases.
        KMessageBox::sorry(this,
                           i18n("%1 out of %2 images have been correlated. Please check the timezone and gap settings if you think that more images should have been correlated.", d->correlationCorrelatedCount, d->correlationTotalCount),
                           i18n("Correlation finished"));
    }

    if (d->correlationCorrelatedCount==0)
    {
        delete d->correlationUndoCommand;
    }
    else
    {
        d->correlationUndoCommand->setText(i18np("1 image correlated",
                                                 "%1 images correlated",
                                                 d->correlationCorrelatedCount));
        emit(signalUndoCommand(d->correlationUndoCommand));
    }

    // enable the UI:
    emit(signalSetUIEnabled(true));
}

void GPSCorrelatorWidget::saveSettingsToGroup(KConfigGroup* const group)
{
    group->writeEntry("Max Gap Time", d->maxGapInput->value() );
    group->writeEntry("Time Zone Mode", d->timeZoneGroup->checkedId() );
    group->writeEntry("Time Zone", d->timeZoneCB->currentIndex() );
    group->writeEntry("Interpolate", d->interpolateBox->isChecked() );
    group->writeEntry("Max Inter Dist Time", d->maxTimeInput->value() );
    group->writeEntry("Offset Enabled", d->offsetEnabled->isChecked());
    group->writeEntry("Offset Sign", d->offsetSign->currentIndex());
    group->writeEntry("Offset Min", d->offsetMin->value());
    group->writeEntry("Offset Sec", d->offsetSec->value());
    group->writeEntry("GPX File Open Last Directory", d->gpxFileOpenLastDirectory);
}

void GPSCorrelatorWidget::readSettingsFromGroup(KConfigGroup* const group)
{
    d->maxGapInput->setValue(group->readEntry("Max Gap Time", 30));
    const int timeZoneGroupIndex = qMax(1, qMin(2, group->readEntry("Time Zone Mode", 1)));
    d->timeZoneGroup->button(timeZoneGroupIndex)->setChecked(true);
    d->timeZoneCB->setCurrentIndex(group->readEntry("Time Zone", 16));  // GMT+00:00
    d->interpolateBox->setChecked(group->readEntry("Interpolate", false));
    d->maxTimeInput->setValue(group->readEntry("Max Inter Dist Time", 15));
    d->offsetEnabled->setChecked(group->readEntry("Offset Enabled", false));
    d->offsetSign->setCurrentIndex(group->readEntry("Offset Sign", 0));
    d->offsetMin->setValue(group->readEntry("Offset Min", 0));
    d->offsetSec->setValue(group->readEntry("Offset Sec", 0));
    d->gpxFileOpenLastDirectory = group->readEntry("GPX File Open Last Directory", KGlobalSettings::documentPath());
    d->maxTimeLabel->setEnabled(d->interpolateBox->isChecked());
    d->maxTimeInput->setEnabled(d->interpolateBox->isChecked());

    updateUIState();
}

void GPSCorrelatorWidget::slotCancelCorrelation()
{
    d->gpsDataParser->cancelCorrelation();
}

void GPSCorrelatorWidget::slotCorrelationCanceled()
{
    d->correlationUndoCommand->undo();

    delete d->correlationUndoCommand;

    emit(signalSetUIEnabled(true));
}

} /* KIPIGPSSyncPlugin */

