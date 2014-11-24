/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "enfusesettings.moc"

// Qt includes

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>

// KDE includes

#include <kdialog.h>
#include <klocale.h>
#include <kseparator.h>

// Libkdcraw includes

#include <libkdcraw/rnuminput.h>

using namespace KDcrawIface;

namespace KIPIExpoBlendingPlugin
{

class EnfuseSettingsWidget::EnfuseSettingsWidgetPriv
{
public:

    EnfuseSettingsWidgetPriv()
    {
        autoLevelsCB    = 0;
        levelsLabel     = 0;
        levelsInput     = 0;
        hardMaskCB      = 0;
        exposureLabel   = 0;
        exposureInput   = 0;
        saturationLabel = 0;
        saturationInput = 0;
        contrastLabel   = 0;
        contrastInput   = 0;
        ciecam02CB      = 0;
    }

    QCheckBox*       autoLevelsCB;
    QCheckBox*       hardMaskCB;
    QCheckBox*       ciecam02CB;

    QLabel*          levelsLabel;
    QLabel*          exposureLabel;
    QLabel*          saturationLabel;
    QLabel*          contrastLabel;

    RIntNumInput*    levelsInput;

    RDoubleNumInput* exposureInput;
    RDoubleNumInput* saturationInput;
    RDoubleNumInput* contrastInput;
};

EnfuseSettingsWidget::EnfuseSettingsWidget(QWidget* const parent)
    : QWidget(parent), d(new EnfuseSettingsWidgetPriv)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* grid = new QGridLayout(this);

    // ------------------------------------------------------------------------

    d->autoLevelsCB = new QCheckBox(i18nc("enfuse settings", "Auto Levels"), this);
    d->autoLevelsCB->setWhatsThis( i18n("Set automatic level selection (maximized) for pyramid blending."));

    d->levelsLabel  = new QLabel(i18nc("enfuse settings", "Levels:"));
    d->levelsInput  = new RIntNumInput();
    d->levelsInput->setRange(1, 29, 1);
    d->levelsInput->setSliderEnabled(true);
    d->levelsInput->setDefaultValue(20);
    d->levelsInput->setWhatsThis(i18n("Set the number of levels for pyramid blending. "
                                      "A low number trades off quality of results for faster "
                                      "execution time and lower memory usage."));

    d->hardMaskCB = new QCheckBox(i18n("Hard Mask"), this);
    d->hardMaskCB->setWhatsThis(i18n("Force hard blend masks and no averaging on finest "
                                     "scale. This is especially useful for focus "
                                     "stacks with thin and high contrast features, "
                                     "improves sharpness at the expense of increased noise."));

    d->exposureLabel = new QLabel(i18nc("enfuse settings", "Exposure:"));
    d->exposureInput = new RDoubleNumInput();
    d->exposureInput->setDecimals(2);
    d->exposureInput->input()->setRange(0.0, 1.0, 0.01, true);
    d->exposureInput->setDefaultValue(1.0);
    d->exposureInput->setWhatsThis( i18n("Set the exposure contribution for the blending process. "
                                         "Higher values will favor well-exposed pixels."));

    d->saturationLabel = new QLabel(i18nc("enfuse settings", "Saturation:"));
    d->saturationInput = new RDoubleNumInput();
    d->saturationInput->setDecimals(2);
    d->saturationInput->input()->setRange(0.0, 1.0, 0.01, true);
    d->saturationInput->setDefaultValue(0.2);
    d->saturationInput->setWhatsThis( i18n("Increasing this value makes pixels with high "
                                           "saturation contribute more to the final output."));

    d->contrastLabel = new QLabel(i18nc("enfuse settings", "Contrast:"));
    d->contrastInput = new RDoubleNumInput();
    d->contrastInput->setDecimals(2);
    d->contrastInput->input()->setRange(0.0, 1.0, 0.01, true);
    d->contrastInput->setDefaultValue(0.0);
    d->contrastInput->setWhatsThis(i18n("Sets the relative weight of high-contrast pixels. "
                                        "Increasing this weight makes pixels with neighboring differently colored "
                                        "pixels contribute more to the final output. Particularly useful for focus stacks."));

    d->ciecam02CB = new QCheckBox(i18n("Use Color Appearance Modelling"), this);
    d->ciecam02CB->setWhatsThis(i18n("Use Color Appearance Modelling (CIECAM02) to render detailed colors. "
                                     "Your input files should have embedded ICC profiles. If no ICC profile is present, "
                                     "sRGB color space will be used instead. The difference between using this option "
                                     "and default color blending algorithm is very slight, and will be most noticeable "
                                     "when you need to blend areas of different primary colors together."));

