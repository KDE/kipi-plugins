/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a kipi plugin to export images to Yandex.Fotki web service
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
 *
 * GUI based on PicasaWeb KIPI Plugin
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "yfwindow.moc"

// C ANSI includes

extern "C"
{
// TODO: remove this include
#include <unistd.h> // getpid
}

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>

// KDE includes

#include <kcombobox.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kde_file.h>
#include <kdeversion.h>
#include <kdialog.h>
#include <khelpmenu.h>
#include <kio/renamedialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpassworddialog.h>
#include <kprogressdialog.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpversion.h"
#include "kpmetadata.h"
#include "kpimageslist.h"
#include "yftalker.h"
#include "yfalbumdialog.h"
#include "logindialog.h"

using namespace KDcrawIface;

namespace KIPIYandexFotkiPlugin
{

/*
 * This tag added to our images after uploading to Fotki web service
 */
const char* YandexFotkiWindow::XMP_SERVICE_ID = "Xmp.kipi.yandexGPhotoId";

YandexFotkiWindow::YandexFotkiWindow(bool import, QWidget* const parent)
    : KPToolDialog(parent)
{
    m_import    = import;

    KStandardDirs dir;
    m_tmpDir    = dir.saveLocation("tmp", "kipiplugin-yandexfotki-" +
                                   QString::number(getpid()) + '/');

    m_mainWidget                  = new QWidget(this);
    QHBoxLayout* const mainLayout = new QHBoxLayout(m_mainWidget);

    m_imgList                     = new KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(true);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(
        i18n("This is the list of images to upload to your Yandex album."));

    QWidget* const settingsBox           = new QWidget(this);
    QVBoxLayout* const settingsBoxLayout = new QVBoxLayout(settingsBox);

    m_headerLabel = new QLabel(settingsBox);
    m_headerLabel->setWhatsThis(i18n("This is a clickable link to open the "
                                     "Yandex.Fotki service in a web browser."));
    m_headerLabel->setOpenExternalLinks(true);
    m_headerLabel->setFocusPolicy(Qt::NoFocus);

    /*
     * Account box
     */
    m_accountBox = new QGroupBox(i18n("Account"), settingsBox);
    m_accountBox->setWhatsThis(i18n("This account is used for authentication."));

    QGridLayout* const accountBoxLayout = new QGridLayout(m_accountBox);
    QLabel* const loginDescLabel        = new QLabel(m_accountBox);
    loginDescLabel->setText(i18n("Name:"));
    loginDescLabel->setWhatsThis(i18n("Your Yandex login"));

    m_loginLabel       = new QLabel(m_accountBox);
    m_changeUserButton = new KPushButton(KGuiItem(i18n("Change Account"), "system-switch-user",
                                         i18n("Change Yandex account used to authenticate")), m_accountBox);

    accountBoxLayout->addWidget(loginDescLabel, 0, 0);
    accountBoxLayout->addWidget(m_loginLabel, 0, 1);

    accountBoxLayout->addWidget(m_changeUserButton, 1, 1);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    connect(m_changeUserButton, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));

    // -- Album box --------------------------------------------------------------------------

    m_albumsBox                        = new QGroupBox(i18n("Album"), settingsBox);
    m_albumsBox->setWhatsThis(i18n("This is the Yandex.Fotki album that will be used for the transfer."));
    QGridLayout* const albumsBoxLayout = new QGridLayout(m_albumsBox);

    m_albumsCombo        = new KComboBox(m_albumsBox);
    m_albumsCombo->setEditable(false);

    m_newAlbumButton     = new KPushButton(KGuiItem(i18n("New Album"), "list-add",
                                           i18n("Create new Yandex.Fotki album")), m_albumsBox);
    m_reloadAlbumsButton = new KPushButton(KGuiItem(i18nc("reload albums list", "Reload"), "view-refresh",
                                             i18n("Reload albums list")), m_albumsBox);

    albumsBoxLayout->addWidget(m_albumsCombo,        0, 0, 1, 5);
    albumsBoxLayout->addWidget(m_newAlbumButton,     1, 3, 1, 1);
    albumsBoxLayout->addWidget(m_reloadAlbumsButton, 1, 4, 1, 1);

