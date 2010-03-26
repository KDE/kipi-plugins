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

#include <qtconcurrentmap.h>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDomDocument>
#include <QFile>
#include <QFuture>
#include <QFutureWatcher>
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

#include "gpsdataparser_time.h"

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
      offsetSign(0),
      offsetMin(0),
      offsetSec(0),
      interpolateBox(0),
      maxGapInput(0),
      maxTimeInput(0)
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
    KComboBox                *offsetSign;
    KIntSpinBox              *offsetMin;
    KIntSpinBox              *offsetSec;

    QCheckBox                *interpolateBox;

    KIntSpinBox              *maxGapInput;
    KIntSpinBox              *maxTimeInput;

    QFutureWatcher<ParsedGPXData> *gpxLoadFutureWatcher;
    QFuture<ParsedGPXData>         gpxLoadFuture;
    QList<ParsedGPXData>           gpxData;
};

GPSCorrelatorWidget::GPSCorrelatorWidget(QWidget* const parent, const int marginHint, const int spacingHint)
: QWidget(parent), d(new GPSCorrelatorWidgetPrivate(marginHint, spacingHint))
{
    QVBoxLayout* const vboxlayout = new QVBoxLayout(this);
    setLayout(vboxlayout);

    QGridLayout *settingsLayout = new QGridLayout(this);
    vboxlayout->addLayout(settingsLayout);
    vboxlayout->addStretch(10);

    d->gpxLoadFilesButton = new QPushButton(i18n("Load GPX files..."), this);

    d->gpxFileList = new QTreeWidget(this);
    d->gpxFileList->setColumnCount(2);
    QStringList gpxHeaderLabels;
    gpxHeaderLabels << i18n("Filename") << i18n("#points");
    d->gpxFileList->setHeaderLabels(gpxHeaderLabels);

    KSeparator *line     = new KSeparator(Qt::Horizontal, this);

    QLabel *maxGapLabel = new QLabel(i18n("Max. time gap (sec.):"), this);
    d->maxGapInput      = new KIntSpinBox(0, 1000000, 1, 30, this);
    d->maxGapInput->setWhatsThis(i18n("Sets the maximum difference in "
                    "seconds from a GPS track point to the image time to be matched. "
                    "If the time difference exceeds this setting, no match will be attempted."));

    QLabel *timeZoneLabel = new QLabel(i18n("Camera time zone:"), this);
    d->timeZoneSystem     = new QRadioButton(i18n("Same as system"), this);
    d->timeZoneSystem->setWhatsThis(i18n(
                    "Use this option if the timezone of the camera "
                    "is the same as the timezone of this system. "
                    "The conversion to GMT will be done automatically."));
    d->timeZoneManual     = new QRadioButton(i18nc("manual time zone selection for gps syncing", "Manual:"), this);
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
    QLabel *offsetLabel = new QLabel(i18n("Fine offset (mm:ss):"), this);
    offsetLabel->setWhatsThis(i18n(
                        "Sets an additional offset in minutes and "
                        "seconds that is used to correlate the photos "
                        "to the GPS track. "
                        "This can be used for fine tuning to adjust a "
                        "wrong camera clock."));

    QWidget *offsetWidget = new QWidget(this);
    d->offsetSign = new KComboBox(offsetWidget);
    d->offsetSign->addItem("+");
    d->offsetSign->addItem("-");
    d->offsetSign->setWhatsThis(i18n("Set whether the camera offset "
        "is negative or positive."));

    d->offsetMin = new KIntSpinBox(0, 59, 1, 0, offsetWidget);
    d->offsetMin->setWhatsThis(i18n("Minutes to fine tune camera offset."));

    d->offsetSec = new KIntSpinBox(0, 59, 1, 0, offsetWidget);
    d->offsetSec->setWhatsThis(i18n("Seconds to fine tune camera offset."));

    QGridLayout *offsetLayout = new QGridLayout(offsetWidget);
    offsetLayout->addWidget(d->offsetSign, 0, 0, 1, 1);
    offsetLayout->addWidget(d->offsetMin, 0, 1, 1, 1);
    offsetLayout->addWidget(d->offsetSec, 0, 2, 1, 1);
    offsetLayout->setSpacing(d->spacingHint);
    offsetLayout->setMargin(d->marginHint);

    // interpolation options
    d->interpolateBox = new QCheckBox(i18n("Interpolate"), this);
    d->interpolateBox->setWhatsThis(i18n("Set this option to interpolate GPS track points "
                    "which are not closely matched to the GPX data file."));

    d->maxTimeLabel = new QLabel(i18n("Difference in min.:"), this);
    d->maxTimeInput = new KIntSpinBox(0, 240, 1, 15, this);
    d->maxTimeInput->setWhatsThis(i18n("Sets the maximum time difference in minutes (240 max.)"
                    " to interpolate GPX file points to image time data."));

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
    settingsLayout->addWidget(offsetLabel,       row, 0, 1, 1);
    settingsLayout->addWidget(offsetWidget,      row, 1, 1, 1);
    row++;
    settingsLayout->addWidget(d->interpolateBox, row, 0, 1, 2);
    row++;
    settingsLayout->addWidget(d->maxTimeLabel,   row, 0, 1, 1);
    settingsLayout->addWidget(d->maxTimeInput,   row, 1, 1, 1);
    settingsLayout->setSpacing(d->spacingHint);
    settingsLayout->setMargin(d->marginHint);

    settingsLayout->setRowStretch(row, 100);

    connect(d->gpxLoadFilesButton, SIGNAL(clicked()),
            this, SLOT(slotLoadGPXFiles()));
}

GPSCorrelatorWidget::~GPSCorrelatorWidget()
{
    delete d;
}

