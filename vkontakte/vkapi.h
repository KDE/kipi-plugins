/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : A KIPI plugin to export images to VKontakte web service.
 *
 * Copyright (C) 2011-2012 by Alexander Potashev <aspotashev at gmail dot com>
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

#ifndef VKAPI_H
#define VKAPI_H

// Qt includes

#include <QObject>

class QWidget;
class KJob;

namespace KIPIVkontaktePlugin
{

class VkAPI : public QObject
{
    Q_OBJECT

public:

    VkAPI(QWidget* const parent);
    ~VkAPI();

    void setAppId(const QString& appId);

    void setInitialAccessToken(const QString& accessToken);
    QString accessToken() const;

    // authentication
    void startAuthentication(bool forceLogout);
    bool isAuthenticated();

Q_SIGNALS:

    void authenticated();

protected Q_SLOTS:

    // authentication
    void slotApplicationPermissionCheckDone(KJob* kjob);
    void slotAuthenticationDialogDone(const QString& accessToken);
    void slotAuthenticationDialogCanceled();

private:

    QString  m_appId;
    QWidget* m_parent;
    QString  m_accessToken;
    bool     m_authenticated;
};

} // namespace KIPIVkontaktePlugin

#endif // VKAPI_H
