/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
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

#ifndef PICASAWEBALBUM_H
#define PICASAWEBALBUM_H

// Qt includes

#include <QRadioButton>
#include <QDateTimeEdit>

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <ktextedit.h>

// Local includes

namespace KIPIPicasawebExportPlugin
{

class PicasaWebAlbum;

class PicasawebNewAlbum : public KDialog
{
    Q_OBJECT

public:

    PicasawebNewAlbum(QWidget* parent);
    ~PicasawebNewAlbum();

    void getAlbumProperties(PicasaWebAlbum& album);

private:

    friend class PicasawebWindow;

    KLineEdit*     m_titleEdt;
    KTextEdit*     m_descEdt;
    KLineEdit*     m_locEdt;
    QDateTimeEdit* m_dtEdt;

    QRadioButton*  m_publicRBtn;
    QRadioButton*  m_unlistedRBtn;
    QRadioButton*  m_protectedRBtn;
};

} // namespace KIPIPicasawebExportPlugin

#endif // PICASAWEBALBUM_H
