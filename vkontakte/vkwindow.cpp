/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to VKontakte web service
 *
 * Copyright (C) 2011, 2012 by Alexander Potashev <aspotashev at gmail dot com>
 *
 * GUI based on Yandex.Fotki KIPI Plugin
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010      by Roman Tsisyk <roman at tsisyk dot com>
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

#include "vkwindow.moc"

// Qt includes

#include <QFileInfo>
#include <QCheckBox>
#include <QGroupBox>
#include <QCloseEvent>
#include <QButtonGroup>
#include <QLabel>
#include <QSpinBox>
#include <QRadioButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>

// KDE includes

#include <kdeversion.h>
#include <kde_file.h>
#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdialog.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kmenu.h>
#include <khelpmenu.h>
#include <kmessagebox.h>
#include <kpassworddialog.h>
#include <kprogressdialog.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kio/renamedialog.h>

// libkvkontakte includes

#include <libkvkontakte/uploadphotosjob.h>
#include <libkvkontakte/getvariablejob.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpaboutdata.h"
#include "kpversion.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"

#include "vkapi.h"
#include "vkalbumdialog.h"
#include "albumchooserwidget.h"

#undef SLOT_JOB_DONE_INIT
#define SLOT_JOB_DONE_INIT(JobClass) \
    JobClass *job = dynamic_cast<JobClass *>(kjob); \
    Q_ASSERT(job);          \
    m_jobs.removeAll(job);  \
    if (job->error())       \
    {                       \
        handleVkError(job); \
        return;             \
    }

namespace KIPIVkontaktePlugin
{

VkontakteWindow::VkontakteWindow(bool import, QWidget* const parent)
    : KPToolDialog(parent)
{
    m_albumsBox = NULL;

    m_vkapi = new VkAPI(this);

    // read settings from file
    readSettings();

    connect(this, SIGNAL(finished()),
            this, SLOT(slotFinished()));

    m_import                = import;
    m_mainWidget            = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(m_mainWidget);
    m_imgList               = new KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(false); // TODO: implement conversion
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(
        i18n("This is the list of images to upload to your VKontakte album."));

    m_settingsBox = new QWidget(this);
    QVBoxLayout *settingsBoxLayout = new QVBoxLayout(m_settingsBox);

    m_headerLabel = new QLabel(m_settingsBox);
    m_headerLabel->setWhatsThis(i18n("This is a clickable link to open the "
                                     "VKontakte service in a web browser."));
    m_headerLabel->setOpenExternalLinks(true);
    m_headerLabel->setFocusPolicy(Qt::NoFocus);

    /*
     * Account box
     */
    initAccountBox();

    /*
     * Album box
     */
    m_albumsBox = new AlbumChooserWidget(this, m_vkapi);
    m_albumsBox->selectAlbum(m_albumToSelect);

    // ------------------------------------------------------------------------

    QGroupBox* uploadBox         = new QGroupBox(i18n("Destination"), m_settingsBox);
    uploadBox->setWhatsThis(i18n("This is the location where VKontakte images will be downloaded."));
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadBox);
    m_uploadWidget               = iface()->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(m_uploadWidget);

    // ------------------------------------------------------------------------

#if 0
    QGroupBox *optionsBox = new QGroupBox(i18n("Options"), m_settingsBox);
    optionsBox->setWhatsThis(
        i18n("These are options that will be applied to images before upload."));
#endif

//     m_checkKeepOriginal = new QCheckBox(i18n("Save in high resolution"), m_settingsBox); // store state in kipirc

//     QVBoxLayout *optionsBoxLayout = new QVBoxLayout(optionsBox);
//     optionsBoxLayout->addWidget(m_checkKeepOriginal);

