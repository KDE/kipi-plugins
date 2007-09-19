/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-09-13
 * Description : save settings widgets
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

// Qt includes.

#include <qcombobox.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qstring.h>

// KDE includes.

#include <kdialog.h>
#include <klocale.h>

// Local includes.

#include "savesettingswidget.h"
#include "savesettingswidget.moc"

namespace KIPIRawConverterPlugin
{

class SaveSettingsWidgetPriv
{
public:

    SaveSettingsWidgetPriv()
    {
        formatLabel         = 0;
        conflictLabel       = 0;
        conflictButtonGroup = 0;
        formatComboBox      = 0;
        overwriteButton     = 0;
        promptButton        = 0;
    }

    QLabel        *formatLabel;
    QLabel        *conflictLabel;

    QVButtonGroup *conflictButtonGroup;

    QComboBox     *formatComboBox;

    QRadioButton  *overwriteButton;
    QRadioButton  *promptButton;
};

SaveSettingsWidget::SaveSettingsWidget(QWidget *parent)
                  : QWidget(parent, 0, Qt::WDestructiveClose)
{
    d = new SaveSettingsWidgetPriv;
    QGridLayout* settingsBoxLayout = new QGridLayout(this, 3, 1, KDialog::spacingHint());

    d->formatLabel    = new QLabel(i18n("Output file format:"), this);
    d->formatComboBox = new QComboBox( false, this );
    d->formatComboBox->insertItem( "JPEG", OUTPUT_JPEG );
    d->formatComboBox->insertItem( "TIFF", OUTPUT_TIFF );
    d->formatComboBox->insertItem( "PPM",  OUTPUT_PPM );
    d->formatComboBox->insertItem( "PNG",  OUTPUT_PNG );
    QWhatsThis::add(d->formatComboBox, i18n("<p>Set here the output file format to use:<p>"
                                       "<b>JPEG</b>: output the processed image in JPEG Format. "
                                       "this format will give smaller-sized files. Minimum JPEG "
                                       "compression level will be used during Raw conversion.<p>"
                                       "<b>Warning!!! duing of destructive compression algorithm, "
                                       "JPEG is a lossy quality format.</b><p>"
                                       "<b>TIFF</b>: output the processed image in TIFF Format. "
                                       "This generates larges, without "
                                       "losing quality. Adobe Deflate compression "
                                       "will be used during conversion.<p>"
                                       "<b>PPM</b>: output the processed image in PPM Format. "
                                       "This generates the largest files, without "
                                       "losing quality.<p>"
                                       "<b>PNG</b>: output the processed image in PNG Format. "
                                       "This generates larges, without "
                                       "losing quality. Maximum PNG compression "
                                       "will be used during conversion."));

    d->conflictLabel       = new QLabel(i18n("If Target File Exists:"), this);
    d->conflictButtonGroup = new QVButtonGroup(this);
    d->overwriteButton     = new QRadioButton(i18n("Overwrite automatically"), d->conflictButtonGroup);
    d->promptButton        = new QRadioButton(i18n("Open rename-file dialog"), d->conflictButtonGroup);
    d->conflictButtonGroup->insert(d->overwriteButton, OVERWRITE);
    d->conflictButtonGroup->insert(d->promptButton,    ASKTOUSER);
    d->conflictButtonGroup->setRadioButtonExclusive(true);
    d->overwriteButton->setChecked(true);
    d->conflictButtonGroup->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
    d->conflictButtonGroup->setInsideMargin(0);
    
    settingsBoxLayout->addMultiCellWidget(d->formatLabel, 0, 0, 0, 0);   
    settingsBoxLayout->addMultiCellWidget(d->formatComboBox, 0, 0, 1, 1);   
    settingsBoxLayout->addMultiCellWidget(d->conflictLabel, 1, 1, 0, 1);   
    settingsBoxLayout->addMultiCellWidget(d->conflictButtonGroup, 2, 2, 0, 1);   
    settingsBoxLayout->setRowStretch(3, 10);   

    connect(d->formatComboBox, SIGNAL(activated(int)),
            this, SIGNAL(signalSaveFormatChanged()));
}

SaveSettingsWidget::~SaveSettingsWidget()
{
    delete d;
}

void SaveSettingsWidget::setDefaultSettings()
{
    setFileFormat(OUTPUT_PNG); 
    setConflictRule(OVERWRITE);
}

SaveSettingsWidget::OutputFormat SaveSettingsWidget::fileFormat()
{
    return(OutputFormat)(d->formatComboBox->currentItem());
}

void SaveSettingsWidget::setFileFormat(SaveSettingsWidget::OutputFormat f)
{
    d->formatComboBox->setCurrentItem((int)f);
}

SaveSettingsWidget::ConflictRule SaveSettingsWidget::conflictRule()
{
    return((ConflictRule)(d->conflictButtonGroup->selectedId()));
}

void SaveSettingsWidget::setConflictRule(SaveSettingsWidget::ConflictRule r)
{
    d->conflictButtonGroup->setButton((int)r);
}

} // NameSpace KIPIRawConverterPlugin
