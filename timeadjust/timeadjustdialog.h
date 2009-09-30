/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <kdialog.h>
#include <kurl.h>

namespace KIPI
{
    class Interface;
}

namespace KIPITimeAdjustPlugin
{

class TimeAdjustDialogPrivate;

class TimeAdjustDialog : public KDialog
{
    Q_OBJECT

public:

    TimeAdjustDialog(KIPI::Interface* interface, QWidget* parent);
    ~TimeAdjustDialog();

    void setImages(const KUrl::List& images);

protected:

    void closeEvent(QCloseEvent *);

private Q_SLOTS:

    void slotSrcTimestampChanged();
    void slotResetDateToCurrent();
    void slotAdjustmentTypeChanged();
    void slotDetAdjustmentByClockPhoto();
    void slotUpdateExample();
    void slotHelp();
    void slotOk();
    void slotCancel();

private:

    void readExampleTimestamps();
    void readApplicationTimestamps();
    void readFileTimestamps();
    void readMetadataTimestamps();
    void readSettings();
    void saveSettings();
    QDateTime calculateAdjustedTime(const QDateTime& time) const;

private:

    TimeAdjustDialogPrivate* const d;
};

}  // namespace KIPITimeAdjustPlugin

#endif /* TIMEADJUSTDIALOG_H */

