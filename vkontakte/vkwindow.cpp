/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to VKontakte web service
 *
 * Copyright (C) 2011 by Alexander Potashev <aspotashev at gmail dot com>
 *
 * GUI based on Yandex.Fotki KIPI Plugin
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009 by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
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

#include "vkwindow.h"

// Qt includes

#include <QtCore/QFileInfo>
#include <QtGui/QSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QCloseEvent>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QButtonGroup>
#include <QtGui/QRadioButton>
#include <QtGui/QProgressBar>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

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

#include <libkvkontakte/getapplicationpermissionsjob.h>
#include <libkvkontakte/authenticationdialog.h>
#include <libkvkontakte/albumlistjob.h>
#include <libkvkontakte/createalbumjob.h>
#include <libkvkontakte/uploadphotosjob.h>
#include <libkvkontakte/getvariablejob.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// LibKIPI includes

#include <kpaboutdata.h>
#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>
#include "pluginsversion.h"

// Local includes

#include "imageslist.h"
#include "vkalbumdialog.h"
#include "vkwindow.h"

namespace KIPIVkontaktePlugin
{

VkontakteWindow::VkontakteWindow(KIPI::Interface *interface,
                                 bool import, QWidget *parent)
    : KDialog(parent)
{
    m_authenticated = false;

    m_interface   = interface;
    m_import      = import;

    m_mainWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(m_mainWidget);

    m_imgList  = new KIPIPlugins::ImagesList(interface, this);
    m_imgList->setControlButtonsPlacement(KIPIPlugins::ImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(false); // TODO: implement conversion
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(
        i18n("This is the list of images to upload to your VKontakte album."));

    QWidget* settingsBox           = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    m_headerLabel = new QLabel(settingsBox);
    m_headerLabel->setWhatsThis(i18n("This is a clickable link to open the "
                                     "VKontakte service in a web browser."));
    m_headerLabel->setOpenExternalLinks(true);
    m_headerLabel->setFocusPolicy(Qt::NoFocus);

    /*
     * Account box
     */
    m_accountBox = new QGroupBox(i18n("Account"), settingsBox);
    m_accountBox->setWhatsThis(i18n("This account is used for authentication."));

    QGridLayout* accountBoxLayout = new QGridLayout(m_accountBox);
    QLabel* loginDescLabel = new QLabel(m_accountBox);
    loginDescLabel->setText(i18n("Name:"));
    loginDescLabel->setWhatsThis(i18n("Your VKontakte login"));

    m_loginLabel = new QLabel(m_accountBox);
    m_changeUserButton = new KPushButton(
        KGuiItem(i18n("Change Account"), "system-switch-user",
                 i18n("Change VKontakte account used to authenticate")), m_accountBox);

    accountBoxLayout->addWidget(loginDescLabel, 0, 0);
    accountBoxLayout->addWidget(m_loginLabel, 0, 1);

    accountBoxLayout->addWidget(m_changeUserButton, 1, 1);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    connect(m_changeUserButton, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));

    /*
     * Album box
     */
    m_albumsBox = new QGroupBox(i18n("Album"), settingsBox);
    m_albumsBox->setWhatsThis(
        i18n("This is the VKontakte album that will be used for the transfer."));
    QGridLayout* albumsBoxLayout  = new QGridLayout(m_albumsBox);

    m_albumsCombo = new KComboBox(m_albumsBox);
    m_albumsCombo->setEditable(false);

    m_newAlbumButton = new KPushButton(
        KGuiItem(i18n("New Album"), "list-add",
                 i18n("Create new VKontakte album")), m_albumsBox);
    m_reloadAlbumsButton = new KPushButton(
        KGuiItem(i18nc("reload albums list", "Reload"), "view-refresh",
                 i18n("Reload albums list")), m_albumsBox);

    albumsBoxLayout->addWidget(m_albumsCombo, 0, 0, 1, 5);
    albumsBoxLayout->addWidget(m_newAlbumButton, 1, 3, 1, 1);
    albumsBoxLayout->addWidget(m_reloadAlbumsButton, 1, 4, 1, 1);

    connect(m_newAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotNewAlbumRequest()));


    connect(m_reloadAlbumsButton, SIGNAL(clicked()),
            this, SLOT(slotReloadAlbumsRequest()));

    // ------------------------------------------------------------------------

