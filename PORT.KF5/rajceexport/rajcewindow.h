/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#ifndef RAJCEWINDOW_H
#define RAJCEWINDOW_H

// Local includes

#include "kptooldialog.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIRajceExportPlugin
{

class RajceWidget;

class RajceWindow : public KPToolDialog
{
    Q_OBJECT

public:

    explicit RajceWindow(const QString& tmpFolder, QWidget* const parent = 0, Qt::WFlags flags = 0);
    ~RajceWindow();

    void reactivate();

private Q_SLOTS:

    void slotSetUploadButtonEnabled(bool);
    void slotClose();

private:

    RajceWidget* m_widget;
};

} // namespace KIPIRajceExportPlugin

#endif // RAJCEWINDOW_H
