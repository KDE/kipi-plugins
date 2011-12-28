/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-12-28
 * Description : Simple gui to select images
 *
 * Copyright (C) 2011-2012 by A Janardhan Reddy <annapareddyjanardhanreddy@gmail.com>
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

#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <kmainwindow.h>
#include <kfiledialog.h>

#include <QPushButton>

#include "actionthread.h"


class ImageSelector : public KMainWindow
{
    Q_OBJECT
public:
    ImageSelector();

public Q_SLOTS:
    void selectImages();

private:
    QPushButton *button;
    ActionThread *mainThread;
};

#endif // IMAGESELECTOR_H
