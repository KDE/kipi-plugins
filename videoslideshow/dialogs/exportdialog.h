/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-06-24
 * Description : Video SlideShow Export Dialog
 *
 * Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

// Qt includes

#include <QString>
#include <QPixmap>

// KDE includes

#include <kdialog.h>
#include <kurl.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>

// Local includes

#include "kptooldialog.h"
#include "myimagelist.h"
#include "actions.h"

class QCloseEvent;

using namespace KIPIPlugins;

namespace KIPIVideoSlideShowPlugin
{

class ExportDialog : public KPToolDialog
{
    Q_OBJECT

public:

    explicit ExportDialog(const ImageCollection& images);
    ~ExportDialog();

    void setImages(const ImageCollection& images);
    void addItems(const KUrl::List& itemList);

protected:

    void closeEvent(QCloseEvent*);

private:

    void readSettings();
    void saveSettings();

    void busy(bool busy);

    void processAll(MyImageListViewItem* const item);
    MyImageListViewItem* setUpImageItems() const;

private Q_SLOTS:

    void slotDefault();
    void slotClose();
    void slotStartStop();
    void slotAborted();
    void slotThreadFinished();
    void slotShowError(const QString& error);
    void slotProcessedFrame(const KIPIVideoSlideShowPlugin::ActionData& ad);
    void updateSettingWidget();
    void updateImageTime(int time);
    void updateImageEffect(const QString& data,     EFFECT effect);
    void updateImageTransition(const QString& data, TRANSITION_TYPE type);
    void updateImageTransSpeed(const QString& data, TRANSITION_SPEED speed);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIVideoSlideShowPlugin

#endif // EXPORTDIALOG_H
