/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-04-12
 * Description : A KIPI Plugin to export albums to rajce.net
 *
 * Copyright (C) 2011 by Lukas Krejci <krejci.l at centrum dot cz>
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

#include "rajcewidget.h"
#include "rajcewidget.moc"

#include "rajcesession.h"
#include "rajcelogindialog.h"
#include "newalbumdialog.h"

#include "imageslist.h"

#include <libkipi/interface.h>
#include <libkipi/uploadwidget.h>

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QProgressBar>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>

// KDE includes

#include <KLocale>
#include <KDialog>
#include <KLineEdit>
#include <KComboBox>
#include <KPushButton>
#include <KConfig>
#include <KColorScheme>

using namespace KIPIRajceExportPlugin;

RajceWidget::RajceWidget(KIPI::Interface* interface, const QString& tmpFolder, QWidget* parent): QWidget(parent)
{
    _lastLoggedInState = false;
    _session = new RajceSession(this, tmpFolder);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // -------------------------------------------------------------------

    _imgList = new KIPIPlugins::ImagesList(interface, this);

    _imgList->setControlButtonsPlacement(KIPIPlugins::ImagesList::ControlButtonsBelow);
    _imgList->setAllowRAW(true);
    _imgList->loadImagesFromCurrentSelection();
    _imgList->listView()->setWhatsThis(
    i18n("This is the list of images to upload to your Rajce.net account."));

    QWidget* settingsBox = new QWidget(this);
    QVBoxLayout* settingsBoxLayout = new QVBoxLayout(settingsBox);

    _headerLbl = new QLabel(settingsBox);
    _headerLbl->setWhatsThis(
        i18n("This is a clickable link to open the Rajce.net home page in a web browser."));
    _headerLbl->setOpenExternalLinks(true);
    _headerLbl->setFocusPolicy(Qt::NoFocus);

    // ------------------------------------------------------------------------

    QGroupBox* accountBox = new QGroupBox(i18n("Account"), settingsBox);
    accountBox->setWhatsThis(
        i18n("This is the Rajce.net account that will be used to authenticate."));
    QGridLayout* accountBoxLayout = new QGridLayout(accountBox);

    _anonymousRBtn = new QRadioButton(i18nc("Picasaweb account login", "Anonymous"), accountBox);
    _anonymousRBtn->setWhatsThis(
        i18n("Login as anonymous to Picasaweb web service."));

    _accountRBtn = new QRadioButton(i18n("Rajce.net Account"), accountBox);
    _accountRBtn->setWhatsThis(
        i18n("Login to Rajce.net using username and password."));

    _userNameLbl = new QLabel(i18nc("Rajce.net account settings", "Name:"), accountBox);
    _userName = new QLabel(accountBox);
    _changeUserBtn = new KPushButton(
        KGuiItem(i18n("Change Account"), "system-switch-user",
                 i18n("Change Rajce.net Account used to authenticate")), accountBox);

    accountBoxLayout->addWidget(_anonymousRBtn, 0, 0, 1, 2);
    accountBoxLayout->addWidget(_accountRBtn,   1, 0, 1, 2);
    accountBoxLayout->addWidget(_userNameLbl,   2, 0, 1, 1);
    accountBoxLayout->addWidget(_userName,      2, 1, 1, 1);
    accountBoxLayout->addWidget(_changeUserBtn, 3, 1, 1, 1);
    accountBoxLayout->setSpacing(KDialog::spacingHint());
    accountBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    QGroupBox* albumsBox = new QGroupBox(i18n("Album"), settingsBox);
    albumsBox->setWhatsThis(
        i18n("This is the Rajce.net album that will be used for transfer."));
    QGridLayout* albumsBoxLayout = new QGridLayout(albumsBox);

    _albumsCoB = new KComboBox(albumsBox);
    _albumsCoB->setEditable(false);

    _newAlbumBtn = new KPushButton(
        KGuiItem(i18n("New Album"), "list-add",
                      i18n("Create new Rajce.net album")), accountBox);
    _reloadAlbumsBtn   = new KPushButton(
        KGuiItem(i18nc("reload album list", "Reload"), "view-refresh",
                       i18n("Reload album list")), accountBox);

    albumsBoxLayout->addWidget(_albumsCoB,       0, 0, 1, 5);
    albumsBoxLayout->addWidget(_newAlbumBtn,     1, 3, 1, 1);
    albumsBoxLayout->addWidget(_reloadAlbumsBtn, 1, 4, 1, 1);

    // ------------------------------------------------------------------------

    QGroupBox* uploadBox = new QGroupBox(i18n("Destination"), settingsBox);
    uploadBox->setWhatsThis(
        i18n("This is the location where Rajce.net images will be downloaded."));
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadBox);
    _uploadWidget = interface->uploadWidget(uploadBox);
    uploadBoxLayout->addWidget(_uploadWidget);

    // ------------------------------------------------------------------------

    QGroupBox* optionsBox = new QGroupBox(i18n("Options"), settingsBox);
    optionsBox->setWhatsThis(
        i18n("These are options that will be applied to images before upload."));
    QGridLayout* optionsBoxLayout = new QGridLayout(optionsBox);

    _dimensionSpB  = new QSpinBox(optionsBox);
    _dimensionSpB->setMinimum(0);
    _dimensionSpB->setMaximum(5000);
    _dimensionSpB->setSingleStep(10);
    _dimensionSpB->setValue(1024);
    _dimensionSpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel* dimensionLbl = new QLabel(i18n("Maximum dimension:"), optionsBox);

    _imageQualitySpB = new QSpinBox(optionsBox);
    _imageQualitySpB->setMinimum(0);
    _imageQualitySpB->setMaximum(100);
    _imageQualitySpB->setSingleStep(1);
    _imageQualitySpB->setValue(85);
    _imageQualitySpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel* imageQualityLbl = new QLabel(i18n("JPEG quality:"), optionsBox);

    optionsBoxLayout->addWidget(imageQualityLbl,   0, 1, 1, 1);
    optionsBoxLayout->addWidget(_imageQualitySpB, 0, 2, 1, 1);
    optionsBoxLayout->addWidget(dimensionLbl,      1, 1, 1, 1);
    optionsBoxLayout->addWidget(_dimensionSpB,    1, 2, 1, 1);
    optionsBoxLayout->setRowStretch(8, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    _progressBar = new QProgressBar(settingsBox);
    _progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    _progressBar->hide();
    _progressBar->setMinimum(0);
    _progressBar->setMaximum(100);

    // ------------------------------------------------------------------------

    settingsBoxLayout->addWidget(_headerLbl);
    settingsBoxLayout->addWidget(accountBox);
    settingsBoxLayout->addWidget(albumsBox);
    settingsBoxLayout->addWidget(uploadBox);
    settingsBoxLayout->addWidget(optionsBox);
    settingsBoxLayout->addWidget(_progressBar);
    settingsBoxLayout->setSpacing(KDialog::spacingHint());
    settingsBoxLayout->setMargin(KDialog::spacingHint());

    // ------------------------------------------------------------------------

    mainLayout->addWidget(_imgList);
    mainLayout->addWidget(settingsBox);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(0);

    update();

    // ------------------------------------------------------------------------
    connect(_session, SIGNAL(busyStarted(unsigned)),
            this, SLOT(progressStarted(unsigned)));

    connect(_session, SIGNAL(busyFinished(unsigned)),
            this, SLOT(progressFinished(unsigned)));

    connect(_session, SIGNAL(busyProgress(unsigned, unsigned)),
            this, SLOT(progressChange(unsigned, unsigned)));

    connect(_changeUserBtn, SIGNAL(clicked()),
            this, SLOT(changeUserClicked()));

    connect(_newAlbumBtn, SIGNAL(clicked()),
            this, SLOT(createAlbum()));

    connect(_reloadAlbumsBtn, SIGNAL(clicked()),
            this, SLOT(loadAlbums()));

    connect(_albumsCoB, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(selectedAlbumChanged(QString)));

    // ------------------------------------------------------------------------

    _anonymousRBtn->hide();
    _accountRBtn->hide();

    uploadBox->hide();

    _uploadingPhotos = false;
}