    connect(m_newAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotNewAlbumRequest()) );


    connect(m_reloadAlbumsButton, SIGNAL(clicked()),
            this, SLOT(slotReloadAlbumsRequest()) );

    // ------------------------------------------------------------------------

    QGroupBox* const uploadBox         = new QGroupBox(i18n("Destination"), settingsBox);
    uploadBox->setWhatsThis(i18n("This is the location where Yandex.Fotki images will be downloaded."));
    QVBoxLayout* const uploadBoxLayout = new QVBoxLayout(uploadBox);
    m_uploadWidget                     = iface()->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(m_uploadWidget);

    // ------------------------------------------------------------------------

    QGroupBox* const optionsBox         = new QGroupBox(i18n("Options"), settingsBox);
    optionsBox->setWhatsThis(i18n("These are options that will be applied to images before upload."));
    QGridLayout* const optionsBoxLayout = new QGridLayout(optionsBox);

    m_resizeCheck = new QCheckBox(optionsBox);
    m_resizeCheck->setText(i18n("Resize photos before uploading"));
    m_resizeCheck->setChecked(false);

    connect(m_resizeCheck, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));

    m_dimensionSpin            = new QSpinBox(optionsBox);
    m_dimensionSpin->setMinimum(0);
    m_dimensionSpin->setMaximum(5000);
    m_dimensionSpin->setSingleStep(10);
    m_dimensionSpin->setValue(600);
    m_dimensionSpin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_dimensionSpin->setEnabled(false);
    QLabel* const dimensionLbl = new QLabel(i18n("Maximum dimension:"), optionsBox);

    m_imageQualitySpin              = new QSpinBox(optionsBox);
    m_imageQualitySpin->setMinimum(0);
    m_imageQualitySpin->setMaximum(100);
    m_imageQualitySpin->setSingleStep(1);
    m_imageQualitySpin->setValue(85);
    m_imageQualitySpin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel* const imageQualityLabel = new QLabel(i18n("JPEG quality:"), optionsBox);

    QSpacerItem* const spacer1 = new QSpacerItem(1, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem* const spacer2 = new QSpacerItem(1, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QLabel* const policyLabel  = new QLabel(i18n("Update policy:"), optionsBox);

    QRadioButton* const policyRadio1 = new QRadioButton(i18n("Update metadata"), optionsBox);
    policyRadio1->setWhatsThis(i18n("Update metadata of remote file and merge remote tags with local"));

/*
    QRadioButton* policyRadio2 = new QRadioButton(i18n("Update metadata, keep tags"), optionsBox);
    policyRadio2->setWhatsThis(
        i18n("Update metadata of remote file but keep remote tags untouched."));
*/

    QRadioButton* const policyRadio3  = new QRadioButton(i18n("Skip photo"), optionsBox);
    policyRadio3->setWhatsThis(i18n("Simple skip photo"));
    QRadioButton* const policyRadio4  = new QRadioButton(i18n("Upload as new"), optionsBox);
    policyRadio4->setWhatsThis(i18n("Add photo as new"));

    QLabel* const accessLabel = new QLabel(i18n("Privacy settings:"), optionsBox);
    m_accessCombo             = new KComboBox(false, optionsBox);
    m_accessCombo->addItem(KIcon("folder"), i18n("Public access"),         YandexFotkiPhoto::ACCESS_PUBLIC);
    m_accessCombo->addItem(KIcon("folder-red"), i18n("Friends access"),    YandexFotkiPhoto::ACCESS_FRIENDS);
    m_accessCombo->addItem(KIcon("folder-locked"), i18n("Private access"), YandexFotkiPhoto::ACCESS_PRIVATE);

    m_hideOriginalCheck = new QCheckBox(i18n("Hide original photo"), optionsBox);
    m_disableCommentsCheck = new QCheckBox(i18n("Disable comments"), optionsBox);
    m_adultCheck = new QCheckBox(i18n("Adult content"), optionsBox);

    m_policyGroup = new QButtonGroup(optionsBox);
    m_policyGroup->addButton(policyRadio1, POLICY_UPDATE_MERGE);
//    m_policyGroup->addButton(policyRadio2, POLICY_UPDATE_KEEP);
    m_policyGroup->addButton(policyRadio3, POLICY_SKIP);
    m_policyGroup->addButton(policyRadio4, POLICY_ADDNEW);

    optionsBoxLayout->addWidget(m_resizeCheck,          0, 0, 1, 5);
    optionsBoxLayout->addWidget(imageQualityLabel,      1, 1, 1, 1);
    optionsBoxLayout->addWidget(m_imageQualitySpin,     1, 2, 1, 1);
    optionsBoxLayout->addWidget(dimensionLbl,           2, 1, 1, 1);
    optionsBoxLayout->addWidget(m_dimensionSpin,        2, 2, 1, 1);
    optionsBoxLayout->addItem(spacer1,                  3, 0, 1, 5);

    optionsBoxLayout->addWidget(accessLabel,            4, 0, 1, 5);
    optionsBoxLayout->addWidget(m_accessCombo,          5, 1, 1, 4);
    optionsBoxLayout->addWidget(m_adultCheck,           6, 1, 1, 4);
    optionsBoxLayout->addWidget(m_hideOriginalCheck,    7, 1, 1, 4);
    optionsBoxLayout->addWidget(m_disableCommentsCheck, 8, 1, 1, 4);
    optionsBoxLayout->addItem(spacer2,                  9, 0, 1, 5);

    optionsBoxLayout->addWidget(policyLabel,            10, 0, 1, 5);
    optionsBoxLayout->addWidget(policyRadio1,           11, 1, 1, 4);
    //optionsBoxLayout->addWidget(policyRadio2,         12, 1, 1, 4);
    optionsBoxLayout->addWidget(policyRadio3,           13, 1, 1, 4);
    optionsBoxLayout->addWidget(policyRadio4,           14, 1, 1, 4);

    optionsBoxLayout->setRowStretch(14, 10);

    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    m_progressBar = new QProgressBar(settingsBox);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    // -- Layouts ---------------------------------------------------------------------

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
    setWindowIcon(KIcon("yandexfotki"));
    setButtons(KDialog::Help|KDialog::User1|KDialog::Close);
    setDefaultButton(Close);
    setModal(false);

    if (!m_import)
    {
        setWindowTitle(i18n("Export to Yandex.Fotki Web Service"));
        setButtonGuiItem(KDialog::User1,
                         KGuiItem(i18n("Start Upload"), "network-workgroup",
                                  i18n("Start upload to Yandex.Fotki service")));
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

    KPAboutData* const about = new KPAboutData(ki18n("Yandex.Fotki Plugin"),
                                               0,
                                               KAboutData::License_GPL,
                                               ki18n("A Kipi plugin to export image collections to "
                                                     "Yandex.Fotki web service."),
                                               ki18n("(c) 2007-2009, Vardhman Jain\n"
                                                     "(c) 2008-2012, Gilles Caulier\n"
                                                     "(c) 2009, Luka Renko\n"
                                                     "(c) 2010, Roman Tsisyk"));

    about->addAuthor(ki18n( "Roman Tsisyk" ), ki18n("Author"),
                     "roman at tsisyk dot com");

    about->setHandbookEntry("YandexFotki");
    setAboutData(about);

    // -- UI slots -----------------------------------------------------------------------

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotStartTransfer()) );

    // -- Talker slots -------------------------------------------------------------------

    connect(&m_talker, SIGNAL(signalError()),
            this, SLOT(slotError()));

    connect(&m_talker, SIGNAL(signalGetSessionDone()),
            this, SLOT(slotGetSessionDone()));

    connect(&m_talker, SIGNAL(signalGetTokenDone()),
            this, SLOT(slotGetTokenDone()));

    connect(&m_talker, SIGNAL(signalGetServiceDone()),
            this, SLOT(slotGetServiceDone()));

    connect(&m_talker, SIGNAL(signalListAlbumsDone(QList<YandexFotkiAlbum>)),
            this, SLOT(slotListAlbumsDone(QList<YandexFotkiAlbum>)));

    connect(&m_talker, SIGNAL(signalListPhotosDone(QList<YandexFotkiPhoto>)),
            this, SLOT(slotListPhotosDone(QList<YandexFotkiPhoto>)));

    connect(&m_talker, SIGNAL(signalUpdatePhotoDone(YandexFotkiPhoto&)),
            this, SLOT(slotUpdatePhotoDone(YandexFotkiPhoto&)));

    connect(&m_talker, SIGNAL(signalUpdateAlbumDone()),
            this, SLOT(slotUpdateAlbumDone()));

    // read settings from file
    readSettings();
}

