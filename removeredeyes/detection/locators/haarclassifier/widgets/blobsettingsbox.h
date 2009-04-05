/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-11-01
 * Description : a widget that holds all blob extraction settings
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef BLOBSETTINGSBOX_H
#define BLOBSETTINGSBOX_H

// Qt includes

#include <QGroupBox>

class QWidget;

namespace KIPIRemoveRedEyesPlugin
{

struct BlobSettingsBoxPriv;

class BlobSettingsBox : public QGroupBox
{
    Q_OBJECT

public:

    BlobSettingsBox(QWidget* parent = 0);
    ~BlobSettingsBox();

    int minBlobSize() const;
    void setMinBlobSize(int);

    double minRoundness() const;
    void setMinRoundness(double);

Q_SIGNALS:

    void settingsChanged();

private:

    BlobSettingsBoxPriv* const d;
};

}

#endif /* BLOBSETTINGSBOX_H */
