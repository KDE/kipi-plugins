/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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

#include "soundtrackdialog.h"

#ifdef HAVE_X11
#include <fixx11h.h>
#endif

// Qt includes

#include <QPointer>
#include <QTime>
#include <QIcon>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialogButtonBox>

// Phonon includes

#include <phonon/backendcapabilities.h>

// Local includes

#include "playbackwidget.h"
#include "maindialog.h"
#include "commoncontainer.h"

namespace KIPIAdvancedSlideshowPlugin
{

SoundtrackPreview::SoundtrackPreview(QWidget* const parent, QList<QUrl>& urls, SharedContainer* const sharedData)
    : QDialog(parent)
{
    setModal(true);
    setWindowTitle(i18n("Soundtrack preview"));

    m_playbackWidget                  = new PlaybackWidget(this, urls, sharedData);
    QDialogButtonBox* const buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);

    connect(buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    QVBoxLayout* const layout = new QVBoxLayout(this);
    layout->addWidget(m_playbackWidget);
    layout->addWidget(buttonBox);
    setLayout(layout);
}

SoundtrackPreview::~SoundtrackPreview()
{
}

// ------------------------------------------------------------------------------------

SoundtrackDialog::SoundtrackDialog(QWidget* const parent, SharedContainer* const sharedData)
    : QWidget(parent)
{
    setupUi(this);

    m_sharedData = sharedData;
    m_totalTime  = QTime(0, 0, 0);
    m_imageTime  = QTime(0, 0, 0);
    m_tracksTime = new QMap<QUrl, QTime>();
    m_soundItems = new QMap<QUrl, SoundItem*>();
    m_timeMutex  = new QMutex();

    m_soundtrackTimeLabel->setText(m_totalTime.toString());
    m_previewButton->setEnabled(false);

    m_rememberSoundtrack->setToolTip(i18n("If set, the soundtrack for the current album "
                                          "will be saved and restored automatically on the next startup."));

    // --------------------------------------------------------

    m_SoundFilesButtonUp->setIcon(QIcon::fromTheme(QStringLiteral("arrow-up")));
    m_SoundFilesButtonDown->setIcon(QIcon::fromTheme(QStringLiteral("arrow-down")));
    m_SoundFilesButtonAdd->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));
    m_SoundFilesButtonDelete->setIcon(QIcon::fromTheme(QStringLiteral("list-remove")));
    m_SoundFilesButtonLoad->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    m_SoundFilesButtonSave->setIcon(QIcon::fromTheme(QStringLiteral("document-save")));
    m_SoundFilesButtonReset->setIcon(QIcon::fromTheme(QStringLiteral("edit-clear-list")));

    m_SoundFilesButtonUp->setText(QString());
    m_SoundFilesButtonDown->setText(QString());
    m_SoundFilesButtonAdd->setText(QString());
    m_SoundFilesButtonDelete->setText(QString());
    m_SoundFilesButtonLoad->setText(QString());
    m_SoundFilesButtonSave->setText(QString());
    m_SoundFilesButtonReset->setText(QString());

    m_SoundFilesButtonUp->setToolTip(i18n("Move the selected track up in the playlist."));
    m_SoundFilesButtonDown->setToolTip(i18n("Move the selected track down in the playlist."));
    m_SoundFilesButtonAdd->setToolTip(i18n("Add new tracks to the playlist."));
    m_SoundFilesButtonDelete->setToolTip(i18n("Delete the selected track from the playlist."));
    m_SoundFilesButtonLoad->setToolTip(i18n("Load playlist from a file."));
    m_SoundFilesButtonSave->setToolTip(i18n("Save playlist to a file."));
    m_SoundFilesButtonReset->setToolTip(i18n("Clear the playlist."));

    // --------------------------------------------------------

    connect( m_SoundFilesListBox, SIGNAL(currentRowChanged(int)),
             this, SLOT(slotSoundFilesSelected(int)) );

    connect( m_SoundFilesListBox, SIGNAL(signalAddedDropItems(QList<QUrl>)),
             this, SLOT(slotAddDropItems(QList<QUrl>)));

    connect( m_SoundFilesButtonAdd, SIGNAL(clicked()),
             this, SLOT(slotSoundFilesButtonAdd()) );

    connect( m_SoundFilesButtonDelete, SIGNAL(clicked()),
             this, SLOT(slotSoundFilesButtonDelete()) );

    connect( m_SoundFilesButtonUp, SIGNAL(clicked()),
             this, SLOT(slotSoundFilesButtonUp()) );

    connect( m_SoundFilesButtonDown, SIGNAL(clicked()),
             this, SLOT(slotSoundFilesButtonDown()) );

    connect( m_SoundFilesButtonLoad, SIGNAL(clicked()),
             this, SLOT(slotSoundFilesButtonLoad()) );

    connect( m_SoundFilesButtonSave, SIGNAL(clicked()),
             this, SLOT(slotSoundFilesButtonSave()) );

    connect( m_SoundFilesButtonReset, SIGNAL(clicked()),
             this, SLOT(slotSoundFilesButtonReset()) );

    connect( m_previewButton, SIGNAL(clicked()),
             this, SLOT(slotPreviewButtonClicked()));

    connect( m_sharedData->mainPage, SIGNAL(signalTotalTimeChanged(QTime)),
             this, SLOT(slotImageTotalTimeChanged(QTime)));
}

