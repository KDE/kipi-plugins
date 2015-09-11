/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a kipi plugin to export images to Yandex.Fotki web service
 *
 * Copyright (C) 2011 by Roman Tsisyk <roman at tsisyk dot com>
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

#ifndef YF_ALBUMDIALOG_H
#define YF_ALBUMDIALOG_H

// KDE includes

#include <klineedit.h>
#include <ktextedit.h>

// Qt includes

#include <QtWidgets/QDialog>

// Local includes

#include "yfalbum.h"
#include "kpnewalbumdialog.h"

using namespace KIPIPlugins;

namespace KIPIYandexFotkiPlugin
{

class YandexFotkiAlbumDialog: public KPNewAlbumDialog
{
    Q_OBJECT

public:

    YandexFotkiAlbumDialog(QWidget* const parent, YandexFotkiAlbum& album);
    ~YandexFotkiAlbumDialog();

    YandexFotkiAlbum& album() const
    {
        return m_album;
    }

protected Q_SLOTS:

    void slotOkClicked();

protected:

    QLineEdit*        m_passwordEdit;

    YandexFotkiAlbum& m_album;
};

} // namespace KIPIYandexFotkiPlugin

#endif // YF_ALBUMDIALOG_H
