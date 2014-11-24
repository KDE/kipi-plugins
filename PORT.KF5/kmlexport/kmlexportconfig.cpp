/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-05-16
 * Description : a tool to export GPS data to KML file.
 *
 * Copyright (C) 2006-2007 by Stephane Pontier <shadow dot walker at free dot fr>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kmlexportconfig.moc"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

// KDE includes

#include <kapplication.h>
#include <kcombobox.h>
#include <kcolorbutton.h>
#include <kconfig.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmenu.h>
#include <knuminput.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>

// Local includes

#include "kpaboutdata.h"
#include "kpversion.h"

namespace KIPIKMLExportPlugin
{

/**
 * @brief Constructs a KIPIKMLExport::KMLExportConfig which is a child of @parent.
 */
KMLExportConfig::KMLExportConfig(QWidget* const parent)
    : KPToolDialog(parent)
{
    setButtons(Help|Ok|Cancel);
    setDefaultButton(Ok);
    setCaption(i18n("KML Export"));
    setModal(true);

    QWidget* const page = new QWidget( this );
    setMainWidget( page );
    KMLExportConfigLayout = new QGridLayout(page);

    // --------------------------------------------------------------
    // Target preferences

    TargetPreferenceGroupBox       = new QGroupBox(i18n("Target Preferences" ), page);
    TargetPreferenceGroupBoxLayout = new QGridLayout(TargetPreferenceGroupBox);

    // target type
    TargetTypeGroupBox          = new QGroupBox(i18n("Target Type" ), page);
    buttonGroupTargetTypeLayout = new QGridLayout(TargetTypeGroupBox);
    buttonGroupTargetType       = new QButtonGroup(TargetTypeGroupBox);
    LocalTargetRadioButton_     = new QRadioButton( i18n( "&Local or web target used by GoogleEarth" ), TargetTypeGroupBox);
    LocalTargetRadioButton_->setChecked( true );

    GoogleMapTargetRadioButton_ = new QRadioButton( i18n( "Web target used by GoogleMaps" ), TargetTypeGroupBox);
    GoogleMapTargetRadioButton_->setToolTip(i18n("When using GoogleMaps, all images must have complete URLs, icons are "
                                                 "squared, and when drawing a track, only line track is exported." ) );

    buttonGroupTargetTypeLayout->addWidget( LocalTargetRadioButton_,     0, 0, 1, 1);
    buttonGroupTargetTypeLayout->addWidget( GoogleMapTargetRadioButton_, 1, 0, 1, 1);
    buttonGroupTargetTypeLayout->setAlignment( Qt::AlignTop );
    buttonGroupTargetTypeLayout->setSpacing(spacingHint());
    buttonGroupTargetTypeLayout->setMargin(0);

    // --------------------------------------------------------------
    // target preference, suite

    QLabel* const AltitudeLabel_ = new QLabel(i18n("Picture Altitude:" ), TargetPreferenceGroupBox);
    AltitudeCB_                  = new KComboBox( TargetPreferenceGroupBox );
    AltitudeCB_->addItem(i18n("clamp to ground"));
    AltitudeCB_->addItem(i18n("relative to ground"));
    AltitudeCB_->addItem(i18n("absolute"));
    AltitudeCB_->setWhatsThis(i18n("<p>Specifies how pictures are displayed"
                                   "<dl><dt>clamp to ground (default)</dt>"
                                   "<dd>Indicates to ignore an altitude specification</dd>"
                                   "<dt>relative to ground</dt>"
                                   "<dd>Sets the altitude of the element relative to the actual ground "
                                   "elevation of a particular location.</dd>"
                                   "<dt>absolute</dt>"
                                   "<dd>Sets the altitude of the coordinate relative to sea level, regardless "
                                   "of the actual elevation of the terrain beneath the element.</dd></dl>"));

    destinationDirectoryLabel_ = new QLabel(i18n("Destination Directory:"), TargetPreferenceGroupBox);

    // DestinationDirectory_ = new KLineEdit( TargetPreferenceGroupBox, "DestinationDirectory_" );
    DestinationDirectory_= new KUrlRequester(TargetPreferenceGroupBox);
    DestinationDirectory_->setWindowTitle(i18n("Select a directory in which to save the kml file and pictures"));
    DestinationDirectory_->setMode(KFile::Directory | KFile::LocalOnly );

    DestinationUrlLabel_ = new QLabel(i18n("Destination URL:"), TargetPreferenceGroupBox);
    DestinationUrl_      = new KLineEdit(TargetPreferenceGroupBox);
    FileNameLabel_       = new QLabel(i18n( "Filename:" ), TargetPreferenceGroupBox);
    FileName_            = new KLineEdit(TargetPreferenceGroupBox);

    TargetPreferenceGroupBoxLayout->addWidget(TargetTypeGroupBox,         0, 0, 2, 5);
    TargetPreferenceGroupBoxLayout->addWidget(AltitudeLabel_,             2, 0, 1, 5);
    TargetPreferenceGroupBoxLayout->addWidget(AltitudeCB_,                2, 2, 1, 3);
    TargetPreferenceGroupBoxLayout->addWidget(destinationDirectoryLabel_, 3, 0, 1, 3);
    TargetPreferenceGroupBoxLayout->addWidget(DestinationDirectory_,      3, 3, 1, 2);
    TargetPreferenceGroupBoxLayout->addWidget(DestinationUrlLabel_,       4, 0, 1, 2);
    TargetPreferenceGroupBoxLayout->addWidget(DestinationUrl_,            4, 2, 1, 3);
    TargetPreferenceGroupBoxLayout->addWidget(FileNameLabel_,             5, 0, 1, 1);
    TargetPreferenceGroupBoxLayout->addWidget(FileName_,                  5, 1, 1, 4);
    TargetPreferenceGroupBoxLayout->setAlignment(Qt::AlignTop);
    TargetPreferenceGroupBoxLayout->setSpacing(spacingHint());
    TargetPreferenceGroupBoxLayout->setMargin(0);

    // --------------------------------------------------------------
    // Sizes

    QGroupBox* const SizeGroupBox = new QGroupBox(i18n("Sizes" ), page);
    SizeGroupBoxLayout            = new QGridLayout(SizeGroupBox);
    IconSizeLabel                 = new QLabel(i18n("Icon Size:" ), SizeGroupBox);
    IconSizeInput_                = new KIntNumInput(SizeGroupBox);
    IconSizeInput_->setValue(33);

    ImageSizeLabel  = new QLabel(i18n("Image Size:"), SizeGroupBox);
    ImageSizeInput_ = new KIntNumInput(SizeGroupBox);
    ImageSizeInput_->setValue(320);

    SizeGroupBoxLayout->addWidget(IconSizeLabel,   0, 0, 1, 1);
    SizeGroupBoxLayout->addWidget(IconSizeInput_,  0, 1, 1, 1);
    SizeGroupBoxLayout->addWidget(ImageSizeLabel,  0, 2, 1, 1);
    SizeGroupBoxLayout->addWidget(ImageSizeInput_, 0, 3, 1, 1);
    SizeGroupBoxLayout->setAlignment(Qt::AlignTop);
    SizeGroupBoxLayout->setSpacing(spacingHint());
    SizeGroupBoxLayout->setMargin(0);

    // --------------------------------------------------------------
    // GPX Tracks

    QGroupBox* const GPXTracksGroupBox         = new QGroupBox(i18n("GPX Tracks"), page);
    QGridLayout* const GPXTracksGroupBoxLayout = new QGridLayout(GPXTracksGroupBox);

    // add a gpx track checkbox
    GPXTracksCheckBox_ = new QCheckBox(i18n("Draw GPX Track"), GPXTracksGroupBox);

    // file selector
    GPXFileLabel_ = new QLabel(i18n("GPX file:"), GPXTracksGroupBox);

    GPXFileKUrlRequester_ = new KUrlRequester( GPXTracksGroupBox);
    GPXFileKUrlRequester_->setFilter(i18n("%1|GPS Exchange Format",QString("*.gpx")));
    GPXFileKUrlRequester_->setWindowTitle(i18n("Select GPX File to Load"));

    timeZoneLabel_ = new QLabel(i18n("Time Zone:"), GPXTracksGroupBox);
    timeZoneCB     = new KComboBox(GPXTracksGroupBox );
    timeZoneCB->addItem(i18n("GMT-12:00"), 0);
    timeZoneCB->addItem(i18n("GMT-11:00"), 1);
    timeZoneCB->addItem(i18n("GMT-10:00"), 2);
    timeZoneCB->addItem(i18n("GMT-09:00"), 3);
    timeZoneCB->addItem(i18n("GMT-08:00"), 4);
    timeZoneCB->addItem(i18n("GMT-07:00"), 5);
    timeZoneCB->addItem(i18n("GMT-06:00"), 6);
    timeZoneCB->addItem(i18n("GMT-05:00"), 7);
    timeZoneCB->addItem(i18n("GMT-04:00"), 8);
    timeZoneCB->addItem(i18n("GMT-03:00"), 9);
    timeZoneCB->addItem(i18n("GMT-02:00"), 10);
    timeZoneCB->addItem(i18n("GMT-01:00"), 11);
    timeZoneCB->addItem(i18n("GMT"),       12);
    timeZoneCB->addItem(i18n("GMT+01:00"), 13);
    timeZoneCB->addItem(i18n("GMT+02:00"), 14);
    timeZoneCB->addItem(i18n("GMT+03:00"), 15);
    timeZoneCB->addItem(i18n("GMT+04:00"), 16);
    timeZoneCB->addItem(i18n("GMT+05:00"), 17);
    timeZoneCB->addItem(i18n("GMT+06:00"), 18);
    timeZoneCB->addItem(i18n("GMT+07:00"), 19);
    timeZoneCB->addItem(i18n("GMT+08:00"), 20);
    timeZoneCB->addItem(i18n("GMT+09:00"), 21);
    timeZoneCB->addItem(i18n("GMT+10:00"), 22);
    timeZoneCB->addItem(i18n("GMT+11:00"), 23);
    timeZoneCB->addItem(i18n("GMT+12:00"), 24);
    timeZoneCB->addItem(i18n("GMT+13:00"), 25);
    timeZoneCB->addItem(i18n("GMT+14:00"), 26);
    timeZoneCB->setWhatsThis(i18n("Sets the time zone of the camera during "
                                  "picture shooting, so that the time stamps of the GPS "
                                  "can be converted to match the local time"));

    GPXLineWidthLabel_ = new QLabel(i18n("Track Width:" ), GPXTracksGroupBox);
    GPXLineWidthInput_ = new KIntNumInput( GPXTracksGroupBox);
    GPXLineWidthInput_->setValue(4);

    GPXColorLabel_ = new QLabel(i18n("Track Color:" ), GPXTracksGroupBox);
    GPXTrackColor_ = new KColorButton(QColor("#ffffff"), GPXTracksGroupBox);

    GPXTracksOpacityInput_ = new KIntNumInput(GPXTracksGroupBox);
    GPXTracksOpacityInput_->setRange(0, 100, 1);
    GPXTracksOpacityInput_->setValue(100 );
    GPXTracksOpacityInput_->setLabel(i18n("Opacity:" ), Qt::AlignVCenter);
    GPXTracksOpacityInput_->setSuffix(QString::fromAscii("%"));

    GPXAltitudeLabel_ = new QLabel(i18n("Track Altitude:"), GPXTracksGroupBox);
    GPXAltitudeCB_    = new KComboBox(GPXTracksGroupBox);
    GPXAltitudeCB_->addItem(i18n("clamp to ground"));
    GPXAltitudeCB_->addItem(i18n("relative to ground"));
    GPXAltitudeCB_->addItem(i18n("absolute"));
    GPXAltitudeCB_->setWhatsThis(i18n("<p>Specifies how the points are displayed"
                                      "<dl><dt>clamp to ground (default)</dt>"
                                      "<dd>Indicates to ignore an altitude specification</dd>"
                                      "<dt>relative to ground</dt>"
                                      "<dd>Sets the altitude of the element relative to the actual ground "
                                      "elevation of a particular location.</dd>"
                                      "<dt>absolute</dt>"
                                      "<dd>Sets the altitude of the coordinate relative to sea level, "
                                      "regardless of the actual elevation of the terrain beneath "
                                      "the element.</dd></dl>"));

    GPXTracksGroupBoxLayout->addWidget( GPXTracksCheckBox_,     0, 0, 1, 4);
    GPXTracksGroupBoxLayout->addWidget( GPXFileLabel_,          1, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXFileKUrlRequester_,  1, 1, 1, 3);
    GPXTracksGroupBoxLayout->addWidget( timeZoneLabel_,         2, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( timeZoneCB,             2, 1, 1, 3);
    GPXTracksGroupBoxLayout->addWidget( GPXLineWidthLabel_,     3, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXLineWidthInput_,     3, 1, 1, 3);
    GPXTracksGroupBoxLayout->addWidget( GPXColorLabel_,         4, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXTrackColor_,         4, 1, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXTracksOpacityInput_, 4, 2, 1, 2);
    GPXTracksGroupBoxLayout->addWidget( GPXAltitudeLabel_,      5, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXAltitudeCB_,         5, 1, 1, 3);
    GPXTracksGroupBoxLayout->setAlignment( Qt::AlignTop );
    GPXTracksGroupBoxLayout->setSpacing(spacingHint());
    GPXTracksGroupBoxLayout->setMargin(0);

    // --------------------------------------------------------------

    KMLExportConfigLayout->addWidget( TargetPreferenceGroupBox, 0, 0);
    KMLExportConfigLayout->addWidget( SizeGroupBox,             1, 0);
    KMLExportConfigLayout->addWidget( GPXTracksGroupBox,        2, 0);
    KMLExportConfigLayout->setSpacing(spacingHint());
    KMLExportConfigLayout->setMargin(0);

    // --------------------------------------------------------------

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));

