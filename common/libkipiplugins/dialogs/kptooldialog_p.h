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

// Local includes

#include "kptooldialog.h"

namespace KIPIPlugins
{

class KPToolDialog::KPToolDialogPriv
{
public:

    KPToolDialogPriv(KDialog* const dlg);
    ~KPToolDialogPriv();

    void setupHelpButton(KPAboutData* const data);
    void callHelpHandbook();

public:

    KPAboutData* about;
    KDialog*     dialog;
};

} // namespace KIPIPlugins

#endif /* KPTOOLDIALOG_PRIVATE_H */
