/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2016-06-06
 * Description : a kipi plugin to export images to the Imgur web service
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
 * Copyright (C) 2016 by Fabian Vogt <fabian at ritter dash vogt dot de>
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

#include "imgurwindow.h"

// Qt includes

#include <QBoxLayout>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QInputDialog>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>
#include <kconfig.h>
#include <kwindowconfig.h>

// Local includes

#include "kipiplugins_debug.h"
#include "kpimageinfo.h"
#include "kpaboutdata.h"
#include "kpversion.h"

static const constexpr char *IMGUR_CLIENT_ID("575411e9c939b2b"),
                            *IMGUR_CLIENT_SECRET("f39bbddf4052062735bdc7168b33aa795cab361f");

namespace KIPIImgurPlugin
{

ImgurWindow::ImgurWindow(QWidget* const /*parent*/)
    : KPToolDialog(0)
{
    api = new ImgurAPI3(QString::fromLatin1(IMGUR_CLIENT_ID),
                        QString::fromLatin1(IMGUR_CLIENT_SECRET), this);
    
    /* Connect API signals */
    connect(api, &ImgurAPI3::authorized, this, &ImgurWindow::apiAuthorized);
    connect(api, &ImgurAPI3::authError, this, &ImgurWindow::apiAuthError);
    connect(api, &ImgurAPI3::progress, this, &ImgurWindow::apiProgress);
    connect(api, &ImgurAPI3::requestPin, this, &ImgurWindow::apiRequestPin);
    connect(api, &ImgurAPI3::success, this, &ImgurWindow::apiSuccess);
    connect(api, &ImgurAPI3::error, this, &ImgurWindow::apiError);
    connect(api, &ImgurAPI3::busy, this, &ImgurWindow::apiBusy);
    
    /* | List | Auth | */
    auto *mainLayout = new QHBoxLayout;
    auto *mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    this->setMainWidget(mainWidget);
    
    this->list = new ImgurImagesList;
    mainLayout->addWidget(list);
    
    /* |  Logged in as:  |
     * | <Not logged in> |
     * |     Forget      | */

    auto *userLabelLabel = new QLabel(i18n("Logged in as:"));
    userLabelLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    userLabelLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    
    this->userLabel = new QLabel; /* Label set in readSettings() */
    userLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    userLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    forgetButton = new QPushButton(i18n("Forget"));

    auto *authLayout = new QVBoxLayout;
    mainLayout->addLayout(authLayout);
    authLayout->addWidget(userLabelLabel);
    authLayout->addWidget(userLabel);
    authLayout->addWidget(forgetButton);
    authLayout->insertStretch(-1, 1);

    /* Add anonymous upload button */
    uploadAnonButton = new QPushButton(i18n("Upload Anonymously"));
    addButton(uploadAnonButton, QDialogButtonBox::ApplyRole);
    
    /* Connect UI signals */
    connect(forgetButton, &QPushButton::clicked,
            this, &ImgurWindow::forgetButtonClicked);
    connect(startButton(), &QPushButton::clicked,
            this, &ImgurWindow::slotUpload);
    connect(uploadAnonButton, &QPushButton::clicked,
            this, &ImgurWindow::slotAnonUpload);
    connect(this, &ImgurWindow::finished,
            this, &ImgurWindow::slotFinished);
    connect(this, &ImgurWindow::cancelClicked,
            this, &ImgurWindow::slotCancel);
    
    setWindowIcon(QIcon::fromTheme(QString::fromLatin1("kipi-imgur")));
    setWindowTitle(i18n("Export to imgur.com"));
    setModal(false);

    startButton()->setText(i18n("Upload"));
    startButton()->setToolTip(i18n("Start upload to Imgur"));
    startButton()->setEnabled(true);

    /* Add about data */

    KPAboutData* const about = new KPAboutData(ki18n("Imgur Export"),
                                   0,
                                   KAboutLicense::GPL,
                                   ki18n("A tool to export images to Imgur web service"),
                                   ki18n("(c) 2012-2013, Marius Orcsik"));

    about->addAuthor(ki18n("Marius Orcsik").toString(),
                     ki18n("Author").toString(),
                     QString::fromLatin1("marius at habarnam dot ro"));

    about->addAuthor(ki18n("Gilles Caulier").toString(),
                     ki18n("Developer").toString(),
                     QString::fromLatin1("caulier dot gilles at gmail dot com"));

    about->addAuthor(ki18n("Fabian Vogt").toString(),
                     ki18n("Developer").toString(),
                     QString::fromLatin1("fabian at ritter dash vogt dot de"));

    about->setHandbookEntry(QString::fromLatin1("imgur"));
    setAboutData(about);

    /* Only used if not overwritten by readSettings() */
    resize(650, 320);
    readSettings();
}

ImgurWindow::~ImgurWindow()
{
    saveSettings();
}

void ImgurWindow::reactivate()
{
    list->loadImagesFromCurrentSelection();
    show();
}

void ImgurWindow::forgetButtonClicked()
{
    api->getAuth().unlink();

    apiAuthorized(false, {});
}

void ImgurWindow::slotUpload()
{
    QList<const ImgurImageListViewItem*> pending = this->list->getPendingItems();
    for(auto item : pending)
    {
        ImgurAPI3Action action;
        action.type = ImgurAPI3ActionType::IMG_UPLOAD;
        action.upload.imgpath = item->url().toLocalFile();
        action.upload.title = item->Title();
        action.upload.description = item->Description();
        
        api->queueWork(action);
    }
}

void ImgurWindow::slotAnonUpload()
{
    QList<const ImgurImageListViewItem*> pending = this->list->getPendingItems();
    for(auto item : pending)
    {
        ImgurAPI3Action action;
        action.type = ImgurAPI3ActionType::ANON_IMG_UPLOAD;
        action.upload.imgpath = item->url().toLocalFile();
        action.upload.title = item->Title();
        action.upload.description = item->Description();
        
        api->queueWork(action);
    }
}

void ImgurWindow::slotFinished()
{
    saveSettings();
}

void ImgurWindow::slotCancel()
{
    api->cancelAllWork();
}

void ImgurWindow::apiAuthorized(bool success, const QString &username)
{
    if(success)
    {
        this->username = username;
        this->userLabel->setText(this->username);
        this->forgetButton->setEnabled(true);
        return;
    }
    
    this->username = QString();
    this->userLabel->setText(i18n("<Not logged in>"));
    this->forgetButton->setEnabled(false);
}

void ImgurWindow::apiAuthError(const QString &msg)
{
    QMessageBox::critical(this,
                          i18n("Authorization Failed"),
                          i18n("Failed to log into Imgur: %1\n", msg));
}


void ImgurWindow::apiProgress(unsigned int /*percent*/, const ImgurAPI3Action &action)
{
    list->processing(QUrl::fromLocalFile(action.upload.imgpath));
}

void ImgurWindow::apiRequestPin(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

void ImgurWindow::apiSuccess(const ImgurAPI3Result &result)
{
    list->slotSuccess(result);
}

void ImgurWindow::apiError(const QString &msg, const ImgurAPI3Action &action)
{
    list->processed(QUrl::fromLocalFile(action.upload.imgpath), false);
    
    /* 1 here because the current item is still in the queue. */
    if(api->workQueueLength() <= 1)
    {
        QMessageBox::critical(this,
                              i18n("Uploading Failed"),
                              i18n("Failed to upload photo to Imgur: %1\n", msg));
        return;
    }

    QMessageBox::StandardButton cont =
            QMessageBox::question(this,
                                  i18n("Uploading Failed"),
                                  i18n("Failed to upload photo to Imgur: %1\n"
                                       "Do you want to continue?", msg));

    if(cont != QMessageBox::Yes)
        api->cancelAllWork();
}

void ImgurWindow::apiBusy(bool busy)
{
    setCursor(busy ? Qt::WaitCursor : Qt::ArrowCursor);
    startButton()->setEnabled(!busy);
}

void ImgurWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
        return;

    slotFinished();
    e->accept();
}

void ImgurWindow::readSettings()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup groupAuth = config.group("Imgur Auth");
    username = groupAuth.readEntry("username", QString());
    apiAuthorized(!username.isEmpty(), username);

    KConfigGroup groupDialog = config.group("Imgur Dialog");
    KWindowConfig::restoreWindowSize(windowHandle(), groupDialog);
}

void ImgurWindow::saveSettings()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup groupAuth = config.group("Imgur Auth");
    groupAuth.writeEntry("username", username);

    KConfigGroup groupDialog = config.group("Imgur Dialog");
    KWindowConfig::saveWindowSize(windowHandle(), groupDialog);
    config.sync();
}

} // namespace KIPIImgurPlugin
