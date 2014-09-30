/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2014-09-30
 * Description : a plugin to export to a remote Piwigo server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
 * Copyright (C) 2010-2014 by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#ifndef PIWIGOS_H
#define PIWIGOS_H

// Qt includes

#include <QString>

namespace KWallet
{
    class Wallet;
}

namespace KIPIPiwigoExportPlugin
{

class Piwigo
{

public:

    Piwigo();
    ~Piwigo();

    QString url()      const;
    QString username() const;
    QString password() const;

    void setUrl(const QString& url);
    void setUsername(const QString& username);
    void setPassword(const QString& password);

public:

    void save();

private:

    void load();

private:

    QString          mUrl;
    QString          mUsername;
    QString          mPassword;

    KWallet::Wallet* mpWallet;
};

} // namespace KIPIPiwigoExportPlugin

#endif /* PIWIGOS_H */
