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

#ifndef SELECTIONPAGE_H
#define SELECTIONPAGE_H

// Qt includes

#include <QWidget>

// Include files for KIPI

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "wizardpage.h"

namespace KIPIFlashExportPlugin
{

class FlashManager; 

class SelectionPage: public KIPIPlugins::WizardPage
{
	Q_OBJECT
public:

	SelectionPage(FlashManager *mngr, KAssistantDialog* dlg);
    ~SelectionPage();
    
    QList<KIPI::ImageCollection> selection() const;

private:

    class SelectionPagePriv;
    SelectionPagePriv* const d;
};

}   // namespace KIPIFlashExportPlugin

#endif /* SelectionPage_H */