YandexFotkiWindow::~YandexFotkiWindow()
{
    reset();
}

void YandexFotkiWindow::reactivate()
{
    m_imgList->loadImagesFromCurrentSelection();

    reset();
    authenticate(false);
    show();
}

void YandexFotkiWindow::reset()
{
    m_talker.reset();
    updateControls(true);
    updateLabels();
}

void YandexFotkiWindow::updateControls(bool val)
{
    if (val)
    {
        if (m_talker.isAuthenticated())
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

void YandexFotkiWindow::updateLabels()
{
    QString urltext;
    QString logintext;

    if (m_talker.isAuthenticated())
    {
        logintext = m_talker.login();
        urltext = YandexFotkiTalker::USERPAGE_URL.arg(m_talker.login());
        m_albumsBox->setEnabled(true);
    }
    else
    {
        logintext = i18n("Unauthorized");
        urltext = YandexFotkiTalker::USERPAGE_DEFAULT_URL;
        m_albumsCombo->clear();
    }

    m_loginLabel->setText(QString("<b>%1</b>").arg(logintext));
    m_headerLabel->setText(QString("<b><h2><a href=\"%1\">"
                                   "<font color=\"#ff000a\">%2</font>"
                                   "<font color=\"black\">%3</font>"
                                   "<font color=\"#009d00\">%4</font>"
                                   "</a></h2></b>")
                           .arg(urltext)
                           .arg(i18nc("Yandex.Fotki", "Y"))
                           .arg(i18nc("Yandex.Fotki", "andex."))
                           .arg(i18nc("Yandex.Fotki", "Fotki")));
}

void YandexFotkiWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group( "YandexFotki Settings");

    // TODO: use kwallet ??
    m_talker.setLogin(grp.readEntry("login", ""));
    // don't store tokens in plaintext
    //m_talker.setToken(grp.readEntry("token", ""));

    if (grp.readEntry("Resize", false))
    {
        m_resizeCheck->setChecked(true);
        m_dimensionSpin->setEnabled(true);
        m_imageQualitySpin->setEnabled(true);
    }
    else
    {
        m_resizeCheck->setChecked(false);
        m_dimensionSpin->setEnabled(false);
        m_imageQualitySpin->setEnabled(false);
    }

    m_dimensionSpin->setValue(grp.readEntry("Maximum Width", 1600));
    m_imageQualitySpin->setValue(grp.readEntry("Image Quality", 85));
    m_policyGroup->button(grp.readEntry("Sync policy", 0))->setChecked(true);
}

void YandexFotkiWindow::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("YandexFotki Settings");

    // TODO: user kwallet ??
    grp.writeEntry("token", m_talker.token());
    // don't store tokens in plaintext
    //grp.writeEntry("login", m_talker.login());

    grp.writeEntry("Resize", m_resizeCheck->isChecked());
    grp.writeEntry("Maximum Width", m_dimensionSpin->value());
    grp.writeEntry("Image Quality", m_imageQualitySpin->value());
    grp.writeEntry("Sync policy", m_policyGroup->checkedId());
}

