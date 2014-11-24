/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-03-26
 * @brief  A widget to configure the GPS correlation
 *
 * @author Copyright (C) 2010, 2014 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 * @author Copyright (C) 2014 by Justus Schwartz
 *         <a href="mailto:justus at gmx dot li">justus at gmx dot li</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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
#include <QTreeView>
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

#include "kipiimagemodel.h"
#include "kipiimageitem.h"
#include "gpsundocommand.h"
#include "track_listmodel.h"

namespace KIPIGPSSyncPlugin
{

class GPSCorrelatorWidget::Private
{
public:

    Private(const int pMarginHint, const int pSpacingHint)
      : marginHint(pMarginHint),
        spacingHint(pSpacingHint),
        gpxLoadFilesButton(0),
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
        showTracksOnMap(0),
        maxGapInput(0),
        maxTimeInput(0),
        correlateButton(0),
        trackManager(0),
        trackCorrelator(0),
        trackListModel(0),
        uiEnabledInternal(true),
        uiEnabledExternal(true),
        imageModel(0),
        correlationTotalCount(0),
        correlationCorrelatedCount(0),
        correlationTriedCount(0),
        correlationUndoCommand(0)
    {
    }

    int                     marginHint;
    int                     spacingHint;

    KUrl                    gpxFileOpenLastDirectory;
    QPushButton*            gpxLoadFilesButton;
    QTreeView*              gpxFileList;
    QLabel*                 maxTimeLabel;

    QButtonGroup*           timeZoneGroup;
    QRadioButton*           timeZoneSystem;
    QRadioButton*           timeZoneManual;
    KComboBox*              timeZoneCB;
    QCheckBox*              offsetEnabled;
    KComboBox*              offsetSign;
    KIntSpinBox*            offsetMin;
    KIntSpinBox*            offsetSec;

    QCheckBox*              interpolateBox;
    QCheckBox*              showTracksOnMap;

    KIntSpinBox*            maxGapInput;
    KIntSpinBox*            maxTimeInput;

    QPushButton*            correlateButton;

    KGeoMap::TrackManager*  trackManager;
    TrackCorrelator*        trackCorrelator;
    TrackListModel*         trackListModel;
    bool                    uiEnabledInternal;
    bool                    uiEnabledExternal;
    KipiImageModel*         imageModel;

