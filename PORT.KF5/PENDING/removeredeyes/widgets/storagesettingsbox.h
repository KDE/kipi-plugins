/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-01
 * Description : a widget that holds all storage settings
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#ifndef STORAGESETTINGSBOX_H
#define STORAGESETTINGSBOX_H

// Qt includes

#include <QGroupBox>

namespace KIPIRemoveRedEyesPlugin
{

class StorageSettingsBox : public QGroupBox
{
    Q_OBJECT

public:

    enum StorageMode
    {
        Subfolder=0,
        Prefix,
        Suffix,
        Overwrite
    };

public:

    explicit StorageSettingsBox(QWidget* const parent = 0);
    ~StorageSettingsBox();

    int storageMode() const;
    void setStorageMode(int mode);

    QString extra() const;
    void setExtra(const QString& extra);

    QString keyword() const;
    void setKeyword(const QString& keyword);
    bool addKeyword() const;
    void setAddKeyword(bool checked);

Q_SIGNALS:

    void settingsChanged();

private Q_SLOTS:

    void buttonClicked(int);
    void keywordToggled(bool);

private:

    struct Private;
    Private* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* STORAGESETTINGSBOX_H */
