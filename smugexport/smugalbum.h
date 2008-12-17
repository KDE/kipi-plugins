/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-12-07
 * Description : a kipi plugin to export images to SmugMug web service
 *
 * Copyright (C) 2008 by Luka Renko <lure at kubuntu dot org>
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

#ifndef SMUGALBUM_H
#define SMUGALBUM_H

// Qt includes.

// KDE includes.
#include <kdialog.h>

class QRadioButton;
class QComboBox;
class KLineEdit;
class KTextEdit;

namespace KIPISmugExportPlugin
{

class SmugAlbum;

class SmugNewAlbum : public KDialog
{
    Q_OBJECT

public:

    SmugNewAlbum(QWidget* parent);
    ~SmugNewAlbum();

    void getAlbumProperties(SmugAlbum &album);

private:

    KLineEdit       *m_titleEdt;
    KTextEdit       *m_descEdt;
    QComboBox       *m_categCoB;
    QComboBox       *m_subCategCoB;

    KLineEdit       *m_passwdEdt;
    KLineEdit       *m_hintEdt;
    QRadioButton    *m_publicRBtn;
    QRadioButton    *m_unlistedRBtn;

    friend class SmugWindow;
};

} // namespace KIPISmugExportPlugin

#endif // SMUGALBUM_H
