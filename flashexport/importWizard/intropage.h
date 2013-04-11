/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#ifndef INTRO_PAGE_H
#define INTRO_PAGE_H

// KDE includes

#include <kcombobox.h>

// Local includes

#include "kpwizardpage.h"
#include "simpleviewersettingscontainer.h"

using namespace KIPIPlugins;

namespace KIPIFlashExportPlugin
{

class FlashManager; 

class IntroPage : public KPWizardPage
{
public:

    explicit IntroPage(KAssistantDialog* const dlg);
    ~IntroPage();

    void settings(SimpleViewerSettingsContainer* const settings);

private:

    class IntroPagePriv;
    IntroPagePriv* const d;
};

}   // namespace KIPIFlashExportPlugin

#endif /* INTRO_PAGE_H */