    // ------------------------------------------------------------------------

    grid->addWidget(d->autoLevelsCB,    0, 0, 1, 2);
    grid->addWidget(d->levelsLabel,     1, 0, 1, 1);
    grid->addWidget(d->levelsInput,     1, 1, 1, 1);
    grid->addWidget(d->hardMaskCB,      2, 0, 1, 2);
    grid->addWidget(d->exposureLabel,   3, 0, 1, 1);
    grid->addWidget(d->exposureInput,   3, 1, 1, 1);
    grid->addWidget(d->saturationLabel, 4, 0, 1, 1);
    grid->addWidget(d->saturationInput, 4, 1, 1, 1);
    grid->addWidget(d->contrastLabel,   5, 0, 1, 1);
    grid->addWidget(d->contrastInput,   5, 1, 1, 1);
    grid->addWidget(d->ciecam02CB,      6, 0, 1, 2);
    grid->setRowStretch(7, 10);
    grid->setMargin(KDialog::spacingHint());
    grid->setSpacing(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    connect(d->autoLevelsCB, SIGNAL(toggled(bool)),
            d->levelsLabel, SLOT(setDisabled(bool)));

    connect(d->autoLevelsCB, SIGNAL(toggled(bool)),
            d->levelsInput, SLOT(setDisabled(bool)));
}

EnfuseSettingsWidget::~EnfuseSettingsWidget()
{
    delete d;
}

void EnfuseSettingsWidget::resetToDefault()
{
    d->autoLevelsCB->setChecked(true);
    d->levelsInput->slotReset();
    d->hardMaskCB->setChecked(false);
    d->exposureInput->slotReset();
    d->saturationInput->slotReset();
    d->contrastInput->slotReset();
    d->ciecam02CB->setChecked(false);
}

void EnfuseSettingsWidget::setSettings(const EnfuseSettings& settings)
{
    d->autoLevelsCB->setChecked(settings.autoLevels);
    d->levelsInput->setValue(settings.levels);
    d->hardMaskCB->setChecked(settings.hardMask);
    d->exposureInput->setValue(settings.exposure);
    d->saturationInput->setValue(settings.saturation);
    d->contrastInput->setValue(settings.contrast);
    d->ciecam02CB->setChecked(settings.ciecam02);
}

EnfuseSettings EnfuseSettingsWidget::settings() const
{
    EnfuseSettings settings;
    settings.autoLevels = d->autoLevelsCB->isChecked();
    settings.levels     = d->levelsInput->value();
    settings.hardMask   = d->hardMaskCB->isChecked();
    settings.exposure   = d->exposureInput->value();
    settings.saturation = d->saturationInput->value();
    settings.contrast   = d->contrastInput->value();
    settings.ciecam02   = d->ciecam02CB->isChecked();
    return settings;
}

void EnfuseSettingsWidget::readSettings(KConfigGroup& group)
{
    d->autoLevelsCB->setChecked(group.readEntry("Auto Levels",       true));
    d->levelsInput->setValue(group.readEntry("Levels Value",         d->levelsInput->defaultValue()));
    d->hardMaskCB->setChecked(group.readEntry("Hard Mask",           false));
    d->exposureInput->setValue(group.readEntry("Exposure Value",     d->exposureInput->defaultValue()));
    d->saturationInput->setValue(group.readEntry("Saturation Value", d->saturationInput->defaultValue()));
    d->contrastInput->setValue(group.readEntry("Contrast Value",     d->contrastInput->defaultValue()));
    d->ciecam02CB->setChecked(group.readEntry("CIECAM02",            false));
}

void EnfuseSettingsWidget::writeSettings(KConfigGroup& group)
{
    group.writeEntry("Auto Levels",      d->autoLevelsCB->isChecked());
    group.writeEntry("Levels Value",     d->levelsInput->value());
    group.writeEntry("Hard Mask",        d->hardMaskCB->isChecked());
    group.writeEntry("Exposure Value",   d->exposureInput->value());
    group.writeEntry("Saturation Value", d->saturationInput->value());
    group.writeEntry("Contrast Value",   d->contrastInput->value());
    group.writeEntry("CIECAM02",         d->ciecam02CB->isChecked());
}

} // namespace KIPIExpoBlendingPlugin