QString YandexFotkiWindow::getDestinationPath() const
{
    return m_uploadWidget->selectedImageCollection().uploadPath().path();
}

void YandexFotkiWindow::slotChangeUserClicked()
{
    // force authenticate window
    authenticate(true);
}

void YandexFotkiWindow::closeEvent(QCloseEvent* event)
{
    kDebug() << "closeEvent";
    writeSettings();
    reset();
    event->accept();
}

void YandexFotkiWindow::slotButtonClicked(int button)
{
    switch (button)
    {
        case KDialog::User1:
            slotStartTransfer();
            break;
        case KDialog::Close:
            if (!isButtonEnabled(KDialog::User1))
            {
                m_talker.cancel();
                updateControls(true);
            }
            break;
        default:
            break;
    }

    KDialog::slotButtonClicked(button);
}

void YandexFotkiWindow::slotResizeChecked()
{
    m_dimensionSpin->setEnabled(m_resizeCheck->isChecked());
    m_imageQualitySpin->setEnabled(m_resizeCheck->isChecked());
}

/*
void YandexFotkiWindow::cancelProcessing()
{
    m_talker.cancel();
    m_transferQueue.clear();
    m_imgList->processed(false);
    progressBar()->hide();
}
*/

void YandexFotkiWindow::authenticate(bool forceAuthWindow)
{
    // update credentials
    if (forceAuthWindow || m_talker.login().isNull() || m_talker.password().isNull())
    {
        QPointer<LoginDialog> dlg = new LoginDialog(this, m_talker.login(), QString());

        if (dlg->exec() == QDialog::Accepted)
        {
            m_talker.setLogin(dlg->login());
            m_talker.setPassword(dlg->password());
        }
        else
        {
            // don't change anything
            return;
        }

        delete dlg;
    }

    /*else
    {
        kDebug() << "Checking old token...";
        m_talker.checkToken();
        return;
    }
    */

    // if new credentials non-empty, authenticate
    if (!m_talker.login().isEmpty() && !m_talker.password().isEmpty())
    {
        // cancel all tasks first
        reset();

        // start authentication chain
        updateControls(false);
        m_talker.getService();
    }
    else
    {
        // we don't have valid credentials, so cancel all transfers and reset
        reset();
    }

/*
        progressBar()->show();
        progressBar()->setFormat("");
*/
}

