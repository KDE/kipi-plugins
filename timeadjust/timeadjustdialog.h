/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
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

#ifndef TIMEADJUSTDIALOG_H
#define TIMEADJUSTDIALOG_H

// Qt includes

#include <QDateTime>

// KDE includes

#include <kurl.h>

// Local includes

#include "kptooldialog.h"
#include "timeadjustsettings.h"

using namespace KIPIPlugins;

namespace KIPITimeAdjustPlugin
{

class TimeAdjustDialog : public KPToolDialog
{
    Q_OBJECT

public:

    explicit TimeAdjustDialog(QWidget* const parent = 0);
    ~TimeAdjustDialog();

    void addItems(const KUrl::List& images);
    void disableApplTimestamp();

Q_SIGNALS:

    void signalMyCloseClicked();

protected:

    void closeEvent(QCloseEvent*);

private Q_SLOTS:

    void slotApplyClicked();
    void slotCloseClicked();
    void slotThreadFinished();
    void slotCancelThread();
    void slotButtonClicked(int);
    void slotProcessStarted(const KUrl&);
    void slotProcessEnded(const KUrl&, int);
    void setBusy(bool);

    /** Read the Used Timestamps for all selected files
     *  (according to the newly selected source timestamp type),
     *  this will also implicitly update listview info.
     */
    void slotReadTimestamps();

private:

    /** Called by readTimestamps() to get KIPI host timestamps
     */
    void readApplicationTimestamps();

    /** Called by readTimestamps() to get file timestamps
     */
    void readFileTimestamps();

    /** Called by readTimestamps() to get file metadata timestamps
     */
    void readMetadataTimestamps();

    void readSettings();
    void saveSettings();

    void updateListView();

private:

    class Private;
    Private* const d;
};

}  // namespace KIPITimeAdjustPlugin

#endif /* TIMEADJUSTDIALOG_H */
