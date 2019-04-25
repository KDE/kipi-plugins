/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2012-04-04
 * Description : Tool dialog
 *
 * Copyright (C) 2012-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kptooldialog.h"

// Qt includes

#include <QIcon>
#include <QAction>
#include <QDialog>
#include <QMenu>
#include <QVBoxLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/PluginLoader>

// Local includes

#include "kpaboutdata.h"
#include "kipiplugins_debug.h"

namespace KIPIPlugins
{

class KPDialogBase::Private
{

public:

    Private()
    {
        about  = nullptr;
        dialog = nullptr;
        iface  = nullptr;

        PluginLoader* const pl = PluginLoader::instance();

        if (pl)
        {
            iface = pl->interface();
        }

    }

    QDialog*     dialog;

    Interface*   iface;
    KPAboutData* about;
};

KPDialogBase::KPDialogBase(QDialog* const dlg)
    : d(new Private)
{
    d->dialog = dlg;
}

KPDialogBase::~KPDialogBase()
{
    delete d->about;
    delete d;
}

Interface* KPDialogBase::iface() const
{
    return d->iface;
}

QPushButton* KPDialogBase::getHelpButton() const
{
    if (!d->dialog)
    {
        return nullptr;
    }

    {
        KPToolDialog* const dlg = dynamic_cast<KPToolDialog*>(d->dialog);

        if (dlg)
        {
            return dlg->helpButton();
        }
    }

    {
        KPWizardDialog* const dlg = dynamic_cast<KPWizardDialog*>(d->dialog);

        if (dlg)
        {
            return dlg->helpButton();
        }
    }

    return nullptr;
}

void KPDialogBase::setAboutData(KPAboutData* const data, QPushButton* help)
{
    if (!data)
    {
        return;
    }

    if (!help)
    {
        help = getHelpButton();
    }

    if (help)
    {
        d->about = data;
        d->about->setHelpButton(help);
    }
}

// -----------------------------------------------------------------------------------

class KPToolDialog::Private
{
public:
    Private()
        : buttonBox(nullptr),
          startButton(nullptr),
          mainWidget(nullptr),
         propagateReject(true)
    {
    }

    QDialogButtonBox* buttonBox;
    QPushButton*      startButton;
    QWidget*          mainWidget;

    bool              propagateReject;
};

KPToolDialog::KPToolDialog(QWidget* const parent)
    : QDialog(parent),
      KPDialogBase(this),
      d(new Private)
{
    d->buttonBox   = new QDialogButtonBox(QDialogButtonBox::Close | QDialogButtonBox::Help, this);
    d->startButton = new QPushButton(i18nc("@action:button", "&Start"), this);
    d->startButton->setIcon(QIcon::fromTheme(QString::fromLatin1("media-playback-start")));
    d->buttonBox->addButton(d->startButton, QDialogButtonBox::ActionRole);
    d->buttonBox->button(QDialogButtonBox::Close)->setDefault(true);

    QVBoxLayout* const mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(d->buttonBox);
    setLayout(mainLayout);

    connect(d->buttonBox, &QDialogButtonBox::rejected,
            this, &KPToolDialog::slotCloseClicked);
}

KPToolDialog::~KPToolDialog()
{
    delete d;
}

void KPToolDialog::setMainWidget(QWidget* const widget)
{
    if (d->mainWidget == widget)
    {
        return;
    }

    layout()->removeWidget(d->buttonBox);

    if (d->mainWidget)
    {
        // Replace existing widget
        layout()->removeWidget(d->mainWidget);
        delete d->mainWidget;
    }

    d->mainWidget = widget;
    layout()->addWidget(d->mainWidget);
    layout()->addWidget(d->buttonBox);
}

void KPToolDialog::setRejectButtonMode(QDialogButtonBox::StandardButton button)
{
    if (button == QDialogButtonBox::Close)
    {
        d->buttonBox->button(QDialogButtonBox::Close)->setText(i18n("Close"));
        d->buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon::fromTheme(QString::fromLatin1("window-close")));
        d->buttonBox->button(QDialogButtonBox::Close)->setToolTip(i18n("Close window"));
        d->propagateReject = true;
    }
    else if (button == QDialogButtonBox::Cancel)
    {
        d->buttonBox->button(QDialogButtonBox::Close)->setText(i18n("Cancel"));
        d->buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon::fromTheme(QString::fromLatin1("dialog-cancel")));
        d->buttonBox->button(QDialogButtonBox::Close)->setToolTip(i18n("Cancel current operation"));
        d->propagateReject = false;
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "Unexpected button mode passed";
    }
}

QPushButton* KPToolDialog::startButton() const
{
    return d->startButton;
}

QPushButton* KPToolDialog::helpButton() const
{
    return d->buttonBox->button(QDialogButtonBox::Help);
}

void KPToolDialog::addButton(QAbstractButton* button, QDialogButtonBox::ButtonRole role)
{
    d->buttonBox->addButton(button, role);
}

void KPToolDialog::slotCloseClicked()
{
    if (d->propagateReject)
    {
        reject();
    }
    else
    {
        emit cancelClicked();
    }
}

// -----------------------------------------------------------------------------------

KPWizardDialog::KPWizardDialog(QWidget* const parent)
    : QWizard(parent),
      KPDialogBase(this)
{
    setOption(QWizard::HaveHelpButton, true);
}

KPWizardDialog::~KPWizardDialog()
{
}

QPushButton* KPWizardDialog::helpButton() const
{
    return dynamic_cast<QPushButton*>(button(QWizard::HelpButton));
}

} // namespace KIPIPlugins
