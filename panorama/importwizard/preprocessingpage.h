/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef PREPROCESSING_PAGE_H
#define PREPROCESSING_PAGE_H

// Local includes

#include "kpwizardpage.h"

// KDE includes

#include <kurl.h>

// Local includes

#include "actions.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

class Manager;

class PreProcessingPage : public KPWizardPage
{
    Q_OBJECT

public:

    PreProcessingPage(Manager* const mngr, KAssistantDialog* const dlg);
    ~PreProcessingPage();

    void process();
    bool cancel();
    void resetPage();

Q_SIGNALS:

    void signalPreProcessed(bool);

private Q_SLOTS:

    void slotProgressTimerDone();
    void slotAction(const KIPIPanoramaPlugin::ActionData&);
    void slotShowDetails();

private:

    void resetTitle();

private:

    struct PreProcessingPagePriv;
    PreProcessingPagePriv* const d;
};

}   // namespace KIPIPanoramaPlugin

#endif /* PREPROCESSING_PAGE_H */