void RajceWidget::update()
{
    bool loggedIn = !_session->state().sessionToken().isEmpty();

    if (loggedIn != _lastLoggedInState) {
        _lastLoggedInState = loggedIn;
        emit loginStatusChanged(loggedIn);
    }

    QString username = loggedIn ? _session->state().username() : "";
    QString nickname = loggedIn ? _session->state().nickname() : i18n("Not logged in");

    _userName->setText(QString("<b>%2</b> <small>%1</small>").arg(username, nickname));

    QString link = loggedIn
        ? "<b><h2><a href='http://" + _session->state().nickname() + ".rajce.net'>"
        "<font color=\"#9ACD32\">Rajce.net</font>"
        "</a></h2></b>"
        : "<b><h2><a href='http://www.rajce.net'>"
        "<font color=\"#9ACD32\">Rajce.net</font>"
        "</a></h2></b>";

    _headerLbl->setText(link);

    disconnect(_albumsCoB, SIGNAL(currentIndexChanged(QString)),
               this, SLOT(selectedAlbumChanged(QString)));

    _albumsCoB->clear();
    Album album;
    int selIdx = 0, i = 0;
    foreach(album, _session->state().albums()) {
        _albumsCoB->addItem(album.name, QVariant::fromValue(album));

        if (_currentAlbumName == album.name) {
            selIdx = i;
        }

        ++i;
    }

    if (!_currentAlbumName.isEmpty()) {
        _albumsCoB->setCurrentIndex(selIdx);
    }

    connect(_albumsCoB, SIGNAL(currentIndexChanged(QString)),
               this, SLOT(selectedAlbumChanged(QString)));

    unsigned max = _session->state().maxHeight();
    max = max > _session->state().maxWidth() ? max : _session->state().maxWidth();
    _dimensionSpB->setMaximum(max);

    if (_dimensionSpB->value() == 0) {
        _dimensionSpB->setValue(max);
    }

    _newAlbumBtn->setEnabled(loggedIn);
    _albumsCoB->setEnabled(loggedIn);
    _reloadAlbumsBtn->setEnabled(loggedIn);
    _dimensionSpB->setEnabled(loggedIn);
    _imageQualitySpB->setEnabled(loggedIn);

    KColorScheme scheme(palette().currentColorGroup(), KColorScheme::Window);
    QColor backgroundColor = scheme.background().color();
    if (_session->state().lastErrorCode() != 0) {
        _progressBar->setVisible(true);
        backgroundColor = scheme.background(KColorScheme::NegativeBackground).color();

        switch (_session->state().lastErrorCode()) {
        case UnknownError: _progressBar->setFormat(i18n("Unknown error")); break;
        case InvalidCommand: _progressBar->setFormat(i18n("Invalid command")); break;
        case InvalidCredentials: _progressBar->setFormat(i18n("Invalid login name or password")); break;
        case InvalidSessionToken: _progressBar->setFormat(i18n("Session expired")); break;
        case InvalidOrRepeatedColumnName: break;
        case InvalidAlbumId: _progressBar->setFormat(i18n("Unknown album")); break;
        case AlbumDoesntExistOrNoPrivileges: _progressBar->setFormat(i18n("Unknown album")); break;
        case InvalidAlbumToken: _progressBar->setFormat(i18n("Failed to open album")); break;
        case FailedToCreateAlbum: _progressBar->setFormat(i18n("Failed to create album")); break;
        case AlbumDoesntExist: _progressBar->setFormat(i18n("Album doesn't exist")); break;
        case UnknownApplication: break;
        case InvalidApplicationKey: break;
        case FileNotAttached: _progressBar->setFormat(i18n("File upload failed")); break;
        case NewerVersionExists: break;
        case SavingFileFailed: _progressBar->setFormat(i18n("File upload failed")); break;
        case UnsupportedFileExtension: _progressBar->setFormat(i18n("Unsupported file extension")); break;
        case UnknownClientVersion: break;
        case NonexistentTarget: break;
        default: break;
        }
    }
    QPalette palette = _progressBar->palette();
    palette.setColor(QPalette::Active, QPalette::Background, backgroundColor);
    _progressBar->setPalette(palette);
}

