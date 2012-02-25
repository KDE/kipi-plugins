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

#ifndef PROGRESS_PAGE_H
#define PROGRESS_PAGE_H

// Local includes

#include "kpwizardpage.h"

using namespace KIPIPlugins;

namespace KIPIFlashExportPlugin
{
    
class FlashManager;

class ProgressPage : public KPWizardPage
{
public:

    ProgressPage(FlashManager* const mngr, KAssistantDialog* const dlg);
    ~ProgressPage();
};

}   // namespace KIPIFlashExportPlugin

#endif /* INTRO_PAGE_H */
