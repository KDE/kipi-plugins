/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-05-31
 * Description : Figure out camera clock delta from a clock picture.
 *
 * Copyright (C) 2009 by Pieter Edelman (p dot edelman at gmx dot net)
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
#include <libkipi/imageinfo.h>

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

#if KDCRAW_VERSION < 0x000400
#include <libkdcraw/dcrawbinary.h>
#endif

// Local includes

#include "imagedialog.h"

namespace KIPITimeAdjustPlugin
{

ImageDisplay::ImageDisplay(QScrollArea *parentArea) : QLabel()
{
    currX  = 0;
    currY  = 0;
    barX   = parentArea->horizontalScrollBar();
    barY   = parentArea->verticalScrollBar();
}

void ImageDisplay::mousePressEvent(QMouseEvent *event)
{
    // If the middle mouse button is pressed on the image, grab the cursor and
    // change its icon, and save the current cursor position on the screen.
    if (event->buttons() == Qt::MidButton)
    {
        currX = event->globalX();
        currY = event->globalY();
        grabMouse(QCursor(Qt::SizeAllCursor));
    }
    QLabel::mousePressEvent(event);
}

void ImageDisplay::mouseMoveEvent(QMouseEvent *event)
{
    // If the mouse is moved while the middle button is still pressed, calculate
    // the position difference compared the last update, move the QScrollArea's
    // sliders according to this difference, and save the new position.

    if (event->buttons() == Qt::MidButton)
    {
        barX->setValue(barX->value() + currX - event->globalX());
        barY->setValue(barY->value() + currY - event->globalY());
        currX = event->globalX();
        currY = event->globalY();
    }
    QLabel::mouseMoveEvent(event);
}

void ImageDisplay::mouseReleaseEvent(QMouseEvent *event)
{
    // When the middle (or rather, any) mouse button is released, we release the
    // cursor again.

    releaseMouse();
    QLabel::mouseReleaseEvent(event);
}

class ClockPhotoDialogPrivate
{

public:

    ClockPhotoDialogPrivate()
    {
        scrollArea    = 0;
        image         = 0;
        imageLabel    = 0;
        zoomSlider    = 0;
        zoomOutButton = 0;
        zoomInButton  = 0;
        calendar      = 0;
        photoDateTime = 0;
    }


    QScrollArea     *scrollArea;
    QPixmap         *image;

    QSlider         *zoomSlider;
    QPushButton     *zoomOutButton;
    QPushButton     *zoomInButton;

    QDateTimeEdit   *calendar;

    QDateTime       *photoDateTime;

    ImageDisplay    *imageLabel;

    KIPI::Interface *interface;
};

ClockPhotoDialog::ClockPhotoDialog(KIPI::Interface* interface, QWidget* parent)
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
    button(User1)->setText(i18n("Load different photo"));
    button(User1)->setIcon(KIcon("document-open"));
    button(Ok)->setEnabled(false);

    // Everything else is stacked in a vertical box.
    setMainWidget(new QWidget(this));
    QVBoxLayout *vBox = new QVBoxLayout(mainWidget());

    // Some explanation.
    QLabel *explanationLabel =
            new QLabel(i18n("If you have a photo in your set with a clock or "
                            "another external time source on it, you can load "
                            "it here and set the indicator to the (date and) "
                            "time displayed. The difference of your internal "
                            "camera clock will be determined from this "
                            "setting."));
    explanationLabel->setWordWrap(true);
    vBox->addWidget(explanationLabel);

    // The image is displayed by loading an ImageDisplay in the scroll area,
    // which is an overloaded QLabel. See the Image Viewer Example from the Qt
    // documentation (http://doc.trolltech.com/4.5/widgets-imageviewer.html).
    d->scrollArea = new QScrollArea(this);
    d->scrollArea->setBackgroundRole(QPalette::Window);
    d->scrollArea->setAlignment(Qt::AlignCenter);
    vBox->addWidget(d->scrollArea);

