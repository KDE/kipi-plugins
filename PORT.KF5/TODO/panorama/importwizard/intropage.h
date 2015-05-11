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
 * Copyright (C) 2009-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QAbstractButton>

// Local includes

#include "kpwizardpage.h"
#include "manager.h"

using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

class Manager;

class IntroPage : public KPWizardPage
{
    Q_OBJECT

public:

    IntroPage(Manager* const mngr, KAssistantDialog* const dlg);
    ~IntroPage();

    bool binariesFound();

private Q_SLOTS:

    void slotToggleGPano(int state);
    void slotChangeFileFormat(QAbstractButton* button);

    // TODO HDR
//     void slotShowFileFormat(int state);

Q_SIGNALS:

    void signalIntroPageIsValid(bool);

private:

    struct Private;
    Private* const d;
};

}   // namespace KIPIPanoramaPlugin

#endif /* INTRO_PAGE_H */
