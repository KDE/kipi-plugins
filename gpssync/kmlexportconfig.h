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

#ifndef KMLEXPORTCONFIG_H
#define KMLEXPORTCONFIG_H

// KDE includes.

#include <kdialogbase.h>

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpacerItem;

class KColorButton;
class KIntNumInput;
class KURLRequester;

namespace KIPIPlugins 
{
    class KPAboutData;
}

namespace KIPIGPSSyncPlugin 
{

class KMLExportConfig : public KDialogBase
{
    Q_OBJECT

public:

    explicit KMLExportConfig( QWidget* parent = 0, const char* name = 0);
    ~KMLExportConfig();

public:

    QLabel        *ImageSizeLabel;
    QLabel        *IconSizeLabel;
    QLabel        *destinationDirectoryLabel_;
    QLabel        *FileNameLabel_;
    QLabel        *DestinationUrlLabel_;
    QLabel        *GPXFileLabel_;
    QLabel        *timeZoneLabel_;
    QLabel        *GPXLineWidthLabel_;
    QLabel        *GPXColorLabel_;
    QLabel        *GPXTracksOpacityLabel_;
    QLabel        *GPXAltitudeLabel_;

    QGroupBox     *TargetPreferenceGroupBox;

    QButtonGroup  *buttonGroupTargetType;

    QRadioButton  *LocalTargetRadioButton_;
    QRadioButton  *GoogleMapTargetRadioButton_;

    QLineEdit     *DestinationURL_;
    QLineEdit     *FileName_;

    QCheckBox     *GPXTracksCheckBox_;

    QComboBox     *AltitudeCB_;
    QComboBox     *timeZoneCB;
    QComboBox     *GPXAltitudeCB_;

    KColorButton  *GPXTrackColor_;

    KURLRequester *DestinationDirectory_;
    KURLRequester *GPXFileKURLRequester_;

    KIntNumInput  *ImageSizeInput_;
    KIntNumInput  *IconSizeInput_;
    KIntNumInput  *GPXTracksOpacityInput_;
    KIntNumInput  *GPXLineWidthInput_;

public slots:

    void GoogleMapTargetRadioButton__toggled(bool);
    void KMLTracksCheckButton__toggled(bool);

signals:

    void okButtonClicked(); // Signal needed by plugin_kmlexport class

protected:

    QSpacerItem              *spacer3;
    QSpacerItem              *spacer4;

    QGridLayout              *KMLExportConfigLayout;
    QGridLayout              *SizeGroupBoxLayout;
    QGridLayout              *TargetPreferenceGroupBoxLayout;
    QGridLayout              *buttonGroupTargetTypeLayout;

    KConfig                  *config_;

    KIPIPlugins::KPAboutData *m_about;

protected:

    void saveSettings();
    void readSettings();

protected slots:

    void slotOk();
    //	void slotOkClicked();
    void slotHelp();
};

} // namespace

#endif // KMLEXPORTCONFIG_H
