/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-15
 * Description : IPTC categories settings page.
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IPTC_CATEGORIES_H
#define IPTC_CATEGORIES_H

// Qt includes

#include <QWidget>
#include <QByteArray>

namespace KIPIMetadataEditPlugin
{

class IPTCCategoriesPriv;

class IPTCCategories : public QWidget
{
    Q_OBJECT

public:

    IPTCCategories(QWidget* parent);
    ~IPTCCategories();

    void applyMetadata(QByteArray& iptcData);
    void readMetadata(QByteArray& iptcData);

Q_SIGNALS:

    void signalModified();

private Q_SLOTS:

    void slotCategorySelectionChanged();
    void slotAddCategory();
    void slotDelCategory();
    void slotRepCategory();

private:

    IPTCCategoriesPriv* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // IPTC_CATEGORIES_H
