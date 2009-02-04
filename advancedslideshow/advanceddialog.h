/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef ADVANCEDDIALOG_H
#define ADVANCEDDIALOG_H

// Local includes.
#include "common.h"
#include "ui_advanceddialog.h"

namespace KIPIAdvancedSlideshowPlugin
{

class SharedData;

class AdvancedDialog : public QWidget, Ui::AdvancedDialog
{
    Q_OBJECT

public:
    AdvancedDialog(QWidget* parent, SharedData* sharedData);
    ~AdvancedDialog();

    void readSettings();
    void saveSettings();

private slots:
    void slotUseMillisecondsToggled( void );
    void slotCacheToggled( void );

signals:
    void useMillisecondsToggled( void );

private:
    SharedData*       m_sharedData;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif //ADVANCEDDIALOG_H
