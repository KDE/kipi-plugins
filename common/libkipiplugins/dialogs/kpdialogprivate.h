/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-04
 * Description : Tool dialog private
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

#ifndef KPTOOLDIALOG_PRIVATE_H
#define KPTOOLDIALOG_PRIVATE_H

// Qt includes

#include <QObject>

// Local includes

#include "kptooldialog.h"

class KPushButton;

namespace KIPIPlugins
{

class KPDialogPrivate : public QObject
{
    Q_OBJECT

public:

    KPDialogPrivate(KDialog* const dlg);
    ~KPDialogPrivate();

    void setAboutData(KPAboutData* const data, KPushButton* help=0);

private Q_SLOTS:

    void slotHelp();

private:

    KPAboutData* m_about;
    KDialog*     m_dialog;
};

} // namespace KIPIPlugins

#endif /* KPTOOLDIALOG_PRIVATE_H */
