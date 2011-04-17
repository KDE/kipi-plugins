/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-05-16
 * Description : a tool to export GPS data to KML file.
 *
 * Copyright (C) 2006-2007 by Stephane Pontier <shadow dot walker at free dot fr>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kdialog.h>

class QButtonGroup;
class QCheckBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class KLineEdit;
class QRadioButton;
class QSpacerItem;

class KComboBox;
class KColorButton;
class KIntNumInput;
class KUrlRequester;

namespace KIPIPlugins
{
    class KPAboutData;
}

namespace KIPIGPSSyncPlugin
{

class KMLExportConfig : public KDialog
{
    Q_OBJECT

public:

    KMLExportConfig(QWidget* parent = 0);
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
    QGroupBox     *TargetTypeGroupBox;

    QButtonGroup  *buttonGroupTargetType;

    QRadioButton  *LocalTargetRadioButton_;
    QRadioButton  *GoogleMapTargetRadioButton_;

    KLineEdit     *DestinationUrl_;
    KLineEdit     *FileName_;

    QCheckBox     *GPXTracksCheckBox_;

    KComboBox     *AltitudeCB_;
    KComboBox     *timeZoneCB;
    KComboBox     *GPXAltitudeCB_;

    KColorButton  *GPXTrackColor_;

    KUrlRequester *DestinationDirectory_;
    KUrlRequester *GPXFileKUrlRequester_;

    KIntNumInput  *ImageSizeInput_;
    KIntNumInput  *IconSizeInput_;
    KIntNumInput  *GPXTracksOpacityInput_;
    KIntNumInput  *GPXLineWidthInput_;

public Q_SLOTS:

    void GoogleMapTargetRadioButton__toggled(bool);
    void KMLTracksCheckButton__toggled(bool);

Q_SIGNALS:

    void okButtonClicked(); // Signal needed by plugin_kmlexport class

protected:

    QSpacerItem              *spacer3;
    QSpacerItem              *spacer4;

    QGridLayout              *KMLExportConfigLayout;
    QGridLayout              *SizeGroupBoxLayout;
    QGridLayout              *TargetPreferenceGroupBoxLayout;
    QGridLayout              *buttonGroupTargetTypeLayout;

    KIPIPlugins::KPAboutData *m_about;

protected:

    void saveSettings();
    void readSettings();

protected Q_SLOTS:

    void slotOk();
    void slotCancel();
    void slotHelp();
};

} // namespace

#endif // KMLEXPORTCONFIG_H