    connect(GoogleMapTargetRadioButton_, SIGNAL(toggled(bool)),
            this, SLOT(slotGoogleMapTargetRadioButtonToggled(bool)));

    connect(GPXTracksCheckBox_, SIGNAL(toggled(bool)),
            this, SLOT(slotKMLTracksCheckButtonToggled(bool)));

    // --------------------------------------------------------------
    // About data and help button.

    KPAboutData* const about = new KPAboutData(ki18n("KML Export"),
                                   0,
                                   KAboutData::License_GPL,
                                   ki18n("A Kipi plugin for KML exporting"),
                                   ki18n("(c) 2006-2007, Stéphane Pontier"));

    about->addAuthor(ki18n("Stéphane Pontier"),
                     ki18n("Developer and maintainer"),
                            "shadow dot walker at free dot fr");

    about->addAuthor(ki18n("Gilles Caulier"),
                     ki18n("Developer and maintainer"),
                           "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Michael G. Hansen"),
                     ki18n("Maintainer"),
                           "mike at mghansen dot de");

    about->setHandbookEntry("kmlexport");
    setAboutData(about);

    // --------------------------------------------------------------
    // Configuration file management

    readSettings();

    // --------------------------------------------------------------
    // Just to initialize the UI
    slotGoogleMapTargetRadioButtonToggled(true);
    slotKMLTracksCheckButtonToggled(false);
}

