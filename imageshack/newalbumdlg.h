/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2010-02-01
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2010 by Jens Mueller <tschenser at gmx dot de>
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

#ifndef NEWALBUMDLG_H
#define NEWALBUMDLG_H

// Qt includes

#include <QRadioButton>
#include <QDateTimeEdit>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCloseEvent>

// Local includes

#include "kpnewalbumdialog.h"

using namespace KIPIPlugins;

namespace KIPIImageshackPlugin
{

class NewAlbumDlg : public KPNewAlbumDialog
{
    Q_OBJECT

public:

    NewAlbumDlg(QWidget* const parent, const QString& pluginName);
    ~NewAlbumDlg();

    QString getAlbumTitle();
};

} // namespace KIPIImageshackPlugin

#endif // NEWALBUMDLG_H
