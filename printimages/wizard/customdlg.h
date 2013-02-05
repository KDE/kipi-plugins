/* ===============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-10-01
 * Description : Dialog to allow a custom page layout for
 *               printimages plugin.
 *
 * Copyright (C) 2010-2012 by Angelo Naselli <anaselli at linux dot it>
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
 * ============================================================== */

#ifndef CUSTOMDLG_H
#define CUSTOMDLG_H

#include "ui_customlayout.h"

// Qt includes

#include <QDialog>
#include <QWidget>

namespace KIPIPrintImagesPlugin
{

class CustomLayoutDlg : public QDialog , public Ui::CustomLayout
{
    Q_OBJECT

public:

    CustomLayoutDlg(QWidget* const parent = 0);
    ~CustomLayoutDlg();

    // retrieve last used data
    void readSettings();
    // save used data
    void saveSettings();
};

} // namespace KIPIPrintImagesPlugin

#endif // CUSTOMDLG_H