void RajceWidget::reactivate()
{
    _imgList->listView()->clear();
    _imgList->loadImagesFromCurrentSelection();
    _session->clearLastError();
    update();
}

void RajceWidget::progressChange(unsigned /*commandType*/, unsigned int percent)
{
    if (_uploadingPhotos) {
        unsigned idx = _currentUploadImage - _uploadQueue.begin();

        float perc = (float) idx / _uploadQueue.size();
        perc += percent / 100.0 / _uploadQueue.size();

        percent = perc * 100;
    }

    _progressBar->setValue(percent);
}

void RajceWidget::progressFinished(unsigned)
{
    if (_uploadingPhotos) {
        unsigned idx = _currentUploadImage - _uploadQueue.begin();

        float perc = (float) idx / _uploadQueue.size();

        _progressBar->setValue(perc * 100);
    } else {
        _progressBar->setVisible(false);
        _setEnabled(true);
        update();
    }
}

void RajceWidget::progressStarted(unsigned commandType)
{
    QString text;

    /*    enum RajceCommandType {
     *        Login = 0,
     *        Logout,
     *        ListAlbums,
     *        CreateAlbum,
     *        OpenAlbum,
     *        CloseAlbum,
     *        AddPhoto
    };*/

    switch(commandType) {
        case Login: text = i18n("Logging in"); break;
        case Logout: text = i18n("Logging out"); break;
        case ListAlbums: text = i18n("Loading albums"); break;
        case CreateAlbum: text = i18n("Creating album"); break;
        case OpenAlbum: text = i18n("Opening album"); break;
        case CloseAlbum: text = i18n("Closing album"); break;
        case AddPhoto: text = i18n("Adding photos"); break;
    }

    text += " %v%";

    if (!_uploadingPhotos) {
        _progressBar->setValue(0);
    }

    _progressBar->setFormat(text);
    _progressBar->setVisible(true);
    _setEnabled(false);
}

