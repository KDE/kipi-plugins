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

#include "authinfowidget.moc"

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

#include <klocalizedstring.h>
#include <kdialog.h>
#include <kpushbutton.h>
#include <kmessagebox.h>

#include <libkvkontakte/getvariablejob.h>

#include "vkapi.h"

// TODO: share this code with `vkwindow.cpp`
#define SLOT_JOB_DONE_INIT(JobClass) \
    JobClass *job = dynamic_cast<JobClass *>(kjob); \
    Q_ASSERT(job);          \
    if (job->error())       \
    {                       \
        handleVkError(job); \
        return;             \
    }

namespace KIPIVkontaktePlugin
{

AuthInfoWidget::AuthInfoWidget(QWidget *parent, VkAPI *vkapi)
    : QGroupBox(i18n("Account"), parent)
{
    m_vkapi = vkapi;

    setWhatsThis(i18n("This account is used for authentication."));

    QGridLayout* accountBoxLayout = new QGridLayout(this);
    QLabel* loginDescLabel = new QLabel(this);
    loginDescLabel->setText(i18n("Name:"));
    loginDescLabel->setWhatsThis(i18n("Your VKontakte login"));

    m_loginLabel = new QLabel(this);
    m_changeUserButton = new KPushButton(
        KGuiItem(i18n("Change Account"), "system-switch-user",
                 i18n("Change VKontakte account used to authenticate")), this);
    m_changeUserButton->hide(); // changing account does not work anyway

    accountBoxLayout->addWidget(loginDescLabel, 0, 0);
    accountBoxLayout->addWidget(m_loginLabel, 0, 1);

    accountBoxLayout->addWidget(m_changeUserButton, 1, 1);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    connect(m_changeUserButton, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));

    connect(m_vkapi, SIGNAL(authenticated()),
            this, SLOT(startGetFullName()));

    connect(m_vkapi, SIGNAL(authenticated()),
            this, SLOT(startGetUserId()));

    connect(this, SIGNAL(signalUpdateAuthInfo()),
            this, SLOT(updateAuthInfo()));
}

AuthInfoWidget::~AuthInfoWidget()
{
}

//---------------------------------------------------------------------------

void AuthInfoWidget::startAuthentication(bool forceLogout)
{
    m_userFullName.clear();
    m_userId = -1;

    m_vkapi->startAuthentication(forceLogout);
    emit authCleared();
}

void AuthInfoWidget::slotChangeUserClicked()
{
    // force authenticate window
    startAuthentication(true);
}

//---------------------------------------------------------------------------

void AuthInfoWidget::startGetUserId()
{
    Vkontakte::GetVariableJob* job = new Vkontakte::GetVariableJob(m_vkapi->accessToken(), 1280);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotGetUserIdDone(KJob*)));

    job->start();
}

void AuthInfoWidget::slotGetUserIdDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::GetVariableJob)

    m_userId = job->variable().toInt();
    emit signalUpdateAuthInfo();
}

//---------------------------------------------------------------------------

void AuthInfoWidget::startGetFullName()
{
    Vkontakte::GetVariableJob* job = new Vkontakte::GetVariableJob(m_vkapi->accessToken(), 1281);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotGetFullNameDone(KJob*)));

    job->start();
}

void AuthInfoWidget::slotGetFullNameDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::GetVariableJob)

    m_userFullName = job->variable().toString();
    emit signalUpdateAuthInfo();
}

//---------------------------------------------------------------------------

void AuthInfoWidget::updateAuthInfo()
{
    QString loginText;

    if (m_vkapi->isAuthenticated())
    {
        loginText = m_userFullName;
    }
    else
    {
        loginText = i18n("Unauthorized");
    }

    m_loginLabel->setText(QString("<b>%1</b>").arg(loginText));
}

// TODO: share this code with `vkwindow.cpp`
void AuthInfoWidget::handleVkError(KJob *kjob)
{
    KMessageBox::error(this, kjob->errorText(), i18nc("@title:window", "Request to VKontakte failed"));
}

//---------------------------------------------------------------------------

QString AuthInfoWidget::albumsURL() const
{
    if (m_vkapi->isAuthenticated() && m_userId != -1)
        return QString("http://vkontakte.ru/albums%1").arg(m_userId);
    else
        return QLatin1String("http://vkontakte.ru/");
}

} // namespace KIPIVkontaktePlugin
