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

// KDE includes

#include <kdialog.h>

// Libkipi includes

#include <libkipi/interface.h>

namespace KIPIPlugins
{
    class KPAboutData;
}

namespace KIPIRajceExportPlugin
{

class RajceWidget;

class RajceWindow : public KDialog
{
    Q_OBJECT

public:

    explicit RajceWindow(KIPI::Interface* interface, const QString& tmpFolder, QWidget* parent = 0, Qt::WFlags flags = 0);
    ~RajceWindow();

    void reactivate();

private Q_SLOTS:

    void showHelp();
    void slotSetUploadButtonEnabled(bool);
    void slotClose();

private:

    KIPI::Interface*          m_interface;
    RajceWidget*              m_widget;
    KIPIPlugins::KPAboutData* m_about;
};

} // namespace KIPIRajceExportPlugin

#endif // RAJCEWINDOW_H
