/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2006-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

// Qt includes

#include <QDateTime>
#include <QScrollArea>

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "timeadjustsettings.h"

class KConfigGroup;

namespace KIPITimeAdjustPlugin
{

class MyImageList;

class SettingsWidget : public QScrollArea
{
    Q_OBJECT

public:

    explicit SettingsWidget(QWidget* const parent = 0);
    ~SettingsWidget();

    void readSettings(KConfigGroup& group);
    void saveSettings(KConfigGroup& group);

    TimeAdjustSettings settings()                          const;
    QDateTime customDate()                                 const;
    QDateTime calculateAdjustedDate(const QDateTime& time) const;

    void disableApplTimestamp();
    void setImageList(MyImageList* const myImageList);

Q_SIGNALS:

    void signalSettingsChanged();

private Q_SLOTS:

    void slotSrcTimestampChanged();
    void slotResetDateToCurrent();
    void slotAdjustmentTypeChanged();
    void slotDetAdjustmentByClockPhoto();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPITimeAdjustPlugin

#endif /* SETTINGSWIDGET_H */
