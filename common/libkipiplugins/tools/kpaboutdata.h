/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins shared library.
 *
 * Copyright (C) 2006-2010 by Angelo Naselli <anaselli at linux dot it>
 * Copyright (C) 2010-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPABOUTDATA_H
#define KPABOUTDATA_H

// Qt includes

#include <QObject>
#include <QList>
#include <QStringList>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "kipiplugins_export.h"

class QPushButton;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPAboutData : public QObject
{
    Q_OBJECT

public:

    explicit KPAboutData(const KLocalizedString& tool,
                         const KLocalizedString& description = KLocalizedString(),
                         const KLocalizedString& copyright = ki18n("Copyright 2003-2016, digiKam developers team"));

    KPAboutData(const KPAboutData& other);
    virtual ~KPAboutData();

    void setHandbookEntry(const QString& entry);
    void setHelpButton(QPushButton* const help);
    void addAuthor(const QString& name, const QString& role, const QString& email);

private Q_SLOTS:

    void slotHelp();
    void slotAbout();

private:

    QString     m_tool;
    QString     m_description;
    QString     m_copyright;
    QString     m_handbookEntry;
    QStringList m_authors;
};

} // namespace KIPIPlugins

#endif // KPABOUTDATA_H
