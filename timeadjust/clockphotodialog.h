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

#ifndef CLOCKPHOTODIALOG_H
#define CLOCKPHOTODIALOG_H

// Qt includes

#include <QDateTime>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>

// KDE includes

#include <kdialog.h>
#include <kurl.h>

namespace KIPI
{
    class Interface;
}

namespace KIPIPlugins
{
    class ImageDialog;
}

namespace KIPITimeAdjustPlugin
{

/* Overloaded class for displaying the image in a QScrollArea, to provide
 * support for dragging it around with the middle button. */
class ImageDisplay : public QLabel
{
    Q_OBJECT

public:

    ImageDisplay(QScrollArea *);

protected:

    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

private:

    int         currX;
    int         currY;

    QScrollBar *barX;
    QScrollBar *barY;
};

// -----------------------------------------------------------------------------------

class ClockPhotoDialogPrivate;

/* Class for determining the camera time difference from a photo of a time
 * display. The user can load a photo and tell the time displayed on this
 * photo. This class calculates the time difference and stores it in a set
 * of public variables: deltaNegative indicates whether the time should be
 * added (false) or subtracted (true). deltaDays, deltaHours, deltaMinutes
 * and deltaSeconds hold the numbers of days, hours, minutes and seconds
 * to add or subtract. Months and years are not used, because these can be
 * ambigious. */
class ClockPhotoDialog : public KDialog
{
    Q_OBJECT

public:

    ClockPhotoDialog(KIPI::Interface* interface, QWidget* parent);
    ~ClockPhotoDialog();

    /* The public variables that hold the time difference. */
    bool deltaNegative;
    int  deltaDays;
    int  deltaHours;
    int  deltaMinutes;
    int  deltaSeconds;

    /* Try to load the photo specified by the KUrl, and set the datetime widget
     * to the photo time. Return true on succes, or false if eithe the photo
     * can't be read or the datetime information can't be read. */
    bool setImage(KUrl);

protected:

    void resizeEvent(QResizeEvent *);

private Q_SLOTS:

    void slotLoadPhoto();
    void slotAdjustZoom(int);
    void slotZoomOut();
    void slotZoomIn();
    void slotOk();
    void slotCancel();

private:

    /* Calculate the minimum value for the scroll slider according to the window
     * size. If fit is true, the image is made to fit in the viewport. */
    void adjustToWindowSize(bool);

    void saveSize();

private:

    QDateTime                photoDateTime;
    ClockPhotoDialogPrivate* const d;
};

}  // namespace KIPITimeAdjustPlugin

#endif /* CLOCKPHOTODIALOG_H */
