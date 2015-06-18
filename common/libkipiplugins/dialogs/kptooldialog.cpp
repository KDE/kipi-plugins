/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-04
 * Description : Tool dialog
 *
 * Copyright (C) 2012-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <khelpmenu.h>
#include <klocalizedstring.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

// Libkipi includes

#include <interface.h>
#include <pluginloader.h>

// Local includes

#include "kpaboutdata.h"
#include "kpoutputdialog.h"
#include "kipiplugins_debug.h"

namespace KIPIPlugins
{

class KPDialogBase::Private
{

public:

    Private()
    {
        about  = 0;
        dialog = 0;
        iface  = 0;

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

QPushButton* KPDialogBase::getHelpButton()
{
    if (!d->dialog)
    {
        return nullptr;
    }

    {
        KDialog* const dlg = dynamic_cast<KDialog*>(d->dialog);

        if (dlg)
        {
            return dlg->button(KDialog::Help);
        }
    }

    {
        KPageDialog* const dlg = dynamic_cast<KPageDialog*>(d->dialog);

        if (dlg)
        {
            return dlg->button(QDialogButtonBox::Help);
        }
    }

    {
        KPOutputDialog* const dlg = dynamic_cast<KPOutputDialog*>(d->dialog);

        if (dlg)
        {
            return dlg->helpButton();
        }
    }

    {
        KPToolDialog* const dlg = dynamic_cast<KPToolDialog*>(d->dialog);

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

KP4ToolDialog::KP4ToolDialog(QWidget* const parent)
    : KDialog(parent), KPDialogBase(this)
{
    setButtons(KDialog::Help | KDialog::Ok);
}

KP4ToolDialog::~KP4ToolDialog()
{
}

// -----------------------------------------------------------------------------------

class KPToolDialog::Private
{
public:
    Private()
        : buttonBox(nullptr)
        , startButton(nullptr)
        , mainWidget(nullptr)
        , propagateReject(true)
    {
    }

    QDialogButtonBox* buttonBox;
    QPushButton* startButton;
    QWidget* mainWidget;

    bool propagateReject;
};

KPToolDialog::KPToolDialog(QWidget* const parent)
    : QDialog(parent), KPDialogBase(this), d(new Private)
{
    d->buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Close | QDialogButtonBox::Help, this);
    d->startButton = new QPushButton(i18nc("@action:button", "&Start"), this);
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
}

void KPToolDialog::setMainWidget(QWidget* widget)
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
        KGuiItem::assign(d->buttonBox->button(QDialogButtonBox::Close),
                         KGuiItem(i18n("Close"), "dialog-close",
                                  i18n("Close window")));
        d->propagateReject = true;
    }
    else if (button == QDialogButtonBox::Cancel)
    {
        KGuiItem::assign(d->buttonBox->button(QDialogButtonBox::Close),
                         KGuiItem(i18n("Cancel"), "dialog-cancel",
                                  i18n("Cancel current operation")));
        d->propagateReject = false;
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "Unexpected button mode passed";
    }
}

QPushButton* KPToolDialog::startButton()
{
    return d->startButton;
}

QPushButton* KPToolDialog::helpButton()
{
    return d->buttonBox->button(QDialogButtonBox::Help);
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
    : KAssistantDialog(parent), KPDialogBase(this)
{
}

KPWizardDialog::~KPWizardDialog()
{
}

// -----------------------------------------------------------------------------------

KPPageDialog::KPPageDialog(QWidget* const parent)
    : KPageDialog(parent), KPDialogBase(this)
{
    setStandardButtons(QDialogButtonBox::Help | QDialogButtonBox::Ok);
}

KPPageDialog::~KPPageDialog()
{
}

} // namespace KIPIPlugins
