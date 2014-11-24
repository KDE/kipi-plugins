/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2003-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005      by Owen Hirst <n8rider@sbcglobal.net>
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

#ifndef RENAMEIMAGESDIALOG_H
#define RENAMEIMAGESDIALOG_H

// KDE includes

#include <kurl.h>

// Local includes

#include "kptooldialog.h"

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIBatchProcessImagesPlugin
{

class RenameImagesWidget;

class RenameImagesDialog : public KPToolDialog
{
    Q_OBJECT

public:

    RenameImagesDialog(const KUrl::List& images, QWidget* parent);
    ~RenameImagesDialog();

private:

    RenameImagesWidget* m_widget;
};

} // namespace KIPIBatchProcessImagesPlugin

#endif /* RENAMEIMAGESDIALOG_H */
