/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-07-05
 * Description : A combobox which also has an intermediate state.
 *               This is akin to the intermediate state in a checkbox and
 *               needed when a single combobox controls more than one item,
 *               which are manually set to different states.
 *               The intermediate state is indicated by appending an extra item
 *               with a user specified text (default is "Various"). Whenever an
 *               other item is set, this special state is removed from the list
 *               so it can never be selected explicitly.
 *
 * Copyright (C) 2009 by Pieter Edelman <pieter dot edelman at gmx dot net>
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

#ifndef COMBOBOXINTERMEDIATE_H_
#define COMBOBOXINTERMEDIATE_H_

// Qt includes

#include <QComboBox>
#include <QString>

// KDE includes

#include <klocale.h>

namespace KIPIFlickrExportPlugin
{

class ComboBoxIntermediate : public QComboBox
{
    Q_OBJECT

public:

    /* Initialize the combobox with a parent and a string to indicate the
     * intermediate state. */
    ComboBoxIntermediate(QWidget * = 0, QString = i18n("Various"));
    ~ComboBoxIntermediate();

    /* Set the state of the combobox to intermediate. The intermediate state is
     * 'unset' when another index is selected. */
    void setIntermediate(bool);

private Q_SLOTS:

    void slotIndexChanged(int);

private:

    bool    m_isIntermediate;
    QString m_intermediateText;
};

} // namespace KIPIFlickrExportPlugin

#endif /* COMBOBOXINTERMEDIATE_H_ */
