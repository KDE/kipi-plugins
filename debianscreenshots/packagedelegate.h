/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

#ifndef PACKAGEDELEGATE_H
#define PACKAGEDELEGATE_H

// Qt includes

#include <QStyledItemDelegate>

namespace KIPIDebianScreenshotsPlugin
{

class PackageDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    explicit PackageDelegate(QObject* parent = 0);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

} // namespace KIPIDebianScreenshotsPlugin

#endif // PACKAGEDELEGATE_H
