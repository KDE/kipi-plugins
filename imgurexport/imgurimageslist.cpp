/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010 by Marius Orcisk <marius at habarnam dot ro>
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

#include "imgurimageslist.moc"

// KDE includes

#include "kdebug.h"

using namespace KIPIPlugins;

namespace KIPIImgurExportPlugin
{

ImgurImagesList::ImgurImagesList(KIPI::Interface* interface, QWidget* parent)
    : KPImagesList(interface, parent)
{
}

ImgurImagesList::~ImgurImagesList()
{
}

void ImgurImagesList::updateItemWidgets()
{
    kDebug() << "update";
}

} // namespace KIPIImgurExportPlugin
