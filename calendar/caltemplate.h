/* ============================================================
 * File  : caltemplate.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-11-04
 * Description :
 *
 * Copyright 2003 by Renchi Raju

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

#ifndef CALTEMPLATE_H
#define CALTEMPLATE_H

// Qt includes.

#include <qwidget.h>

class QFrame;
class QComboBox;
class QVButtonGroup;
class QRadioButton;
class QCheckBox;
class QSlider;
class QSpinBox;
class QLabel;
class QTimer;

namespace KIPICalendarPlugin
{

class CalWidget;

class CalTemplate : public QWidget
{
    Q_OBJECT

public:
    
    CalTemplate(QWidget* parent, const char* name=0);
    ~CalTemplate();

private:
    
    QComboBox*     comboPaperSize_;
    QVButtonGroup* btnGroupImagePos_;
    QCheckBox*     checkBoxDrawLines_;
    QSlider*       sliderSpacing_;
    QSlider*       sliderRatio_;
    QComboBox*     comboFont_;
    
    CalWidget*    calWidget_;
    QTimer*       timer_;
    int           previewSize_;

private slots:

    void slotParamsChanged();
    void slotUpdatePreview();
    
};

}  // NameSpace KIPICalendarPlugin

#endif // CALTEMPLATE_H
