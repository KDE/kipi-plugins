/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export to flash
 *
 * Copyright (C) 2011 by Veaceslav Munteanu <slavuttici at gmail dot com>
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

#ifndef GENERAL_PAGE_H
#define GENERAL_PAGE_H

// Local includes

#include "wizardpage.h"
#include "simpleviewersettingscontainer.h"

namespace KIPIFlashExportPlugin
{

class FlashManager;

class GeneralPage: public KIPIPlugins::WizardPage
{
    Q_OBJECT

public:

    GeneralPage(KAssistantDialog* dlg);
    ~GeneralPage();

    void setSettings(const SimpleViewerSettingsContainer* settings);
    void settings(SimpleViewerSettingsContainer* settings);

private:

    class GeneralPagePriv;
    GeneralPagePriv* const d;
};

}   // namespace KIPIFlashExportPlugin

#endif /* GENERAL_PAGE_H */
