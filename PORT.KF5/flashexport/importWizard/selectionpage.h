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

#ifndef SELECTIONPAGE_H
#define SELECTIONPAGE_H

// Qt includes

#include <QWidget>

// Include files for KIPI

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpwizardpage.h"
#include "simpleviewersettingscontainer.h"

using namespace KIPIPlugins;

namespace KIPIFlashExportPlugin
{

class FlashManager; 

class SelectionPage: public KPWizardPage
{
    Q_OBJECT

public:

    SelectionPage(FlashManager* const mngr, KAssistantDialog* const dlg);
    ~SelectionPage();

    /**
     * Set a collection selector or a image dialog according to user choice on the intropage
     * @param choice - 0 -collection, 1 - image dialog
     */
    void setPageContent(int choice);

    /**
     * Set settings intro SimpleViewerSettingsContainer
     */
    void settings(SimpleViewerSettingsContainer* const container);

    /**
     * Check if at least one collection or one image is selected
     * User won't be allowed to start export with no images selected
     * @param imageGetOption 0 - collection 1 - iamgeDialog
     * to know where to check for selected images
     */
    bool isSelectionEmpty(int imageGetOption);

private:

    class SelectionPagePriv;
    SelectionPagePriv* const d;
};

}   // namespace KIPIFlashExportPlugin

#endif /* SelectionPage_H */