/**
 * @brief Destroys the object and frees any allocated resources
 */
KMLExportConfig::~KMLExportConfig()
{
}

void KMLExportConfig::slotOk()
{
    saveSettings();

    emit okButtonClicked();
    accept();
}

void KMLExportConfig::slotCancel()
{
    saveSettings();
    done(Close);
}

void KMLExportConfig::slotGoogleMapTargetRadioButtonToggled(bool)
{
    if (GoogleMapTargetRadioButton_->isChecked())
    {
        DestinationUrlLabel_->setEnabled(true);
        DestinationUrl_->setEnabled(true);
        IconSizeLabel->setEnabled(false);
        IconSizeInput_->setEnabled(false);
    }
    else
    {
        DestinationUrlLabel_->setEnabled(false);
        DestinationUrl_->setEnabled(false);
        IconSizeLabel->setEnabled(true);
        IconSizeInput_->setEnabled(true);
    }
}

void KMLExportConfig::slotKMLTracksCheckButtonToggled(bool)
{
    if (GPXTracksCheckBox_->isChecked())
    {
        GPXFileKUrlRequester_->setEnabled(true);
        GPXFileLabel_->setEnabled(true);
        timeZoneCB->setEnabled(true);
        GPXColorLabel_->setEnabled(true);
        GPXAltitudeLabel_->setEnabled(true);
        timeZoneLabel_->setEnabled(true);
        GPXAltitudeCB_->setEnabled(true);
        GPXTrackColor_->setEnabled(true);
        GPXLineWidthLabel_->setEnabled(true);
        GPXLineWidthInput_->setEnabled(true);
        GPXTracksOpacityInput_->setEnabled(true);
    }
    else
    {
        GPXFileKUrlRequester_->setEnabled(false);
        GPXFileLabel_->setEnabled(false);
        timeZoneCB->setEnabled(false);
        GPXColorLabel_->setEnabled(false);
        GPXAltitudeLabel_->setEnabled(false);
        timeZoneLabel_->setEnabled(false);
        GPXAltitudeCB_->setEnabled(false);
        GPXTrackColor_->setEnabled(false);
        GPXLineWidthLabel_->setEnabled(false);
        GPXLineWidthInput_->setEnabled(false);
        GPXTracksOpacityInput_->setEnabled(false);
    }
}

