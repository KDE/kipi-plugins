/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-06-25
 * Description : Tool dialog (KDELibs4 compatibility version)
 *
 * (c) 2015  Alexander Potashev <aspotashev@gmail.com>
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

#ifndef KP4TOOLDIALOG_H
#define KP4TOOLDIALOG_H

// KDE includes

#include <kdialog.h>

// Local includes

#include "kptooldialog.h"
#include "kipiplugins_export.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KP4ToolDialog : public KDialog, public KPDialogBase
{
public:

    KP4ToolDialog(QWidget* const parent=0);
    virtual ~KP4ToolDialog();
};

} // namespace KIPIPlugins

#endif /* KP4TOOLDIALOG_H */
