/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-10-23
 * Description : a kipi plugin to export images to shwup.com web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2009      by Timothée Groleau <kde at timotheegroleau dot com>
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

#ifndef SWALBUM_H
#define SWALBUM_H

// KDE includes

#include <kdialog.h>

class KLineEdit;
class KTextEdit;
class KComboBox;

namespace KIPIShwupPlugin
{

class SwAlbum;

class SwNewAlbum : public KDialog
{
    Q_OBJECT

public:

    SwNewAlbum(QWidget* const parent);
    ~SwNewAlbum();

    void getAlbumProperties(SwAlbum& album);

private:

    KLineEdit* m_titleEdt;

    friend class SwWindow;
};

} // namespace KIPIShwupPlugin

#endif // SWALBUM_H
