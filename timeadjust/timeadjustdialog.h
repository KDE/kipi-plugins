/* ============================================================
 * File  : timeadjustdialog.h
 * Author: Jesper K. Pedersen <blackie@kde.org>
 * Date  : 2004-05-16
 * Description :
 *
 * Copyright 2003 by Jesper Pedersen
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
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

class QSpinBox;
class QRadioButton;
class QPushButton;

namespace KIPITimeAdjustPlugin
{

class TimeAdjustDialog :public KDialogBase 
{
    Q_OBJECT

public:

    TimeAdjustDialog( KIPI::Interface* interface, QWidget* parent, const char* name = 0 );
    void setImages( const KURL::List& images );

protected slots:

    void updateExample();
    void slotOK();
    void slotHelp();

protected:

    void addInfoPage();
    void addConfigPage();
    QDateTime updateTime( QDateTime time ) const;

private:

    KIPI::Interface* m_interface;
    
    KURL::List m_images;
    
    QRadioButton* m_add;
    
    QLabel* m_infoLabel;
    QLabel* m_exampleAdj;
    
    QPushButton* m_helpButton;
    
    QSpinBox* m_secs;
    QSpinBox* m_minutes;
    QSpinBox* m_hours;
    QSpinBox* m_days;
    QSpinBox* m_months;
    QSpinBox* m_years;
    
    QDateTime m_exampleDate;

};

}  // NameSpace KIPITimeAdjustPlugin

#endif /* TIMEADJUSTDIALOG_H */

