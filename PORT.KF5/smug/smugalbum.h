/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-07
 * Description : a kipi plugin to import/export images to/from 
                 SmugMug web service
 *
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

// KDE includes

#include <kdialog.h>

class QGroupBox;
class QRadioButton;
class KComboBox;
class KLineEdit;
class KTextEdit;

namespace KIPISmugPlugin
{

class SmugAlbum;

class SmugNewAlbum : public KDialog
{
    Q_OBJECT

public:

    SmugNewAlbum(QWidget* parent);
    ~SmugNewAlbum();

    void getAlbumProperties(SmugAlbum& album);

private:

    KLineEdit*    m_titleEdt;
    KTextEdit*    m_descEdt;
    KComboBox*    m_categCoB;
    KComboBox*    m_subCategCoB;
    KComboBox*    m_templateCoB;

    QGroupBox*    m_privBox;
    KLineEdit*    m_passwdEdt;
    KLineEdit*    m_hintEdt;
    QRadioButton* m_publicRBtn;
    QRadioButton* m_unlistedRBtn;

    friend class SmugWindow;
};

} // namespace KIPISmugPlugin

#endif // SMUGALBUM_H
