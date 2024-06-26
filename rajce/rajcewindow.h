/* ============================================================
 *
 * This file is a part of KDE project
 *
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

namespace KIPIRajcePlugin
{

class RajceWidget;

class RajceWindow : public KPToolDialog
{
    Q_OBJECT

public:

    explicit RajceWindow(const QString& tmpFolder, QWidget* const parent = nullptr);
    ~RajceWindow();

    void reactivate();

protected:
    void closeEvent(QCloseEvent* e) override;

private Q_SLOTS:

    void slotSetUploadButtonEnabled(bool);
    void slotFinished();

private:

    RajceWidget* m_widget;
};

} // namespace KIPIRajcePlugin

#endif // RAJCEWINDOW_H
