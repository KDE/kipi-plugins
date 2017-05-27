/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
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

// Qt includes

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
#include <QComboBox>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>
#include <kconfig.h>
#include <kconfiggroup.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/UploadWidget>

// Local includes

#include "rajcesession.h"
#include "newalbumdialog.h"
#include "kpimageslist.h"
#include "kpsettingswidget.h"
#include "kplogindialog.h"

namespace KIPIRajcePlugin
{

RajceWidget::RajceWidget(KIPI::Interface* const interface, const QString& tmpFolder, QWidget* const parent)
    : KPSettingsWidget(parent, interface, QString::fromLatin1("Rajce.net"))
{
    m_lastLoggedInState           = false;
    m_session                     = new RajceSession(this, tmpFolder);

    m_uploadingPhotos = false;
    m_albumsCoB       = getAlbumsCoB();
    m_dimensionSpB    = getDimensionSpB();
    m_imageQualitySpB = getImgQualitySpB();
    m_newAlbumBtn     = getNewAlbmBtn();
    m_reloadAlbumsBtn = getReloadBtn();
    m_progressBar     = progressBar();
    m_imgList         = imagesList();
    m_changeUserBtn   = getChangeUserBtn();

    getUploadBox()->hide();
    getSizeBox()->hide();

    updateLabels();

    // ------------------------------------------------------------------------

    connect(m_session, SIGNAL(busyStarted(uint)),
            this, SLOT(progressStarted(uint)));

    connect(m_session, SIGNAL(busyFinished(uint)),
            this, SLOT(progressFinished(uint)));

    connect(m_session, SIGNAL(busyProgress(uint,uint)),
            this, SLOT(progressChange(uint,uint)));

    connect(m_changeUserBtn, SIGNAL(clicked()),
            this, SLOT(changeUserClicked()));

    connect(m_newAlbumBtn, SIGNAL(clicked()),
            this, SLOT(createAlbum()));

    connect(m_reloadAlbumsBtn, SIGNAL(clicked()),
            this, SLOT(loadAlbums()));

    connect(m_albumsCoB, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(selectedAlbumChanged(QString)));
}

void RajceWidget::updateLabels(const QString&, const QString&)
{
    bool loggedIn = !m_session->state().sessionToken().isEmpty();

    if (loggedIn != m_lastLoggedInState)
    {
        m_lastLoggedInState = loggedIn;
        emit loginStatusChanged(loggedIn);
    }

    QString username = loggedIn ? m_session->state().username() : QString::fromLatin1("");
    QString nickname = loggedIn ? m_session->state().nickname() : i18n("Not logged in");

    getUserNameLabel()->setText(QString::fromLatin1("<b>%2</b> <small>%1</small>").arg(username, nickname));

    QString link = loggedIn
        ? QString::fromLatin1("<b><h2><a href='http://") + m_session->state().nickname() +
        QString::fromLatin1(".rajce.net'>"
        "<font color=\"#9ACD32\">Rajce.net</font>"
        "</a></h2></b>")
        : QString::fromLatin1("<b><h2><a href='http://www.rajce.net'>"
        "<font color=\"#9ACD32\">Rajce.net</font>"
        "</a></h2></b>");

    getHeaderLbl()->setText(link);

    disconnect(m_albumsCoB, SIGNAL(currentIndexChanged(QString)),
               this, SLOT(selectedAlbumChanged(QString)));

    m_albumsCoB->clear();
    Album album;
    int   selIdx = 0, i = 0;

    foreach(album, m_session->state().albums())
    {
        m_albumsCoB->addItem(album.name, QVariant::fromValue(album));

        if (m_currentAlbumName == album.name)
        {
            selIdx = i;
        }

        ++i;
    }

    if (!m_currentAlbumName.isEmpty())
    {
        m_albumsCoB->setCurrentIndex(selIdx);
    }

    connect(m_albumsCoB, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(selectedAlbumChanged(QString)));

    unsigned max = m_session->state().maxHeight();
    max          = max > m_session->state().maxWidth() ? max
                                                       : m_session->state().maxWidth();
    m_dimensionSpB->setMaximum(max);

    if (m_dimensionSpB->value() == 0)
    {
        m_dimensionSpB->setValue(max);
    }

    m_newAlbumBtn->setEnabled(loggedIn);
    m_albumsCoB->setEnabled(loggedIn);
    m_reloadAlbumsBtn->setEnabled(loggedIn);
    m_dimensionSpB->setEnabled(loggedIn);
    m_imageQualitySpB->setEnabled(loggedIn);

    if (m_session->state().lastErrorCode() != 0)
    {
        m_progressBar->setVisible(true);

        switch (m_session->state().lastErrorCode())
        {
            case UnknownError:                   m_progressBar->setFormat(i18n("Unknown error"));                  break;
            case InvalidCommand:                 m_progressBar->setFormat(i18n("Invalid command"));                break;
            case InvalidCredentials:             m_progressBar->setFormat(i18n("Invalid login name or password")); break;
            case InvalidSessionToken:            m_progressBar->setFormat(i18n("Session expired"));                break;
            case InvalidOrRepeatedColumnName:                                                                      break;
            case InvalidAlbumId:                 m_progressBar->setFormat(i18n("Unknown album"));                  break;
            case AlbumDoesntExistOrNoPrivileges: m_progressBar->setFormat(i18n("Unknown album"));                  break;
            case InvalidAlbumToken:              m_progressBar->setFormat(i18n("Failed to open album"));           break;
            case AlbumNameEmpty:                 m_progressBar->setFormat(i18n("The album name cannot be empty")); break;
            case FailedToCreateAlbum:            m_progressBar->setFormat(i18n("Failed to create album"));         break;
            case AlbumDoesntExist:               m_progressBar->setFormat(i18n("Album does not exist"));           break;
            case UnknownApplication:                                                                               break;
            case InvalidApplicationKey:                                                                            break;
            case FileNotAttached:                m_progressBar->setFormat(i18n("File upload failed"));             break;
            case NewerVersionExists:                                                                               break;
            case SavingFileFailed:               m_progressBar->setFormat(i18n("File upload failed"));             break;
            case UnsupportedFileExtension:       m_progressBar->setFormat(i18n("Unsupported file extension"));     break;
            case UnknownClientVersion:                                                                             break;
            case NonexistentTarget:                                                                                break;
            default:                                                                                               break;
        }

        QPalette palette = m_progressBar->palette();
        palette.setColor(QPalette::Active, QPalette::Background, Qt::darkRed);
        m_progressBar->setPalette(palette);
    }
}

void RajceWidget::reactivate()
{
    m_imgList->listView()->clear();
    m_imgList->loadImagesFromCurrentSelection();
    m_session->clearLastError();
    updateLabels();
}

void RajceWidget::progressChange(unsigned /*commandType*/, unsigned int percent)
{
    if (m_uploadingPhotos)
    {
        unsigned idx  = m_currentUploadImage - m_uploadQueue.begin() - 1;
        float perc    = (float)idx / m_uploadQueue.size();
        perc         += (float)percent / 100 / m_uploadQueue.size();
        percent       = perc * 100;
    }

    m_progressBar->setValue(percent);
}

void RajceWidget::progressFinished(unsigned)
{
    if (m_uploadingPhotos)
    {
        unsigned idx = m_currentUploadImage - m_uploadQueue.begin();
        float perc   = (float)idx / m_uploadQueue.size();

        m_progressBar->setValue(perc * 100);
    }
    else
    {
        m_progressBar->setVisible(false);
        _setEnabled(true);
        updateLabels();
    }
}

void RajceWidget::progressStarted(unsigned commandType)
{
    QString text;

/*
    enum RajceCommandType
    {
        Login = 0,
        Logout,
        ListAlbums,
        CreateAlbum,
        OpenAlbum,
        CloseAlbum,
        AddPhoto
    };
*/

    switch(commandType)
    {
        case Login:       text = i18n("Logging in %v%");     break;
        case Logout:      text = i18n("Logging out %v%");    break;
        case ListAlbums:  text = i18n("Loading albums %v%"); break;
        case CreateAlbum: text = i18n("Creating album %v%"); break;
        case OpenAlbum:   text = i18n("Opening album %v%");  break;
        case CloseAlbum:  text = i18n("Closing album %v%");  break;
        case AddPhoto:    text = i18n("Adding photos %v%");  break;
    }

    if (!m_uploadingPhotos)
    {
        m_progressBar->setValue(0);
    }

    m_progressBar->setFormat(text);
    m_progressBar->setVisible(true);
    _setEnabled(false);
}

void RajceWidget::changeUserClicked()
{
    KIPIPlugins::KPLoginDialog* const dlg = new KIPIPlugins::KPLoginDialog(this, QString::fromLatin1("Rajce.net"));

    if (dlg->exec() == QDialog::Accepted)
    {
        m_session->clearLastError();

        connect(m_session, SIGNAL(busyFinished(uint)),
                this, SLOT(loadAlbums()));

        m_session->login(dlg->login(), dlg->password());
    }

    delete dlg;
}

void RajceWidget::loadAlbums()
{
    disconnect(m_session, SIGNAL(busyFinished(uint)),
               this, SLOT(loadAlbums()));

    m_session->loadAlbums();
}

void RajceWidget::createAlbum()
{
    NewAlbumDialog* const dlg = new NewAlbumDialog(this);

    if (dlg->exec() == QDialog::Accepted)
    {
        m_session->clearLastError();

        connect(m_session, SIGNAL(busyFinished(uint)),
                this, SLOT(loadAlbums()));

        m_session->createAlbum(dlg->albumName(), dlg->albumDescription(), dlg->albumVisible());
    }

    delete dlg;
}

void RajceWidget::startUpload()
{
    m_session->clearLastError();
    _setEnabled(false);

    m_uploadQueue.clear();

    foreach(const QUrl& image, m_imgList->imageUrls(true))
    {
        QString imagePath = image.toLocalFile();
        m_uploadQueue.append(imagePath);
    }

    if (m_uploadQueue.isEmpty())
    {
        _setEnabled(true);
        return;
    }

    connect(m_session, SIGNAL(busyFinished(uint)),
            this, SLOT(startUploadAfterAlbumOpened()));

    QString albumName = m_albumsCoB->currentText();
    Album album;

    foreach(Album a, m_session->state().albums())
    {
        if (a.name == albumName)
        {
            album = a;
            break;
        }
    }

    if (album.name == albumName)
    {
        m_session->openAlbum(album);
    }
}

void RajceWidget::startUploadAfterAlbumOpened()
{
    disconnect(m_session, SIGNAL(busyFinished(uint)),
               this, SLOT(startUploadAfterAlbumOpened()));

    connect(m_session, SIGNAL(busyFinished(uint)),
            this, SLOT(uploadNext()));

    m_uploadingPhotos    = true;
    m_progressBar->setValue(0);
    progressStarted(AddPhoto);
    m_currentUploadImage = m_uploadQueue.begin();
    uploadNext();
}

void RajceWidget::closeAlbum()
{
    _setEnabled(true);

    disconnect(m_session, SIGNAL(busyFinished(uint)),
               this, SLOT(closeAlbum()));

    m_uploadQueue.clear();
    m_progressBar->setVisible(false);

    m_uploadingPhotos = false;
}

void RajceWidget::uploadNext()
{
    QList<QString>::Iterator tmp = m_currentUploadImage;

    if (m_currentUploadImage == m_uploadQueue.end())
    {
        m_imgList->processed(QUrl::fromLocalFile(*(--tmp)), (m_session->state().lastErrorCode() == 0));
        cancelUpload();
        return;
    }

    if (m_currentUploadImage != m_uploadQueue.begin())
    {
        m_imgList->processed(QUrl::fromLocalFile(*(--tmp)), (m_session->state().lastErrorCode() == 0));
    }

    m_imgList->processing(QUrl::fromLocalFile(*m_currentUploadImage));

    QString currentPhoto = *m_currentUploadImage;
    ++m_currentUploadImage;

    unsigned dimension   = m_dimensionSpB->value();
    int jpgQuality       = m_imageQualitySpB->value();

    m_session->uploadPhoto(currentPhoto, dimension, jpgQuality);
}

void RajceWidget::cancelUpload()
{
    if (m_uploadingPhotos && m_currentUploadImage != m_uploadQueue.begin() &&
        m_currentUploadImage != m_uploadQueue.end())
    {
        m_imgList->processed(QUrl::fromLocalFile(*m_currentUploadImage), false);
    }

    disconnect(m_session, SIGNAL(busyFinished(uint)),
               this, SLOT(uploadNext()));

    connect(m_session, SIGNAL(busyFinished(uint)),
            this, SLOT(closeAlbum()));

    m_session->cancelCurrentCommand();
    m_session->closeAlbum();
    m_uploadQueue.clear();
}

void RajceWidget::selectedAlbumChanged(const QString& newName)
{
    m_currentAlbumName = newName;
}

void RajceWidget::_setEnabled(bool enabled)
{
    m_changeUserBtn->setEnabled(enabled);
    m_newAlbumBtn->setEnabled(enabled);
    m_albumsCoB->setEnabled(enabled);
    m_reloadAlbumsBtn->setEnabled(enabled);
    m_dimensionSpB->setEnabled(enabled);
    m_imageQualitySpB->setEnabled(enabled);
    emit loginStatusChanged(enabled);
}

void RajceWidget::readSettings()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup grp = config.group("RajceExport Settings");

    SessionState state;

    state.sessionToken() = grp.readEntry("token");
    state.username()     = grp.readEntry("username");
    state.nickname()     = grp.readEntry("nickname");
    m_currentAlbumName   = grp.readEntry("album");
    state.maxHeight()    = grp.readEntry("maxHeight",    1200);
    state.maxWidth()     = grp.readEntry("maxWidth",     1200);
    state.imageQuality() = grp.readEntry("imageQuality", 85);

    m_session->init(state);

    if (!m_session->state().sessionToken().isEmpty())
    {
        m_session->loadAlbums();
    }
}

void RajceWidget::writeSettings()
{
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup grp          = config.group("RajceExport Settings");
    const SessionState& state = m_session->state();

    grp.writeEntry("token",        state.sessionToken());
    grp.writeEntry("username",     state.username());
    grp.writeEntry("nickname",     state.nickname());
    grp.writeEntry("album",        m_currentAlbumName);
    grp.writeEntry("maxWidth",     state.maxWidth());
    grp.writeEntry("maxHeight",    state.maxHeight());
    grp.writeEntry("imageQuality", state.imageQuality());
}

} // namespace KIPIRajcePlugin
