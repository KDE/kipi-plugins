/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a tool to export GPS data to KML file.
 *
 * Copyright (C) 2006-2007 by Stephane Pontier <shadow dot walker at free dot fr>
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

// Qt includes.

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>

// KDE includes.

#include <kapplication.h>
#include <kconfig.h>
#include <kcolorbutton.h>
#include <kdialog.h>
#include <khelpmenu.h>
#include <klocale.h>
#include <klineedit.h>
#include <knuminput.h>
#include "kpaboutdata.h"
#include <kmenu.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>

// Local includes.

#include "kmlexportconfig.h"
#include "kmlexportconfig.moc"

namespace KIPIGPSSyncPlugin 
{

/*
 *  Constructs a KIPIKMLExport::KMLExportConfig which is a child of 'parent'.
 */
KMLExportConfig::KMLExportConfig(QWidget* parent)
               : KDialog(parent)
{
    setButtons(Help|Ok|Cancel);
    setDefaultButton(Ok);
    setCaption(i18n("KML Export"));
    setModal(true);

    QWidget *page = new QWidget( this );
    setMainWidget( page );
    KMLExportConfigLayout = new QGridLayout(page); 

    // --------------------------------------------------------------
    // Target preferences
    TargetPreferenceGroupBox       = new QGroupBox(i18n( "Target Preferences" ), page);
    TargetPreferenceGroupBoxLayout = new QGridLayout(TargetPreferenceGroupBox);

    // target type
    buttonGroupTargetType       = new QButtonGroup(i18n( "Target Type" ), TargetPreferenceGroupBox);
    buttonGroupTargetTypeLayout = new QGridLayout( buttonGroupTargetType->layout(), 2, 1, KDialog::spacingHint() );

    LocalTargetRadioButton_ = new QRadioButton( i18n( "&Local or web target used by GoogleEarth" ), buttonGroupTargetType);
    LocalTargetRadioButton_->setChecked( true );

    GoogleMapTargetRadioButton_ = new QRadioButton( i18n( "Web target used by GoogleMap" ), buttonGroupTargetType);
    GoogleMapTargetRadioButton_->setToolTip(i18n("When using GoogleMap, all image must have complete URL, icons are "
                                                 "squared and when drawing a track, only linetrack is exported" ) );

    buttonGroupTargetTypeLayout->addWidget( LocalTargetRadioButton_, 0, 0, 1, 1);
    buttonGroupTargetTypeLayout->addWidget( GoogleMapTargetRadioButton_, 1, 0, 1, 1);
    buttonGroupTargetTypeLayout->setAlignment( Qt::AlignTop );
    buttonGroupTargetTypeLayout->setSpacing(spacingHint());
    buttonGroupTargetTypeLayout->setMargin(0);

    // --------------------------------------------------------------
    // target preference, suite

    QLabel *AltitudeLabel_ = new QLabel(i18n("Picture altitude" ),
                                        TargetPreferenceGroupBox, "AltitudeLabel_");
    AltitudeCB_ = new QComboBox( false, TargetPreferenceGroupBox );
    AltitudeCB_->insertItem(i18n("clamp to ground"));
    AltitudeCB_->insertItem(i18n("relative to ground"));
    AltitudeCB_->insertItem(i18n("absolute"));
    AltitudeCB_->setWhatsThis(i18n("<p>Specifies how pictures are displayed"
                                   "<dl><dt>clamp to ground (default)</dt>"
                                   "<dd>Indicates to ignore an altitude specification</dd>"
                                   "<dt>relative to ground</dt>"
                                   "<dd>Sets the altitude of the element relative to the actual ground "
                                   "elevation of a particular location.</dd>"
                                   "<dt>absolute</dt>"
                                   "<dd>Sets the altitude of the coordinate relative to sea level, regardless "
                                   "of the actual elevation of the terrain beneath the element.</dd></dl>"));

    destinationDirectoryLabel_ = new QLabel( i18n( "Destination directory" ), TargetPreferenceGroupBox);

    // DestinationDirectory_ = new QLineEdit( TargetPreferenceGroupBox, "DestinationDirectory_" );
    DestinationDirectory_= new KURLRequester( TargetPreferenceGroupBox);
    DestinationDirectory_->setCaption(i18n("Select a directory to save the kml file and pictures"));
    DestinationDirectory_->setMode(KFile::Directory | KFile::LocalOnly );

    DestinationUrlLabel_ = new QLabel( i18n( "Destination URL" ), TargetPreferenceGroupBox);
    DestinationURL_      = new QLineEdit( TargetPreferenceGroupBox);
    FileNameLabel_       = new QLabel( i18n( "File name" ), TargetPreferenceGroupBox);
    FileName_            = new QLineEdit( TargetPreferenceGroupBox);

    TargetPreferenceGroupBoxLayout->addWidget( buttonGroupTargetType,      0, 0, 2, 5 );
    TargetPreferenceGroupBoxLayout->addWidget( AltitudeLabel_,             2, 0, 1, 5 );
    TargetPreferenceGroupBoxLayout->addWidget( AltitudeCB_,                2, 2, 1, 4 - 2+1);
    TargetPreferenceGroupBoxLayout->addWidget( destinationDirectoryLabel_, 3, 0, 1, 3 );
    TargetPreferenceGroupBoxLayout->addWidget( DestinationDirectory_,      3, 3, 1, 4 - 3+1);
    TargetPreferenceGroupBoxLayout->addWidget( DestinationUrlLabel_,       4, 0, 1, 2 );
    TargetPreferenceGroupBoxLayout->addWidget( DestinationURL_,            4, 2, 1, 4 - 2+1);
    TargetPreferenceGroupBoxLayout->addWidget( FileNameLabel_,             5, 0, 1, 1);
    TargetPreferenceGroupBoxLayout->addWidget( FileName_,                  5, 1, 1, 4 );
    TargetPreferenceGroupBoxLayout->setAlignment( Qt::AlignTop );
    TargetPreferenceGroupBoxLayout->setSpacing(spacingHint());
    TargetPreferenceGroupBoxLayout->setMargin(0);

    // --------------------------------------------------------------
    // Sizes
    QGroupBox *SizeGroupBox = new QGroupBox(i18n( "Sizes" ), page );
    SizeGroupBoxLayout      = new QGridLayout(SizeGroupBox);
    IconSizeLabel           = new QLabel( i18n( "Icon size" ), SizeGroupBox);
    IconSizeInput_          = new KIntNumInput( SizeGroupBox);
    IconSizeInput_->setValue( 33 );

    spacer3 = new QSpacerItem( 191, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );

    ImageSizeLabel  = new QLabel( i18n( "Image size" ), SizeGroupBox);
    ImageSizeInput_ = new KIntNumInput( SizeGroupBox );
    ImageSizeInput_->setValue( 320 );

    spacer4 = new QSpacerItem( 191, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    SizeGroupBoxLayout->addWidget( IconSizeLabel,   0, 0 );
    SizeGroupBoxLayout->addWidget( IconSizeInput_,  0, 1 );
    SizeGroupBoxLayout->addItem( spacer3,           0, 2 );
    SizeGroupBoxLayout->addWidget( ImageSizeLabel,  1, 0 );
    SizeGroupBoxLayout->addWidget( ImageSizeInput_, 1, 1 );
    SizeGroupBoxLayout->addItem( spacer4,           1, 2 );
    SizeGroupBoxLayout->setAlignment( Qt::AlignTop );
    SizeGroupBoxLayout->setSpacing(spacingHint());
    SizeGroupBoxLayout->setMargin(0);

    // --------------------------------------------------------------
    // GPX Tracks
    QGroupBox *GPXTracksGroupBox         = new QGroupBox(i18n( "GPX Tracks" ), page);
    QGridLayout *GPXTracksGroupBoxLayout = new QGridLayout(GPXTracksGroupBox);

    // add a gpx track checkbox
    GPXTracksCheckBox_ = new QCheckBox( i18n( "Draw GPX track" ), GPXTracksGroupBox);

    // file selector
    GPXFileLabel_ = new QLabel( i18n( "GPX file" ), GPXTracksGroupBox);

    GPXFileKURLRequester_ = new KURLRequester( GPXTracksGroupBox);
    GPXFileKURLRequester_->setFilter(i18n("%1|GPS Exchange Format").arg("*.gpx"));
    GPXFileKURLRequester_->setCaption(i18n("Select GPX File to Load"));

    timeZoneLabel_ = new QLabel(i18n("Time zone"), GPXTracksGroupBox);
    timeZoneCB     = new QComboBox(GPXTracksGroupBox );
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
    timeZoneCB->setWhatsThis(i18n("<p>Sets the time zone of the camera during "
                                  "picture shooting, so that the time stamps of the GPS "
                                  "can be converted to match the local time"));

    GPXLineWidthLabel_ = new QLabel( i18n( "Track width" ), GPXTracksGroupBox);
    GPXLineWidthInput_ = new KIntNumInput( GPXTracksGroupBox);
    GPXLineWidthInput_->setValue( 4 );

    GPXColorLabel_ = new QLabel( i18n( "Track color" ), GPXTracksGroupBox);
    GPXTrackColor_ = new KColorButton(QColor("#ffffff"), GPXTracksGroupBox);

    GPXTracksOpacityInput_ = new KIntNumInput( GPXTracksGroupBox);
    GPXTracksOpacityInput_->setRange( 0, 100, 1, false );
    GPXTracksOpacityInput_->setValue( 100 );
    GPXTracksOpacityInput_->setLabel( i18n( "Opacity:" ), AlignVCenter);
    GPXTracksOpacityInput_->setSuffix( QString::fromAscii( "%" ) );

    GPXAltitudeLabel_ = new QLabel( i18n( "Track altitude" ), GPXTracksGroupBox);
    GPXAltitudeCB_    = new QComboBox( GPXTracksGroupBox );
    GPXAltitudeCB_->insertItem(i18n("clamp to ground"));
    GPXAltitudeCB_->insertItem(i18n("relative to ground"));
    insertItem(i18n("absolute"));
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

    GPXTracksGroupBoxLayout->addWidget( GPXTracksCheckBox_, 0, 0, 1, 4 );
    GPXTracksGroupBoxLayout->addWidget( GPXFileLabel_, 1, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXFileKURLRequester_, 1, 1, 1, 3);
    GPXTracksGroupBoxLayout->addWidget( timeZoneLabel_, 2, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( timeZoneCB, 2, 1, 1, 3);
    GPXTracksGroupBoxLayout->addWidget( GPXLineWidthLabel_, 3, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXLineWidthInput_, 3, 1, 1, 3);
    GPXTracksGroupBoxLayout->addWidget( GPXColorLabel_, 4, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXTrackColor_, 4, 1, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXTracksOpacityInput_, 4, 2, 1, 3- 2+1);
    GPXTracksGroupBoxLayout->addWidget( GPXAltitudeLabel_, 5, 0, 1, 1);
    GPXTracksGroupBoxLayout->addWidget( GPXAltitudeCB_, 5, 1, 1, 3);
    GPXTracksGroupBoxLayout->setAlignment( Qt::AlignTop );
    GPXTracksGroupBoxLayout->setSpacing(spacingHint());
    GPXTracksGroupBoxLayout->setMargin(0);

    // --------------------------------------------------------------

    KMLExportConfigLayout->addWidget( TargetPreferenceGroupBox, 0, 0 );
    KMLExportConfigLayout->addWidget( SizeGroupBox,             1, 0 );
    KMLExportConfigLayout->addWidget( GPXTracksGroupBox,        2, 0 );
    KMLExportConfigLayout->setSpacing(spacingHint());
    KMLExportConfigLayout->setMargin(0);

    // --------------------------------------------------------------

    connect( GoogleMapTargetRadioButton_, SIGNAL( toggled(bool) ), 
             this, SLOT( GoogleMapTargetRadioButton__toggled(bool) ) );

    connect( GPXTracksCheckBox_, SIGNAL( toggled(bool) ), 
             this, SLOT( KMLTracksCheckButton__toggled(bool) ) );

    // --------------------------------------------------------------
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("KML Export"),
                   QByteArray(),
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin for kml exporting"),
                   ki18n("(c) 2006-2007, Stéphane Pontier"));

    m_about->addAuthor(ki18n("Stéphane Pontier"), 
                        ki18n("Developer and maintainer"),
                              "shadow dot walker at free dot fr");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KPushButton *helpButton = button( Help );
    KHelpMenu* helpMenu     = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook       = new QAction(i18n("Plugin Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    helpButton->setDelayedMenu( helpMenu->menu() );

    // --------------------------------------------------------------
    // Configuration file management 

    config_ = new KConfig("kipirc");
    config_->setGroup("KMLExport Settings");

    readSettings();

    // --------------------------------------------------------------
    // Just to initialize the UI
    GoogleMapTargetRadioButton__toggled(true);
    KMLTracksCheckButton__toggled(false);
}

/*
 *  Destroys the object and frees any allocated resources
 */
KMLExportConfig::~KMLExportConfig()
{
    // no need to delete child widgets, Qt does it all for us
    if(config_) 
        delete config_;
    delete m_about;
}

void KMLExportConfig::slotOk()
    //void KMLExportConfig::slotOkClicked()
{
    saveSettings();

    emit okButtonClicked();
    accept();
}

void KMLExportConfig::slotHelp()
{
    KApplication::kApplication()->invokeHelp("KMLExport", "kipi-plugins");
}

void KMLExportConfig::GoogleMapTargetRadioButton__toggled(bool)
{
    if (GoogleMapTargetRadioButton_->isChecked()) 
    {
        DestinationUrlLabel_->setEnabled(true);
        DestinationURL_->setEnabled(true);
        IconSizeLabel->setEnabled(false);
        IconSizeInput_->setEnabled(false);
    } 
    else 
    {
        DestinationUrlLabel_->setEnabled(false);
        DestinationURL_->setEnabled(false);
        IconSizeLabel->setEnabled(true);
        IconSizeInput_->setEnabled(true);
    }
}

void KMLExportConfig::KMLTracksCheckButton__toggled(bool) 
{
    if (GPXTracksCheckBox_->isChecked()) 
    {
        GPXFileKURLRequester_->setEnabled(true);
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
        GPXFileKURLRequester_->setEnabled(false);
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

void KMLExportConfig::saveSettings()
{
    if (!config_) return;

    config_->writeEntry("localTarget", LocalTargetRadioButton_->isChecked());
    config_->writeEntry("optimize_googlemap", GoogleMapTargetRadioButton_->isChecked());
    config_->writeEntry("iconSize", IconSizeInput_->value());
    config_->writeEntry("size", ImageSizeInput_->value());
    QString destination = DestinationDirectory_->url();
    if (!destination.endsWith("/")) 
    {
        destination.append("/");
    }
    config_->writeEntry("baseDestDir",destination);
    QString url = DestinationURL_->text();
    if (!url.endsWith("/")) 
    {
        url.append("/");
    }
    config_->writeEntry("UrlDestDir",url);
    config_->writeEntry("KMLFileName",FileName_->text());
    config_->writeEntry("Altitude Mode", AltitudeCB_->currentItem() );

    config_->writeEntry("UseGPXTracks", GPXTracksCheckBox_->isChecked());

    config_->writeEntry("GPXFile", GPXFileKURLRequester_->lineEdit()->originalText());
    config_->writeEntry("Time Zone", timeZoneCB->currentItem() );
    config_->writeEntry("Line Width", GPXLineWidthInput_->value());
    config_->writeEntry("Track Color", GPXTrackColor_->color().name () );
    config_->writeEntry("Track Opacity", GPXTracksOpacityInput_->value() );
    config_->writeEntry("GPX Altitude Mode", GPXAltitudeCB_->currentItem() );

    config_->sync();
}

void KMLExportConfig::readSettings()
{
    bool localTarget;
    bool optimize_googlemap;
    int iconSize;
    //	int googlemapSize;
    int size;
    QString UrlDestDir;
    QString baseDestDir;
    QString KMLFileName;
    int AltitudeMode;

    bool GPXtracks;
    QString GPXFile;
    int TimeZone;
    int LineWidth;
    QString GPXColor;
    int GPXOpacity;
    int GPXAltitudeMode;

    localTarget	       = config_->readBoolEntry("localTarget", true);
    optimize_googlemap = config_->readBoolEntry("optimize_googlemap", false);
    iconSize           = config_->readNumEntry("iconSize", 33);
    // not saving this size as it should not change
    //	googlemapSize = config_->readNumEntry("googlemapSize", 32);
    size               = config_->readNumEntry("size", 320);
    // UrlDestDir have to have the trailing /
    baseDestDir	       = config_->readEntry("baseDestDir", "/tmp/");
    UrlDestDir	       = config_->readEntry("UrlDestDir", "http://www.example.com/");
    KMLFileName        = config_->readEntry("KMLFileName", "kmldocument");
    AltitudeMode       = config_->readNumEntry("Altitude Mode", 0);

    GPXtracks          = config_->readBoolEntry("UseGPXTracks", false);
    GPXFile            = config_->readEntry("GPXFile", "");
    TimeZone           = config_->readNumEntry("Time Zone", 12);
    LineWidth          = config_->readNumEntry("Line Width", 4);
    GPXColor           = config_->readEntry("Track Color", "#17eeee" );
    GPXOpacity         = config_->readNumEntry("Track Opacity", 64 );
    GPXAltitudeMode    = config_->readNumEntry("GPX Altitude Mode", 0);

    // -- Apply Settings to widgets ------------------------------

    LocalTargetRadioButton_->setChecked(localTarget);
    GoogleMapTargetRadioButton_->setChecked(optimize_googlemap);

    IconSizeInput_->setValue(iconSize);
    ImageSizeInput_->setValue(size);

    AltitudeCB_->setCurrentItem(AltitudeMode);
    DestinationDirectory_->setURL(baseDestDir);
    DestinationURL_->setText(UrlDestDir);
    FileName_->setText(KMLFileName);

    timeZoneCB->setCurrentItem(TimeZone);
    GPXLineWidthInput_->setValue(LineWidth);
    GPXTrackColor_->setColor(GPXColor);
    GPXTracksOpacityInput_->setValue(GPXOpacity);
    GPXAltitudeCB_->setCurrentItem(GPXAltitudeMode);
}

} //namespace KIPIGPSSyncPlugin
