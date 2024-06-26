/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2009-11-13
 * Description : a template to create wizzard page.
 *
 * Copyright (C) 2009-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPWIZARD_PAGE_H
#define KPWIZARD_PAGE_H

// Qt includes

#include <QWizardPage>
#include <QString>
#include <QPixmap>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KPWizardDialog;

class KIPIPLUGINS_EXPORT KPWizardPage : public QWizardPage
{

public:

    KPWizardPage(KPWizardDialog* const dlg, const QString& title);
    virtual ~KPWizardPage();

    void setPageWidget(QWidget* const w);
    void removePageWidget(QWidget* const w);
    void setLeftBottomPix(const QPixmap& pix);
    void setShowLeftView(bool v);

    void setComplete(bool b);
    bool isComplete() const override;

    int  id() const;

    KPWizardDialog* assistant() const;

private:

    class Private;
    Private* const d;
};

}   // namespace KIPIPlugins

#endif /* KPWIZARD_PAGE_H */