static ParsedGPXData LoadGPXFile(const KUrl& url)
{
    // TODO: store some kind of error message
    ParsedGPXData parsedData;
    parsedData.url = url;
    parsedData.isValid = false;

    QFile gpxfile(url.path());

    if (!gpxfile.open(QIODevice::ReadOnly))
        return parsedData;

    QDomDocument gpxDoc("gpx");
    if (!gpxDoc.setContent(&gpxfile))
        return parsedData;

    QDomElement gpxDocElem = gpxDoc.documentElement();
    if (gpxDocElem.tagName()!="gpx")
        return parsedData;

    for (QDomNode nTrk = gpxDocElem.firstChild();
         !nTrk.isNull(); nTrk = nTrk.nextSibling())
    {
        QDomElement trkElem = nTrk.toElement();
        if (trkElem.isNull()) continue;
        if (trkElem.tagName() != "trk") continue;

        for (QDomNode nTrkseg = trkElem.firstChild();
            !nTrkseg.isNull(); nTrkseg = nTrkseg.nextSibling())
        {
            QDomElement trksegElem = nTrkseg.toElement();
            if (trksegElem.isNull()) continue;
            if (trksegElem.tagName() != "trkseg") continue;

            for (QDomNode nTrkpt = trksegElem.firstChild();
                !nTrkpt.isNull(); nTrkpt = nTrkpt.nextSibling())
            {
                QDomElement trkptElem = nTrkpt.toElement();
                if (trkptElem.isNull()) continue;
                if (trkptElem.tagName() != "trkpt") continue;

                QDateTime ptDateTime;
                WMW2::WMWGeoCoordinate coordinates;

                // Get GPS position. If not available continue to next point.
                QString lat = trkptElem.attribute("lat");
                QString lon = trkptElem.attribute("lon");
                if (lat.isEmpty() || lon.isEmpty()) continue;

                coordinates.setLatLon(lat.toDouble(), lon.toDouble());

                // Get metadata of track point (altitude and time stamp)
                for (QDomNode nTrkptMeta = trkptElem.firstChild();
                    !nTrkptMeta.isNull(); nTrkptMeta = nTrkptMeta.nextSibling())
                {
                    QDomElement trkptMetaElem = nTrkptMeta.toElement();
                    if (trkptMetaElem.isNull()) continue;
                    if (trkptMetaElem.tagName() == QString("time"))
                    {
                        // Get GPS point time stamp. If not available continue to next point.
                        const QString time = trkptMetaElem.text();
                        if (time.isEmpty()) continue;
                        ptDateTime = GPSDataParserParseTime(time);
                    }
                    if (trkptMetaElem.tagName() == QString("ele"))
                    {
                        // Get GPS point altitude. If not available continue to next point.
                        QString ele = trkptMetaElem.text();
                        if (!ele.isEmpty())
                            coordinates.setAlt(ele.toDouble());
                    }
                }

                if (ptDateTime.isNull())
                    continue;

                parsedData.gpxDataMap.insert(ptDateTime, coordinates);
                parsedData.nPoints++;

            }
        }
    }

//     for (int i=0; i<60000; ++i)
//     {
//         parsedData.gpxDataMap.insert(QDateTime(), WMW2::WMWGeoCoordinate());
//         parsedData.nPoints++;
//     }

    parsedData.isValid = parsedData.nPoints > 0;
    return parsedData;
}

void GPSCorrelatorWidget::slotLoadGPXFiles()
{
    const KUrl::List gpxFiles = KFileDialog::getOpenUrls(d->gpxFileOpenLastDirectory,
                                                        i18n("%1|GPS Exchange Format", QString("*.gpx")), this,
                                                        i18n("Select GPX File to Load") );

    if (gpxFiles.isEmpty())
        return;

    d->gpxFileOpenLastDirectory = gpxFiles.first().upUrl();

    emit(signalSetUIEnabled(false));

    d->gpxLoadFutureWatcher = new QFutureWatcher<ParsedGPXData>(this);

    connect(d->gpxLoadFutureWatcher, SIGNAL(resultsReadyAt(int, int)),
            this, SLOT(slotGPXFileReadyAt(int, int)));

    d->gpxLoadFuture = QtConcurrent::mapped(gpxFiles, LoadGPXFile);
    d->gpxLoadFutureWatcher->setFuture(d->gpxLoadFuture);

    // results are reported to slotGPXFileReadyAt
}

void GPSCorrelatorWidget::slotGPXFileReadyAt(int beginIndex, int endIndex)
{
    for (int i=beginIndex; i<endIndex; ++i)
    {
        d->gpxData << d->gpxLoadFuture.resultAt(i);

        QTreeWidgetItem* const treeItem = new QTreeWidgetItem(d->gpxFileList);
        treeItem->setText(0, d->gpxData.last().url.fileName());
        // TODO: use KDE number formatting
        treeItem->setText(1, QString::number(d->gpxData.last().nPoints));
    }

    // are all files done?
    if (d->gpxLoadFuture.progressMaximum() == d->gpxData.count() )
    {
        d->gpxLoadFutureWatcher->deleteLater();
        emit(signalSetUIEnabled(true));
    }
}

void GPSCorrelatorWidget::setUIEnabled(const bool state)
{
    d->gpxLoadFilesButton->setEnabled(state);
    d->timeZoneSystem->setEnabled(state);
    d->timeZoneManual->setEnabled(state);
    d->timeZoneCB->setEnabled(state);
    d->offsetSign->setEnabled(state);
    d->offsetMin->setEnabled(state);
    d->offsetSec->setEnabled(state);
    d->interpolateBox->setEnabled(state);
    d->maxGapInput->setEnabled(state);
    d->maxTimeInput->setEnabled(state);
}

} /* KIPIGPSSyncPlugin */