    d->imageLabel = new ImageDisplay(d->scrollArea);
    d->imageLabel->setBackgroundRole(QPalette::Base);
    d->imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    d->imageLabel->setScaledContents(true);

    // The label is also used to display text (warning and waiting messages).
    d->imageLabel->setWordWrap(true);
    d->imageLabel->setAlignment(Qt::AlignCenter);
    d->scrollArea->setWidget(d->imageLabel);

    // For zooming support, a scale is displayed beneath the image with zoom out
    // and zoom in buttons.
    QLabel *scale_label = new QLabel(i18n("Scale:"));
    d->zoomOutButton    = new QPushButton(KIcon("zoom-out"), "");
    d->zoomInButton     = new QPushButton(KIcon("zoom-in"), "");
    d->zoomInButton->setFlat(true);
    d->zoomOutButton->setFlat(true);
    d->zoomSlider = new QSlider(Qt::Horizontal);
    d->zoomSlider->setRange(10, 150);
    QHBoxLayout *hBox1 = new QHBoxLayout(mainWidget());
    hBox1->addWidget(scale_label);
    hBox1->addWidget(d->zoomOutButton);
    hBox1->addWidget(d->zoomSlider);
    hBox1->addWidget(d->zoomInButton);
    vBox->addLayout(hBox1);
    d->zoomSlider->setEnabled(false);
    d->zoomOutButton->setEnabled(false);
    d->zoomInButton->setEnabled(false);

    // The date and time entry widget allows the user to enter the date and time
    // displayed in the image. The format is explicitly set, otherwise seconds
    // might not get displayed.
    QLabel *dtLabel = new QLabel(i18n("The clock date and time:"));
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

    connect(d->zoomSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(slotAdjustZoom(int)));

    connect(d->zoomOutButton, SIGNAL(clicked()),
            this, SLOT(slotZoomOut()));

    connect(d->zoomInButton, SIGNAL(clicked()),
            this, SLOT(slotZoomIn()));

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
    delete d;
}

bool ClockPhotoDialog::setImage(KUrl imageFile)
{
    bool success = false;

    // Raw housekeeping.
#if KDCRAW_VERSION < 0x000400
    QString rawFilesExt(KDcrawIface::DcrawBinary::instance()->rawFiles());
#else
    QString rawFilesExt(KDcrawIface::KDcraw::rawFiles());
#endif
    QFileInfo info(imageFile.path());

    // Try to load the image into the d->image variable.
    bool imageLoaded;
    if (rawFilesExt.toUpper().contains(info.suffix().toUpper()))
    {
        // In case of raw images, load the image the a QImage and convert it to
        // the QPixmap for display.
        QImage tmp  = QImage();
        imageLoaded = KDcrawIface::KDcraw::loadDcrawPreview(tmp, imageFile.path());
        d->image    = new QPixmap(d->image->fromImage(tmp));
    }
    else
    {
        imageLoaded = d->image->load(imageFile.path());
    }

    if (imageLoaded)
    {
        // Try to read the datetime data.
        KExiv2Iface::KExiv2 exiv2Iface;
        bool result = exiv2Iface.load(imageFile.path());
        if (result)
        {
            d->photoDateTime = new QDateTime(exiv2Iface.getImageDateTime());
            if (d->photoDateTime->isValid()) {
                // Set the datetime widget to the photo datetime.
                d->calendar->setDateTime(*(d->photoDateTime));
                d->calendar->setEnabled(true);
                success = true;
            }
            else
            {
                // If datetime information couldn't be loaded, display a
                // warning and disable the datetime widget.
                QString warning = QString(i18n("<font color=\"red\"><b>Could not "
                                          "obtain date and time information "
                                          "from image %1.</b></font>")).
                                          arg(imageFile.fileName());
                d->imageLabel->setText(warning);
            }
        }
    }
    else
    {
        // If the image couldn't be loaded, display a warning, disable all the
        // GUI elements and load an empty photo into d->image.
        QString warning = QString(i18n("<font color=\"red\"><b>Could not load "
                                       "image %1.</b></font>")).
                                       arg(imageFile.fileName());
        d->imageLabel->setText(warning);
    }

    if (success)
    {
        // Load the photo and enable all image display GUI elements.
        d->imageLabel->setPixmap(*(d->image));

        // The following line is a bit of a hack: if the QLabel is shown for
        // the first time, it is broken (parts of it are missing). After a scale
        // action, this problem is solved. So we first scale it here.
        d->imageLabel->resize(d->image->width(), d->image->height());
        d->zoomSlider->setEnabled(true);
        d->zoomOutButton->setEnabled(true);
        d->zoomInButton->setEnabled(true);

        // Fit the image to the viewport.
        adjustToWindowSize(true);
    }
    else
    {
        // Disable all the GUI elements.
        d->zoomSlider->setEnabled(false);
        d->zoomOutButton->setEnabled(false);
        d->zoomInButton->setEnabled(false);
        d->calendar->setEnabled(false);

        // Make sure we de-load a previous image if a faulty url was provided.
        d->image = new QPixmap();

        // Scale the imageLabel so that the warning shows up.
        d->imageLabel->resize(d->scrollArea->viewport()->size());
    }

    return success;
}