void YandexFotkiWindow::slotListPhotosDone(const QList <YandexFotkiPhoto>& photosList)
{
    if (m_import)
    {
        slotListPhotosDoneForDownload(photosList);
    }
    else
    {
        slotListPhotosDoneForUpload(photosList);
    }
}

void YandexFotkiWindow::slotListPhotosDoneForDownload(const QList <YandexFotkiPhoto>& photosList)
{
    Q_UNUSED(photosList);
    updateControls(true);
}

void YandexFotkiWindow::slotListPhotosDoneForUpload(const QList <YandexFotkiPhoto>& photosList)
{
    updateControls(true);

    QMap<QString, int> dups;
    int i = 0;

    foreach(const YandexFotkiPhoto& photo, photosList)
    {
        dups.insert(photo.urn(), i);
        i++;
    }

    const UpdatePolicy policy             = static_cast<UpdatePolicy>(m_policyGroup->checkedId());
    const YandexFotkiPhoto::Access access = static_cast<YandexFotkiPhoto::Access>(
                                            m_accessCombo->itemData(m_accessCombo->currentIndex()).toInt());

    kDebug() << "";
    kDebug() << "----";
    m_transferQueue.clear();

    foreach(const KUrl& url, m_imgList->imageUrls(true))
    {
        KPImageInfo info(url);
        KPMetadata  meta;

        const QString imgPath = url.toLocalFile();

        // check if photo alredy uploaded

        int oldPhotoId = -1;

        if (meta.load(imgPath))
        {
            QString localId = meta.getXmpTagString(XMP_SERVICE_ID);
            oldPhotoId      = dups.value(localId, -1);
        }

        // get tags
        QStringList tags = info.tagsPath();
        bool updateFile  = true;

        QSet<QString> oldtags;

        if (oldPhotoId != -1)
        {
            if (policy == POLICY_SKIP)
            {
                kDebug() << "SKIP: " << imgPath;
                continue;
            }

            // old photo copy
            m_transferQueue.push(photosList[oldPhotoId]);

            if (policy == POLICY_UPDATE_MERGE)
            {
                foreach(const QString& t, m_transferQueue.top().tags)
                {
                    oldtags.insert(t);
                }
            }

            if (policy != POLICY_ADDNEW)
            {
                updateFile = false;
            }
        }
        else
        {
            // empty photo
            m_transferQueue.push(YandexFotkiPhoto());
        }


        YandexFotkiPhoto& photo = m_transferQueue.top();
        // TODO: updateFile is not used
        photo.setOriginalUrl(imgPath);
        photo.setTitle(info.name());
        photo.setSummary(info.description());
        photo.setAccess(access);
        photo.setHideOriginal(m_hideOriginalCheck->isChecked());
        photo.setDisableComments(m_disableCommentsCheck->isChecked());

        // adult flag can't be removed, API restrictions
        if (!photo.isAdult())
            photo.setAdult(m_adultCheck->isChecked());

        foreach(const QString& t, tags)
        {
            if (!oldtags.contains(t))
            {
                photo.tags.append(t);
            }
        }

        if (updateFile)
        {
            kDebug() << "METADATA + IMAGE: " << imgPath;
        }
        else
        {
            kDebug() << "METADATA: " << imgPath;
        }
    }

    if (m_transferQueue.isEmpty())
    {
        return;    // nothing to do
    }

    kDebug() << "----";
    kDebug() << "";

    updateControls(false);
    updateNextPhoto();
}

