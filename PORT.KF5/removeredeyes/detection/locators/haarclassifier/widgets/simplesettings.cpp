/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-10-31
 * Description : a widget to display simple settings
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "simplesettings.moc"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QSlider>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>

// Local includes

#include "haarsettings.h"

namespace KIPIRemoveRedEyesPlugin
{

struct SimpleSettings::Private
{
    Private()
    {
        settingsSlider = 0;
    }

    QSlider*     settingsSlider;
    HaarSettings settings;
};

SimpleSettings::SimpleSettings(QWidget* const parent)
    : QWidget(parent), d(new Private)
{
    d->settingsSlider = new QSlider(Qt::Vertical);
    d->settingsSlider->setRange(0, 2);
    d->settingsSlider->setValue(Standard);
    d->settingsSlider->setTickPosition(QSlider::TicksRight);

    QString sliderStyle("QSlider::groove:vertical {"
                        "   border: 1px solid #999999;"
                        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                        "               stop:0 #ff0000, stop:0.5 #ffff00 stop:1 #00ff00);"
                        "   width:4px;"
                        "   margin: 2px 0;"
                        "}"

                        "QSlider::handle:vertical {"
                        "    height: 10px;"
                        "    border: 1px solid #999999;"
                        "    border-radius: 4px;"
                        "   background: #dddddd;"
                        "   margin: 0 -8px; /* expand outside the groove */"
                        "}"

                        "QSlider::handle:vertical::hover {"
                        "   background: #cccccc;"
                        "   margin: 0 -8px; /* expand outside the groove */"
                        "}"
                       );
    d->settingsSlider->setStyleSheet(sliderStyle);

    QLabel* const lSlow = new QLabel(i18n("<p><b>Slower<br/>(low-res / slightly blurred images)</b></p>"
                                    "<p>In this mode the automatic detection and correction of "
                                    "red-eye is the most reliable. The drawback "
                                    "of the higher detection rate is a much slower computation.</p>"));

    QLabel* const lStd = new QLabel(i18n("<p><b>Standard</b></p>"
                                   "<p>This mode is adequate for most image resolutions. Always try "
                                   "this mode first.</p>"));

    QLabel* const lFast = new QLabel(i18n("<p><b>Faster<br/>(high-res images)</b></p>"
                                    "<p>In this mode the automatic detection will be faster, but more "
                                    "false positives may occur. Use this mode only for very high-resolution "
                                    "and sharp images.</p>"));

    lSlow->setWordWrap(true);
    lStd->setWordWrap(true);
    lFast->setWordWrap(true);

    // --------------------------------------------------------

    const QString colorStyle("background: #999999;");
    const unsigned int maxColorLabelHeight = 1;

    QLabel* const coloredSpacer = new QLabel;
    coloredSpacer->setStyleSheet(colorStyle);
    coloredSpacer->setMaximumHeight(maxColorLabelHeight);

    QLabel* const coloredSpacer2 = new QLabel;
    coloredSpacer2->setStyleSheet(colorStyle);
    coloredSpacer2->setMaximumHeight(maxColorLabelHeight);

    // --------------------------------------------------------

    QGridLayout* const sliderLayout = new QGridLayout;
    sliderLayout->addWidget(d->settingsSlider,  0, 0, 5, 1);
    sliderLayout->addWidget(lSlow,              0, 1, 1, 1);
    sliderLayout->addWidget(coloredSpacer,      1, 0, 1, 2);
    sliderLayout->addWidget(lStd,               2, 1, 1, 1);
    sliderLayout->addWidget(coloredSpacer2,     3, 0, 1, 2);
    sliderLayout->addWidget(lFast,              4, 1, 1, 1);
    sliderLayout->setSpacing(10);
    sliderLayout->setColumnStretch(1, 10);
    sliderLayout->setRowStretch(6, 10);

    QGridLayout* const mainLayout = new QGridLayout;
    mainLayout->addLayout(sliderLayout,     0, 0, 1, 1);
    mainLayout->setRowStretch(2, 10);
    setLayout(mainLayout);

    connect(d->settingsSlider, SIGNAL(valueChanged(int)),
            this, SLOT(simpleModeChanged(int)));

    connect(d->settingsSlider, SIGNAL(valueChanged(int)),
            this, SIGNAL(settingsChanged()));
}

SimpleSettings::~SimpleSettings()
{
    delete d;
}

void SimpleSettings::simpleModeChanged(int value)
{
    switch (value)
    {
        case Standard:
            d->settings.minRoundness   = 31;
            d->settings.scaleFactor    = 1.2;
            d->settings.minBlobsize    = 10;
            d->settings.neighborGroups = 2;
            break;

        case Fast:
            d->settings.minRoundness   = 50;
            d->settings.scaleFactor    = 1.2;
            d->settings.minBlobsize    = 3;
            d->settings.neighborGroups = 2;
            break;

        case Slow:
            d->settings.minRoundness   = 31;
            d->settings.scaleFactor    = 1.05;
            d->settings.minBlobsize    = 6;
            d->settings.neighborGroups = 2;
            break;
    }

    d->settings.simpleMode = value;
}

void SimpleSettings::prepareSettings()
{
    d->settings.useStandardClassifier  = true;
    d->settings.useSimpleMode          = true;
    d->settings.simpleMode             = d->settingsSlider->value();
    d->settings.classifierFile         = STANDARD_CLASSIFIER;
}

void SimpleSettings::loadSettings(HaarSettings& newSettings)
{
    d->settings = newSettings;
    d->settingsSlider->setValue(d->settings.simpleMode);
    simpleModeChanged(d->settings.simpleMode);
}

HaarSettings SimpleSettings::readSettings()
{
    prepareSettings();
    return d->settings;
}

int SimpleSettings::simpleMode() const
{
    return d->settings.simpleMode;
}

} // namespace KIPIRemoveRedEyesPlugin
