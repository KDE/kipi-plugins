/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-05-31
 * Description : Figure out camera clock delta from a clock picture.
 *
 * Copyright (C) 2009      by Pieter Edelman <p dot edelman at gmx dot net>
 * Copyright (C) 2011-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2014      by Michael G. Hansen <mike at mghansen dot de>
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

#include "clockphotodialog.h"

// Qt includes

#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSize>
#include <QIcon>
#include <QDialogButtonBox>
#include <QPushButton>

// KDE includes

#include <kconfig.h>
#include <klocalizedstring.h>
#include <kwindowconfig.h>

// Local includes

#include "kpimagedialog.h"
#include "kpmetadata.h"
#include "kppreviewmanager.h"
#include "timeadjustsettings.h"

using namespace KIPIPlugins;

namespace KIPITimeAdjustPlugin
{

class ClockPhotoDialog::Private
{

public:

    Private()
    {
        buttons      = 0;
        calendar     = 0;
        imagePreview = 0;
    }

    DeltaTime         deltaValues;

    QDialogButtonBox* buttons;
    QDateTimeEdit*    calendar;
    QDateTime         photoDateTime;

    KPPreviewManager* imagePreview;
};

ClockPhotoDialog::ClockPhotoDialog(QWidget* const parent, const QUrl& defaultUrl)
    : QDialog(parent),
      d(new Private)
{
    // This dialog should be modal with three buttons: Ok, Cancel, and load
    // photo. For this third button, the User1 button from KDialog is used.
    // The Ok button is only enable when a photo is loaded.

    setWindowTitle(i18n("Determine time difference with clock photo"));

    d->buttons = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Cancel)->setDefault(true);
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
    setMinimumWidth(500);
    setMinimumHeight(500);
    d->buttons->button(QDialogButtonBox::Apply)->setText(i18n("Load different photo"));
    d->buttons->button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme("document-open"));

    QWidget* const mainWidget = new QWidget(this);
    QVBoxLayout* const vBox   = new QVBoxLayout(mainWidget);

    // Some explanation.
    QLabel* const explanationLabel = new QLabel(i18n("If you have a photo in your set with a clock or "
                                                     "another external time source on it, you can load "
                                                     "it here and set the indicator to the (date and) "
                                                     "time displayed. The difference of your internal "
                                                     "camera clock will be determined from this "
                                                     "setting."));
    explanationLabel->setWordWrap(true);
    vBox->addWidget(explanationLabel);

    // The image is displayed by loading a KPPreviewManager
    d->imagePreview = new KPPreviewManager(this);
    d->imagePreview->setBackgroundRole(QPalette::Window);
    d->imagePreview->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    vBox->addWidget(d->imagePreview);

    // The date and time entry widget allows the user to enter the date and time
    // displayed in the image. The format is explicitly set, otherwise seconds
    // might not get displayed.
    QLabel* const dtLabel    = new QLabel(i18n("The clock date and time:"));
    d->calendar              = new QDateTimeEdit();
    d->calendar->setDisplayFormat("d MMMM yyyy, hh:mm:ss");
    d->calendar->setCalendarPopup(true);
    d->calendar->setEnabled(false);
    QHBoxLayout* const hBox2 = new QHBoxLayout(mainWidget);
    hBox2->addStretch();
    hBox2->addWidget(dtLabel);
    hBox2->addWidget(d->calendar);
    vBox->addLayout(hBox2);

    vBox->addWidget(d->buttons);
    setLayout(vBox);

    // Setup the signals and slots.
    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotOk()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(slotCancel()));

    connect(d->buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
            this, SLOT(slotLoadPhoto()));

    // Show the window.
    loadSettings();
    show();

    if (defaultUrl.isValid())
    {
        setImage(defaultUrl);
    }
    else
    {
        // No default url was given.
        // Upon initialization, present the user with a photo loading dialog. This
        // is done before the main dialog is drawn.
        slotLoadPhoto();
    }
}

ClockPhotoDialog::~ClockPhotoDialog()
{
    delete d;
}

DeltaTime ClockPhotoDialog::deltaValues() const
{
    return d->deltaValues;
}

bool ClockPhotoDialog::setImage(const QUrl& imageFile)
{
    bool success = false;

    if (d->imagePreview->load(imageFile))
    {
        // Try to read the datetime data.
        KPMetadata meta;

        if (meta.load(imageFile.toLocalFile()))
        {
            d->photoDateTime = meta.getImageDateTime();

            if (d->photoDateTime.isValid())
            {
                // Set the datetime widget to the photo datetime.
                d->calendar->setDateTime(d->photoDateTime);
                d->calendar->setEnabled(true);
                success = true;
            }
            else
            {
                // If datetime information couldn't be loaded, display a
                // warning and disable the datetime widget.
                QString warning = i18n("<font color=\"red\"><b>Could not "
                                       "obtain<br>date and time information<br>"
                                       "from image %1.</b></font>",
                                       imageFile.fileName());
                d->imagePreview->setText(warning);
            }
        }
    }
    else
    {
        // If the image couldn't be loaded, display a warning, disable all the
        // GUI elements and load an empty photo into d->image.
        QString warning = i18n("<font color=\"red\"><b>Could not load<br>"
                               "image %1.</b></font>",
                               imageFile.fileName());
        d->imagePreview->setText(warning);
    }

    // Disable all the GUI elements if loading failed.
    d->calendar->setEnabled(success);

    // enable the ok button if loading succeeded
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(success);

    return success;
}

void ClockPhotoDialog::loadSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Clock Photo Dialog"));
    KWindowConfig::restoreWindowSize(windowHandle(), group);
}

void ClockPhotoDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Clock Photo Dialog"));
    KWindowConfig::saveWindowSize(windowHandle(), group);
}

void ClockPhotoDialog::slotLoadPhoto()
{
    // Present the user with a dialog to load the photo.
    KPImageDialog dlg(this, true, false);

    if (!dlg.url().isEmpty())
    {
        // If the user selected a proper photo, try to load it.
        setImage(dlg.url());
    }
}

void ClockPhotoDialog::slotOk()
{
    // Called when the ok button is pressed. Calculate the time difference
    // between the photo and the user set datetime information, and store it in
    // the public variables.

    // Determine the number of seconds between the dates.
    int delta = d->photoDateTime.secsTo(d->calendar->dateTime());

    // If the photo datetime is newer than the user datetime, it results in
    // subtraction.
    if (delta < 0)
    {
        d->deltaValues.deltaNegative = true;
        delta                       *= -1;
    }
    else
    {
        d->deltaValues.deltaNegative = false;
    }

    // Calculate the number of days, hours, minutes and seconds.
    d->deltaValues.deltaDays    = delta / 86400;
    delta                       = delta % 86400;
    d->deltaValues.deltaHours   = delta / 3600;
    delta                       = delta % 3600;
    d->deltaValues.deltaMinutes = delta / 60;
    delta                       = delta % 60;
    d->deltaValues.deltaSeconds = delta;

    // Accept the dialog.
    saveSettings();
    accept();
}

/** If the cancel button is clicked, reject the dialog.
 */
void ClockPhotoDialog::slotCancel()
{
    saveSettings();
    reject();
}

}  // namespace KIPITimeAdjustPlugin
