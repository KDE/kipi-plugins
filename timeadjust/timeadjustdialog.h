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
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

using namespace KIPIPlugins;

namespace KIPITimeAdjustPlugin
{

class TimeAdjustDialog : public KPToolDialog
{
    Q_OBJECT

public:

    TimeAdjustDialog(QWidget* const parent);
    ~TimeAdjustDialog();

    void setImages(const KUrl::List& images);

Q_SIGNALS:

    void myCloseClicked();

protected:

    void closeEvent(QCloseEvent*);

private Q_SLOTS:

    void slotSrcTimestampChanged();
    void slotResetDateToCurrent();
    void slotAdjustmentTypeChanged();
    void slotDetAdjustmentByClockPhoto();
    void slotUpdateExample();
    void slotApplyClicked();
    void slotCloseClicked();
    void slotProgressChanged(int);
    void slotThreadFinished();
    void slotErrorFilesUpdate(const QString&, const QString&);
    void slotCancelThread();
    void slotButtonClicked(int);

    void setBusy(bool);

private:

    void readExampleTimestamps();
    void readApplicationTimestamps();
    void readFileTimestamps();
    void readMetadataTimestamps();
    void readSettings();
    void saveSettings();
    QDateTime calculateAdjustedTime(const QDateTime& time) const;

private:

    class TimeAdjustDialogPrivate;
    TimeAdjustDialogPrivate* const d;
};

}  // namespace KIPITimeAdjustPlugin

#endif /* TIMEADJUSTDIALOG_H */
