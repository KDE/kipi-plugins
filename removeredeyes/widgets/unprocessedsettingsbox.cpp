/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-28
 * Description : a widget that holds unprocessed image settings
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

#include "unprocessedsettingsbox.moc"

// Qt includes

#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>

// KDE includes

#include <klocale.h>

// Local includes

#include "removeredeyeswindow.h"

namespace KIPIRemoveRedEyesPlugin
{

struct UnprocessedSettingsBox::Private
{
    Private()
    {
        unprocessedGroup = 0;
    }

    QButtonGroup* unprocessedGroup;
};

UnprocessedSettingsBox::UnprocessedSettingsBox(QWidget* const parent)
    : QGroupBox(parent), d(new Private)
{
    setTitle(i18n("Unprocessed Image Handling"));

    QString whatsThis = i18n("<p>These settings control the handling of unprocessed images when "
                             "running in test-mode. To speed up the final correction, unprocessed images can be "
                             "removed from the list, so that they are not analyzed again. There are three modes "
                             "to choose from:</p>"
                             "<p><ul>"
                             "<li><b>Ask the user:</b> A popup window will appear to ask for further actions.</li>"
                             "<li><b>Keep in file list:</b> All unprocessed images will be kept in the list.</li>"
                             "<li><b>Remove from file list:</b> All unprocessed images are removed after the test run.</li>"
                             "</ul></p>");
    setWhatsThis(whatsThis);

    // ----------------------------------------------------------------

    d->unprocessedGroup = new QButtonGroup(this);
    d->unprocessedGroup->setExclusive(true);

    // ----------------------------------------------------------------

    QRadioButton* const askMode = new QRadioButton(i18n("&Ask the user"));
    askMode->setToolTip(i18n("If checked, the user will be asked to keep or "
                             "remove unprocessed images."));
    d->unprocessedGroup->addButton(askMode, Ask);

    QRadioButton* const keepMode = new QRadioButton(i18n("&Keep in file list"));
    keepMode->setToolTip(i18n("If checked, unprocessed images will be kept in the "
                              "file list after the test-run."));
    d->unprocessedGroup->addButton(keepMode, Keep);

    QRadioButton* const removeMode = new QRadioButton(i18n("&Remove from file list"));
    removeMode->setToolTip(i18n("If checked, unprocessed images will be removed from "
                                "the file list after the test-run."));
    d->unprocessedGroup->addButton(removeMode, Remove);

    // ----------------------------------------------------------------

    QGridLayout* unprocessedGroupLayout = new QGridLayout;
    unprocessedGroupLayout->addWidget(askMode,      0, 0, 1, 1);
    unprocessedGroupLayout->addWidget(keepMode,     1, 0, 1, 1);
    unprocessedGroupLayout->addWidget(removeMode,   2, 0, 1, 1);
    setLayout(unprocessedGroupLayout);

    // ----------------------------------------------------------------

    connect(d->unprocessedGroup, SIGNAL(buttonClicked(int)),
            this, SIGNAL(settingsChanged()));
}

UnprocessedSettingsBox::~UnprocessedSettingsBox()
{
    delete d;
}

int UnprocessedSettingsBox::handleMode() const
{
    return d->unprocessedGroup->checkedId();
}

void UnprocessedSettingsBox::setHandleMode(int mode)
{
    d->unprocessedGroup->button(mode)->setChecked(true);
}

} // namespace KIPIRemoveRedEyesPlugin
