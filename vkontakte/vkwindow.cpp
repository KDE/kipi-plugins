/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to VKontakte web service
 *
 * Copyright (C) 2011 by Alexander Potashev <aspotashev at gmail dot com>
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

#include <libkvkontakte/getapplicationpermissionsjob.h>
#include <libkvkontakte/authenticationdialog.h>
#include <libkvkontakte/albumlistjob.h>
#include <libkvkontakte/createalbumjob.h>
#include <libkvkontakte/editalbumjob.h>
#include <libkvkontakte/deletealbumjob.h>
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
#include "vkalbumdialog.h"
#include "kpprogresswidget.h"

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

VkontakteWindow::VkontakteWindow(Interface* const interface, bool import, QWidget* const parent)
    : KPToolDialog(parent)
{
    m_authenticated = false;

    // read settings from file
    readSettings();

    connect(this, SIGNAL(finished()), 
            this, SLOT(slotFinished()));

    m_interface = interface;
    m_import    = import;

    m_mainWidget            = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(m_mainWidget);

    m_imgList  = new KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(false); // TODO: implement conversion
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(
        i18n("This is the list of images to upload to your VKontakte album."));

    QWidget *settingsBox = new QWidget(this);
    QVBoxLayout *settingsBoxLayout = new QVBoxLayout(settingsBox);

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
    m_changeUserButton->hide(); // changing account does not work anyway

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
    m_albumsBox = new QGroupBox(i18nc("@title:group Header above controls for managing albums", "Album"), settingsBox);
    m_albumsBox->setWhatsThis(
        i18n("This is the VKontakte album that will be used for the transfer."));
    QVBoxLayout *albumsBoxLayout = new QVBoxLayout(m_albumsBox);

    m_albumsCombo = new KComboBox(m_albumsBox);
    m_albumsCombo->setEditable(false);

    m_newAlbumButton = new KPushButton(
        KGuiItem(i18n("New Album"), "list-add",
                 i18n("Create new VKontakte album")), m_albumsBox);
    m_reloadAlbumsButton = new KPushButton(
        KGuiItem(i18nc("reload albums list", "Reload"), "view-refresh",
                 i18n("Reload albums list")), m_albumsBox);

    m_editAlbumButton = new QToolButton(m_albumsBox);
    m_editAlbumButton->setToolTip(i18n("Edit selected album"));
    m_editAlbumButton->setEnabled(false);
    m_editAlbumButton->setIcon(KIcon("document-edit"));

    m_deleteAlbumButton = new QToolButton(m_albumsBox);
    m_deleteAlbumButton->setToolTip(i18n("Delete selected album"));
    m_deleteAlbumButton->setEnabled(false);
    m_deleteAlbumButton->setIcon(KIcon("edit-delete"));

    QWidget *currentAlbumWidget = new QWidget(m_albumsBox);
    QHBoxLayout *currentAlbumWidgetLayout = new QHBoxLayout(currentAlbumWidget);
    currentAlbumWidgetLayout->addWidget(m_albumsCombo);
    currentAlbumWidgetLayout->addWidget(m_editAlbumButton);
    currentAlbumWidgetLayout->addWidget(m_deleteAlbumButton);

    QWidget *albumButtons = new QWidget(m_albumsBox);
    QHBoxLayout *albumButtonsLayout = new QHBoxLayout(albumButtons);
    albumButtonsLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    albumButtonsLayout->addWidget(m_newAlbumButton);
    albumButtonsLayout->addWidget(m_reloadAlbumsButton);

    albumsBoxLayout->addWidget(currentAlbumWidget);
    albumsBoxLayout->addWidget(albumButtons);

    connect(m_editAlbumButton, SIGNAL(clicked()), 
            this, SLOT(slotEditAlbumRequest()));

    connect(m_deleteAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotDeleteAlbumRequest()));

    connect(m_newAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotNewAlbumRequest()));

    connect(m_reloadAlbumsButton, SIGNAL(clicked()),
            this, SLOT(slotReloadAlbumsRequest()));

    // ------------------------------------------------------------------------

    QGroupBox* uploadBox         = new QGroupBox(i18n("Destination"), settingsBox);
    uploadBox->setWhatsThis(i18n("This is the location where VKontakte images will be downloaded."));
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadBox);
    m_uploadWidget               = interface->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(m_uploadWidget);

    // ------------------------------------------------------------------------

