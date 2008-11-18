/* ============================================================
 * File  : caltemplate.cpp
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Orgad Shaneh <orgads@gmail.com>
 * Date  : 2008-11-13
 * Description: template selection for calendar
 *
 * Copyright 2003 by Renchi Raju
 * Copyright 2008 by Orgad Shaneh
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

// Local includes.

#include "caltemplate.h"
#include "calsettings.h"

namespace KIPICalendarPlugin
{

CalTemplate::CalTemplate(QWidget* parent)
           : QWidget(parent)
{
    ui.setupUi( this );

    CalSettings *settings = CalSettings::instance();

    // set initial settings
    settings->setPaperSize(ui.paperSizeCombo->currentText());
    settings->setImagePos(ui.imagePosButtonGroup->selected());
    settings->setDrawLines(ui.drawLinesCheckBox->isChecked());
    settings->setRatio(ui.ratioSlider->value());
    settings->setFont(ui.fontCombo->currentText());

    ui.calendarWidget->recreate();

    connect(ui.paperSizeCombo, SIGNAL(currentIndexChanged(const QString &)),
            settings, SLOT(setPaperSize(const QString &)));
    connect(ui.imagePosButtonGroup, SIGNAL(changed(int)),
            settings, SLOT(setImagePos(int)));
    connect(ui.drawLinesCheckBox, SIGNAL(toggled(bool)),
            settings, SLOT(setDrawLines(bool)));
    connect(ui.ratioSlider, SIGNAL(valueChanged(int)),
            settings, SLOT(setRatio(int)));
    connect(ui.fontCombo, SIGNAL(currentIndexChanged(const QString &)),
            settings, SLOT(setFont(const QString &)));

    connect(settings, SIGNAL(settingsChanged()),
            ui.calendarWidget, SLOT(recreate()));
}

CalTemplate::~CalTemplate()
{
}

}  // NameSpace KIPICalendarPlugin
