/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Dropbox web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#ifndef DBWIDGET_H
#define DBWIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include "kpsettingswidget.h"

class QLabel;
class QSpinBox;
class QCheckBox;
class QButtonGroup;

class QComboBox;
class KPushButton;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

using namespace KIPIPlugins;

namespace KIPIDropboxPlugin
{

class DropboxWidget : public KPSettingsWidget
{
    Q_OBJECT

public:

    DropboxWidget(QWidget* const parent, KIPI::Interface* const iface, const QString& pluginName);
    ~DropboxWidget();

    virtual void updateLabels(const QString& name = QString(), const QString& url = QString());

    friend class DBWindow;
};

} // namespace KIPIDropboxPlugin

#endif /*DBWIDGET_H*/