#if 0
    QGroupBox *optionsBox = new QGroupBox(i18n("Options"), settingsBox);
    optionsBox->setWhatsThis(
        i18n("These are options that will be applied to images before upload."));
#endif

//     m_checkKeepOriginal = new QCheckBox(i18n("Save in high resolution"), settingsBox); // store state in kipirc

//     QVBoxLayout *optionsBoxLayout = new QVBoxLayout(optionsBox);
//     optionsBoxLayout->addWidget(m_checkKeepOriginal);

    m_progressBar = new KPProgressWidget(settingsBox);
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
    mainLayout->addWidget(settingsBox);
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
        m_newAlbumButton->hide();
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

    about->handbookEntry = QString("VKontakte");
    setAboutData(about);

    /*
     * UI slots
     */
    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartTransfer()));

    connect(this, SIGNAL(signalAuthenticationDone()),
            this, SLOT(startAlbumsUpdate()));

    connect(this, SIGNAL(signalAuthenticationDone()),
            this, SLOT(startGetFullName()));

    connect(this, SIGNAL(signalAuthenticationDone()),
            this, SLOT(startGetUserId()));

    // for startReactivation()
    connect(this, SIGNAL(signalAuthenticationDone()),
            this, SLOT(show()));
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

        //m_changeUserButton->setEnabled(true); // does not work anyway
        setCursor(Qt::ArrowCursor);

        setButtonGuiItem(KDialog::Close,
                         KGuiItem(i18n("Close"), "dialog-close",
                                  i18n("Close window")));
    }
    else
    {
        setCursor(Qt::WaitCursor);
        m_albumsBox->setEnabled(false);
        //m_changeUserButton->setEnabled(false); // does not work anyway
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
        QString("<b><h2><a href=\"%1\"><font color=\"black\">%2</font></a></h2></b>")
            .arg(urlText).arg(i18n("VKontakte")));
}

Vkontakte::AlbumInfoPtr VkontakteWindow::currentAlbum()
{
    int index = m_albumsCombo->currentIndex();
    if (index >= 0)
        return m_albums.at(index);
    else
        return Vkontakte::AlbumInfoPtr();
}

void VkontakteWindow::selectAlbum(int aid)
{
    for (int i = 0; i < m_albums.size(); i ++)
        if (m_albums.at(i)->aid() == aid)
        {
            m_albumsCombo->setCurrentIndex(i);
            break;
        }
}

//---------------------------------------------------------------------------

void VkontakteWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("VKontakte Settings");

    m_appId         = grp.readEntry("VkAppId", "2446321");
    m_accessToken   = grp.readEntry("AccessToken", "");
    m_albumToSelect = grp.readEntry("SelectedAlbumId", -1);
}

void VkontakteWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("VKontakte Settings");

    grp.writeEntry("VkAppId", m_appId);

    if (!m_accessToken.isEmpty())
        grp.writeEntry("AccessToken", m_accessToken);

    Vkontakte::AlbumInfoPtr album = currentAlbum();

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

