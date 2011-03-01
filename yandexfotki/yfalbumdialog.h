/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#ifndef YANDEXFOTKI_ALBUMDIALOG_H
#define YANDEXFOTKI_ALBUMDIALOG_H

// KDE includes

#include <KDialog>
#include <KLineEdit>
#include <KTextEdit>

// Local includes

#include "yfalbum.h"

namespace KIPIYandexFotkiPlugin
{

class YandexFotkiAlbumDialog: public KDialog
{
    Q_OBJECT

public:

    YandexFotkiAlbumDialog(QWidget* parent, YandexFotkiAlbum& album);
    ~YandexFotkiAlbumDialog();

    YandexFotkiAlbum& album() const
    {
        return m_album;
    }

protected Q_SLOTS:

    void slotButtonClicked(int button);

protected:

    KLineEdit*        m_titleEdit;
    KTextEdit*        m_summaryEdit;
    KLineEdit*        m_passwordEdit;

    YandexFotkiAlbum& m_album;
};

} // namespace KIPIYandexFotkiPlugin

#endif // YANDEXFOTKI_ALBUMDIALOG_H
