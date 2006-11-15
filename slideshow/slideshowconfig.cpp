/* ============================================================
 * File  : slideshowconfig.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-17
 * Description : KIPI slideshow plugin.
 *
 * Copyright 2003-2004 by Renchi Raju
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

// Qt includes.

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qmap.h>
#include <qframe.h>
#include <qpushbutton.h>

// Kde includes.

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>

// Local includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "slideshow.h"
#include "slideshowgl.h"
#include "slideshowconfig.h"
#include "slideshowconfig.moc"

namespace KIPISlideShowPlugin
{

SlideShowConfig::SlideShowConfig(bool allowSelectedOnly, QWidget *parent, const char* name)
    :SlideShowConfigBase(parent, name) 
{
    // About data and help button.

    about = new KIPIPlugins::KPAboutData(I18N_NOOP("Slide Show"),
                                         NULL,
                                         KAboutData::License_GPL,
                                         I18N_NOOP("A Kipi plugin for image slideshow"),
                                         "(c) 2003-2004, Renchi Raju");

    about->addAuthor("Renchi Raju", I18N_NOOP("Author"),
                     "renchi@pooh.tam.uiuc.edu");
    about->addAuthor("Valerio Fuoglio", I18N_NOOP("Maintainer"),
                     "valerio.fuoglio@kdemail.net");

     KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
     helpMenu->menu()->removeItemAt(0);
     helpMenu->menu()->insertItem(i18n("SlideShow Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
     m_helpButton->setPopup( helpMenu->menu() );


    // Switch to selected files only (it depends on allowSelectedOnly)

    selectedFilesButton_->setEnabled( allowSelectedOnly );

    // Signal to Slot connections

    connect(openglCheckBox_, SIGNAL(toggled(bool)), SLOT(slotOpenGLToggled()));
    connect(buttonOk, SIGNAL(clicked()), this, SLOT(slotOkClicked()));   

    // Configuration file management 

    config_ = new KConfig("kipirc");
    config_->setGroup("SlideShow Settings");

    readSettings();
}

SlideShowConfig::~SlideShowConfig()
{
    if (config_) {
        delete config_;
    }
    
    delete about;
}


void SlideShowConfig::loadEffectNames()
{
    effectsComboBox_->clear();

    QMap<QString,QString> effectNames = SlideShow::effectNamesI18N();
    QStringList effects;

    QMap<QString,QString>::Iterator it;
    for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.data());

    effectsComboBox_->insertStringList(effects);

    for (int i=0; i<effectsComboBox_->count(); i++) {
        if (effectNames[effectName_] == effectsComboBox_->text(i)) {
            effectsComboBox_->setCurrentItem(i);
            break;
        }
    }
}

void SlideShowConfig::loadEffectNamesGL()
{
    effectsComboBox_->clear();

    QMap<QString,QString> effectNames = SlideShowGL::effectNamesI18N();
    QStringList effects;

    QMap<QString,QString>::Iterator it;
    for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.data());

    effectsComboBox_->insertStringList(effects);

    for (int i=0; i<effectsComboBox_->count(); i++) {
        if (effectNames[effectNameGL_] == effectsComboBox_->text(i)) {
            effectsComboBox_->setCurrentItem(i);
            break;
        }
    }
}

void SlideShowConfig::readSettings()
{
    bool  opengl;
    int   delay;
    bool  printFileName;
    bool  loop;
    bool  shuffle;
    bool  showSelectedFilesOnly;

    opengl                = config_->readBoolEntry("OpenGL", false);
    delay                 = config_->readNumEntry("Delay", 1500);
    printFileName         = config_->readBoolEntry("Print Filename", true);
    loop                  = config_->readBoolEntry("Loop", false);
    shuffle               = config_->readBoolEntry("Shuffle", false);
    showSelectedFilesOnly = config_->readBoolEntry("Show Selected Files Only", false);
    effectName_           = config_->readEntry("Effect Name", "Random");
    effectNameGL_         = config_->readEntry("Effect Name (OpenGL)", "Random");

    // -- Apply Settings to widgets ------------------------------

    openglCheckBox_->setChecked(opengl);

    delaySpinBox_->setValue(delay);

    printNameCheckBox_->setChecked(printFileName);

    loopCheckBox_->setChecked(loop);

    shuffleCheckBox_->setChecked(shuffle);

    if (showSelectedFilesOnly && selectedFilesButton_->isEnabled() )
        selectedFilesButton_->setChecked(true);
    else
        allFilesButton_->setChecked(true);

    slotOpenGLToggled();
}

void SlideShowConfig::saveSettings()
{
    if (!config_) return;

    config_->writeEntry("OpenGL", openglCheckBox_->isChecked());
    config_->writeEntry("Delay", delaySpinBox_->value());
    config_->writeEntry("Print Filename", printNameCheckBox_->isChecked());
    config_->writeEntry("Loop", loopCheckBox_->isChecked());
    config_->writeEntry("Shuffle", shuffleCheckBox_->isChecked());
    config_->writeEntry("Show Selected Files Only", selectedFilesButton_->isChecked());

    if (!openglCheckBox_->isChecked()) {

        QString effect;
        QMap<QString,QString> effectNames = SlideShow::effectNamesI18N();
        QMap<QString,QString>::Iterator it;

        for (it = effectNames.begin(); it != effectNames.end(); ++it) {
            if (it.data() == effectsComboBox_->currentText()) {
                effect = it.key();
                break;
            }
        }

        config_->writeEntry("Effect Name", effect);

    }
    else {

        QString effect;
        QMap<QString,QString> effectNames = SlideShowGL::effectNamesI18N();
        QMap<QString,QString>::Iterator it;

        for (it = effectNames.begin(); it != effectNames.end(); ++it) {
            if (it.data() == effectsComboBox_->currentText()) {
                effect = it.key();
                break;
            }
        }

        config_->writeEntry("Effect Name (OpenGL)", effect);

    }

    config_->sync();
}

void SlideShowConfig::slotOpenGLToggled()
{
    if (openglCheckBox_->isChecked()) {
        loadEffectNamesGL();
    }
    else {
        loadEffectNames();
    }
}

void SlideShowConfig::slotOkClicked()
{
    saveSettings();

    emit okButtonClicked();
}

void SlideShowConfig::slotHelp()
{
    KApplication::kApplication()->invokeHelp("slideshow",
                                             "kipi-plugins");
}

}  // NameSpace KIPISlideShowPlugin