    m_progressBar = new KPProgressWidget(m_settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    /*
     * Layouts
     */
    settingsBoxLayout->addWidget(m_headerLabel);
    settingsBoxLayout->addWidget(m_accountBox);
    settingsBoxLayout->addWidget(m_albumsBox);
    settingsBoxLayout->addWidget(uploadBox);
//     settingsBoxLayout->addWidget(optionsBox);
    settingsBoxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    settingsBoxLayout->addWidget(m_progressBar);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(m_settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    setMainWidget(m_mainWidget);
    setWindowIcon(KIcon("vkontakte"));
    setButtons(KDialog::Help | KDialog::User1 | KDialog::Close);
    setDefaultButton(Close);
    setModal(false);

    if (!m_import)
    {
        setWindowTitle(i18nc("@title:window", "Export to VKontakte Web Service"));
        setButtonGuiItem(KDialog::User1,
                         KGuiItem(i18n("Start Upload"), "network-workgroup",
                                  i18n("Start upload to VKontakte service")));
        setMinimumSize(700, 520);
        uploadBox->hide();
    }
    else
    {
        // TODO: import support
        m_imgList->hide();
//         optionsBox->hide();
    }

    KPAboutData* about = new KPAboutData(ki18n("VKontakte Plugin"),
                                         0,
                                         KAboutData::License_GPL,
                                         ki18n("A Kipi plugin to export image collections to "
                                               "VKontakte web service."),
                                         ki18n("(c) 2007-2009, Vardhman Jain\n"
                                               "(c) 2008-2010, Gilles Caulier\n"
                                               "(c) 2009, Luka Renko\n"
                                               "(c) 2010, Roman Tsisyk\n"
                                               "(c) 2011, Alexander Potashev"));

    about->addAuthor(ki18n("Alexander Potashev"), ki18n("Author"), "aspotashev@gmail.com");

    about->setHandbookEntry("VKontakte");
    setAboutData(about);

    /*
     * UI slots
     */
    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartTransfer()));

    connect(m_vkapi, SIGNAL(authenticated()),
            this, SLOT(startGetFullName()));

    connect(m_vkapi, SIGNAL(authenticated()),
            this, SLOT(startGetUserId()));

    // for startReactivation()
    connect(m_vkapi, SIGNAL(authenticated()),
            this, SLOT(show()));

    /*
     * Dialog update slots
     */
    connect(this, SIGNAL(signalUpdateBusyStatus(bool)),
            this, SLOT(updateBusyStatus(bool)));

    connect(this, SIGNAL(signalUpdateBusyStatus(bool)),
            this, SLOT(updateBusyStatus(bool)));

    connect(this, SIGNAL(signalUpdateAuthInfo()),
            this, SLOT(updateAuthInfo()));

    updateBusyStatus(true);
}

void VkontakteWindow::initAccountBox()
{
    m_accountBox = new QGroupBox(i18n("Account"), m_settingsBox);
    m_accountBox->setWhatsThis(i18n("This account is used for authentication."));

    QGridLayout* accountBoxLayout = new QGridLayout(m_accountBox);
    QLabel* loginDescLabel = new QLabel(m_accountBox);
    loginDescLabel->setText(i18n("Name:"));
    loginDescLabel->setWhatsThis(i18n("Your VKontakte login"));

    m_loginLabel = new QLabel(m_accountBox);
    m_changeUserButton = new KPushButton(
        KGuiItem(i18n("Change Account"), "system-switch-user",
                 i18n("Change VKontakte account used to authenticate")), m_accountBox);
    m_changeUserButton->hide(); // changing account does not work anyway

    accountBoxLayout->addWidget(loginDescLabel, 0, 0);
    accountBoxLayout->addWidget(m_loginLabel, 0, 1);

    accountBoxLayout->addWidget(m_changeUserButton, 1, 1);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    connect(m_changeUserButton, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));
}

VkontakteWindow::~VkontakteWindow()
{
    reset();
}

//---------------------------------------------------------------------------

void VkontakteWindow::startReactivation()
{
    m_imgList->loadImagesFromCurrentSelection();

    reset();
    startAuthentication(false); // show() will be called after that
}

void VkontakteWindow::reset()
{
    emit signalUpdateBusyStatus(false);
    emit signalUpdateAuthInfo();
}

void VkontakteWindow::updateBusyStatus(bool busy)
{
    if (m_albumsBox)
        m_albumsBox->setEnabled(!busy && m_vkapi->isAuthenticated());

    if (!busy)
    {
        setCursor(Qt::ArrowCursor);
        enableButton(User1, m_vkapi->isAuthenticated());
        setButtonGuiItem(KDialog::Close,
                         KGuiItem(i18n("Close"), "dialog-close",
                                  i18n("Close window")));
    }
    else
    {
        setCursor(Qt::WaitCursor);
        enableButton(User1, false);
        setButtonGuiItem(KDialog::Close,
                         KGuiItem(i18n("Cancel"), "dialog-cancel",
                                  i18n("Cancel current operation")));
    }
}

void VkontakteWindow::updateAuthInfo()
{
    QString loginText;
    QString urlText;

    if (m_vkapi->isAuthenticated())
    {
        loginText = m_userFullName;
        if (m_albumsBox)
            m_albumsBox->setEnabled(true);
    }
    else
    {
        loginText = i18n("Unauthorized");
        if (m_albumsBox)
            m_albumsBox->clearList();
    }

    if (m_vkapi->isAuthenticated() && m_userId != -1)
        urlText = QString("http://vkontakte.ru/albums%1").arg(m_userId);
    else
        urlText = "http://vkontakte.ru/";

    m_loginLabel->setText(QString("<b>%1</b>").arg(loginText));
    m_headerLabel->setText(
        QString("<b><h2><a href=\"%1\"><font color=\"black\">%2</font></a></h2></b>")
            .arg(urlText).arg(i18n("VKontakte")));
}

