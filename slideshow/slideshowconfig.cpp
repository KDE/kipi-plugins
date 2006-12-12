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

#include "interface.h"

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
    connect(printCommentsCheckBox_, SIGNAL(toggled(bool)), this, SLOT(slotPrintCommentsToggled()));
    connect(m_commentsFontColor, SIGNAL(changed(const QColor &)), this, SLOT(slotCommentsFontColorChanged()));
    connect(m_commentsBgColor, SIGNAL(changed(const QColor &)), this, SLOT(slotCommentsBgColorChanged()));

    // Configuration file management 

    config_ = new KConfig("kipirc");
    config_->setGroup("SlideShow Settings");

    readSettings();

    // Comments tab management

    m_commentsFontChooser->setSampleText(
            i18n("Slideshow is part of KIPI-Plugins (http://www.kipi-plugins.org)"));

    // Host application images has comments
    if ( ! ImagesHasComments ) {
        printCommentsCheckBox_->setEnabled(FALSE);
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

    openglCheckBox_->setChecked(opengl);

    delaySpinBox_->setValue(delay);

    printNameCheckBox_->setChecked(printFileName);

    printCommentsCheckBox_->setChecked(printFileComments);

    loopCheckBox_->setChecked(loop);

    shuffleCheckBox_->setChecked(shuffle);

    if (showSelectedFilesOnly && selectedFilesButton_->isEnabled() )
        selectedFilesButton_->setChecked(true);
    else
        allFilesButton_->setChecked(true);

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

    config_->writeEntry("OpenGL", openglCheckBox_->isChecked());
    config_->writeEntry("Delay", delaySpinBox_->value());
    config_->writeEntry("Print Filename", printNameCheckBox_->isChecked());
    config_->writeEntry("Print Comments", printCommentsCheckBox_->isChecked());
    config_->writeEntry("Loop", loopCheckBox_->isChecked());
    config_->writeEntry("Shuffle", shuffleCheckBox_->isChecked());
    config_->writeEntry("Show Selected Files Only", selectedFilesButton_->isChecked());

    
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
    m_tabWidget->setTabEnabled(commentsTab, printCommentsCheckBox_->isChecked());
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

