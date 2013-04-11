/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-11-28
 * Description : a widget that holds unprocessed image settings
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

#ifndef UNPROCESSEDSETTINGSBOX_H
#define UNPROCESSEDSETTINGSBOX_H

// Qt includes

#include <QGroupBox>

namespace KIPIRemoveRedEyesPlugin
{

class UnprocessedSettingsBox : public QGroupBox
{
    Q_OBJECT

public:

    enum HandleMode
    {
        Ask=0,
        Keep,
        Remove
    };

public:

    explicit UnprocessedSettingsBox(QWidget* const parent = 0);
    ~UnprocessedSettingsBox();

    int handleMode() const;
    void setHandleMode(int mode);

Q_SIGNALS:

    void settingsChanged();

private:

    struct Private;
    Private* const d;
};

} // namespace KIPIRemoveRedEyesPlugin

#endif /* UNPROCESSEDSETTINGSBOX_H */
