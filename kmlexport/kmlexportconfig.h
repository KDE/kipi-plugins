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

#ifndef KMLEXPORTCONFIG_H
#define KMLEXPORTCONFIG_H

// Libkipi includes

#include <imagecollection.h>

// Local includes

#include "kptooldialog.h"
#include "kmlexport.h"

class QButtonGroup;
class QCheckBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpacerItem;
class QComboBox;

class KColorButton;
class KUrlRequester;

namespace KDcrawIface
{
class RIntNumInput;
}

using namespace KDcrawIface;
using namespace KIPIPlugins;

namespace KIPIKMLExportPlugin
{

class KMLExportConfig : public KPToolDialog
{
    Q_OBJECT

public:

    KMLExportConfig(QWidget* const parent,
                    bool hostFeatureImagesHasComments, bool hostFeatureImagesHasTime,
                    const QString& hostAlbumName, const KIPI::ImageCollection& hostSelection);
    ~KMLExportConfig();

public:

    QLabel*        ImageSizeLabel;
    QLabel*        IconSizeLabel;
    QLabel*        destinationDirectoryLabel_;
    QLabel*        FileNameLabel_;
    QLabel*        DestinationUrlLabel_;
    QLabel*        GPXFileLabel_;
    QLabel*        timeZoneLabel_;
    QLabel*        GPXLineWidthLabel_;
    QLabel*        GPXColorLabel_;
    QLabel*        GPXAltitudeLabel_;
    QLabel*        GPXTracksOpacityLabel_;

    QGroupBox*     TargetPreferenceGroupBox;
    QGroupBox*     TargetTypeGroupBox;

    QButtonGroup*  buttonGroupTargetType;

    QRadioButton*  LocalTargetRadioButton_;
    QRadioButton*  GoogleMapTargetRadioButton_;

    QLineEdit*     DestinationUrl_;
    QLineEdit*     FileName_;

    QCheckBox*     GPXTracksCheckBox_;

    QComboBox*     AltitudeCB_;
    QComboBox*     timeZoneCB;
    QComboBox*     GPXAltitudeCB_;

    KColorButton*  GPXTrackColor_;

    KUrlRequester* DestinationDirectory_;
    KUrlRequester* GPXFileUrlRequester_;

    RIntNumInput*  ImageSizeInput_;
    RIntNumInput*  IconSizeInput_;
    RIntNumInput*  GPXTracksOpacityInput_;
    RIntNumInput*  GPXLineWidthInput_;

public Q_SLOTS:

    void slotGoogleMapTargetRadioButtonToggled(bool);
    void slotKMLTracksCheckButtonToggled(bool);

Q_SIGNALS:

    void okButtonClicked(); // Signal needed by plugin_kmlexport class

protected:

    void saveSettings();
    void readSettings();

protected Q_SLOTS:

    void slotCancel();
    void slotKMLGenerate();

protected:

    QSpacerItem* spacer3;
    QSpacerItem* spacer4;

    QGridLayout* KMLExportConfigLayout;
    QGridLayout* SizeGroupBoxLayout;
    QGridLayout* TargetPreferenceGroupBoxLayout;
    QGridLayout* buttonGroupTargetTypeLayout;

    KmlExport m_kmlExport;
};

} // KIPIKMLExportPlugin

#endif // KMLEXPORTCONFIG_H
