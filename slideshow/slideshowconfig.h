/* ============================================================
 * File  : slideshowconfig.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-17
 * Description : Digikam slideshow plugin.
 * 
 * Copyright 2003-2004 by Renchi Raju
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

#ifndef SLIDESHOWCONFIG_H
#define SLIDESHOWCONFIG_H

// Qt includes.

#include <qstring.h>

// KDE includes.

#include <kdialogbase.h>

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QRadioButton;
class QSpinBox;
class QPushButton;

class KConfig;

namespace KIPISlideShowPlugin
{

class SlideShowConfig : public KDialogBase
{
    Q_OBJECT

public:

    SlideShowConfig();
    ~SlideShowConfig();

private:

    void loadEffectNames();
    void loadEffectNamesGL();
    void readSettings();
    void saveSettings();

private slots:

    void slotOkClicked();
    void slotHelp();
    void slotOpenGLToggled();

private:
    
    QPushButton*  m_helpButton;

    QButtonGroup* FileSrcButtonGroup_;
    
    QRadioButton* allFilesButton_;
    QRadioButton* selectedFilesButton_;
    
    QCheckBox*    openglCheckBox_;
    QCheckBox*    printNameCheckBox_;
    QCheckBox*    loopCheckBox_;
    
    QSpinBox*     delaySpinBox_;
    
    QComboBox*    effectsComboBox_;
    
    QString       effectName_;
    QString       effectNameGL_;

    KConfig*      config_;    
};

}  // NameSpace KIPISlideShowPlugin

#endif
