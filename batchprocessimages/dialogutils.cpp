/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Copyright (C) 2009 by Aurélien Gâteau <agateau@kde.org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */
#include "dialogutils.h"

#include <kdialog.h>
#include <khelpmenu.h>

namespace DialogUtils {

void setupHelpButton(KDialog* dialog, KAboutData* aboutData)
{
    disconnect(dialog, SIGNAL(helpClicked()), dialog, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(dialog, aboutData, false);
    KMenu* menu = helpMenu->menu();
    menu->removeAction(menu->actions().first());

    QAction* openHandbookAction = new QAction(i18n("Handbook"), dialog);
    connect(openHandbookAction, SIGNAL(triggered(bool)),
            dialog, SLOT(slotHelp()));
    menu->insertAction(menu->actions().first(), openHandbookAction);
    button(Help)->setMenu(menu);
}

} // namespace
