/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-02-19
 * Description : a kipi plugin to export images to VKontakte.ru web service
 *
 * Copyright (C) 2011 by Roman Tsisyk <roman at tsisyk dot com>
 * Copyright (C) 2011 by Alexander Potashev <aspotashev at gmail dot com>
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

#ifndef VKONTAKTE_ALBUMDIALOG_H
#define VKONTAKTE_ALBUMDIALOG_H

// KDE includes

#include <kdialog.h>

// libkvkontakte includes

#include <libkvkontakte/albuminfo.h>

class KComboBox;
class KTextEdit;
class KLineEdit;

namespace KIPIVkontaktePlugin
{

class VkontakteAlbumDialog : public KDialog
{
    Q_OBJECT

public:
    VkontakteAlbumDialog(QWidget *parent, AlbumInfoPtr album);
    ~VkontakteAlbumDialog();

    AlbumInfoPtr album() const;

protected Q_SLOTS:
    void slotButtonClicked(int button);

protected:
    KLineEdit *m_titleEdit;
    KTextEdit *m_summaryEdit;
    KComboBox *m_albumPrivacyCombo;
    KComboBox *m_commentsPrivacyCombo;

    AlbumInfoPtr m_album;
};

} // namespace KIPIVkontaktePlugin

#endif // VKONTAKTE_ALBUMDIALOG_H