void KMLExportConfig::readSettings()
{
    bool    localTarget;
    bool    optimize_googlemap;
    int     iconSize;

    //	int googlemapSize;
    int     size;
    QString UrlDestDir;
    QString baseDestDir;
    QString KMLFileName;
    int     AltitudeMode;

    bool    GPXtracks;
    QString GPXFile;
    int     TimeZone;
    int     LineWidth;
    QString GPXColor;
    int     GPXOpacity;
    int     GPXAltitudeMode;

    KConfig config("kipirc");
    KConfigGroup group  = config.group(QString("KMLExport Settings"));

    localTarget	        = group.readEntry("localTarget", true);
    optimize_googlemap  = group.readEntry("optimize_googlemap", false);
    iconSize            = group.readEntry("iconSize", 33);
    // not saving this size as it should not change
    //	googlemapSize = group.readNumEntry("googlemapSize", 32);
    size                = group.readEntry("size", 320);
    // UrlDestDir have to have the trailing /
    baseDestDir	        = group.readEntry("baseDestDir", "/tmp/");
    UrlDestDir	        = group.readEntry("UrlDestDir", "http://www.example.com/");
    KMLFileName         = group.readEntry("KMLFileName", "kmldocument");
    AltitudeMode        = group.readEntry("Altitude Mode", 0);

    GPXtracks           = group.readEntry("UseGPXTracks", false);
    GPXFile             = group.readEntry("GPXFile", QString());
    TimeZone            = group.readEntry("Time Zone", 12);
    LineWidth           = group.readEntry("Line Width", 4);
    GPXColor            = group.readEntry("Track Color", "#17eeee" );
    GPXOpacity          = group.readEntry("Track Opacity", 64 );
    GPXAltitudeMode     = group.readEntry("GPX Altitude Mode", 0);

    KConfigGroup group2 = config.group(QString("KMLExport Dialog"));
    restoreDialogSize(group2);

    // -- Apply Settings to widgets ------------------------------

    LocalTargetRadioButton_->setChecked(localTarget);
    GoogleMapTargetRadioButton_->setChecked(optimize_googlemap);

    IconSizeInput_->setValue(iconSize);
    ImageSizeInput_->setValue(size);

    AltitudeCB_->setCurrentIndex(AltitudeMode);
    DestinationDirectory_->setUrl(baseDestDir);
    DestinationUrl_->setText(UrlDestDir);
    FileName_->setText(KMLFileName);

    GPXTracksCheckBox_->setChecked(GPXtracks);
    timeZoneCB->setCurrentIndex(TimeZone);
    GPXLineWidthInput_->setValue(LineWidth);
    GPXTrackColor_->setColor(GPXColor);
    GPXTracksOpacityInput_->setValue(GPXOpacity);
    GPXAltitudeCB_->setCurrentIndex(GPXAltitudeMode);
}