void VkontakteWindow::startAuthentication(bool forceLogout)
{
    m_userFullName.clear();
    m_userId = -1;
    m_editAlbumButton->setEnabled(false);
    m_deleteAlbumButton->setEnabled(false);

    if (forceLogout)
        m_accessToken.clear();

    if (!m_accessToken.isEmpty())
    {
        Vkontakte::GetApplicationPermissionsJob* job = new Vkontakte::GetApplicationPermissionsJob(m_accessToken);

        connect(job, SIGNAL(result(KJob*)), 
                this, SLOT(slotApplicationPermissionCheckDone(KJob*)));

        m_jobs.append(job);
        job->start();
    }
    else
    {
        QStringList permissions;
        permissions << "photos" << "offline";
        Vkontakte::AuthenticationDialog *authDialog = new Vkontakte::AuthenticationDialog(this);
        authDialog->setAppId(m_appId);
        authDialog->setPermissions(permissions);

        connect(authDialog, SIGNAL(authenticated(QString)),
                this, SLOT(slotAuthenticationDialogDone(QString)));

        connect(authDialog, SIGNAL(canceled()),
                this, SLOT(slotAuthenticationDialogCanceled()));

        authDialog->start();
    }
}

void VkontakteWindow::slotApplicationPermissionCheckDone(KJob* kjob)
{
    Vkontakte::GetApplicationPermissionsJob* job = dynamic_cast<Vkontakte::GetApplicationPermissionsJob*>(kjob);
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

//---------------------------------------------------------------------------

void VkontakteWindow::handleVkError(KJob *kjob)
{
    KMessageBox::error(this, kjob->errorText(), i18nc("@title:window", "Request to VKontakte failed"));
}

//---------------------------------------------------------------------------

void VkontakteWindow::startAlbumsUpdate()
{
    Vkontakte::AlbumListJob* job = new Vkontakte::AlbumListJob(m_accessToken);

    connect(job, SIGNAL(result(KJob*)), 
            this, SLOT(slotAlbumsUpdateDone(KJob*)));

    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotAlbumsUpdateDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::AlbumListJob)

    m_albumsCombo->clear();
    m_albums = job->list();

    foreach(const Vkontakte::AlbumInfoPtr &album, m_albums)
        m_albumsCombo->addItem(KIcon("folder-image"), album->title());

    if (m_albumToSelect != -1)
    {
        selectAlbum(m_albumToSelect);
        m_albumToSelect = -1;
    }
    m_albumsCombo->setEnabled(true);


    if (!m_albums.empty())
    {
        m_editAlbumButton->setEnabled(true);
        m_deleteAlbumButton->setEnabled(true);
    }

    updateControls(true);
}

//---------------------------------------------------------------------------

void VkontakteWindow::startGetFullName()
{
    Vkontakte::GetVariableJob* job = new Vkontakte::GetVariableJob(m_accessToken, 1281);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotGetFullNameDone(KJob*)));

    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotGetFullNameDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::GetVariableJob)

    m_userFullName = job->variable().toString();
    updateLabels();
}

//---------------------------------------------------------------------------

void VkontakteWindow::startGetUserId()
{
    Vkontakte::GetVariableJob* job = new Vkontakte::GetVariableJob(m_accessToken, 1280);

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotGetUserIdDone(KJob*)));

    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotGetUserIdDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::GetVariableJob)

    m_userId = job->variable().toInt();
    updateLabels();
}

//---------------------------------------------------------------------------

void VkontakteWindow::startAlbumCreation(Vkontakte::AlbumInfoPtr album)
{
    Vkontakte::CreateAlbumJob* job = new Vkontakte::CreateAlbumJob(
        m_accessToken,
        album->title(), album->description(),
        album->privacy(), album->commentPrivacy());

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotAlbumCreationDone(KJob*)));

    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotAlbumCreationDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::CreateAlbumJob)

    // Select the newly created album in the combobox later (in "slotAlbumsUpdateDone()")
    m_albumToSelect = job->album()->aid();

    startAlbumsUpdate();
    m_albumsCombo->setEnabled(false);
    m_editAlbumButton->setEnabled(false);
    m_deleteAlbumButton->setEnabled(false);
    updateControls(false);
}

//---------------------------------------------------------------------------