//---------------------------------------------------------------------------

void VkontakteWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("VKontakte Settings");

    m_appId         = grp.readEntry("VkAppId", "2446321");
    m_albumToSelect = grp.readEntry("SelectedAlbumId", -1);
    m_vkapi->setInitialAccessToken(grp.readEntry("AccessToken", ""));
}

void VkontakteWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("VKontakte Settings");

    grp.writeEntry("VkAppId", m_appId);

    if (!m_vkapi->accessToken().isEmpty())
        grp.writeEntry("AccessToken", m_vkapi->accessToken());

    Vkontakte::AlbumInfoPtr album = m_albumsBox->currentAlbum();

    if (album.isNull())
        grp.deleteEntry("SelectedAlbumId");
    else
        grp.writeEntry("SelectedAlbumId", album->aid());
}

//---------------------------------------------------------------------------

QString VkontakteWindow::getDestinationPath() const
{
    return m_uploadWidget->selectedImageCollection().uploadPath().path();
}

void VkontakteWindow::slotChangeUserClicked()
{
    // force authenticate window
    startAuthentication(true);
}

void VkontakteWindow::slotFinished()
{
    writeSettings();
    reset();
}

void VkontakteWindow::slotButtonClicked(int button)
{
    switch (button)
    {
        case KDialog::User1:
            slotStartTransfer();
            break;
        case KDialog::Close:
            // TODO: grab better code from picasawebexport/picasawebwindow.cpp:219
            reset();
        default:
            KDialog::slotButtonClicked(button);
    }
}

void VkontakteWindow::startAuthentication(bool forceLogout)
{
    m_userFullName.clear();
    m_userId = -1;
    if (m_albumsBox)
        m_albumsBox->setEnabled(false);

    m_vkapi->startAuthentication(forceLogout);
}

//---------------------------------------------------------------------------

void VkontakteWindow::handleVkError(KJob *kjob)
{
    KMessageBox::error(this, kjob->errorText(), i18nc("@title:window", "Request to VKontakte failed"));
}

//---------------------------------------------------------------------------

void VkontakteWindow::startGetFullName()
{
    Vkontakte::GetVariableJob* job = new Vkontakte::GetVariableJob(m_vkapi->accessToken(), 1281);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotGetFullNameDone(KJob*)));

    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotGetFullNameDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::GetVariableJob)

    m_userFullName = job->variable().toString();
    updateAuthInfo();
}

//---------------------------------------------------------------------------

void VkontakteWindow::startGetUserId()
{
    Vkontakte::GetVariableJob* job = new Vkontakte::GetVariableJob(m_vkapi->accessToken(), 1280);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotGetUserIdDone(KJob*)));

    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotGetUserIdDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::GetVariableJob)

    m_userId = job->variable().toInt();
    emit signalUpdateAuthInfo();
}

//---------------------------------------------------------------------------

void VkontakteWindow::slotStartTransfer()
{
    Vkontakte::AlbumInfoPtr album = m_albumsBox->currentAlbum();
    if (album.isNull())
    {
        // TODO: offer the user to create an album if there are no albums yet
        KMessageBox::information(this, i18n("Please select album first."));
        return;
    }

    // TODO: import support
    if (!m_import)
    {
        emit signalUpdateBusyStatus(true);

        QStringList files;
        foreach(const KUrl& url, m_imgList->imageUrls(true))
            files.append(url.toLocalFile());

        Vkontakte::UploadPhotosJob* job = new Vkontakte::UploadPhotosJob(
            m_vkapi->accessToken(), files, false /*m_checkKeepOriginal->isChecked()*/, album->aid());

        connect(job, SIGNAL(result(KJob*)),
                this, SLOT(slotPhotoUploadDone(KJob*)));

        connect(job, SIGNAL(progress(int)),
                m_progressBar, SLOT(setValue(int)));

        m_jobs.append(job);
        job->start();
    }

    m_progressBar->show();
    m_progressBar->progressScheduled(i18n("Vkontakte Export"), false, true);
    m_progressBar->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));
}

void VkontakteWindow::slotPhotoUploadDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::UploadPhotosJob)

    m_progressBar->hide();
    m_progressBar->progressCompleted();
    emit signalUpdateBusyStatus(false);
}

} // namespace KIPIVkontaktePlugin
