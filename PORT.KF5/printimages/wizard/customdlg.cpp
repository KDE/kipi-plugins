/* ===============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-10-01
 * Description : Dialog to allow a custom page layout for
 *               printimages plugin.
 *
 * Copyright (C) 2010-2012 by Angelo Naselli <anaselli at linux dot it>
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

// Local includes

#include "customdlg.moc"

namespace KIPIPrintImagesPlugin
{

enum CustomChoice
{
    PHOTO_GRID              = 1,
    FIT_AS_MANY_AS_POSSIBLE = 2,
    PHOTOS_PER_PAGE         = 3
};

CustomLayoutDlg::CustomLayoutDlg(QWidget* const parent)
    : QDialog ( parent )
{
    setupUi ( this );

    connect ( m_doneButton, SIGNAL (clicked()),
              this, SLOT (accept()) );

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

void CustomLayoutDlg::readSettings()
{
    KConfig config ( "kipirc" );
    KConfigGroup group = config.group ( QString ( "PrintAssistant" ) );

    QSize gridSize = group.readEntry  ( "Custom-gridSize", QSize(3,8));
    m_gridRows->setValue(gridSize.width());
    m_gridColumns->setValue(gridSize.height());

    QSize photoSize = group.readEntry  ( "Custom-photoSize", QSize(5,4));
    m_photoHeight->setValue(photoSize.height());
    m_photoWidth->setValue(photoSize.width());

    int index       = group.readEntry  ( "Custom-photoUnits", 0);
    m_photoUnits->setCurrentIndex(index);

    bool autorotate = group.readEntry ( "Custom-autorotate", 0 ) == 1;
    m_autorotate->setChecked(autorotate);

    int choice      = group.readEntry  ( "Custom-choice", (int)PHOTO_GRID);

    if (choice == PHOTOS_PER_PAGE)
    {
      m_photosXPageCheck->setChecked(true);
    }
    else if (choice == FIT_AS_MANY_AS_POSSIBLE)
    {
      m_fitAsManyCheck->setChecked(true);
    }
    else
    {
      m_photoGridCheck->setChecked(true);
    }
}

void CustomLayoutDlg::saveSettings()
{
    KConfig config ( "kipirc" );
    KConfigGroup group = config.group ( QString ( "PrintAssistant" ) );

    int choice = PHOTO_GRID;

    if (m_fitAsManyCheck->isChecked())
      choice = FIT_AS_MANY_AS_POSSIBLE;
    else if (m_photosXPageCheck->isChecked())
      choice = PHOTOS_PER_PAGE;

    group.writeEntry("Custom-choice", choice);
    group.writeEntry ("Custom-gridSize",  QSize(m_gridRows->value(), m_gridColumns->value()));
    group.writeEntry ("Custom-photoSize", QSize(m_photoWidth->value(), m_photoHeight->value()));
    group.writeEntry ("Custom-photoUnits", m_photoUnits->currentIndex());
    group.writeEntry( "Custom-autorotate", (m_autorotate->isChecked() ? 1 : 0));
}

} // namespace KIPIGalleryExportPlugin
