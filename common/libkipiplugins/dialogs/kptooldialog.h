/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-04
 * Description : Tool dialog
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPTOOLDIALOG_H
#define KPTOOLDIALOG_H

// KDE includes

#include <kdialog.h>
#include <kpagedialog.h>
#include <kassistantdialog.h>

// Local includes

#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KPAboutData;

class KIPIPLUGINS_EXPORT KPToolDialog : public KDialog
{
    Q_OBJECT

public:

    KPToolDialog(QWidget* const parent=0);
    ~KPToolDialog();

    void setAboutData(KPAboutData* const about);

private Q_SLOTS:

    void slotHelp();

private:

    class KPToolDialogPriv;
    KPToolDialogPriv* const d;
};

// -----------------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPWizardDialog : public KAssistantDialog
{
    Q_OBJECT

public:

    KPWizardDialog(QWidget* const parent=0);
    ~KPWizardDialog();

    void setAboutData(KPAboutData* const about);

private Q_SLOTS:

    void slotHelp();

private:

    class KPWizardDialogPriv;
    KPWizardDialogPriv* const d;
};

// -----------------------------------------------------------------------------------

class KIPIPLUGINS_EXPORT KPPageDialog : public KPageDialog
{
    Q_OBJECT

public:

    KPPageDialog(QWidget* const parent=0);
    ~KPPageDialog();

    void setAboutData(KPAboutData* const about);

private Q_SLOTS:

    void slotHelp();

private:

    class KPPageDialogPriv;
    KPPageDialogPriv* const d;
};

} // namespace KIPIPlugins

#endif /* KPTOOLDIALOG_H */
