/*
 * A KIPI plugin to export images to VKontakte web service
 * Copyright (C) 2011, 2012
 * Alexander Potashev <aspotashev at gmail dot com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef AUTHINFOWIDGET_H
#define AUTHINFOWIDGET_H

#include <QGroupBox>

class QLabel;

class KPushButton;
class KJob;

namespace KIPIVkontaktePlugin
{

class VkAPI;


class AuthInfoWidget : public QGroupBox
{
    Q_OBJECT

public:

    AuthInfoWidget(QWidget *parent, VkAPI *vkapi);
    ~AuthInfoWidget();

    QString albumsURL() const;

Q_SIGNALS:

    void authCleared();

    void signalUpdateAuthInfo();

public Q_SLOTS:

    void startAuthentication(bool forceLogout);

protected Q_SLOTS:

    void slotChangeUserClicked();

    void updateAuthInfo();

    void startGetUserId();
    void startGetFullName();
    void slotGetUserIdDone(KJob *kjob);
    void slotGetFullNameDone(KJob *kjob);

protected:

    void handleVkError(KJob *kjob);

private:

    // VK.com interface
    VkAPI *m_vkapi;

    // Data
    int m_userId;
    QString m_userFullName;

    // GUI
    QLabel *m_loginLabel;
    KPushButton *m_changeUserButton;
};

} // namespace KIPIVkontaktePlugin

#endif // AUTHINFOWIDGET_H