SoundtrackDialog::~SoundtrackDialog()
{
    delete m_sharedData;
    delete m_tracksTime;
    delete m_soundItems;
    delete m_timeMutex;
}

void SoundtrackDialog::readSettings()
{
    m_rememberSoundtrack->setChecked(m_sharedData->soundtrackRememberPlaylist);
    m_loopCheckBox->setChecked(m_sharedData->soundtrackLoop);

    connect( m_sharedData->mainPage, SIGNAL(signalTotalTimeChanged(QTime)),
             this, SLOT(slotImageTotalTimeChanged(QTime)) );

    // if tracks are already set in m_sharedData, add them now
    if (!m_sharedData->soundtrackUrls.isEmpty())
        addItems(m_sharedData->soundtrackUrls);

    updateFileList();
    updateTracksNumber();
}

void SoundtrackDialog::saveSettings()
{
    m_sharedData->soundtrackRememberPlaylist = m_rememberSoundtrack->isChecked();
    m_sharedData->soundtrackLoop             = m_loopCheckBox->isChecked();
    m_sharedData->soundtrackUrls             = m_urlList;
}

void SoundtrackDialog::addItems(const QList<QUrl>& fileList)
{
    if (fileList.isEmpty())
        return;

    QList<QUrl> Files = fileList;

    for (QList<QUrl>::ConstIterator it = Files.constBegin(); it != Files.constEnd(); ++it)
    {
        QUrl currentFile             = *it;
        QUrl path                    = QUrl(currentFile.path().section(QLatin1Char('/'), 0, -1));
        m_sharedData->soundtrackPath = path;
        SoundItem* const item        = new SoundItem(m_SoundFilesListBox, path);
        item->setName(currentFile.path().section(QLatin1Char('/'), -1));
        m_SoundFilesListBox->insertItem(m_SoundFilesListBox->count() - 1, item);

        m_soundItems->insert(path, item);

        connect(m_soundItems->value(path), SIGNAL(signalTotalTimeReady(QUrl,QTime)),
                this, SLOT(slotAddNewTime(QUrl,QTime)));

        m_urlList.append(path);
    }

    m_SoundFilesListBox->setCurrentItem(m_SoundFilesListBox->item(m_SoundFilesListBox->count() - 1)) ;

    slotSoundFilesSelected(m_SoundFilesListBox->currentRow());
    m_SoundFilesListBox->scrollToItem(m_SoundFilesListBox->currentItem());
    m_previewButton->setEnabled(true);
}

void SoundtrackDialog::updateTracksNumber()
{
    QTime displayTime(0, 0, 0);
    int number = m_SoundFilesListBox->count();

    if ( number > 0 )
    {
        displayTime.addMSecs(1000 * (number - 1));

        for (QMap<QUrl, QTime>::iterator it = m_tracksTime->begin(); it != m_tracksTime->end(); ++it)
        {
            int hours = it.value().hour()   + displayTime.hour();
            int mins  = it.value().minute() + displayTime.minute();
            int secs  = it.value().second() + displayTime.second();

            /* QTime doesn't get a overflow value in input. They need
             * to be cut down to size.
             */

            mins        = mins + (int)(secs / 60);
            secs        = secs % 60;
            hours       = hours + (int)(mins / 60);
            displayTime = QTime(hours, mins, secs);
        }
    }

    m_timeLabel->setText(i18ncp("number of tracks and running time", "1 track [%2]", "%1 tracks [%2]", number, displayTime.toString()));

    m_soundtrackTimeLabel->setText(displayTime.toString());

    m_totalTime = displayTime;

    compareTimes();
}

void SoundtrackDialog::updateFileList()
{
    QList<QUrl> files = m_SoundFilesListBox->fileUrls();
    m_urlList         = files;

    m_SoundFilesButtonUp->setEnabled(!files.isEmpty());
    m_SoundFilesButtonDown->setEnabled(!files.isEmpty());
    m_SoundFilesButtonDelete->setEnabled(!files.isEmpty());
    m_SoundFilesButtonSave->setEnabled(!files.isEmpty());
    m_SoundFilesButtonReset->setEnabled(!files.isEmpty());
    m_sharedData->soundtrackPlayListNeedsUpdate = true;
}