void KMLExportConfig::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("KMLExport Settings"));

    group.writeEntry("localTarget",        LocalTargetRadioButton_->isChecked());
    group.writeEntry("optimize_googlemap", GoogleMapTargetRadioButton_->isChecked());
    group.writeEntry("iconSize",           IconSizeInput_->value());
    group.writeEntry("size",               ImageSizeInput_->value());

    QString destination = DestinationDirectory_->url().path();

    if (!destination.endsWith('/'))
    {
        destination.append('/');
    }

    group.writeEntry("baseDestDir", destination);
    QString url = DestinationUrl_->text();

    if (!url.endsWith('/'))
    {
        url.append('/');
    }

    group.writeEntry("UrlDestDir",        url);
    group.writeEntry("KMLFileName",       FileName_->text());
    group.writeEntry("Altitude Mode",     AltitudeCB_->currentIndex() );
    group.writeEntry("UseGPXTracks",      GPXTracksCheckBox_->isChecked());
    group.writeEntry("GPXFile",           GPXFileKUrlRequester_->lineEdit()->originalText());
    group.writeEntry("Time Zone",         timeZoneCB->currentIndex() );
    group.writeEntry("Line Width",        GPXLineWidthInput_->value());
    group.writeEntry("Track Color",   	  GPXTrackColor_->color().name () );
    group.writeEntry("Track Opacity",     GPXTracksOpacityInput_->value() );
    group.writeEntry("GPX Altitude Mode", GPXAltitudeCB_->currentIndex() );

    KConfigGroup group2 = config.group(QString("KMLExport Dialog"));
    saveDialogSize(group2);

    config.sync();
}

} // namespace KIPIKMLExportPlugin
