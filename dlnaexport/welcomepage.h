/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

// KDE includes

#include <kcombobox.h>

// Qt includes

#include <QWidget>

namespace KIPIDLNAExportPlugin
{

class WelcomePage : public QWidget
{
    Q_OBJECT

public:

    enum ImageGetOption
    {
        COLLECTION = 0,
        IMAGEDIALOG
    };

public:

    explicit WelcomePage(QWidget* const parent);
    ~WelcomePage();

    bool getImageDialogOptionSelected() const;

public Q_SLOTS:

private:

    class Private;
    Private* const d;
};

} // namespace KIPIDLNAExportPlugin

#endif // WELCOMEPAGE_H