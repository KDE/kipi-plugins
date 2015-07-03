/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#ifndef CAPTIONDIALOG_H
#define CAPTIONDIALOG_H

// Local includes

#include "ui_captiondialog.h"

namespace KIPIAdvancedSlideshowPlugin
{

class SharedContainer;

class CaptionDialog : public QWidget, Ui::CaptionDialog
{
    Q_OBJECT

public:

    CaptionDialog(QWidget* const parent, SharedContainer* const sharedData);
    ~CaptionDialog();

    void readSettings();
    void saveSettings();

private Q_SLOTS:

    void slotCommentsFontColorChanged();
    void slotCommentsBgColorChanged();

private:

    SharedContainer* m_sharedData;
};

} // namespace KIPIAdvancedSlideshowPlugin

#endif //CAPTIONDIALOG_H
