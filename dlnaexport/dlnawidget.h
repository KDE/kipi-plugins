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

#ifndef DLNAWIDGET_H
#define DLNAWIDGET_H

// Qt includes

#include <QWidget>
#include <QString>
#include <kurl.h>

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIDLNAExportPlugin
{

class MediaServerWindow;

class DLNAWidget : public QWidget
{
    Q_OBJECT

public:

    explicit DLNAWidget(QWidget* const parent);
    ~DLNAWidget();
	void setControlButtons(bool);
	void setImages(KUrl::List);

public Q_SLOTS:

    void reactivate();
//    void slotSelectDirectory();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIDLNAExportPlugin

#endif // DLNAWIDGET_H
