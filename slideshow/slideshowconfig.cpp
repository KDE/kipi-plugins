/* ============================================================
 * File  : slideshowconfig.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-02-17
 * Description : KIPI slideshow plugin.
 *
 * Copyright 2003-2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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
#include <qtabwidget.h>
#include <qcolor.h>
#include <qnamespace.h>

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
#include <kfontdialog.h>
#include <kcolorbutton.h>

// libkipi includes

#include <libkipi/interface.h>

// Common includes.

#include "kpaboutdata.h"
#include "pluginsversion.h"

// Local includes

#include "slideshow.h"
#include "slideshowgl.h"
#include "slideshowconfig.h"
#include "slideshowconfig.moc"

namespace KIPISlideShowPlugin
{

SlideShowConfig::SlideShowConfig(bool allowSelectedOnly, QWidget *parent, const char* name, bool ImagesHasComments)
    :SlideShowConfigBase(parent, name) 
{
    // About data and help button.

    KIPIPlugins::KPAboutData * about = new KIPIPlugins::KPAboutData(I18N_NOOP("Slide Show"),
                                        NULL,
                                        KAboutData::License_GPL,
                                        I18N_NOOP("A Kipi plugin for image slideshow"),
                                        "(c) 2003-2004, Renchi Raju");

    about->addAuthor("Renchi Raju", I18N_NOOP("Author"),
                     "renchi@pooh.tam.uiuc.edu");
    about->addAuthor("Valerio Fuoglio", I18N_NOOP("Maintainer"),
                     "valerio@ulteo.com");

     KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
     helpMenu->menu()->removeItemAt(0);
     helpMenu->menu()->insertItem(i18n("SlideShow Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
     m_helpButton->setPopup( helpMenu->menu() );


    // Switch to selected files only (it depends on allowSelectedOnly)

    m_selectedFilesButton->setEnabled( allowSelectedOnly );

    
    m_delayMsMaxValue = 100000;
    m_delayMsMinValue = 100;
    m_delayMsLineStep = 10;
    
    m_delaySpinBox->setMinValue(m_delayMsMinValue);
    m_delaySpinBox->setMaxValue(m_delayMsMaxValue);
    m_delaySpinBox->setLineStep(m_delayMsLineStep); 
    
    // Signal to Slot connections

    connect(m_openglCheckBox, SIGNAL(toggled(bool)), SLOT(slotOpenGLToggled()));
    connect(m_buttonStart, SIGNAL(clicked()), this, SLOT(slotStartClicked()));
    connect(m_printCommentsCheckBox, SIGNAL(toggled(bool)), this, SLOT(slotPrintCommentsToggled()));
    connect(m_commentsFontColor, SIGNAL(changed(const QColor &)), this, SLOT(slotCommentsFontColorChanged()));
    connect(m_commentsBgColor, SIGNAL(changed(const QColor &)), this, SLOT(slotCommentsBgColorChanged()));
    connect(m_useMillisecondsCheckBox, SIGNAL(toggled(bool)), SLOT(slotUseMillisecondsToggled()));
    
    // Configuration file management 

    config_ = new KConfig("kipirc");
    config_->setGroup("SlideShow Settings");

    readSettings();

    slotUseMillisecondsToggled();
    
    // Comments tab management

    m_commentsFontChooser->setSampleText(
            i18n("Slideshow is part of KIPI-Plugins (http://www.kipi-plugins.org)"));

    // Host application images has comments
    if ( ! ImagesHasComments ) {
        m_printCommentsCheckBox->setEnabled(FALSE);
        m_tabWidget->setTabEnabled(commentsTab, FALSE);
    }
}

SlideShowConfig::~SlideShowConfig()
{
    if (config_) {
        delete config_;
    }
}


void SlideShowConfig::loadEffectNames()
{
    m_effectsComboBox->clear();

    QMap<QString,QString> effectNames = SlideShow::effectNamesI18N();
    QStringList effects;

    QMap<QString,QString>::Iterator it;
    for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.data());

    m_effectsComboBox->insertStringList(effects);

    for (int i=0; i<m_effectsComboBox->count(); i++) {
        if (effectNames[effectName_] == m_effectsComboBox->text(i)) {
            m_effectsComboBox->setCurrentItem(i);
            break;
        }
    }
}

void SlideShowConfig::loadEffectNamesGL()
{
    m_effectsComboBox->clear();

    QMap<QString,QString> effectNames = SlideShowGL::effectNamesI18N();
    QStringList effects;

    QMap<QString,QString>::Iterator it;
    for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.data());

    m_effectsComboBox->insertStringList(effects);

    for (int i=0; i<m_effectsComboBox->count(); i++) {
        if (effectNames[effectNameGL_] == m_effectsComboBox->text(i)) {
            m_effectsComboBox->setCurrentItem(i);
            break;
        }
    }
}

void SlideShowConfig::readSettings()
{
    bool  opengl;
    int   delay;
    bool  printFileName;
    bool  printFileComments;
    bool  loop;
    bool  shuffle;
    bool  showSelectedFilesOnly;


    opengl                = config_->readBoolEntry("OpenGL", false);
    delay                 = config_->readNumEntry("Delay", 1500);
    printFileName         = config_->readBoolEntry("Print Filename", true);
    printFileComments     = config_->readBoolEntry("Print Comments", false);
    loop                  = config_->readBoolEntry("Loop", false);
    shuffle               = config_->readBoolEntry("Shuffle", false);
    showSelectedFilesOnly = config_->readBoolEntry("Show Selected Files Only", false);
    effectName_           = config_->readEntry("Effect Name", "Random");
    effectNameGL_         = config_->readEntry("Effect Name (OpenGL)", "Random");
    
    m_useMillisecondsCheckBox->setChecked(config_->readBoolEntry("Use Milliseconds", false));
    
    // Comments tab settings
    uint  commentsFontColor;
    uint  commentsBgColor;
    int   commentsLinesLength;
    
    QFont *savedFont = new QFont();
    savedFont->setFamily(config_->readEntry("Comments Font Family"));
    savedFont->setPointSize(config_->readNumEntry("Comments Font Size", 10 ));
    savedFont->setBold(config_->readBoolEntry("Comments Font Bold", false));
    savedFont->setItalic(config_->readBoolEntry("Comments Font Italic", false));
    savedFont->setUnderline(config_->readBoolEntry("Comments Font Underline", false));
    savedFont->setOverline(config_->readBoolEntry("Comments Font Overline", false));
    savedFont->setStrikeOut(config_->readBoolEntry("Comments Font StrikeOut", false));
    savedFont->setFixedPitch(config_->readBoolEntry("Comments Font FixedPitch", false));
    
    commentsFontColor     = config_->readUnsignedNumEntry("Comments Font Color", 0xffffff);
    commentsBgColor       = config_->readUnsignedNumEntry("Comments Bg Color", 0x000000);
    
    commentsLinesLength   = config_->readNumEntry("Comments Lines Length", 72);
    // -- Apply Settings to widgets ------------------------------

    m_openglCheckBox->setChecked(opengl);

    m_delaySpinBox->setValue(delay);

    m_printNameCheckBox->setChecked(printFileName);

    m_printCommentsCheckBox->setChecked(printFileComments);

    m_loopCheckBox->setChecked(loop);

    m_shuffleCheckBox->setChecked(shuffle);

    if (showSelectedFilesOnly && m_selectedFilesButton->isEnabled() )
        m_selectedFilesButton->setChecked(true);
    else
        m_allFilesButton->setChecked(true);

    m_commentsLinesLengthSpinBox->setValue(commentsLinesLength);
    m_commentsFontColor->setColor(QColor(commentsFontColor));
    m_commentsBgColor->setColor(QColor(commentsBgColor));
    m_commentsFontChooser->setFont(*savedFont);
    delete savedFont;

    slotOpenGLToggled();
}

void SlideShowConfig::saveSettings()
{
    if (!config_) return;

    bool useMilliseconds = m_useMillisecondsCheckBox->isChecked();

    config_->writeEntry("OpenGL", m_openglCheckBox->isChecked());

    // Delay will be always saved as millisecond value, to keep compatibility
    if ( useMilliseconds ) config_->writeEntry("Delay", m_delaySpinBox->value());
    else config_->writeEntry("Delay", m_delaySpinBox->value()*1000); 

    config_->writeEntry("Print Filename", m_printNameCheckBox->isChecked());
    config_->writeEntry("Print Comments", m_printCommentsCheckBox->isChecked());
    config_->writeEntry("Loop", m_loopCheckBox->isChecked());
    config_->writeEntry("Shuffle", m_shuffleCheckBox->isChecked());
    config_->writeEntry("Show Selected Files Only", m_selectedFilesButton->isChecked());

    config_->writeEntry("Use Milliseconds", useMilliseconds); 

    // Comments tab settings
    QFont* commentsFont = new QFont(m_commentsFontChooser->font());
    config_->writeEntry("Comments Font Family", commentsFont->family());
    config_->writeEntry("Comments Font Size", commentsFont->pointSize());
    config_->writeEntry("Comments Font Bold", commentsFont->bold());
    config_->writeEntry("Comments Font Italic", commentsFont->italic());
    config_->writeEntry("Comments Font Underline", commentsFont->underline());
    config_->writeEntry("Comments Font Overline", commentsFont->overline());
    config_->writeEntry("Comments Font StrikeOut", commentsFont->strikeOut());
    config_->writeEntry("Comments Font FixedPitch", commentsFont->fixedPitch());
    delete commentsFont;
    
    QColor* fontColor = new QColor(m_commentsFontColor->color());
    uint commentsFontColorRGB = fontColor->rgb(); 
    delete fontColor;
    config_->writeEntry("Comments Font Color", commentsFontColorRGB);

    QColor* bgColor = new QColor(m_commentsBgColor->color());
    uint commentsBgColorRGB = bgColor->rgb();
    delete bgColor;
    config_->writeEntry("Comments Bg Color", commentsBgColorRGB);

    config_->writeEntry("Comments Lines Length", m_commentsLinesLengthSpinBox->value());
    
    if (!m_openglCheckBox->isChecked()) {

        QString effect;
        QMap<QString,QString> effectNames = SlideShow::effectNamesI18N();
        QMap<QString,QString>::Iterator it;

        for (it = effectNames.begin(); it != effectNames.end(); ++it) {
            if (it.data() == m_effectsComboBox->currentText()) {
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
            if (it.data() == m_effectsComboBox->currentText()) {
                effect = it.key();
                break;
            }
        }

        config_->writeEntry("Effect Name (OpenGL)", effect);

    }

    config_->sync();
}

void SlideShowConfig::slotCommentsBgColorChanged()
{
    m_commentsFontChooser->setBackgroundColor(m_commentsBgColor->color());
}

void SlideShowConfig::slotCommentsFontColorChanged()
{
    m_commentsFontChooser->setColor(m_commentsFontColor->color());
}

void SlideShowConfig::slotPrintCommentsToggled()
{
    m_tabWidget->setTabEnabled(commentsTab, m_printCommentsCheckBox->isChecked());
}

void SlideShowConfig::slotUseMillisecondsToggled()
{

    int delayValue = m_delaySpinBox->value();
    
    m_delaySpinBox->setValue(0);
    
    if ( m_useMillisecondsCheckBox -> isChecked() ) {
        m_delayLabel->setText(QString("Delay between images (ms):"));

        m_delaySpinBox->setMinValue(m_delayMsMinValue);
        m_delaySpinBox->setMaxValue(m_delayMsMaxValue);
        m_delaySpinBox->setLineStep(m_delayMsLineStep); 
        
        m_delaySpinBox->setValue(delayValue*1000);
    }
    else { 
        m_delayLabel->setText(QString("Delay between images  (s):"));
        
        m_delaySpinBox->setMinValue(m_delayMsMinValue/1000);
        m_delaySpinBox->setMaxValue(m_delayMsMaxValue/100);
        m_delaySpinBox->setLineStep(m_delayMsLineStep/10); 
        
        m_delaySpinBox->setValue(delayValue/1000);
    }
}

void SlideShowConfig::slotOpenGLToggled()
{
    if (m_openglCheckBox->isChecked()) {
        loadEffectNamesGL();
    }
    else {
        loadEffectNames();
    }
}

void SlideShowConfig::slotStartClicked()
{
    saveSettings();

    emit buttonStartClicked();
}

void SlideShowConfig::slotHelp()
{
    KApplication::kApplication()->invokeHelp("slideshow",
                                             "kipi-plugins");
}

}  // NameSpace KIPISlideShowPlugin