void VkontakteWindow::startAlbumEditing(Vkontakte::AlbumInfoPtr album)
{
    // Select the same album again in the combobox later (in "slotAlbumsUpdateDone()")
    m_albumToSelect = album->aid();

    Vkontakte::EditAlbumJob *job = new Vkontakte::EditAlbumJob(
        m_accessToken,
        album->aid(), album->title(), album->description(),
        album->privacy(), album->commentPrivacy());

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotAlbumEditingDone(KJob*)));

    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotAlbumEditingDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::EditAlbumJob)

    startAlbumsUpdate();
    m_albumsCombo->setEnabled(false);
    m_editAlbumButton->setEnabled(false);
    m_deleteAlbumButton->setEnabled(false);
    updateControls(false);
}

//---------------------------------------------------------------------------

void VkontakteWindow::startAlbumDeletion(Vkontakte::AlbumInfoPtr album)
{
    Vkontakte::DeleteAlbumJob* job = new Vkontakte::DeleteAlbumJob(m_accessToken, album->aid());

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotAlbumDeletionDone(KJob*)));

    m_jobs.append(job);
    job->start();
}

void VkontakteWindow::slotAlbumDeletionDone(KJob *kjob)
{
    SLOT_JOB_DONE_INIT(Vkontakte::DeleteAlbumJob)

    startAlbumsUpdate();
    m_albumsCombo->setEnabled(false);
    m_editAlbumButton->setEnabled(false);
    m_deleteAlbumButton->setEnabled(false);
    updateControls(false);
}

//---------------------------------------------------------------------------

void VkontakteWindow::slotNewAlbumRequest()
{
    Vkontakte::AlbumInfoPtr album(new Vkontakte::AlbumInfo());
    QPointer<VkontakteAlbumDialog> dlg = new VkontakteAlbumDialog(this, album);
    if (dlg->exec() == QDialog::Accepted)
    {
        updateControls(false);
        startAlbumCreation(album);
    }

    delete dlg;
}

void VkontakteWindow::slotEditAlbumRequest()
{
    Vkontakte::AlbumInfoPtr album = currentAlbum();
    if (album.isNull())
        return;

    QPointer<VkontakteAlbumDialog> dlg = new VkontakteAlbumDialog(this, album, true);
    if (dlg->exec() == QDialog::Accepted)
    {
        updateControls(false);
        startAlbumEditing(album);
    }

    delete dlg;
}

void VkontakteWindow::slotDeleteAlbumRequest()
{
    Vkontakte::AlbumInfoPtr album = currentAlbum();
    if (album.isNull())
        return;

    if (KMessageBox::warningContinueCancel(
        this,
        i18n("<qt>Are you sure you want to remove the album <b>%1</b> including all photos in it?</qt>", album->title()),
        i18nc("@title:window", "Confirm Album Deletion"),
        KStandardGuiItem::del(),
        KStandardGuiItem::cancel(),
        QString("kipi_vkontakte_delete_album_with_photos")) != KMessageBox::Continue)
    {
        return;
    }

    startAlbumDeletion(album);
}

void VkontakteWindow::slotReloadAlbumsRequest()
{
    updateControls(false);

    Vkontakte::AlbumInfoPtr album = currentAlbum();
    if (!album.isNull())
        m_albumToSelect = album->aid();

    startAlbumsUpdate();
}

void VkontakteWindow::slotStartTransfer()
{
    Vkontakte::AlbumInfoPtr album = currentAlbum();
    if (album.isNull() || m_albumsCombo->count() == 0)
    {
        // TODO: offer the user to create an album if there are no albums yet
        KMessageBox::information(this, i18n("Please select album first."));
        return;
    }

    // TODO: import support
    if (!m_import)
    {
        updateControls(false);

        QStringList files;
        foreach(const KUrl& url, m_imgList->imageUrls(true))
            files.append(url.toLocalFile());

        Vkontakte::UploadPhotosJob* job = new Vkontakte::UploadPhotosJob(
            m_accessToken, files, false /*m_checkKeepOriginal->isChecked()*/, album->aid());

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
    updateControls(true);
}

bool VkontakteWindow::isAuthenticated()
{
    return m_authenticated;
}

} // namespace KIPIVkontaktePlugin
