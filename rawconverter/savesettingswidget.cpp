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

#include <QComboBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>

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

    QLabel       *formatLabel;
    QLabel       *conflictLabel;

    QButtonGroup *conflictButtonGroup;

    QComboBox    *formatComboBox;

    QRadioButton *overwriteButton;
    QRadioButton *promptButton;
};

SaveSettingsWidget::SaveSettingsWidget(QWidget *parent)
                  : QWidget(parent)
{
    d = new SaveSettingsWidgetPriv;
    setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* settingsBoxLayout = new QGridLayout(this);

    d->formatLabel    = new QLabel(i18n("Output file format:"), this);
    d->formatComboBox = new QComboBox( this );
    d->formatComboBox->setWhatsThis(i18n("<p>Set here the output file format to use:<p>"
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
    slotPopulateImageFormat(false);

    d->conflictLabel       = new QLabel(i18n("If Target File Exists:"), this);
    QWidget *conflictBox   = new QWidget(this);
    QVBoxLayout *vlay      = new QVBoxLayout(conflictBox);
    d->conflictButtonGroup = new QButtonGroup(conflictBox);
    d->overwriteButton     = new QRadioButton(i18n("Overwrite automatically"), conflictBox);
    d->promptButton        = new QRadioButton(i18n("Open rename-file dialog"), conflictBox);
    d->conflictButtonGroup->addButton(d->overwriteButton, OVERWRITE);
    d->conflictButtonGroup->addButton(d->promptButton,    ASKTOUSER);
    d->conflictButtonGroup->setExclusive(true);
    d->overwriteButton->setChecked(true);
    
    vlay->setMargin(KDialog::spacingHint());
    vlay->setSpacing(KDialog::spacingHint());
    vlay->addWidget(d->overwriteButton);
    vlay->addWidget(d->promptButton);

    settingsBoxLayout->addWidget(d->formatLabel, 0, 0, 1, 1);   
    settingsBoxLayout->addWidget(d->formatComboBox, 0, 1, 1, 1);   
    settingsBoxLayout->addWidget(d->conflictLabel, 1, 0, 1, 1);   
    settingsBoxLayout->addWidget(conflictBox, 2, 0, 1, 1);   
    settingsBoxLayout->setRowStretch(3, 10);   
    settingsBoxLayout->setMargin(KDialog::spacingHint());
    settingsBoxLayout->setSpacing(KDialog::spacingHint());

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
    return(OutputFormat)(d->formatComboBox->currentIndex());
}

void SaveSettingsWidget::setFileFormat(SaveSettingsWidget::OutputFormat f)
{
    d->formatComboBox->setCurrentIndex((int)f);
}

SaveSettingsWidget::ConflictRule SaveSettingsWidget::conflictRule()
{
    return((ConflictRule)(d->conflictButtonGroup->checkedId()));
}

void SaveSettingsWidget::setConflictRule(SaveSettingsWidget::ConflictRule r)
{
    d->conflictButtonGroup->button((int)r)->setChecked(true);
}

void SaveSettingsWidget::slotPopulateImageFormat(bool sixteenBits)
{
    d->formatComboBox->clear();
    d->formatComboBox->insertItem( OUTPUT_PNG,  "PNG" );
    d->formatComboBox->insertItem( OUTPUT_TIFF, "TIFF" );

    if (!sixteenBits)
    {    
        d->formatComboBox->insertItem( OUTPUT_JPEG, "JPEG" );
        d->formatComboBox->insertItem( OUTPUT_PPM,  "PPM" );
    }
}

} // NameSpace KIPIRawConverterPlugin
