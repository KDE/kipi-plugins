/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-10-27
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2008 by Andi Clemens <andi dot clemens at gmx dot net>
 * Copyright (C) 2007-2008 by Vardhman Jain <vardhman at gmail dot com>
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

#ifndef NEWALBUMDIALOG_H
#define NEWALBUMDIALOG_H

// Qt includes

#include <QDialog>

// Local includes

#include "ui_picasawebnewalbumdialog.h"

namespace KIPIPicasawebExportPlugin
{

class NewAlbumDialog : public QDialog, public Ui::NewAlbumDialog
{

public:

    NewAlbumDialog( QWidget *parent ) : QDialog( parent )
    {
	setupUi( this );
    }
};

} // namespace KIPIPicasawebExportPlugin

#endif // NEWALBUMDIALOG_H