void YandexFotkiWindow::updateNextPhoto()
{
    // select only one image from stack
    while (!m_transferQueue.isEmpty())
    {
        YandexFotkiPhoto& photo = m_transferQueue.top();

        if (!photo.originalUrl().isNull())
        {
            QImage image;

            // check if we have to RAW file -> use preview image then
            bool isRAW = KPMetadata::isRawFile(photo.originalUrl());

            if (isRAW)
            {
                KDcraw::loadRawPreview(image, photo.originalUrl());
            }
            else
            {
                image.load(photo.originalUrl());
            }

            photo.setLocalUrl(m_tmpDir + QFileInfo(photo.originalUrl())
                              .baseName()
                              .trimmed() + ".jpg");

            bool prepared = false;

            if (!image.isNull())
            {
                // get temporary file name

                // rescale image if requested
                int maxDim = m_dimensionSpin->value();

                if (m_resizeCheck->isChecked() && (image.width() > maxDim || image.height() > maxDim))
                {
                    kDebug() << "Resizing to " << maxDim;
                    image = image.scaled(maxDim, maxDim, Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation);
                }

                // copy meta data to temporary image
                KPMetadata meta;

                if (image.save(photo.localUrl(), "JPEG", m_imageQualitySpin->value()) &&
                    meta.load(photo.originalUrl()))
                {
                    meta.setImageDimensions(image.size());
                    meta.setImageProgramId("Kipi-plugins",
                                                 kipiplugins_version);
                    meta.save(photo.localUrl());
                    prepared = true;
                }
            }

            if (!prepared)
            {
                if (KMessageBox::warningContinueCancel(this,
                                                       i18n("Failed to prepare image %1\n"
                                                               "Do you want to continue?", photo.originalUrl())) != KMessageBox::Continue)
                {
                    // stop uploading
                    m_transferQueue.clear();
                    continue;
                }
                else
                {
                    m_transferQueue.pop();
                    continue;
                }
            }
        }

        const YandexFotkiAlbum& album = m_talker.albums().at(m_albumsCombo->currentIndex());

        kDebug() << photo.originalUrl();

        m_talker.updatePhoto(photo, album);

        return;
    }

    updateControls(true);

    KMessageBox::information(this, i18n("Images has been uploaded"));
    return;
}

void YandexFotkiWindow::slotNewAlbumRequest()
{
    YandexFotkiAlbum album;
    QPointer<YandexFotkiAlbumDialog> dlg = new YandexFotkiAlbumDialog(this, album);

    if (dlg->exec() == QDialog::Accepted)
    {
        updateControls(false);
        m_talker.updateAlbum(album);
    }

    delete dlg;
}

void YandexFotkiWindow::slotReloadAlbumsRequest()
{
    updateControls(false);
    m_talker.listAlbums();
}

void YandexFotkiWindow::slotStartTransfer()
{
    kDebug() << "slotStartTransfer invoked";

    if (m_albumsCombo->currentIndex() == -1 || m_albumsCombo->count() == 0)
    {
        KMessageBox::information(this, i18n("Please select album first"));
        return;
    }

    // TODO: import support
    if (!m_import)
    {
        // list photos of the album, then start upload
        const YandexFotkiAlbum& album = m_talker.albums().at(m_albumsCombo->currentIndex());

        kDebug() << "Album selected" << album;

        updateControls(false);
        m_talker.listPhotos(album);
    }
}

