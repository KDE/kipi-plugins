/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#ifndef NEWALBUMDIALOG_H
#define NEWALBUMDIALOG_H

// Local includes

#include "kpnewalbumdialog.h"

class QCheckBox;

using namespace KIPIPlugins;

namespace KIPIRajcePlugin
{

class NewAlbumDialog : public KPNewAlbumDialog
{
public:

    explicit NewAlbumDialog(QWidget* const parent = nullptr);

    QString albumName()        const;
    QString albumDescription() const;
    bool    albumVisible()     const;

private:

    QCheckBox* m_albumVisible;
};

} // namespace KIPIRajcePlugin

#endif // NEWALBUMDIALOG_H
