/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-05-31
 * Description : Figure out camera clock delta from a clock picture.
 *
 * Copyright (C) 2009      by Pieter Edelman <p dot edelman at gmx dot net>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "clockphotodialog.moc"

// Qt includes

#include <QDateTimeEdit>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QSize>
#include <QSlider>
#include <QVBoxLayout>

// KDE includes

#include <kconfig.h>
#include <kdebug.h>
#include <kicon.h>
#include <klocale.h>
#include <kpushbutton.h>

// LibKIPI includes

#include <libkipi/interface.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// Local includes

#include "kpimagedialog.h"
#include "kpmetadata.h"
#include "kppreviewmanager.h"

using namespace KDcrawIface;

namespace KIPITimeAdjustPlugin
{

class ClockPhotoDialog::ClockPhotoDialogPrivate
{

public:

    ClockPhotoDialogPrivate()
    {
        image         = 0;
        imagePreview  = 0;
        calendar      = 0;
        photoDateTime = 0;
    }

    QPixmap*          image;

    QDateTimeEdit*    calendar;

    QDateTime*        photoDateTime;

    KPPreviewManager* imagePreview;

    Interface*        interface;
};

ClockPhotoDialog::ClockPhotoDialog(Interface* const interface, QWidget* const parent)
    : KDialog(parent), d(new ClockPhotoDialogPrivate)
{
    d->interface     = interface;

    // Initialize the variables.
    d->image         = new QPixmap();
    d->photoDateTime = new QDateTime();
    deltaNegative    = false;
    deltaDays        = 0;
    deltaHours       = 0;
    deltaMinutes     = 0;
    deltaSeconds     = 0;

    // This dialog should be modal with three buttons: Ok, Cancel, and load
    // photo. For this third button, the User1 button from KDialog is used.
    // The Ok button is only enable when a photo is loaded.
    setCaption(i18n("Determine time difference with clock photo"));
    setButtons(User1 | Ok | Cancel);
    setMinimumWidth(500);
    setMinimumHeight(500);
    button(User1)->setText(i18n("Load different photo"));
    button(User1)->setIcon(KIcon("document-open"));
    button(Ok)->setEnabled(false);

    // Everything else is stacked in a vertical box.
    setMainWidget(new QWidget(this));
    QVBoxLayout* vBox = new QVBoxLayout(mainWidget());

    // Some explanation.
    QLabel* explanationLabel =
            new QLabel(i18n("If you have a photo in your set with a clock or "
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
    QLabel* dtLabel = new QLabel(i18n("The clock date and time:"));
    d->calendar     = new QDateTimeEdit();
    d->calendar->setDisplayFormat("d MMMM yyyy, hh:mm:ss");
    d->calendar->setCalendarPopup(true);
    d->calendar->setEnabled(false);
    QHBoxLayout *hBox2 = new QHBoxLayout(mainWidget());
    hBox2->addStretch();
    hBox2->addWidget(dtLabel);
    hBox2->addWidget(d->calendar);
    vBox->addLayout(hBox2);

    // Setup the signals and slots.
    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotLoadPhoto()));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    // Show the window.
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Clock Photo Dialog"));
    restoreDialogSize(group);
    show();

    // Upon initialization, present the user with a photo loading dialog. This
    // is done before the main dialog is drawn.
    slotLoadPhoto();
}

ClockPhotoDialog::~ClockPhotoDialog()
{
    delete d->image;
    delete d->photoDateTime;
    delete d;
}

bool ClockPhotoDialog::setImage(const KUrl& imageFile)
{
    bool success = false;

    // Raw housekeeping.
    QString rawFilesExt(KDcraw::rawFiles());
    QFileInfo info(imageFile.path());
    // Try to load the image into the d->image variable.
    bool imageLoaded;
    if (rawFilesExt.toUpper().contains(info.suffix().toUpper()))
    {
        // In case of raw images, load the image the a QImage and convert it to
        // the QPixmap for display.
        QImage tmp  = QImage();
        imageLoaded = KDcraw::loadDcrawPreview(tmp, imageFile.path());
        delete d->image;
        d->image    = new QPixmap(d->image->fromImage(tmp));
        d->imagePreview->setImage(tmp, true);
    }
    else
    {
        imageLoaded = d->image->load(imageFile.path());
    }

    if (imageLoaded)
    {
        // Try to read the datetime data.
        KPMetadata meta;
        bool result = meta.load(imageFile.path());
        if (result)
        {
            delete d->photoDateTime;
            d->photoDateTime = new QDateTime(meta.getImageDateTime());
            if (d->photoDateTime->isValid())
            {
                // Set the datetime widget to the photo datetime.
                d->calendar->setDateTime(*(d->photoDateTime));
                d->calendar->setEnabled(true);
                success = true;
            }
            else
            {
                // If datetime information couldn't be loaded, display a
                // warning and disable the datetime widget.
                QString warning = i18n("<font color=\"red\"><b>Could not "
                                       "obtain date and time information "
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
        QString warning = i18n("<font color=\"red\"><b>Could not load "
                               "image %1.</b></font>",
                               imageFile.fileName());
        d->imagePreview->setText(warning);
    }

    if (success)
    {
        // Load the photo.
        d->imagePreview->load(imageFile.path(), true);
    }
    else
    {
        // Disable all the GUI elements.
        d->calendar->setEnabled(false);

        // Make sure we de-load a previous image if a faulty url was provided.
        delete d->image;
        d->image = new QPixmap();
    }

    return success;
}

void ClockPhotoDialog::saveSize()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Clock Photo Dialog"));
    saveDialogSize(group);
}

void ClockPhotoDialog::slotLoadPhoto()
{
    // Present the user with a dialog to load the photo.
    KPImageDialog dlg(this, d->interface, true, false);

    if (!dlg.url().isEmpty())
    {
        // If the user selected a proper photo, try to load it. If this
        // succeeds, enable the Ok button, otherwise disable it.
        if (setImage(dlg.url()))
        {
            button(Ok)->setEnabled(true);
        }
        else
        {
            button(Ok)->setEnabled(false);
        }
    }
}

void ClockPhotoDialog::slotOk()
{
    // Called when the ok button is pressed. Calculate the time difference
    // between the photo and the user set datetime information, and store it in
    // the public variables.

    // Determine the number of seconds between the dates.
    int delta = d->photoDateTime->secsTo(d->calendar->dateTime());

    // If the photo datetime is newer than the user datetime, it results in
    // subtraction.
    if (delta < 0)
    {
        deltaNegative = true;
        delta *= -1;
    }
    else
    {
        deltaNegative = false;
    }

    // Calculate the number of days, hours, minutes and seconds.
    deltaDays    = delta / 86400;
    delta        = delta % 86400;
    deltaHours   = delta / 3600;
    delta        = delta % 3600;
    deltaMinutes = delta / 60;
    delta        = delta % 60;
    deltaSeconds = delta;

    // Accept the dialog.
    saveSize();
    accept();
}

void ClockPhotoDialog::slotCancel()
{
    /* If the cancel button is clicked, reject the dialog. */
    saveSize();
    reject();
}

}  // namespace KIPITimeAdjustPlugin