void RajceWidget::changeUserClicked()
{
    RajceLoginDialog * dlg = new RajceLoginDialog(this, _session->state().username());

    if (dlg->exec() == QDialog::Accepted) {
        _session->clearLastError();
        connect(_session, SIGNAL(busyFinished(unsigned)),
                this, SLOT(loadAlbums()));
        _session->login(dlg->username(), dlg->password());
    }

    delete dlg;
}

void RajceWidget::loadAlbums()
{
    disconnect(_session, SIGNAL(busyFinished(unsigned)),
               this, SLOT(loadAlbums()));

    _session->loadAlbums();
}

void RajceWidget::createAlbum()
{
    NewAlbumDialog * dlg = new NewAlbumDialog(this);

    if (dlg->exec() == QDialog::Accepted) {
        _session->clearLastError();
        connect(_session, SIGNAL(busyFinished(unsigned)),
                this, SLOT(loadAlbums()));
        _session->createAlbum(dlg->albumName(), dlg->albumDescription(), dlg->albumVisible());
    }

    delete dlg;
}

void RajceWidget::startUpload()
{
    _session->clearLastError();
    _setEnabled(false);

    _uploadQueue.clear();
    foreach(KUrl image, _imgList->imageUrls(true)) {
        QString imagePath = image.toLocalFile();
        _uploadQueue.append(imagePath);
    }

    connect(_session, SIGNAL(busyFinished(unsigned)),
        this, SLOT(startUploadAfterAlbumOpened()));

    QString albumName = _albumsCoB->currentText();
    Album album;
    foreach(Album a, _session->state().albums()) {
        if (a.name == albumName) {
            album = a;
            break;
        }
    }

    if (album.name == albumName) {
        _session->openAlbum(album);
    }
}

