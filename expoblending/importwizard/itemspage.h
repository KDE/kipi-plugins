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

#ifndef ITEMS_PAGE_H
#define ITEMS_PAGE_H

// Local includes

#include "wizardpage.h"

// KDE includes

#include <kurl.h>

// Local includes

#include "actions.h"

using namespace KIPIPlugins;

namespace KIPIExpoBlendingPlugin
{

class Manager;
class ItemsPagePriv;

class ItemsPage : public WizardPage
{
    Q_OBJECT

public:

    ItemsPage(Manager* mngr, KAssistantDialog* dlg);
    ~ItemsPage();

    KUrl::List itemUrls() const;

Q_SIGNALS:

    void signalItemsPageIsValid(bool);

private:

    void setIdentity(const KUrl& url, const QString& identity);

private Q_SLOTS:

    void slotSetupList();
    void slotImageListChanged();
    void slotAddItems(const KUrl::List&);
    void slotAction(const KIPIExpoBlendingPlugin::ActionData&);

private:

    ItemsPagePriv* const d;
};

}   // namespace KIPIExpoBlendingPlugin

#endif /* ITEMS_PAGE_H */
