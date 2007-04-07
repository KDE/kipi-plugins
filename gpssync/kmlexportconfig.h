/***************************************************************************
 *   Copyright (C) 2007 by Stéphane Pontier                                *
 *   shadow.walker@free.fr                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef KMLEXPORTCONFIG_H
#define KMLEXPORTCONFIG_H

// Qt includes.

#include <qvariant.h>
#include "qcheckbox.h"

// KDE includes.

#include "kpaboutdata.h"
#include <kdialogbase.h>
#include <kurlrequester.h>
#include <kcolorbutton.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class KIntNumInput;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class QLineEdit;

namespace KIPIGPSSyncPlugin {
class KMLExportConfig : public KDialogBase
{
    Q_OBJECT

public:
    KMLExportConfig( QWidget* parent = 0, const char* name = 0);
    ~KMLExportConfig();

    QLabel* ImageSizeLabel;
    KIntNumInput* ImageSizeInput_;
    QLabel* IconSizeLabel;
    KIntNumInput* IconSizeInput_;
    QGroupBox* TargetPreferenceGroupBox;
    QButtonGroup* buttonGroupTargetType;
    QRadioButton* LocalTargetRadioButton_;
    QRadioButton* GoogleMapTargetRadioButton_;
    QComboBox* AltitudeCB_;
    QLabel* destinationDirectoryLabel_;
    QLineEdit* DestinationDirectory_;
    QLineEdit* DestinationURL_;
    QLabel* FileNameLabel_;
    QLineEdit* FileName_;
    QLabel* DestinationUrlLabel_;

    KURLRequester* GPXFileKURLRequester_;
    QCheckBox* GPXTracksCheckBox_;
    QLabel* GPXFileLabel_;
    QLabel* timeZoneLabel_;
    QComboBox* timeZoneCB;
    QLabel* GPXColorLabel_;
    KColorButton* GPXTrackColor_;
    QLabel* GPXTracksOpacityLabel_;
    KIntNumInput* GPXTracksOpacityInput_;
    QLabel* GPXAltitudeLabel_;
    QComboBox* GPXAltitudeCB_;
    QLabel *GPXLineWidthLabel_;
    KIntNumInput* GPXLineWidthInput_;

public slots:
    void GoogleMapTargetRadioButton__toggled(bool);
    void KMLTracksCheckButton__toggled(bool);

protected:
    KConfig* config_;

    void saveSettings();
    void readSettings();

    QGridLayout* KMLExportConfigLayout;
    QGridLayout* SizeGroupBoxLayout;
    QSpacerItem* spacer3;
    QSpacerItem* spacer4;
    QGridLayout* TargetPreferenceGroupBoxLayout;
    QGridLayout* buttonGroupTargetTypeLayout;

    KIPIPlugins::KPAboutData * m_about;

protected slots:
    void slotOk();
    //	void slotOkClicked();
    void slotHelp();


signals:
    void okButtonClicked(); // Signal needed by plugin_kmlexport class
};

} // namespace

#endif // KMLEXPORTCONFIG_H
