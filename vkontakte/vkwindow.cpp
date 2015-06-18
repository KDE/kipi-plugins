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
#include <libkvkontakte/vkapi.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpaboutdata.h"
#include "kpversion.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"
#include "albumchooserwidget.h"
#include "authinfowidget.h"

namespace KIPIVkontaktePlugin
{

VkontakteWindow::VkontakteWindow(bool import, QWidget* const parent)
    : KPToolDialog(parent)
{
    m_albumsBox = NULL;
    m_vkapi     = new Vkontakte::VkApi(this);

    // read settings from file
    readSettings();

    connect(this, SIGNAL(finished(int)),
            this, SLOT(slotFinished()));

    m_import                      = import;
    m_mainWidget                  = new QWidget(this);
    QHBoxLayout* const mainLayout = new QHBoxLayout(m_mainWidget);
    m_imgList                     = new KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(false); // TODO: implement conversion
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(i18n("This is the list of images to upload to your VKontakte album."));

    m_settingsBox                        = new QWidget(this);
    QVBoxLayout* const settingsBoxLayout = new QVBoxLayout(m_settingsBox);

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
    m_albumsBox = new AlbumChooserWidget(m_settingsBox, m_vkapi);
    m_albumsBox->selectAlbum(m_albumToSelect);

    // ------------------------------------------------------------------------

    QGroupBox* const uploadBox         = new QGroupBox(i18n("Destination"), m_settingsBox);
    uploadBox->setWhatsThis(i18n("This is the location where VKontakte images will be downloaded."));
    QVBoxLayout* const uploadBoxLayout = new QVBoxLayout(uploadBox);
    m_uploadWidget                     = iface()->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(m_uploadWidget);

    // ------------------------------------------------------------------------

#if 0
    QGroupBox* const optionsBox = new QGroupBox(i18n("Options"), m_settingsBox);
    optionsBox->setWhatsThis(i18n("These are options that will be applied to images before upload."));
#endif

//     m_checkKeepOriginal = new QCheckBox(i18n("Save in high resolution"), m_settingsBox); // store state in kipirc

//     QVBoxLayout* const optionsBoxLayout = new QVBoxLayout(optionsBox);
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
    setWindowIcon(KIcon("kipi"));
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

    KPAboutData* const about = new KPAboutData(ki18n("VKontakte Plugin"),
                                               0,
                                               KAboutData::License_GPL,
                                               ki18n("A Kipi plugin to export image collections to "
                                                     "VKontakte web service."),
                                               ki18n("(c) 2007-2009, Vardhman Jain\n"
                                                     "(c) 2008-2013, Gilles Caulier\n"
                                                     "(c) 2009, Luka Renko\n"
                                                     "(c) 2010-2013, Roman Tsisyk\n"
                                                     "(c) 2011-2013, Alexander Potashev"));

    about->addAuthor(ki18n("Alexander Potashev"), ki18n("Author"), "aspotashev@gmail.com");
    about->setHandbookEntry("VKontakte");
    setAboutData(about);

    /*
     * UI slots
     */
    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartTransfer()));

    // for startReactivation()
    connect(m_vkapi, SIGNAL(authenticated()),
            this, SLOT(show()));

    /*
     * Dialog update slots
     */
    connect(this, SIGNAL(signalUpdateBusyStatus(bool)),
            this, SLOT(updateBusyStatus(bool)));

    connect(m_vkapi, SIGNAL(authenticated()), // TBD: busy status handling needs improvement
            this, SLOT(updateBusyStatusReady()));

    updateBusyStatus(true);
}

void VkontakteWindow::initAccountBox()
{
    m_accountBox = new AuthInfoWidget(m_settingsBox, m_vkapi);

    connect(m_vkapi, SIGNAL(authenticated()),
            this, SLOT(authenticated()));

    connect(m_accountBox, SIGNAL(authCleared()),
            this, SLOT(authCleared()));

    connect(m_accountBox, SIGNAL(signalUpdateAuthInfo()),
            this, SLOT(updateHeaderLabel()));
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
    m_accountBox->startAuthentication(false); // show() will be called after that
}

void VkontakteWindow::reset()
{
    emit signalUpdateBusyStatus(false);
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

void VkontakteWindow::updateBusyStatusReady()
{
    updateBusyStatus(false);
}

//---------------------------------------------------------------------------

void VkontakteWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("VKontakte Settings");

    m_appId         = grp.readEntry("VkAppId", "2446321");
    m_albumToSelect = grp.readEntry("SelectedAlbumId", -1);
    m_vkapi->setAppId(m_appId);
    m_vkapi->setRequiredPermissions(Vkontakte::AppPermissions::Photos);
    m_vkapi->setInitialAccessToken(grp.readEntry("AccessToken", ""));
}

void VkontakteWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("VKontakte Settings");

    grp.writeEntry("VkAppId", m_appId);

    if (!m_vkapi->accessToken().isEmpty())
        grp.writeEntry("AccessToken", m_vkapi->accessToken());

    int aid = 0;
    if (!m_albumsBox->getCurrentAlbumId(aid))
    {
        grp.deleteEntry("SelectedAlbumId");
    }
    else
    {
        grp.writeEntry("SelectedAlbumId", aid);
    }
}

//---------------------------------------------------------------------------

void VkontakteWindow::closeEvent(QCloseEvent* event)
{
    if (!event)
    {
        return;
    }

    slotFinished();
    event->accept();
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
            done(KDialog::Close);
            break;
        default:
            KDialog::slotButtonClicked(button);
            break;
    }
}

void VkontakteWindow::authenticated()
{
    if (m_albumsBox)
        m_albumsBox->setEnabled(true);
}

void VkontakteWindow::authCleared()
{
    if (m_albumsBox)
    {
        m_albumsBox->setEnabled(false);
        m_albumsBox->clearList();
    }
}

void VkontakteWindow::updateHeaderLabel()
{
    m_headerLabel->setText(QString("<b><h2><a href=\"%1\"><font color=\"black\">%2</font></a></h2></b>")
                           .arg(m_accountBox->albumsURL()).arg(i18n("VKontakte")));
}

//---------------------------------------------------------------------------

void VkontakteWindow::handleVkError(KJob* kjob)
{
    KMessageBox::error(this, kjob->errorText(), i18nc("@title:window", "Request to VKontakte failed"));
}

//---------------------------------------------------------------------------

void VkontakteWindow::slotStartTransfer()
{
    int aid = 0;
    if (!m_albumsBox->getCurrentAlbumId(aid))
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

        Vkontakte::UploadPhotosJob* const job = new Vkontakte::UploadPhotosJob(m_vkapi->accessToken(),
                                                                               files, 
                                                                               false /*m_checkKeepOriginal->isChecked()*/,
                                                                               aid);

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
    Vkontakte::UploadPhotosJob* const job = dynamic_cast<Vkontakte::UploadPhotosJob*>(kjob);
    Q_ASSERT(job);
    m_jobs.removeAll(job);

    if (job == 0 || job->error())
    {
        handleVkError(job);
    }

    m_progressBar->hide();
    m_progressBar->progressCompleted();
    emit signalUpdateBusyStatus(false);
}

} // namespace KIPIVkontaktePlugin