void SoundtrackDialog::compareTimes()
{
    QFont statusBarFont = m_statusBarLabel->font();

    if ( m_imageTime > m_totalTime )
    {
        m_statusBarLabel->setText(i18n("Slide time is greater than soundtrack time. Suggestion: add more sound files."));


        QPalette paletteStatusBar = m_statusBarLabel->palette();
        paletteStatusBar.setColor(QPalette::WindowText, Qt::red);
        m_statusBarLabel->setPalette(paletteStatusBar);

        QPalette paletteTimeLabel = m_soundtrackTimeLabel->palette();
        paletteTimeLabel.setColor(QPalette::WindowText, Qt::red);
        m_soundtrackTimeLabel->setPalette(paletteTimeLabel);

        statusBarFont.setItalic(true);
    }
    else
    {
        m_statusBarLabel->setText(QStringLiteral(""));

        QPalette paletteStatusBar = m_statusBarLabel->palette();
        paletteStatusBar.setColor(QPalette::WindowText, Qt::red);
        m_statusBarLabel->setPalette(paletteStatusBar);

        QPalette paletteTimeLabel = m_soundtrackTimeLabel->palette();

        if ( m_imageTime < m_totalTime )
            paletteTimeLabel.setColor(QPalette::WindowText, Qt::black);
        else
            paletteTimeLabel.setColor(QPalette::WindowText, Qt::green);

        m_soundtrackTimeLabel->setPalette(paletteTimeLabel);

        statusBarFont.setItalic(false);
    }

    m_statusBarLabel->setFont(statusBarFont);
}

void SoundtrackDialog::slotAddNewTime(const QUrl& url, const QTime& trackTime)
{
    m_timeMutex->lock();
    m_tracksTime->insert(url, trackTime);
    updateTracksNumber();
    m_timeMutex->unlock();
}

void SoundtrackDialog::slotSoundFilesSelected( int row )
{
    QListWidgetItem* const item = m_SoundFilesListBox->item(row);

    if ( !item || m_SoundFilesListBox->count() == 0 )
    {
        return;
    }
}

void SoundtrackDialog::slotAddDropItems(const QList<QUrl>& filesUrl)
{
    if (!filesUrl.isEmpty())
    {
        addItems(filesUrl);
        updateFileList();
    }
}

void SoundtrackDialog::slotSoundFilesButtonAdd()
{
    QPointer<QFileDialog> dlg = new QFileDialog(this,
                                                i18n("Select sound files"),
                                                m_sharedData->soundtrackPath.toString());

    // Setting available mime-types (filtering out non audio mime-types)
    dlg->setMimeTypeFilters(Phonon::BackendCapabilities::availableMimeTypes().filter(QStringLiteral("audio/")));
    dlg->setAcceptMode(QFileDialog::AcceptOpen);
    dlg->setFileMode(QFileDialog::ExistingFiles);
    dlg->exec();

    QList<QUrl> urls = dlg->selectedUrls();

    if (!urls.isEmpty())
    {
        addItems(urls);
        updateFileList();
    }

    delete dlg;
}

void SoundtrackDialog::slotSoundFilesButtonDelete()
{
    int Index = m_SoundFilesListBox->currentRow();

    if( Index < 0 )
       return;

    SoundItem* const pitem = static_cast<SoundItem*>(m_SoundFilesListBox->takeItem(Index));
    m_urlList.removeAll(pitem->url());
    m_soundItems->remove(pitem->url());
    m_timeMutex->lock();
    m_tracksTime->remove(pitem->url());
    updateTracksNumber();
    m_timeMutex->unlock();
    delete pitem;
    slotSoundFilesSelected(m_SoundFilesListBox->currentRow());

    if (m_SoundFilesListBox->count() == 0)
        m_previewButton->setEnabled(false);

    updateFileList();
}

void SoundtrackDialog::slotSoundFilesButtonUp()
{
    int Cpt = 0;

    for (int i = 0 ; i < m_SoundFilesListBox->count() ; ++i)
    {
        if (m_SoundFilesListBox->currentRow() == i)
            ++Cpt;
    }

    if (Cpt == 0)
    {
        return;
    }

    if (Cpt > 1)
    {
        QMessageBox::critical(this, QString(), i18n("You can only move image files up one at a time."));
        return;
    }

    unsigned int Index = m_SoundFilesListBox->currentRow();

    if (Index == 0)
    {
        return;
    }

    SoundItem* const pitem = static_cast<SoundItem*>(m_SoundFilesListBox->takeItem(Index));

    m_SoundFilesListBox->insertItem(Index - 1, pitem);
    m_SoundFilesListBox->setCurrentItem(pitem);

    updateFileList();
}

