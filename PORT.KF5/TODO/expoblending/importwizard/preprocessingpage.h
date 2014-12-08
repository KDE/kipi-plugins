/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

namespace KIPIExpoBlendingPlugin
{

class Manager;

class PreProcessingPage : public KPWizardPage
{
    Q_OBJECT

public:

    PreProcessingPage(Manager* const mngr, KAssistantDialog* const dlg);
    ~PreProcessingPage();

    void process();
    void cancel();

Q_SIGNALS:

    void signalPreProcessed(const ItemUrlsMap&);

private Q_SLOTS:

    void slotProgressTimerDone();
    void slotAction(const KIPIExpoBlendingPlugin::ActionData&);
    void slotShowDetails();

private:

    void resetTitle();

private:

    class Private;
    Private* const d;
};

}   // namespace KIPIExpoBlendingPlugin

#endif /* PREPROCESSING_PAGE_H */