void YandexFotkiWindow::slotError()
{
    switch (m_talker.state())
    {
        case YandexFotkiTalker::STATE_GETSESSION_ERROR:
            KMessageBox::error(this, i18n("Session error"));
            break;
        case YandexFotkiTalker::STATE_GETTOKEN_ERROR:
            KMessageBox::error(this, i18n("Token error"));
            break;
        case YandexFotkiTalker::STATE_INVALID_CREDENTIALS:
            KMessageBox::error(this, i18n("Invalid credentials"));
//            authenticate(true);
            break;
        case YandexFotkiTalker::STATE_GETSERVICE_ERROR:
            KMessageBox::error(this, i18n("Cannot get service document"));
            break;
/*
        case YandexFotkiTalker::STATE_CHECKTOKEN_INVALID:
            // remove old expired token
            kDebug() << "CheckToken invalid";
            m_talker.setToken(QString());
            // don't say anything, simple show new auth window
            authenticate(true);
            break;
*/
        case YandexFotkiTalker::STATE_LISTALBUMS_ERROR:
            m_albumsCombo->clear();
            KMessageBox::error(this, i18n("Cannot list albums"));
            break;
        case YandexFotkiTalker::STATE_LISTPHOTOS_ERROR:
            KMessageBox::error(this, i18n("Cannot list photos"));
            break;
        case YandexFotkiTalker::STATE_UPDATEALBUM_ERROR:
            KMessageBox::error(this, i18n("Cannot update album info"));
            break;
        case YandexFotkiTalker::STATE_UPDATEPHOTO_FILE_ERROR:
        case YandexFotkiTalker::STATE_UPDATEPHOTO_INFO_ERROR:
            kDebug() << "UpdatePhotoError";
            if (KMessageBox::warningContinueCancel(this,
                     i18n("Failed to upload image %1\n"
                          "Do you want to continue?",
                           m_transferQueue.top().originalUrl())) !=
                    KMessageBox::Continue)
            {
                // clear upload stack
                m_transferQueue.clear();
            }
            else
            {
                // cancel current operation
                m_talker.cancel();
                // remove only bad image
                m_transferQueue.pop();
                // and try next
                updateNextPhoto();
                return;
            }
            break;
        default:
            kDebug() << "Unhandled error" << m_talker.state();
            KMessageBox::error(this, i18n("Unknown error"));
    }

    // cancel current operation
    m_talker.cancel();
    updateControls(true);
}

void YandexFotkiWindow::slotGetServiceDone()
{
    kDebug() << "GetService Done";
    m_talker.getSession();
}

void YandexFotkiWindow::slotGetSessionDone()
{
    kDebug() << "GetSession Done";
    m_talker.getToken();
}

void YandexFotkiWindow::slotGetTokenDone()
{
    updateLabels();
    slotReloadAlbumsRequest();
}

void YandexFotkiWindow::slotListAlbumsDone(const QList<YandexFotkiAlbum>& albumsList)
{
    m_albumsCombo->clear();

    foreach(const YandexFotkiAlbum& album, albumsList)
    {
        QString albumIcon;

        if (album.isProtected())
        {
            albumIcon = "folder-locked";
        }
        else
        {
            albumIcon = "folder-image";
        }

        m_albumsCombo->addItem(KIcon(albumIcon), album.toString());
    }

    m_albumsCombo->setEnabled(true);
    updateControls(true);
}

void YandexFotkiWindow::slotUpdatePhotoDone(YandexFotkiPhoto& photo)
{
    kDebug() << "photoUploaded" << photo;

    KPMetadata meta;

    if (meta.supportXmp() && meta.canWriteXmp(photo.originalUrl()) &&
        meta.load(photo.originalUrl()))
    {
        // ignore errors here
        if (meta.setXmpTagString(XMP_SERVICE_ID, photo.urn(), false) &&
            meta.save(photo.originalUrl()))
        {
            kDebug() << "MARK: " << photo.originalUrl();
        }
    }

    m_transferQueue.pop();
    updateNextPhoto();
}

void YandexFotkiWindow::slotUpdateAlbumDone()
{
    kDebug() << "Album created";
    m_albumsCombo->clear();
    m_talker.listAlbums();
}

} // namespace KIPIYandexFotkiPlugin