void ClockPhotoDialog::adjustToWindowSize(bool fit)
{
    int scroll_width   = d->scrollArea->viewport()->width();
    int scroll_height  = d->scrollArea->viewport()->height();
    float scale_width  = (float)scroll_width  / d->image->width();
    float scale_height = (float)scroll_height / d->image->height();
    float scale        = scale_width;

    if (scale_height < scale_width) scale = scale_height;

    d->zoomSlider->setMinimum((int)(scale * 100.0));

    if (fit)
    {
        d->imageLabel->resize((int)(d->image->width() * scale), (int)(d->image->height() * scale));
        d->zoomSlider->setValue((int)(scale * 100.0));
    }
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
    KIPIPlugins::ImageDialog dlg(this, d->interface, true, false);

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

void ClockPhotoDialog::slotAdjustZoom(int percentage)
{
    // Callback for when the zoom slider is adjusted. Scale the image to the new
    // value and adjust the scrollbars adjusted so that the center of the
    // display remains the same.

    // Remember what the old width was.
    float oldWidth = d->scrollArea->widget()->width();

    // Convert the percentage to an absolute scale and scale the image.
    float absScale = (float)percentage / 100;
    d->imageLabel->resize(d->image->size() * absScale);

    // Calculate the size increase.
    float relScale = d->scrollArea->widget()->width() / oldWidth;

    // Adjust the scrollbars to the size increase.
    QScrollBar *barX = d->scrollArea->horizontalScrollBar();
    QScrollBar *barY = d->scrollArea->verticalScrollBar();
    barX->setValue(int(relScale * barX->value() + ((relScale - 1) * barX->pageStep()/2)));
    barY->setValue(int(relScale * barY->value() + ((relScale - 1) * barY->pageStep()/2)));
}

void ClockPhotoDialog::slotZoomOut()
{
    d->zoomSlider->triggerAction(QAbstractSlider::SliderPageStepSub);
    slotAdjustZoom(d->zoomSlider->value());
}

void ClockPhotoDialog::slotZoomIn()
{
    d->zoomSlider->triggerAction(QAbstractSlider::SliderPageStepAdd);
    slotAdjustZoom(d->zoomSlider->value());
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

void ClockPhotoDialog::resizeEvent(QResizeEvent *event)
{
    // If the image is completely zoomed out, make it so in the new window size
    // as well, otherwise, leave the zoom size intact.
    if (d->zoomSlider->value() == d->zoomSlider->minimum())
    {
        adjustToWindowSize(true);
    }
    else
    {
        adjustToWindowSize(false);
    }
    QWidget::resizeEvent(event);
}

}  // namespace KIPITimeAdjustPlugin
