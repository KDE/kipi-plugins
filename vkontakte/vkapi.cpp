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

#include "vkapi.moc"

// Qt includes

#include <QPointer>

// LibKvkontakte includes

#include <libkvkontakte/authenticationdialog.h>
#include <libkvkontakte/getapplicationpermissionsjob.h>

namespace KIPIVkontaktePlugin
{

VkAPI::VkAPI(QWidget* const parent)
    : m_parent(parent),
      m_authenticated(false)
{
}

VkAPI::~VkAPI()
{
}

void VkAPI::setAppId(const QString &appId)
{
    m_appId = appId;
}

void VkAPI::setInitialAccessToken(const QString& accessToken)
{
    // Does nothing if m_accessToken is already set, because this function
    // is only for parameter initialization from a configuration file.
    if (m_accessToken.isEmpty())
        m_accessToken = accessToken;
}

QString VkAPI::accessToken() const
{
    return m_accessToken;
}

#include <stdio.h>

void VkAPI::startAuthentication(bool forceLogout)
{
    if (forceLogout)
        m_accessToken.clear();

    if (!m_accessToken.isEmpty())
    {
        Vkontakte::GetApplicationPermissionsJob* const job = new Vkontakte::GetApplicationPermissionsJob(m_accessToken);

        connect(job, SIGNAL(result(KJob*)),
                this, SLOT(slotApplicationPermissionCheckDone(KJob*)));

        job->start();
    }
    else
    {
        QStringList permissions;
        permissions << "photos" << "offline"; // TODO: rm hardcoded permission list
        QPointer<Vkontakte::AuthenticationDialog> authDialog = new Vkontakte::AuthenticationDialog(m_parent);
        authDialog->setAppId(m_appId);
        authDialog->setPermissions(permissions);

        connect(authDialog, SIGNAL(authenticated(QString)),
                this, SLOT(slotAuthenticationDialogDone(QString)));

        connect(authDialog, SIGNAL(canceled()),
                this, SLOT(slotAuthenticationDialogCanceled()));

        authDialog->start();
    }
}

void VkAPI::slotApplicationPermissionCheckDone(KJob* kjob)
{
    Vkontakte::GetApplicationPermissionsJob* const job = dynamic_cast<Vkontakte::GetApplicationPermissionsJob *>(kjob);
    Q_ASSERT(job);

    if (job && (job->error() || (job->permissions() & 4) != 4)) // TODO: rm hardcoded constant
    {
        startAuthentication(true);
    }
    else
    {
        m_authenticated = true;
        emit authenticated();
    }
}

void VkAPI::slotAuthenticationDialogCanceled()
{
}

void VkAPI::slotAuthenticationDialogDone(const QString& accessToken)
{
    m_accessToken   = accessToken;
    m_authenticated = true;
    emit authenticated();
}

bool VkAPI::isAuthenticated()
{
    return m_authenticated;
}

} // namespace KIPIVkontaktePlugin
