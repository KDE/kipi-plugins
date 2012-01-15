/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <kassistantdialog.h>

// Local includes

#include "actions.h"

class KPageWidgetItem;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIPanoramaPlugin
{

class Manager;
class PanoramaAboutData;

class ImportWizardDlg : public KAssistantDialog
{
    Q_OBJECT

public:
    explicit ImportWizardDlg(Manager* mngr, QWidget* parent=0);
    ~ImportWizardDlg();

    KUrl::List itemUrls() const;

    Manager* manager() const;

private Q_SLOTS:

    void next();
    void back();
    void accept();

    void slotIntroPageIsValid(bool);
    void slotItemsPageIsValid(bool);
    void slotPreProcessed(const ItemUrlsMap&);
    void slotOptimized(const KUrl&);
    void slotStitchingFinished(const KUrl&);
    void slotCopyFinished(bool);
    void slotHelp();

private:

    struct ImportWizardDlgPriv;
    ImportWizardDlgPriv* const d;
};

}   // namespace KIPIPanoramaPlugin

#endif /* IMPORTWIZARD_DLG_H */