    int                     correlationTotalCount;
    int                     correlationCorrelatedCount;
    int                     correlationTriedCount;
    GPSUndoCommand*         correlationUndoCommand;
};

GPSCorrelatorWidget::GPSCorrelatorWidget(QWidget* const parent, KipiImageModel* const imageModel, KGeoMap::TrackManager* const trackManager, const int marginHint, const int spacingHint)
    : QWidget(parent), d(new Private(marginHint, spacingHint))
{
    d->imageModel = imageModel;
    d->trackManager = trackManager;
    d->trackCorrelator = new TrackCorrelator(d->trackManager, this);
    d->trackListModel = new TrackListModel(d->trackManager, this);

    connect(d->trackManager, SIGNAL(signalAllTrackFilesReady()),
            this, SLOT(slotAllTrackFilesReady()));

    connect(d->trackCorrelator, SIGNAL(signalItemsCorrelated(KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List)),
            this, SLOT(slotItemsCorrelated(KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List)));

    connect(d->trackCorrelator, SIGNAL(signalAllItemsCorrelated()),
            this, SLOT(slotAllItemsCorrelated()));

    connect(d->trackCorrelator, SIGNAL(signalCorrelationCanceled()),
            this, SLOT(slotCorrelationCanceled()));

    QVBoxLayout* const vboxlayout = new QVBoxLayout(this);
    setLayout(vboxlayout);

    QGridLayout* const settingsLayout = new QGridLayout(this);
    vboxlayout->addLayout(settingsLayout);
    vboxlayout->addStretch(10);

    d->gpxLoadFilesButton = new QPushButton(i18n("Load GPX files..."), this);

    d->gpxFileList = new QTreeView(this);
    d->gpxFileList->setModel(d->trackListModel);
    d->gpxFileList->setHeaderHidden(false);
    d->gpxFileList->setRootIsDecorated(false);

    KSeparator* const line    = new KSeparator(Qt::Horizontal, this);
    QLabel* const maxGapLabel = new QLabel(i18n("Max. time gap (sec.):"), this);
    d->maxGapInput            = new KIntSpinBox(0, 1000000, 1, 30, this);
    d->maxGapInput->setWhatsThis(i18n("Sets the maximum difference in "
                    "seconds from a GPS track point to the image time to be matched. "
                    "If the time difference exceeds this setting, no match will be attempted."));

    QLabel* const timeZoneLabel = new QLabel(i18n("Camera time zone:"), this);
    d->timeZoneSystem           = new QRadioButton(i18n("Same as system"), this);
    d->timeZoneSystem->setWhatsThis(i18n(
                    "Use this option if the timezone of the camera "
                    "is the same as the timezone of this system. "
                    "The conversion to GMT will be done automatically."));
    d->timeZoneManual           = new QRadioButton(i18nc("manual time zone selection for gps syncing", "Manual:"), this);
    d->timeZoneManual->setWhatsThis(i18n(
                    "Use this option if the timezone of the camera "
                    "is different from this system and you have to "
                    "specify the difference to GMT manually."));
    d->timeZoneGroup            = new QButtonGroup(this);
    d->timeZoneGroup->addButton(d->timeZoneSystem, 1);
    d->timeZoneGroup->addButton(d->timeZoneManual, 2);

    d->timeZoneCB               = new KComboBox(this);

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
    d->timeZoneCB->addItem("GMT+05:30");    // See bug # 149491
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
    d->offsetSign               = new KComboBox(offsetWidget);
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

    d->showTracksOnMap = new QCheckBox(i18n("Show tracks on Map"), this);
    d->showTracksOnMap->setWhatsThis(i18n("Set this option to show tracks on the Map"));

    connect(d->showTracksOnMap, SIGNAL(stateChanged(int)),
            this, SLOT(slotShowTracksStateChanged(int)));
    
    d->maxTimeLabel = new QLabel(i18n("Difference in min.:"), this);
    d->maxTimeInput = new KIntSpinBox(0, 240, 1, 15, this);
    d->maxTimeInput->setWhatsThis(i18n("Sets the maximum time difference in minutes (240 max.)"
                    " to interpolate GPX file points to image time data."));

    d->correlateButton = new QPushButton(i18n("Correlate"), this);

    // layout form
    int row = 0;
    settingsLayout->addWidget(d->gpxLoadFilesButton, row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->gpxFileList,        row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(line,                  row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(maxGapLabel,           row, 0, 1, 1);
    settingsLayout->addWidget(d->maxGapInput,        row, 1, 1, 1);
    row++;
    settingsLayout->addWidget(timeZoneLabel,         row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->timeZoneSystem,     row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->timeZoneManual,     row, 0, 1, 1);
    settingsLayout->addWidget(d->timeZoneCB,         row, 1, 1, 1);
    row++;
    settingsLayout->addWidget(d->offsetEnabled,      row, 0, 1, 1);
    settingsLayout->addWidget(offsetWidget,          row, 1, 1, 1);
    row++;
    settingsLayout->addWidget(d->interpolateBox,     row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->showTracksOnMap,    row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->maxTimeLabel,       row, 0, 1, 1);
    settingsLayout->addWidget(d->maxTimeInput,       row, 1, 1, 1);
    row++;
    settingsLayout->addWidget(d->correlateButton,    row, 0, 1, 1);
    settingsLayout->setSpacing(d->spacingHint);
    settingsLayout->setMargin(d->marginHint);

    settingsLayout->setRowStretch(row, 100);

    connect(d->gpxLoadFilesButton, SIGNAL(clicked()),
            this, SLOT(slotLoadTrackFiles()));

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

}

void GPSCorrelatorWidget::slotLoadTrackFiles()
{
    const KUrl::List gpxFiles = KFileDialog::getOpenUrls(d->gpxFileOpenLastDirectory,
                                                        i18n("%1|GPS Exchange Format", QString("*.gpx")), this,
                                                        i18n("Select GPX File to Load") );

    if (gpxFiles.isEmpty())
        return;

    d->gpxFileOpenLastDirectory = gpxFiles.first().upUrl();

    setUIEnabledInternal(false);

    d->trackManager->loadTrackFiles(gpxFiles);
}

void GPSCorrelatorWidget::slotAllTrackFilesReady()
{
    // are there any invalid files?
    QStringList invalidFiles;
    const QList<QPair<KUrl, QString> > loadErrorFiles = d->trackManager->readLoadErrors();

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

    emit(signalAllTrackFilesReady());
    
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

    const bool haveValidGpxFiles = d->trackManager->trackCount()>0;
    d->correlateButton->setEnabled(state && haveValidGpxFiles);
}

void GPSCorrelatorWidget::slotCorrelate()
{
    // disable the UI of the entire dialog:
    emit(signalSetUIEnabled(false, this, SLOT(slotCancelCorrelation())));

    // store the options:
    TrackCorrelator::CorrelationOptions options;
    options.maxGapTime               = d->maxGapInput->value();
    options.photosHaveSystemTimeZone = (d->timeZoneGroup->checkedId() == 1);

    if (!options.photosHaveSystemTimeZone)
    {
        const QString tz   = d->timeZoneCB->currentText();
        const int hh       = QString(QString(tz[4])+QString(tz[5])).toInt();
        const int mm       = QString(QString(tz[7])+QString(tz[8])).toInt();
        int timeZoneOffset = hh*3600 + mm*60;

        if (tz[3] == QChar('-'))
        {
            timeZoneOffset = (-1) * timeZoneOffset;
        }

        options.secondsOffset+= timeZoneOffset;
    }

    if (d->offsetEnabled->isChecked())
    {
        int userOffset = d->offsetMin->value() * 60 + d->offsetSec->value();

        if (d->offsetSign->currentText() == "-")
        {
            userOffset = (-1) * userOffset;
        }

        options.secondsOffset+=userOffset;
    }

    options.interpolate          = d->interpolateBox->isChecked();
    options.interpolationDstTime = d->maxTimeInput->value()*60;

    // create a list of items to be correlated
    TrackCorrelator::Correlation::List itemList;

    const int imageCount = d->imageModel->rowCount();

    for (int i = 0; i<imageCount; ++i)
    {
        QPersistentModelIndex imageIndex = d->imageModel->index(i, 0);
        KipiImageItem* const imageItem   = d->imageModel->itemFromIndex(imageIndex);

        if (!imageItem)
            continue;

        TrackCorrelator::Correlation correlationItem;
        correlationItem.userData = QVariant::fromValue(imageIndex);
        correlationItem.dateTime = imageItem->dateTime();

        itemList << correlationItem;
    }

    d->correlationTotalCount      = imageCount;
    d->correlationCorrelatedCount = 0;
    d->correlationTriedCount      = 0;
    d->correlationUndoCommand     = new GPSUndoCommand;

    emit(signalProgressSetup(imageCount, i18n("Correlating images - %p%")));

    d->trackCorrelator->correlate(itemList, options);

    // results will be sent to slotItemsCorrelated and slotAllItemsCorrelated
}

void GPSCorrelatorWidget::slotItemsCorrelated(const KIPIGPSSyncPlugin::TrackCorrelator::Correlation::List& correlatedItems)
{
    kDebug()<<correlatedItems.count();
    d->correlationTriedCount+=correlatedItems.count();

    for (int i=0; i<correlatedItems.count(); ++i)
    {
        const TrackCorrelator::Correlation& itemCorrelation = correlatedItems.at(i);
        const QPersistentModelIndex itemIndex                = itemCorrelation.userData.value<QPersistentModelIndex>();

        if (!itemIndex.isValid())
            continue;

        KipiImageItem* const imageItem                       = d->imageModel->itemFromIndex(itemIndex);

        if (!imageItem)
            continue;

        if (itemCorrelation.flags&TrackCorrelator::CorrelationFlagCoordinates)
        {
            d->correlationCorrelatedCount++;

            GPSDataContainer newData;
            newData.setCoordinates(itemCorrelation.coordinates);

            if (itemCorrelation.nSatellites>=0)
                newData.setNSatellites(itemCorrelation.nSatellites);

            // if hDop is available, use it
            if (itemCorrelation.hDop>=0)
                newData.setDop(itemCorrelation.hDop);

            // but if pDop is available, prefer pDop over hDop
            if (itemCorrelation.pDop>=0)
                newData.setDop(itemCorrelation.pDop);

            if (itemCorrelation.fixType>=0)
            {
                newData.setFixType(itemCorrelation.fixType);
            }
            if (itemCorrelation.speed>=0)
            {
                newData.setSpeed(itemCorrelation.speed);
            }

            GPSUndoCommand::UndoInfo undoInfo(itemIndex);
            undoInfo.readOldDataFromItem(imageItem);

            imageItem->setGPSData(newData);
            undoInfo.readNewDataFromItem(imageItem);

            d->correlationUndoCommand->addUndoInfo(undoInfo);
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
    group->writeEntry("ShowTracksOnMap", d->showTracksOnMap->isChecked() );
    group->writeEntry("Max Inter Dist Time", d->maxTimeInput->value() );
    group->writeEntry("Offset Enabled", d->offsetEnabled->isChecked());
    group->writeEntry("Offset Sign", d->offsetSign->currentIndex());
    group->writeEntry("Offset Min", d->offsetMin->value());
    group->writeEntry("Offset Sec", d->offsetSec->value());
    group->writeEntry("GPX File Open Last Directory", d->gpxFileOpenLastDirectory);
}

void GPSCorrelatorWidget::readSettingsFromGroup(const KConfigGroup* const group)
{
    d->maxGapInput->setValue(group->readEntry("Max Gap Time", 30));
    const int timeZoneGroupIndex = qMax(1, qMin(2, group->readEntry("Time Zone Mode", 1)));
    d->timeZoneGroup->button(timeZoneGroupIndex)->setChecked(true);
    d->timeZoneCB->setCurrentIndex(group->readEntry("Time Zone", 16));  // GMT+00:00
    d->interpolateBox->setChecked(group->readEntry("Interpolate", false));
    d->showTracksOnMap->setChecked(group->readEntry("ShowTracksOnMap", true));
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
    d->trackCorrelator->cancelCorrelation();
}

void GPSCorrelatorWidget::slotCorrelationCanceled()
{
    d->correlationUndoCommand->undo();

    delete d->correlationUndoCommand;

    emit(signalSetUIEnabled(true));
}

QList<KGeoMap::GeoCoordinates::List> GPSCorrelatorWidget::getTrackCoordinates() const
{
    QList<KGeoMap::GeoCoordinates::List> trackList;
  
    for (int i=0; i<d->trackManager->trackCount(); ++i)
    {
        const KGeoMap::TrackManager::Track& gpxData = d->trackManager->getTrack(i);

        KGeoMap::GeoCoordinates::List track;
        for (int coordIdx = 0; coordIdx < gpxData.points.count(); ++coordIdx)
        {
            KGeoMap::TrackManager::TrackPoint const& point = gpxData.points.at(coordIdx);
            track << point.coordinates;
        }

        trackList << track;
    }

    return trackList;
}
  
void GPSCorrelatorWidget::slotShowTracksStateChanged(int state)
{
    const bool doShowTracks = state == Qt::Checked;
    d->trackManager->setVisibility(doShowTracks);
}

bool GPSCorrelatorWidget::getShowTracksOnMap() const
{
    return d->showTracksOnMap->isChecked();
}

} /* namespace KIPIGPSSyncPlugin */
