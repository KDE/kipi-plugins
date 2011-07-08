/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef OPTIMIZE_PAGE_H
#define OPTIMIZE_PAGE_H

// Local includes

#include "wizardpage.h"

// Local includes

#include "actions.h"

namespace KIPIPanoramaPlugin
{

class Manager;
struct OptimizePagePriv;

class OptimizePage : public KIPIPlugins::WizardPage
{
    Q_OBJECT

public:

    OptimizePage(Manager* mngr, KAssistantDialog* dlg);
    ~OptimizePage();

private:

    struct OptimizePagePriv;
    OptimizePagePriv* const d;
};

}   // namespace KIPIPanoramaPlugin

#endif /* OPTIMIZE_PAGE_H */
