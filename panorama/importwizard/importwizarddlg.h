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

#ifndef IMPORTWIZARD_DLG_H
#define IMPORTWIZARD_DLG_H

// Qt includes

#include <QString>
#include <QWidget>

// KDE includes

#include <kurl.h>

// Local includes

#include "actions.h"
#include "kptooldialog.h"

class KPageWidgetItem;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIPanoramaPlugin
{

class Manager;
class PanoramaAboutData;

class ImportWizardDlg : public KPWizardDialog
{
    Q_OBJECT

public:

    explicit ImportWizardDlg(Manager* const mngr, QWidget* const parent=0);
    ~ImportWizardDlg();

    KUrl::List itemUrls() const;

    Manager* manager() const;

private Q_SLOTS:

    void next();
    void back();
    void accept();

    void slotIntroPageIsValid(bool);
    void slotItemsPageIsValid(bool);
    void slotPreProcessed(bool);
    void slotOptimized(bool);
    void slotPreviewStitchingFinished(bool);
    void slotStitchingFinished(bool);
    void slotCopyFinished(bool);
    void slotLastPageIsValid(bool);

private:

    struct Private;
    Private* const d;
};

}   // namespace KIPIPanoramaPlugin

#endif /* IMPORTWIZARD_DLG_H */
