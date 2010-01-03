/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef LAST_PAGE_H
#define LAST_PAGE_H

// Local includes

#include "wizardpage.h"

namespace KIPIExpoBlendingPlugin
{

class Manager;
class LastPagePriv;

class LastPage : public KIPIPlugins::WizardPage
{
public:

    LastPage(Manager* mngr, KAssistantDialog* dlg);
    ~LastPage();

private:

    LastPagePriv* const d;
};

}   // namespace KIPIExpoBlendingPlugin

#endif /* LAST_PAGE_H */
