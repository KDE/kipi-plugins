/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Local includes

#include "kpwizardpage.h"
#include "manager.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class IntroPage : public KPWizardPage
{
    Q_OBJECT

public:

    IntroPage(Manager* const mngr, KAssistantDialog* const dlg);
    ~IntroPage();

    bool binariesFound();

Q_SIGNALS:

    void signalIntroPageIsValid(bool);

private:

    class IntroPagePriv;
    IntroPagePriv* const d;
};

}   // namespace KIPIExpoBlendingPlugin

#endif /* INTRO_PAGE_H */