    QGroupBox* uploadBox = new QGroupBox(i18n("Destination"), settingsBox);
    uploadBox->setWhatsThis(
        i18n("This is the location where VKontakte images will be downloaded."));
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadBox);
    m_uploadWidget = interface->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(m_uploadWidget);

    // ------------------------------------------------------------------------

    QGroupBox* optionsBox = new QGroupBox(i18n("Options"), settingsBox);
    optionsBox->setWhatsThis(
        i18n("These are options that will be applied to images before upload."));

    m_progressBar = new QProgressBar(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();


    /*
     * Layouts
     */
    settingsBoxLayout->addWidget(m_headerLabel);
    settingsBoxLayout->addWidget(m_accountBox);
    settingsBoxLayout->addWidget(m_albumsBox);
    settingsBoxLayout->addWidget(uploadBox);
    settingsBoxLayout->addWidget(optionsBox);
    settingsBoxLayout->addWidget(m_progressBar);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    mainLayout->addWidget(m_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    setMainWidget(m_mainWidget);
    setWindowIcon(KIcon("vkontakte"));
    setButtons(KDialog::Help|KDialog::User1|KDialog::Close);
    setDefaultButton(Close);
    setModal(false);


    if (!m_import)
    {
        setWindowTitle(i18n("Export to VKontakte Web Service"));
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
        m_newAlbumButton->hide();
        optionsBox->hide();
    }

    KIPIPlugins::KPAboutData* about = new KIPIPlugins::KPAboutData(
        ki18n("VKontakte Plugin"),
        0,
        KAboutData::License_GPL,
        ki18n("A Kipi plugin to export image collections to "
              "VKontakte web service."),
        ki18n( "(c) 2007-2009, Vardhman Jain\n"
               "(c) 2008-2010, Gilles Caulier\n"
               "(c) 2009, Luka Renko\n"
               "(c) 2010, Roman Tsisyk\n"
               "(c) 2011, Alexander Potashev" )
    );

    about->addAuthor(ki18n( "Alexander Potashev" ), ki18n("Author"),
                     "aspotashev@gmail.com");

    disconnect(this, SIGNAL(helpClicked()), this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction* handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)), this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    /*
     * UI slots
     */
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotStartTransfer()));


    connect(this, SIGNAL(signalAuthenticationDone()), this, SLOT(startAlbumsUpdate()));
    connect(this, SIGNAL(signalAuthenticationDone()), this, SLOT(startGetFullName()));
    connect(this, SIGNAL(signalAuthenticationDone()), this, SLOT(startGetUserId()));
    connect(this, SIGNAL(signalAuthenticationDone()), this, SLOT(updateLabels()));

    // for startReactivation()
    connect(this, SIGNAL(signalAuthenticationDone()), this, SLOT(show()));

    // read settings from file
    //readSettings();
}

VkontakteWindow::~VkontakteWindow()
{
    reset();
}

void VkontakteWindow::startReactivation()
{
    m_imgList->loadImagesFromCurrentSelection();

    reset();
    startAuthentication(false); // show() will be called after that
}

void VkontakteWindow::reset()
{
    //m_talker.reset();
    updateControls(true);
    updateLabels();
}

void VkontakteWindow::updateControls(bool val)
{
    if (val)
    {
        if (isAuthenticated())
        {
            m_albumsBox->setEnabled(true);
            enableButton(User1, true);
        }
        else
        {
            m_albumsBox->setEnabled(false);
            enableButton(User1, false);
        }

        m_changeUserButton->setEnabled(true);
        setCursor(Qt::ArrowCursor);

        setButtonGuiItem(KDialog::Close,
                         KGuiItem(i18n("Close"), "dialog-close",
                                  i18n("Close window")));
    }
    else
    {
        setCursor(Qt::WaitCursor);
        m_albumsBox->setEnabled(false);
        m_changeUserButton->setEnabled(false);
        enableButton(User1, false);

        setButtonGuiItem(KDialog::Close,
                         KGuiItem(i18n("Cancel"), "dialog-cancel",
                                  i18n("Cancel current operation")));
    }
}

void VkontakteWindow::updateLabels()
{
    QString loginText;
    QString urlText;

    if (isAuthenticated())
    {
        loginText = m_userFullName;
        m_albumsBox->setEnabled(true);
    }
    else
    {
        loginText = i18n("Unauthorized");
        m_albumsCombo->clear();
    }

    if (isAuthenticated() && m_userId != -1)
        urlText = QString("http://vkontakte.ru/albums%1").arg(m_userId);
    else
        urlText = "http://vkontakte.ru/";

    m_loginLabel->setText(QString("<b>%1</b>").arg(loginText));
    m_headerLabel->setText(
        QString("<b><h2><a href=\"%1\"> <font color=\"black\">%2</font> </a></h2></b>")
            .arg(urlText).arg(i18n("VKontakte")));
}

