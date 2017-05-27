/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : a kipi plugin to export images to VKontakte.ru web service
 *
 * Copyright (C) 2011 by Roman Tsisyk <roman at tsisyk dot com>
 * Copyright (C) 2011, 2015  Alexander Potashev <aspotashev@gmail.com>
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

#ifndef VKALBUMDIALOG_H
#define VKALBUMDIALOG_H

// Qt includes

#include <QDialog>

class QComboBox;
class QTextEdit;
class QLineEdit;

namespace KIPIVkontaktePlugin
{

class VkontakteAlbumDialog : public QDialog
{
    Q_OBJECT

public:

    struct AlbumInfo
    {
        QString title;
        QString description;
        int privacy;
        int commentPrivacy;
    };

public:

    /**
     * @brief Album creation dialog
     *
     * @param parent Parent widget
     */
    VkontakteAlbumDialog(QWidget* const parent);

    /**
     * @brief Album editing dialog
     *
     * @param parent Parent widget
     * @param album Initial album properties
     */
    VkontakteAlbumDialog(QWidget* const parent, const AlbumInfo& album);

    ~VkontakteAlbumDialog();

    const AlbumInfo &album() const;

protected Q_SLOTS:

    virtual void accept();

private:

    void initDialog(bool editing);

private:

    QLineEdit* m_titleEdit;
    QTextEdit* m_summaryEdit;
    QComboBox* m_albumPrivacyCombo;
    QComboBox* m_commentsPrivacyCombo;

    AlbumInfo  m_album;
};

} // namespace KIPIVkontaktePlugin

#endif // VKALBUMDIALOG_H