void RajceWidget::startUploadAfterAlbumOpened()
{
    disconnect(_session, SIGNAL(busyFinished(unsigned)),
               this, SLOT(startUploadAfterAlbumOpened()));

    connect(_session, SIGNAL(busyFinished(unsigned)),
            this, SLOT(uploadNext()));

    _uploadingPhotos = true;
    _progressBar->setValue(0);
    progressStarted(AddPhoto);
    _currentUploadImage = _uploadQueue.begin();
    uploadNext();
}

void RajceWidget::closeAlbum()
{
    _setEnabled(true);

    disconnect(_session, SIGNAL(busyFinished(unsigned)),
               this, SLOT(closeAlbum()));

    _uploadQueue.clear();
    _progressBar->setVisible(false);

    _uploadingPhotos = false;
}

void RajceWidget::uploadNext()
{
    if (_currentUploadImage != _uploadQueue.begin()) {
        _imgList->processed(_session->state().lastErrorCode() == 0);
    }

    if (_currentUploadImage == _uploadQueue.end()) {
        cancelUpload();
        return;
    }

    _imgList->processing(KUrl::fromLocalFile(*_currentUploadImage));

    QString currentPhoto = *_currentUploadImage;
    ++_currentUploadImage;

    unsigned dimension = _dimensionSpB->value();
    int jpgQuality = _imageQualitySpB->value();

    _session->uploadPhoto(currentPhoto, dimension, jpgQuality);
}

void RajceWidget::cancelUpload()
{
    if (_currentUploadImage != _uploadQueue.begin() && _currentUploadImage != _uploadQueue.end()) {
        _imgList->processed(false);
    }

    disconnect(_session, SIGNAL(busyFinished(unsigned)),
            this, SLOT(uploadNext()));

    connect(_session, SIGNAL(busyFinished(unsigned)),
            this, SLOT(closeAlbum()));

    _session->cancelCurrentCommand();

    _session->closeAlbum();

    _uploadQueue.clear();
}

void RajceWidget::selectedAlbumChanged(const QString& newName)
{
    _currentAlbumName = newName;
}

void RajceWidget::_setEnabled(bool enabled)
{
    _changeUserBtn->setEnabled(enabled);
    _accountRBtn->setEnabled(enabled);
    _newAlbumBtn->setEnabled(enabled);
    _albumsCoB->setEnabled(enabled);
    _reloadAlbumsBtn->setEnabled(enabled);
    _dimensionSpB->setEnabled(enabled);
    _imageQualitySpB->setEnabled(enabled);
    emit loginStatusChanged(enabled);
}

void RajceWidget::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("RajceExport Settings");

    SessionState state;

    state.sessionToken() = grp.readEntry("token");
    state.username() = grp.readEntry("username");
    state.nickname() = grp.readEntry("nickname");
    _currentAlbumName = grp.readEntry("album");
    state.maxHeight() = grp.readEntry("maxHeight", 1200);
    state.maxWidth() = grp.readEntry("maxWidth", 1200);
    state.imageQuality() = grp.readEntry("imageQuality", 85);

    _session->init(state);

    if (!_session->state().sessionToken().isEmpty()) {
        _session->loadAlbums();
    }
}

void RajceWidget::writeSettings()
{
    KConfig config("kipirc");
    KConfigGroup grp = config.group("RajceExport Settings");
    const SessionState & state = _session->state();
    grp.writeEntry("token", state.sessionToken());
    grp.writeEntry("username", state.username());
    grp.writeEntry("nickname", state.nickname());
    grp.writeEntry("album", _currentAlbumName);
    grp.writeEntry("maxWidth", state.maxWidth());
    grp.writeEntry("maxHeight", state.maxHeight());
    grp.writeEntry("imageQuality", state.imageQuality());
}

