/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
 * Date        : 2008-09-12
 * Description : a kipi plugin to export to Yandex.Fotki
 *
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#ifndef YANDEXFOTKIWIDGET_H
#define YANDEXFOTKIWIDGET_H

// Local includes

#include "kpsettingswidget.h"

namespace KIPIPlugins
{
    class KPImagesList;
    class KPProgressWidget;
}

using namespace KIPIPlugins;

namespace KIPIYandexFotkiPlugin
{

class YandexFotkiWidget : public KPSettingsWidget
{
    Q_OBJECT

public:

    enum UpdatePolicy
    {
        POLICY_UPDATE_MERGE = 0,
        POLICY_UPDATE_KEEP, // is not used in GUI
        POLICY_SKIP,
        POLICY_ADDNEW
    };

public:

    YandexFotkiWidget(QWidget* const parent, KIPI::Interface* const iface, const QString& pluginName);
    ~YandexFotkiWidget();

    virtual void updateLabels(const QString& name = QString(), const QString& url = QString());

private:

    // upload settings
    QComboBox*    m_accessCombo;
    QCheckBox*    m_hideOriginalCheck;
    QCheckBox*    m_disableCommentsCheck;
    QCheckBox*    m_adultCheck;
    QButtonGroup* m_policyGroup;

    friend class YandexFotkiWindow;

}; // class YandexFotkiWidget

}  // namespace KIPIYandexFotkiPlugin

#endif // YANDEXFOTKIWIDGET_H