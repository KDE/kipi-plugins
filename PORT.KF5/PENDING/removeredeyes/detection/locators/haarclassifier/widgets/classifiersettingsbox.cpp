/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-01
 * Description : a widget that holds all classifier settings
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

#include "classifiersettingsbox.moc"

// Qt includes

#include <QCheckBox>
#include <QGridLayout>

// KDE includes

#include <klocale.h>
#include <kurlrequester.h>
#include <knuminput.h>

namespace KIPIRemoveRedEyesPlugin
{

struct ClassifierSettingsBox::Private
{
    Private()
    {
        standardClassifierCheckBox  = 0;
        neighborGroupsNumInput      = 0;
        scalingFactorNumInput       = 0;
        classifierUrlRequester      = 0;
    }

    QCheckBox*       standardClassifierCheckBox;

    KIntNumInput*    neighborGroupsNumInput;
    KDoubleNumInput* scalingFactorNumInput;
    KUrlRequester*   classifierUrlRequester;
};

ClassifierSettingsBox::ClassifierSettingsBox(QWidget* const parent)
    : QGroupBox(parent), d(new Private)
{
    setTitle(i18n("Classifier Settings"));

    d->standardClassifierCheckBox    = new QCheckBox(i18n("Use &standard classifier"));
    d->classifierUrlRequester        = new KUrlRequester;
    d->classifierUrlRequester->setFilter("*.xml");
    d->standardClassifierCheckBox->setToolTip(i18n("If checked, the standard classifier "
                                                   "shipped with this KIPI plugin is used."));
    d->classifierUrlRequester->setToolTip(i18n("Enter the location of the classifier here."));

    d->neighborGroupsNumInput    = new KIntNumInput;
    d->neighborGroupsNumInput->setLabel(i18n("&Neighbor groups:"));
    d->neighborGroupsNumInput->setRange(1, 10, 1);
    d->neighborGroupsNumInput->setSliderEnabled(true);

    d->scalingFactorNumInput    = new KDoubleNumInput;
    d->scalingFactorNumInput->setLabel(i18n("&Scaling factor:"));
    d->scalingFactorNumInput->setRange(1.05, 10.0, 0.01);

    QGridLayout* const mainLayout = new QGridLayout;
    mainLayout->addWidget(d->standardClassifierCheckBox, 0, 0, 1, 1);
    mainLayout->addWidget(d->classifierUrlRequester,     1, 0, 1, 1);
    mainLayout->addWidget(d->neighborGroupsNumInput,     2, 0, 1,-1);
    mainLayout->addWidget(d->scalingFactorNumInput,      3, 0, 1,-1);
    mainLayout->setRowStretch(1, 10);
    setLayout(mainLayout);

    connect(d->standardClassifierCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(standardClassifierChecked()));

    connect(d->standardClassifierCheckBox, SIGNAL(stateChanged(int)),
            this, SIGNAL(settingsChanged()));

    connect(d->neighborGroupsNumInput, SIGNAL(valueChanged(int)),
            this, SIGNAL(settingsChanged()));

    connect(d->scalingFactorNumInput, SIGNAL(valueChanged(double)),
            this, SIGNAL(settingsChanged()));
}

ClassifierSettingsBox::~ClassifierSettingsBox()
{
    delete d;
}

void ClassifierSettingsBox::standardClassifierChecked()
{
    d->classifierUrlRequester->setEnabled(!d->standardClassifierCheckBox->isChecked());
}

bool ClassifierSettingsBox::useStandardClassifier() const
{
    return d->standardClassifierCheckBox->isChecked();
}

void ClassifierSettingsBox::setUseStandardClassifier(bool mode)
{
    d->standardClassifierCheckBox->setChecked(mode);
}

int ClassifierSettingsBox::neighborGroups() const
{
    return d->neighborGroupsNumInput->value();
}

void ClassifierSettingsBox::setNeighborGroups(int value)
{
    d->neighborGroupsNumInput->setValue(value);
}

double ClassifierSettingsBox::scalingFactor() const
{
    return d->scalingFactorNumInput->value();
}

void ClassifierSettingsBox::setScalingFactor(double value)
{
    d->scalingFactorNumInput->setValue(value);
}

QString ClassifierSettingsBox::classifierUrl() const
{
    return d->classifierUrlRequester->url().pathOrUrl();
}

void ClassifierSettingsBox::setClassifierUrl(const QString& url)
{
    d->classifierUrlRequester->setUrl(url);
}

} // namespace KIPIRemoveRedEyesPlugin
