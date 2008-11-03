/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-10-31
 * Description : a widget to display simple settings
 *
 * Copyright (C) 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "simplesettings.h"
#include "simplesettings.moc"

// Qt includes.

#include <QGridLayout>
#include <QLabel>
#include <QSlider>

// KDE includes.

#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes.

#include "settings.h"
#include "storagesettingsbox.h"

namespace KIPIRemoveRedEyesPlugin
{

class SimpleSettingsPriv
{

public:

    SimpleSettingsPriv()
    {
        settingsSlider      = 0;
        storageSettingsBox  = 0;
        settings            = 0;
    }

    QSlider*            settingsSlider;
    StorageSettingsBox* storageSettingsBox;
    RemovalSettings*    settings;
};

SimpleSettings::SimpleSettings(QWidget* parent)
              : QWidget(parent),
                d(new SimpleSettingsPriv)
{
    d->settings       = new RemovalSettings;
    d->settingsSlider = new QSlider(Qt::Vertical);
    d->settingsSlider->setRange(0, 2);
    d->settingsSlider->setValue(Standard);
    d->settingsSlider->setTickPosition(QSlider::TicksRight);

    d->storageSettingsBox = new StorageSettingsBox;

    QString sliderStyle("QSlider::groove:vertical {"
                        "border: 1px solid #999999;"
                        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ff0000, stop:0.5 #ffff00 stop:1 #00ff00);"
                        "width:5px;"
                        "margin: 2px 0;"
                        "}"

                        "QSlider::handle:vertical {"
                        "height: 8px;"
                        "border: 1px solid #999999;"
                        "border-radius: 4px;"
                        "background: #cccccc;"
                        "margin: 0 -8px; /* expand outside the groove */"
                        "}"
    );
    d->settingsSlider->setStyleSheet(sliderStyle);

    QLabel* lSlow = new QLabel("<p><b>More reliable (very slow)</b></p>"
                               "<p>In this mode the automatic detection and correction of the "
                               "red eyes is the most reliable. The drawback "
                               "of the higher detection rate is a much slower computation.</p>");

    QLabel* lAcc = new QLabel("<p><b>Standard</b></p>"
                              "<p>This mode is quite accurate for most image resolutions. If the images "
                              "are not too small or blurry, detection is quite good. Always try this mode "
                              "first.</p>");

    QLabel* lFast = new QLabel("<p><b>Less reliable (faster)</b></p>"
                               "<p>In this mode the automatic detection and correction of the red eyes "
                               "will be very fast, but more false positives may occur. Use this mode "
                               "only for very high-resolution images.</p>");

    lSlow->setWordWrap(true);
    lAcc->setWordWrap(true);
    lFast->setWordWrap(true);

    // FIXME: since I'm not able to use the same object twice
    //        in a layout, I need two labels here
    QLabel* coloredSpacer = new QLabel;
    coloredSpacer->setStyleSheet("background: #999999;");
    coloredSpacer->setMaximumHeight(1);
    QLabel* coloredSpacer2 = new QLabel;
    coloredSpacer2->setStyleSheet("background: #999999;");
    coloredSpacer2->setMaximumHeight(1);

    QGridLayout* sliderLayout = new QGridLayout;
    sliderLayout->addWidget(d->settingsSlider,    0, 0, 5, 1);
    sliderLayout->addWidget(lSlow,                0, 1, 1, 1);
    sliderLayout->addWidget(coloredSpacer,        1, 0, 1, 2);
    sliderLayout->addWidget(lAcc,                 2, 1, 1, 1);
    sliderLayout->addWidget(coloredSpacer2,       3, 0, 1, 2);
    sliderLayout->addWidget(lFast,                4, 1, 1, 1);
    sliderLayout->setSpacing(10);
    sliderLayout->setColumnStretch(1, 10);
    sliderLayout->setRowStretch(6, 10);

    QGridLayout* storageLayout = new QGridLayout;
    storageLayout->addWidget(d->storageSettingsBox, 0, 0, 1, 1);
    storageLayout->setRowStretch(1, 10);

    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addLayout(sliderLayout,     0, 0, 1, 1);
    mainLayout->addLayout(storageLayout,    0, 1, 1, 1);
    mainLayout->setRowStretch(2, 10);
    setLayout(mainLayout);

    connect(d->settingsSlider, SIGNAL(valueChanged(int)),
            this, SLOT(simpleModeChanged(int)));

    connect(d->storageSettingsBox, SIGNAL(settingsChanged()),
            this, SLOT(storageSettingsChanged()));
}

SimpleSettings::~SimpleSettings()
{
    delete d->settings;
    delete d;
}

void SimpleSettings::simpleModeChanged(int value)
{
    switch (value)
    {
        case Standard:
            d->settings->minRoundness           = 3.2;
            d->settings->scaleFactor            = 1.2;
            d->settings->minBlobsize            = 10;
            d->settings->neighborGroups         = 2;
            break;

        case Fast:
            d->settings->minRoundness           = 6;
            d->settings->scaleFactor            = 3.6;
            d->settings->minBlobsize            = 20;
            d->settings->neighborGroups         = 1;
            break;

        case Slow:
            d->settings->minRoundness           = 3.2;
            d->settings->scaleFactor            = 1.05;
            d->settings->minBlobsize            = 10;
            d->settings->neighborGroups         = 2;
            break;
    }
}

void SimpleSettings::prepareSettings()
{
    d->settings->useStandardClassifier  = true;
    d->settings->simpleMode             = d->settingsSlider->value();
    d->settings->classifierFile         = STANDARD_CLASSIFIER;
    storageSettingsChanged();
}

void SimpleSettings::storageSettingsChanged()
{
    d->settings->storageMode            = d->storageSettingsBox->storageMode();
    d->settings->subfolderName          = d->storageSettingsBox->subfolder();
    d->settings->prefixName             = d->storageSettingsBox->prefix();
}

void SimpleSettings::loadSettings(RemovalSettings* newSettings)
{
    d->settings = newSettings;
    applySettings();
}

void SimpleSettings::applySettings()
{
    d->storageSettingsBox->setPrefix(d->settings->prefixName);
    d->storageSettingsBox->setSubfolder(d->settings->subfolderName);
    d->storageSettingsBox->setStorageMode(d->settings->storageMode);
    d->settingsSlider->setValue(d->settings->simpleMode);
    simpleModeChanged(d->settingsSlider->value());
}

RemovalSettings* SimpleSettings::readSettings()
{
    prepareSettings();
    return d->settings;
}

} // namespace KIPIRemoveRedEyesPlugin
