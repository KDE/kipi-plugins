/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2012-07-01
 * @brief  Settings Widget
 *
 * @author Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef SLIDESHOWSETTINGSWIDGET_H
#define SLIDESHOWSETTINGSWIDGET_H

#include <ktabwidget.h>
#include "myimagelist.h"

namespace KIPIVideoSlideShowPlugin
{

class SlideShowSettingsWidget: public KTabWidget
{
    Q_OBJECT

public:

    SlideShowSettingsWidget(QWidget* const parent = 0, Qt::WFlags flags = 0);
    ~SlideShowSettingsWidget();

    QString getTempDirPath();
    void    setTempDirPath(QString& path);

    void resetToDefault();
    void updateData(int time, const QString& transition, const QString& transSpeed, const QString& effect);

Q_SIGNALS:

    void DataChanged(const QString& data, MyImageList::FieldType type);

private:

    void setUpPPMSettings();
    void setUpVideoSettings();

private Q_SLOTS:

    void slotSelectTempDirectory();
    void effectIndexChanged(int index);
    void transIndexChanged(int index);
    void transSpeedIndexChanged(int index);
    void timeValueChanged(int time);

private:

    class SlideShowPriv;
    SlideShowPriv* const d;
};

} // namespace KIPIVideoSlideShowPlugin

#endif // SLIDESHOWSETTINGSWIDGET_H
