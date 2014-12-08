/* ============================================================
*
* This file is a part of kipi-plugins project
* http://www.digikam.org
*
* Date        : 2012-02-02
* Description : a plugin to export photos or videos to ImageShack web service
*
* Copyright (C) 2012 Dodon Victor <dodonvictor at gmail dot com>
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

#include "imageshack.h"

// Qt includes

#include <QString>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <klocale.h>

namespace KIPIImageshackExportPlugin
{

Imageshack::Imageshack()
{
    readSettings();
    m_loggedIn = false;
}

Imageshack::~Imageshack()
{
}

bool Imageshack::loggedIn() const
{
    return m_loggedIn;
}

QString Imageshack::registrationCode() const
{
    return m_registrationCode;
}

QString Imageshack::username() const
{
    return m_username;
}

QString Imageshack::email() const
{
    return m_email;
}

QString Imageshack::credits() const
{
    return m_credits;
}

void Imageshack::setRegistrationCode(const QString& code)
{
    m_registrationCode = code;
}

void Imageshack::setUsername(const QString& username)
{
    m_username = username;
}

void Imageshack::setEmail(const QString& email)
{
    m_email = email;
}

void Imageshack::logOut()
{
    m_loggedIn = false;
    m_registrationCode.clear();
    m_username.clear();
    m_email.clear();
    m_credits.clear();
    saveSettings();
}

void Imageshack::readSettings()
{
    static bool bLoaded = false;
    if (bLoaded) return;
    bLoaded = true;

    KConfig config("kipirc");
    KConfigGroup group = config.group("Imageshack Settings");

    m_registrationCode = group.readEntry("RegistrationCode", QString());
}

void Imageshack::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Imageshack Settings");

    group.writeEntry(QString("RegistrationCode"), registrationCode());
    config.sync();
}

} // namespace KIPIImageshackExportPlugin
