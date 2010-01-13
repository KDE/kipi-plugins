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
            
  m_photoGridCheck->setToolTip(i18n("Choose your grid size"));
  m_photoGridCheck->setWhatsThis(i18n("Choose your grid size"));
  m_gridRows->setToolTip(i18n("Number of rows"));
  m_gridRows->setWhatsThis(i18n("Insert number of rows"));
  m_gridColumns->setToolTip(i18n("Number of columns"));
  m_gridColumns->setWhatsThis(i18n("Insert number of columns"));
  
  m_fitAsManyCheck->setToolTip(i18n("Choose to have a custom photo size album"));
  m_fitAsManyCheck->setWhatsThis(i18n("Choose to have a custom photo size album"));
  m_photoHeight->setToolTip(i18n("Photo height"));
  m_photoHeight->setWhatsThis(i18n("Insert photo height"));
  m_photoWidth->setToolTip(i18n("Photo width"));
  m_photoWidth->setWhatsThis(i18n("Insert photo width"));
  
  m_autorotate->setToolTip(i18n("Auto rotate photo"));  
}

CustomLayoutDlg:: ~CustomLayoutDlg()
{
}

} // namespace KIPIGalleryExportPlugin
