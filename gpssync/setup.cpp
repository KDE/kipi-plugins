/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.kipi-plugins.org">http://www.kipi-plugins.org</a>
 *
 * @date   2010-08-27
 * @brief  Setup widget for geo correlator.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "setup.moc"

// KDE includes

#include <kconfig.h>
#include <klocale.h>

// local includes

#include "setup_general.h"

namespace KIPIGPSSyncPlugin
{

class SetupGlobalObjectCreator
{
public:

    SetupGlobalObject object;
};

K_GLOBAL_STATIC(SetupGlobalObjectCreator, setupGlobalObjectCreator);

class SetupGlobalObject::Private
{
public:
    Private()
    {
    }

    QHash<QString, QVariant> settings;
};

SetupGlobalObject::SetupGlobalObject()
 : QObject(), d(new Private())
{
}
    
SetupGlobalObject::~SetupGlobalObject()
{
    delete d;
}

SetupGlobalObject* SetupGlobalObject::instance()
{
    return &(setupGlobalObjectCreator->object);
}

QVariant SetupGlobalObject::readEntry(const QString& name)
{
    return d->settings.value(name);
}

void SetupGlobalObject::writeEntry(const QString& name, const QVariant& value)
{
    d->settings[name] = value;
}

void SetupGlobalObject::triggerSignalSetupChanged()
{
    emit(signalSetupChanged());
}

SetupTemplate::SetupTemplate(QWidget* const parent)
  : QWidget(parent)
{
}

SetupTemplate::~SetupTemplate()
{
}

class Setup::Private
{
public:
    Private()
    {
    }

    KPageWidgetItem* pageGeneral;
    SetupGeneral* setupGeneral;

};

Setup::Setup(QWidget* const parent)
  : KPageDialog(parent), d(new Private())
{
    setCaption(i18n("Configure"));
    setButtons(Apply|Ok|Cancel);
    setDefaultButton(Ok);
    setModal(true);

    d->setupGeneral = new SetupGeneral(this);
    d->pageGeneral = addPage(d->setupGeneral, i18nc("General setup", "General"));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotApplyClicked()));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOkClicked()));
}

Setup::~Setup()
{
    delete d;
}

void Setup::slotApplyClicked()
{
    d->setupGeneral->slotApplySettings();

    SetupGlobalObject::instance()->triggerSignalSetupChanged();
}

void Setup::slotOkClicked()
{
    slotApplyClicked();
    accept();
}

} /* namespace KIPIGPSSyncPlugin */

