/* ===============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-10-01
 * Description : Dialog to allow a custom page layout for
 *               printimages plugin.
 *
 * Copyright (C) 2010 by Angelo Naselli <anaselli at linux dot it>
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
 * ============================================================== */

// Local includes.

#include "customdlg.h"
#include "customdlg.moc"

namespace KIPIPrintImagesPlugin
{

CustomLayoutDlg::CustomLayoutDlg ( QWidget *parent )
    : QDialog ( parent )
{
  setupUi ( this );

  connect ( m_doneButton, SIGNAL ( clicked() ),
            this, SLOT ( accept() ) );
}

CustomLayoutDlg:: ~CustomLayoutDlg()
{
}

} // namespace KIPIGalleryExportPlugin
