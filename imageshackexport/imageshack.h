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

#ifndef IMAGESHACK_H
#define IMAGESHACK_H

// Qt includes

#include <QString>

namespace KIPIImageshackExportPlugin
{

class Imageshack
{

public:

    Imageshack();
    ~Imageshack();

    bool    loggedIn()             const;
    QString registrationCode()     const;
    QString username()             const;
    QString email()                const;
    QString credits()              const;

    void setRegistrationCode(const QString& code);
    void setUsername(const QString& username);
    void setEmail(const QString& email);

    void readSettings();

    void logOut();

private:

    void saveSettings();

private:

    bool    m_loggedIn;

    QString m_registrationCode;
    QString m_username;
    QString m_email;
    QString m_credits;

    friend class ImageshackWindow;
    friend class ImageshackWidget;
    friend class ImageshackTalker;

}; // Imageshack class

} // KIPIImageshackExportPlugin

#endif // IMAGESHACK_H