// void VkontakteWindow::readSettings()
// {
//     KConfig config("kipirc");
//     KConfigGroup grp = config.group( "YandexFotki Settings");
//
//     // TODO: use kwallet ??
//     m_talker.setLogin(grp.readEntry("login", ""));
//     // don't store tokens in plaintext
//     //m_talker.setToken(grp.readEntry("token", ""));
//
//     if (grp.readEntry("Resize", false))
//     {
//         m_resizeCheck->setChecked(true);
//         m_dimensionSpin->setEnabled(true);
//         m_imageQualitySpin->setEnabled(true);
//     }
//     else
//     {
//         m_resizeCheck->setChecked(false);
//         m_dimensionSpin->setEnabled(false);
//         m_imageQualitySpin->setEnabled(false);
//     }
//
//     m_dimensionSpin->setValue(grp.readEntry("Maximum Width", 1600));
//     m_imageQualitySpin->setValue(grp.readEntry("Image Quality", 85));
//     m_policyGroup->button(grp.readEntry("Sync policy", 0))->setChecked(true);
// }

// void VkontakteWindow::writeSettings()
// {
//     KConfig config("kipirc");
//     KConfigGroup grp = config.group("VKontakte Settings");
//
//     // TODO: user kwallet ??
//     grp.writeEntry("token", m_talker.token());
//     // don't store tokens in plaintext
//     //grp.writeEntry("login", m_talker.login());
//
//     grp.writeEntry("Resize", m_resizeCheck->isChecked());
//     grp.writeEntry("Maximum Width", m_dimensionSpin->value());
//     grp.writeEntry("Image Quality", m_imageQualitySpin->value());
//     grp.writeEntry("Sync policy", m_policyGroup->checkedId());
// }


QString VkontakteWindow::getDestinationPath() const
{
    return m_uploadWidget->selectedImageCollection().uploadPath().path();
}

void VkontakteWindow::slotChangeUserClicked()
{
    // force authenticate window
    startAuthentication(true);
}

void VkontakteWindow::slotCloseEvent(QCloseEvent* event)
{
    kDebug() << "closeEvent";
    //writeSettings();
    reset();
    event->accept();
}

void VkontakteWindow::slotButtonClicked(int button)
{
    switch (button)
    {
        case KDialog::User1:
            slotStartTransfer();
            break;
        case KDialog::Close:
            if (!isButtonEnabled(KDialog::User1))
            {
                //m_talker.cancel();
                updateControls(true);
                break;
            }
        default:
            KDialog::slotButtonClicked(button);
    }
}

void VkontakteWindow::slotHelp()
{
    KToolInvocation::invokeHelp("VKontakte", "kipi-plugins");
}

void VkontakteWindow::startAuthentication(bool forceAuthWindow)
{
    m_userFullName = QString();
    m_userId = -1;

    if (forceAuthWindow)
        m_accessToken = QString();

    if (!m_accessToken.isEmpty())
    {
        GetApplicationPermissionsJob *job = new GetApplicationPermissionsJob(m_accessToken);
        connect(job, SIGNAL(result(KJob*)), this, SLOT(slotApplicationPermissionCheckDone(KJob*)));
        m_jobs.append(job);
        job->start();
    }
    else
    {
        QStringList permissions;
        permissions << "photos" << "offline";
        AuthenticationDialog *authDialog = new AuthenticationDialog(this);
        authDialog->setAppId(/*Settings::self()->appID()*/ QString("2383230")); // TODO: this was for Akonadi app, create a new app for KIPI-Plugins
        authDialog->setPermissions(permissions);
        connect(authDialog, SIGNAL(authenticated(QString)),
                this, SLOT(slotAuthenticationDialogDone(QString)));
        connect(authDialog, SIGNAL(canceled()),
                this, SLOT(slotAuthenticationDialogCanceled()));
        authDialog->start();
    }
}

void VkontakteWindow::slotApplicationPermissionCheckDone(KJob *kjob)
{
    GetApplicationPermissionsJob *job = dynamic_cast<GetApplicationPermissionsJob *>(kjob);
    Q_ASSERT(job);
    m_jobs.removeAll(job);
    if (job->error() || (job->permissions() & 4) != 4)
        startAuthentication(true);
    else
    {
        m_authenticated = true;
        emit signalAuthenticationDone();
    }
}

void VkontakteWindow::slotAuthenticationDialogCanceled()
{
}

void VkontakteWindow::slotAuthenticationDialogDone(const QString &accessToken)
{
    m_accessToken = accessToken;
    m_authenticated = true;
    emit signalAuthenticationDone();
}

//------------------------------

