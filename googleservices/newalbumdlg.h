/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

// KDE includes

#include <klocale.h>
#include <kdialog.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <ktextedit.h>

// Local includes

namespace KIPIGoogleServicesPlugin
{

class GSFolder;

class NewAlbumDlg : public KDialog
{
    Q_OBJECT

public:

    NewAlbumDlg(QWidget* const parent, const QString& serviceName);
    ~NewAlbumDlg();

    void getAlbumProperties(GSFolder& album);

private:

    KLineEdit*     m_titleEdt;
    KTextEdit*     m_descEdt;
    KLineEdit*     m_locEdt;
    QDateTimeEdit* m_dtEdt;

    QString        m_serviceName;
    QRadioButton*  m_publicRBtn;
    QRadioButton*  m_unlistedRBtn;
    QRadioButton*  m_protectedRBtn;
};

} // namespace KIPIGoogleServicesPlugin

#endif // NEWALBUMDLG_H