void SoundtrackDialog::slotSoundFilesButtonDown()
{
    int Cpt = 0;

    for (int i = 0 ; i < m_SoundFilesListBox->count() ; ++i)
    {
        if (m_SoundFilesListBox->currentRow() == i)
            ++Cpt;
    }

    if (Cpt == 0)
    {
        return;
    }

    if (Cpt > 1)
    {
        QMessageBox::critical(this, QString(), i18n("You can only move files down one at a time."));
        return;
    }

    int Index = m_SoundFilesListBox->currentRow();

    if (Index == m_SoundFilesListBox->count())
    {
        return;
    }

    SoundItem* const pitem = static_cast<SoundItem*>(m_SoundFilesListBox->takeItem(Index));

    m_SoundFilesListBox->insertItem(Index + 1, pitem);
    m_SoundFilesListBox->setCurrentItem(pitem);

    updateFileList();
}

void SoundtrackDialog::slotSoundFilesButtonLoad()
{
    QPointer<QFileDialog> dlg = new QFileDialog(this, i18n("Load playlist"),
                                                QString(), i18n("Playlist (*.m3u)"));
    dlg->setAcceptMode(QFileDialog::AcceptOpen);
    dlg->setFileMode(QFileDialog::ExistingFile);

    if (dlg->exec() != QDialog::Accepted)
    {
        delete dlg;
        return;
    }

    QString filename = dlg->selectedFiles().isEmpty() ? QString() : dlg->selectedFiles().at(0);

    if (!filename.isEmpty())
    {
        QFile file(filename);

        if (file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QTextStream in(&file);
            QList<QUrl> playlistFiles;

            while (!in.atEnd())
            {
                QString line = in.readLine();

                // we ignore the extended information of the m3u playlist file
                if (line.startsWith(QLatin1Char('#')) || line.isEmpty())
                    continue;

                QUrl fUrl(line);

                if (fUrl.isValid())
                {
                    if (fUrl.isLocalFile())
                    {
                        playlistFiles << fUrl;
                    }
                }
            }

            if (!playlistFiles.isEmpty())
            {
                m_SoundFilesListBox->clear();
                addItems(playlistFiles);
                updateFileList();
            }
        }
    }

    delete dlg;
}

void SoundtrackDialog::slotSoundFilesButtonSave()
{
    QPointer<QFileDialog> dlg = new QFileDialog(this, i18n("Save playlist"),
                                                QString(), i18n("Playlist (*.m3u)"));
    dlg->setAcceptMode(QFileDialog::AcceptSave);
    dlg->setFileMode(QFileDialog::AnyFile);

    if (dlg->exec() != QDialog::Accepted)
    {
        delete dlg;
        return;
    }

    QString filename = dlg->selectedFiles().isEmpty() ? QString() : dlg->selectedFiles().at(0);

    if (!filename.isEmpty())
    {
        QFile file(filename);

        if (file.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            QTextStream out(&file);
            QList<QUrl> playlistFiles = m_SoundFilesListBox->fileUrls();

            for (int i = 0; i < playlistFiles.count(); ++i)
            {
                QUrl fUrl(playlistFiles.at(i));

                if (fUrl.isValid())
                {
                    if (fUrl.isLocalFile())
                    {
                        out << fUrl.toLocalFile() << endl;
                    }
                }
            }

            file.close();
        }
    }

    delete dlg;
}

void SoundtrackDialog::slotSoundFilesButtonReset()
{
    m_SoundFilesListBox->clear();
    updateFileList();
}

void SoundtrackDialog::slotPreviewButtonClicked()
{
    QList<QUrl> urlList;

    for (int i = 0 ; i < m_SoundFilesListBox->count() ; ++i)
    {
        SoundItem* const pitem = static_cast<SoundItem*>( m_SoundFilesListBox->item(i) );
        QString path           = pitem->url().toLocalFile();

        if (!QFile::exists(path))
        {
            QMessageBox::critical(this, QString(), i18n("Cannot access file %1. Please check the path is correct.", path));
            return;
        }

        urlList.append(QUrl::fromLocalFile(path));  // Input sound files.
    }

    if ( urlList.isEmpty() )
    {
        QMessageBox::critical(this, QString(), i18n("Cannot create a preview of an empty file list."));
        return;
    }

    // Update SharedContainer from interface
    saveSettings();

    QPointer<SoundtrackPreview> preview = new SoundtrackPreview(this, urlList, m_sharedData);
    preview->exec();

    delete preview;
    return;
}

void SoundtrackDialog::slotImageTotalTimeChanged( const QTime& imageTotalTime )
{
    m_imageTime = imageTotalTime;
    m_slideTimeLabel->setText(imageTotalTime.toString());
    compareTimes();
}

} // namespace KIPIAdvancedSlideshowPlugin