void VkontakteWindow::handleVkError(KJob *kjob)
{
    KMessageBox::error(this, kjob->errorText(), i18n("Request to VKontakte failed"));
}

void VkontakteWindow::startAlbumsUpdate()
{
    AlbumListJob *job = new AlbumListJob(m_accessToken);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(slotAlbumsUpdateDone(KJob*)));
    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotAlbumsUpdateDone(KJob *kjob)
{
    AlbumListJob *job = dynamic_cast<AlbumListJob *>(kjob);
    Q_ASSERT(job);
    m_jobs.removeAll(job);
    if (job->error())
    {
        handleVkError(job);
        return;
    }

    m_albumsCombo->clear();
    m_albums = job->list();
    foreach (const AlbumInfoPtr &album, m_albums)
    {
        m_albumsCombo->addItem(KIcon("folder-image"), album->title());
    }
    m_albumsCombo->setEnabled(true);
    updateControls(true);
}

//------------------------------

void VkontakteWindow::startGetFullName()
{
    GetVariableJob *job = new GetVariableJob(m_accessToken, 1281);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(slotGetFullNameDone(KJob*)));
    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotGetFullNameDone(KJob *kjob)
{
    GetVariableJob *job = dynamic_cast<GetVariableJob *>(kjob);
    Q_ASSERT(job);
    m_jobs.removeAll(job);
    if (job->error())
    {
        handleVkError(job);
        return;
    }

    m_userFullName = job->variable().toString();
    updateLabels();
}

//------------------------------

void VkontakteWindow::startGetUserId()
{
    GetVariableJob *job = new GetVariableJob(m_accessToken, 1280);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(slotGetUserIdDone(KJob*)));
    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotGetUserIdDone(KJob *kjob)
{
    GetVariableJob *job = dynamic_cast<GetVariableJob *>(kjob);
    Q_ASSERT(job);
    m_jobs.removeAll(job);
    if (job->error())
    {
        handleVkError(job);
        return;
    }

    m_userId = job->variable().toInt();
    updateLabels();
}

//------------------------------

void VkontakteWindow::startAlbumCreation(AlbumInfoPtr album)
{
    CreateAlbumJob *job = new CreateAlbumJob(
        m_accessToken,
        album->title(), album->description(),
        album->privacy(), album->commentPrivacy());
    connect(job, SIGNAL(result(KJob*)), this, SLOT(slotAlbumCreationDone(KJob*)));
    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotAlbumCreationDone(KJob *kjob)
{
    CreateAlbumJob *job = dynamic_cast<CreateAlbumJob *>(kjob);
    Q_ASSERT(job);
    m_jobs.removeAll(job);
    if (job->error())
    {
        handleVkError(job);
        return;
    }

    // TODO: automatically select the newly created album in the combobox

    startAlbumsUpdate();
    m_albumsCombo->setEnabled(false);
    updateControls(false);
}

void VkontakteWindow::slotNewAlbumRequest()
{
    AlbumInfoPtr album(new AlbumInfo());
    VkontakteAlbumDialog dlg(this, album);

    if (dlg.exec() == QDialog::Accepted)
    {
        updateControls(false);
        startAlbumCreation(album);
    }
}

void VkontakteWindow::slotReloadAlbumsRequest()
{
    updateControls(false);
    startAlbumsUpdate();
}

void VkontakteWindow::slotStartTransfer()
{
    if (m_albumsCombo->currentIndex() == -1 || m_albumsCombo->count() == 0)
    {
        KMessageBox::information(this, i18n("Please select album first"));
        return;
    }

    // TODO: import support
    if (!m_import)
    {
        // list photos of the album, then start upload
        AlbumInfoPtr album = m_albums.at(m_albumsCombo->currentIndex());

        updateControls(false);

        QStringList files;
        foreach (const KUrl &url, m_imgList->imageUrls(true))
            files.append(url.toLocalFile());

        UploadPhotosJob *job = new UploadPhotosJob(m_accessToken, files, album->aid());
        connect(job, SIGNAL(result(KJob*)), this, SLOT(slotPhotoUploadDone(KJob*)));
        m_jobs.append(job);
        job->start();
    }
}

void VkontakteWindow::slotPhotoUploadDone(KJob *kjob)
{
    UploadPhotosJob *job = dynamic_cast<UploadPhotosJob *>(kjob);
    Q_ASSERT(job);
    m_jobs.removeAll(job);
    if (job->error())
    {
        handleVkError(job);
        return;
    }

    updateControls(true);
}

bool VkontakteWindow::isAuthenticated()
{
    return m_authenticated;
}

} // namespace KIPIVkontaktePlugin

#include "vkwindow.moc"
