/* ============================================================
 * Authors: Jesper K. Pedersen <blackie@kde.org>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright 2003-2005 by Jesper Pedersen
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef TIMEADJUSTDIALOG_H
#define TIMEADJUSTDIALOG_H

// KDE includes.

#include <kdialogbase.h>
#include <kurl.h>

// LibKIPi includes.

#include <libkipi/interface.h>

// Local includes

#include "kpaboutdata.h"

class QSpinBox;
class QRadioButton;
class QPushButton;
class QVButtonGroup;
class QVGroupBox;
class QButtonGroup;
class QCheckBox;

class KDateTimeWidget;

namespace KIPITimeAdjustPlugin
{

class TimeAdjustDialog :public KDialogBase 
{
    Q_OBJECT

public:

    TimeAdjustDialog(KIPI::Interface* interface, QWidget* parent);
    ~TimeAdjustDialog();
    
    void setImages(const KURL::List& images);

protected slots:

    void slotUpdateExample();
    void slotAdjustmentTypeChanged();
    void slotOk();
    void slotCancel();
    void slotHelp();

protected:

    void closeEvent(QCloseEvent *);

private:

    void readSettings();
    void saveSettings();
    QDateTime updateTime(const KURL& url, const QDateTime& time) const;
    
private:

    QRadioButton             *m_add;
    QRadioButton             *m_subtract;
    QRadioButton             *m_exif;
    QRadioButton             *m_custom;

    QCheckBox                *m_syncEXIFDateCheck;
    QCheckBox                *m_syncIPTCDateCheck;

    QVGroupBox               *m_exampleBox;
    QVButtonGroup            *m_adjustValGrp;
    QButtonGroup             *m_adjustTypeGrp;
    
    QLabel                   *m_infoLabel;
    QLabel                   *m_exampleAdj;
    
    QPushButton              *m_helpButton;
    
    QSpinBox                 *m_secs;
    QSpinBox                 *m_minutes;
    QSpinBox                 *m_hours;
    QSpinBox                 *m_days;
    QSpinBox                 *m_months;
    QSpinBox                 *m_years;
    
    QDateTime                 m_exampleDate;

    KDateTimeWidget          *m_dateCreatedSel;

    KURL::List                m_images;

    KIPI::Interface          *m_interface;

    KIPIPlugins::KPAboutData *m_about;
};

}  // NameSpace KIPITimeAdjustPlugin

#endif /* TIMEADJUSTDIALOG_H */

