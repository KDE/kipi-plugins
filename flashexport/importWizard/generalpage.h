/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export to flash
 *
 * Copyright (C) 2011      by Veaceslav Munteanu <slavuttici at gmail dot com>
 * Copyright (C) 2009-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "kpwizardpage.h"
#include "simpleviewersettingscontainer.h"

using namespace KIPIPlugins;

namespace KIPIFlashExportPlugin
{

class FlashManager;

class GeneralPage: public KPWizardPage
{
    Q_OBJECT

public:

    explicit GeneralPage(KPWizardDialog* const dlg);
    ~GeneralPage();

    void setSettings(const SimpleViewerSettingsContainer* const settings);
    void settings(SimpleViewerSettingsContainer* const settings);

private:

    class Private;
    Private* const d;
};

}   // namespace KIPIFlashExportPlugin

#endif /* GENERAL_PAGE_H */
