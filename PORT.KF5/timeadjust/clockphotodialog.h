/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-05-31
 * Description : Figure out camera clock delta from a clock picture.
 *
 * Copyright (C) 2009      by Pieter Edelman <p dot edelman at gmx dot net>
 * Copyright (C) 2011-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <kdialog.h>
#include <kurl.h>

namespace KIPITimeAdjustPlugin
{

class DeltaTime;

class ClockPhotoDialog : public KDialog
{
    Q_OBJECT

public:

    explicit ClockPhotoDialog(QWidget* const parent, const KUrl& defaultUrl);
    ~ClockPhotoDialog();

    /** Try to load the photo specified by the KUrl, and set the datetime widget
     *  to the photo time. Return true on succes, or false if either the photo
     *  can't be read or the datetime information can't be read.
     */
    bool setImage(const KUrl&);

    DeltaTime deltaValues() const;

private Q_SLOTS:

    void slotLoadPhoto();
    void slotOk();
    void slotCancel();

private:

    void loadSettings();
    void saveSettings();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPITimeAdjustPlugin

#endif /* CLOCKPHOTODIALOG_H */
