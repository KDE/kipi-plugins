/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a template to create wizzard page.
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

#ifndef WIZARD_PAGE_H
#define WIZARD_PAGE_H

// Qt includes

#include <QScrollArea>
#include <QString>
#include <QPixmap>

// Local includes

#include "kipiplugins_export.h"

class KAssistantDialog;
class KPageWidgetItem;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT WizardPage : public QScrollArea
{

public:

    WizardPage(KAssistantDialog* dlg, const QString& title);
    virtual ~WizardPage();

    KPageWidgetItem* page() const;

    void setPageWidget(QWidget* w);
    void setLeftBottomPix(const QPixmap& pix);

private:

    class WizardPagePriv;
    WizardPagePriv* const d;
};

}   // namespace KIPIPlugins

#endif /* WIZARD_PAGE_H */
