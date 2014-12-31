/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-14-09
 * Description : Kipi-Plugins shared library.
 *
 * Copyright (C) 2006-2010 by Angelo Naselli <anaselli at linux dot it>
 * Copyright (C) 2010-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <QObject>

// KDE includes

#include <kaboutdata.h>

// Local includes

#include "kipiplugins_export.h"

class KPushButton;

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPAboutData : public QObject, public KAboutData
{
    Q_OBJECT

public:

    explicit KPAboutData(const KLocalizedString& pluginName,
                         const QByteArray& pluginVersion            = QByteArray(),
                         enum  LicenseKey licenseType               = License_Unknown,
                         const KLocalizedString& pluginDescription  = KLocalizedString(),
                         const KLocalizedString& copyrightStatement = ki18n("Copyright 2003-2015, digiKam developers team"));

    KPAboutData(const KPAboutData& other);
    virtual ~KPAboutData();

    void setHandbookEntry(const QString& entry);
    void setHelpButton(KPushButton* const help);

private Q_SLOTS:

    void slotHelp();

private:

    QString m_handbookEntry;
};

} // namespace KIPIPlugins

#endif //KPABOUTDATA_H
