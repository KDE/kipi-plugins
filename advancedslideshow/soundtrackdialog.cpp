/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
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
#include "soundtrackdialog.moc"
#ifdef Q_WS_X11
#include <fixx11h.h>
#endif
// Phonon includes

#include <Phonon/BackendCapabilities>
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>

// KDE includes

#include <kfile.h>
#include <kicon.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>

// Local includes

#include "playbackwidget.h"

namespace KIPIAdvancedSlideshowPlugin
{

// ===================

SoundtrackPreview::SoundtrackPreview( QWidget* parent, KUrl::List& urls, SharedData* sharedData )
        : KDialog(parent)
{

    setModal(true);
    setButtons(KDialog::Close);
    setCaption( i18n("Soundtrack preview") );

    m_playbackWidget = new PlaybackWidget(this, urls, sharedData);
    setMainWidget(m_playbackWidget);
}

SoundtrackPreview::~SoundtrackPreview()
{
//    delete m_playbackWidget;
}

// ===================

SoundtrackDialog::SoundtrackDialog( QWidget* parent, SharedData* sharedData )
        : QWidget(parent)
{
    setupUi(this);

    m_sharedData = sharedData;
    m_totalTime  = QTime(0, 0, 0);
    m_imageTime  = QTime(0, 0, 0);
    m_tracksTime = new QMap<KUrl, QTime>();
    m_soundItems = new QMap<KUrl, SoundItem*>();
    m_timeMutex  = new QMutex();

    m_soundtrackTimeLabel->setText(m_totalTime.toString());
    m_previewButton->setEnabled(false);

    m_SoundFilesButtonUp->setIcon(KIcon("arrow-up"));
    m_SoundFilesButtonDown->setIcon(KIcon("arrow-down"));
    m_SoundFilesButtonAdd->setIcon(KIcon("list-add"));
    m_SoundFilesButtonDelete->setIcon(KIcon("list-remove"));

    m_SoundFilesButtonUp->setText("");
    m_SoundFilesButtonDown->setText("");
    m_SoundFilesButtonAdd->setText("");
    m_SoundFilesButtonDelete->setText("");


    connect( m_SoundFilesListBox, SIGNAL( currentRowChanged( int ) ),
             this, SLOT( slotSoundFilesSelected( int ) ) );
    connect( m_SoundFilesListBox, SIGNAL( addedDropItems(KUrl::List) ),
             this, SLOT( slotAddDropItems(KUrl::List)));
    connect( m_SoundFilesButtonAdd, SIGNAL( clicked() ),
             this, SLOT( slotSoundFilesButtonAdd() ) );
    connect( m_SoundFilesButtonDelete, SIGNAL( clicked() ),
             this, SLOT( slotSoundFilesButtonDelete() ) );
    connect( m_SoundFilesButtonUp, SIGNAL( clicked() ),
             this, SLOT( slotSoundFilesButtonUp() ) );
    connect( m_SoundFilesButtonDown, SIGNAL( clicked() ),
             this, SLOT( slotSoundFilesButtonDown() ) );

    connect( m_previewButton, SIGNAL( clicked() ),
             this, SLOT( slotPreviewButtonClicked() ));

    connect( m_sharedData->mainPage, SIGNAL(totalTimeChanged(QTime)),
             this, SLOT( slotImageTotalTimeChanged(QTime)));
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
    m_loopCheckBox->setChecked(m_sharedData->soundtrackLoop);

    connect( m_sharedData->mainPage, SIGNAL(totalTimeChanged(QTime)),
             this, SLOT(slotImageTotalTimeChanged(QTime) ) );

    updateTracksNumber();
}

void SoundtrackDialog::saveSettings()
{
    m_sharedData->soundtrackLoop = m_loopCheckBox->isChecked();
    m_sharedData->soundtrackUrls = m_urlList;
}

void SoundtrackDialog::addItems(const KUrl::List& fileList)
{
    if (fileList.isEmpty()) return;

    KUrl::List Files = fileList;

    for ( KUrl::List::ConstIterator it = Files.constBegin() ; it != Files.constEnd() ; ++it )
    {
        KUrl currentFile = *it;
        KUrl path = KUrl(currentFile.path().section('/', 0, -1));
        m_sharedData->soundtrackPath = path;
        SoundItem *item = new SoundItem( m_SoundFilesListBox, path );
        item->setName( currentFile.path().section('/', -1) );
        m_SoundFilesListBox->insertItem(m_SoundFilesListBox->count() - 1, item);

        m_soundItems->insert(path, item);

        connect(m_soundItems->value(path), SIGNAL(totalTimeReady(KUrl, QTime)),
                this, SLOT(slotAddNewTime(KUrl, QTime)));

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

        for ( QMap<KUrl, QTime>::iterator it = m_tracksTime->begin(); it != m_tracksTime->end(); it++ )
        {
            int hours = it.value().hour() + displayTime.hour();
            int mins = it.value().minute() + displayTime.minute();
            int secs = it.value().second() + displayTime.second();

            /* QTime doesn't get a overflow value in imput. They need
             * to be cutted down to size.
             */

            mins = mins + (int)(secs / 60);
            secs = secs % 60;
            hours = hours + (int)(mins / 60);

            displayTime = QTime(hours, mins, secs);
        }
    }

    m_timeLabel->setText(i18np("1 track [%2]", "%1 tracks [%2]", number, displayTime.toString()));

    m_soundtrackTimeLabel->setText(displayTime.toString());

    m_totalTime = displayTime;

    compareTimes();
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
        m_statusBarLabel->setText("");

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

void SoundtrackDialog::slotAddNewTime(KUrl url, QTime trackTime)
{
    m_timeMutex->lock();
    m_tracksTime->insert(url, trackTime);
    updateTracksNumber();
    m_timeMutex->unlock();
}

void SoundtrackDialog::slotSoundFilesSelected( int row )
{
    QListWidgetItem* item = m_SoundFilesListBox->item(row);

    if ( !item || m_SoundFilesListBox->count() == 0 )
    {
        return;
    }
}

void SoundtrackDialog::slotAddDropItems(KUrl::List filesUrl)
{
    addItems(filesUrl);
}

void SoundtrackDialog::slotSoundFilesButtonAdd( void )
{
    KFileDialog dlg(m_sharedData->soundtrackPath, "", this);

    // Setting available mime-types (filtering out non audio mime-types)
    dlg.setMimeFilter( Phonon::BackendCapabilities::availableMimeTypes().filter("audio/") );

    dlg.setOperationMode(KFileDialog::Opening);
    dlg.setMode( KFile::Files );
    dlg.setWindowTitle(i18n("Select sound files"));
    dlg.exec();

    KUrl::List urls = dlg.selectedUrls();

    if (!urls.isEmpty())
    {
        addItems(urls);
    }

}

void SoundtrackDialog::slotSoundFilesButtonDelete( void )
{
    int Index = m_SoundFilesListBox->currentRow();
    if( Index < 0 )
       return;
    SoundItem* pitem = static_cast<SoundItem*>(m_SoundFilesListBox->takeItem(Index));
    m_urlList.removeAll(pitem->url());
    m_soundItems->remove(pitem->url());
    m_timeMutex->lock();
    m_tracksTime->remove(pitem->url());
    updateTracksNumber();
    m_timeMutex->unlock();
    delete pitem;
    slotSoundFilesSelected(m_SoundFilesListBox->currentRow());

    if (m_SoundFilesListBox->count() == 0) m_previewButton->setEnabled(false);
}

void SoundtrackDialog::slotSoundFilesButtonUp( void )
{
    int Cpt = 0;

    for (int i = 0 ; i < m_SoundFilesListBox->count() ; ++i)
        if (m_SoundFilesListBox->currentRow() == i)
            ++Cpt;

    if  (Cpt == 0)
        return;

    if  (Cpt > 1)
    {
        KMessageBox::error(this, i18n("You can only move image files up one at a time."));
        return;
    }

    unsigned int Index = m_SoundFilesListBox->currentRow();

    if (Index == 0)
        return;

    SoundItem* pitem = static_cast<SoundItem*>(m_SoundFilesListBox->takeItem(Index));

    m_SoundFilesListBox->insertItem(Index - 1, pitem);

    m_SoundFilesListBox->setCurrentItem(pitem);
}

void SoundtrackDialog::slotSoundFilesButtonDown( void )
{
    int Cpt = 0;

    for (int i = 0 ; i < m_SoundFilesListBox->count() ; ++i)
        if (m_SoundFilesListBox->currentRow() == i)
            ++Cpt;

    if (Cpt == 0)
        return;

    if (Cpt > 1)
    {
        KMessageBox::error(this, i18n("You can only move files down one at a time."));
        return;
    }

    int Index = m_SoundFilesListBox->currentRow();

    if (Index == m_SoundFilesListBox->count())
        return;

    SoundItem* pitem = static_cast<SoundItem*>(m_SoundFilesListBox->takeItem(Index));

    m_SoundFilesListBox->insertItem(Index + 1, pitem);

    m_SoundFilesListBox->setCurrentItem(pitem);
}

void SoundtrackDialog::slotPreviewButtonClicked( void )
{
    KUrl::List urlList;

    for (int i = 0 ; i < m_SoundFilesListBox->count() ; ++i)
    {
        SoundItem *pitem = static_cast<SoundItem*>( m_SoundFilesListBox->item(i) );
        QString path = pitem->url().path();

        if (!QFile::exists(path))
        {
            KMessageBox::error(this,
                               i18n("Cannot access file %1. Please check the path is correct.", path));
            return;
        }

        urlList.append(path);  // Input sound files.
    }

    if ( urlList.isEmpty() )
    {
        KMessageBox::error(this,
                           i18n("Cannot create a preview of an empty file list."));
        return;
    }

    // Update SharedData from interface
    saveSettings();

    SoundtrackPreview* preview = new SoundtrackPreview(this, urlList, m_sharedData);

    preview->exec();

    delete preview;

    return;
}

void SoundtrackDialog::slotImageTotalTimeChanged( QTime imageTotalTime )
{
    m_imageTime = imageTotalTime;
    m_slideTimeLabel->setText(imageTotalTime.toString());
    compareTimes();
}

} // namespace KIPIAdvancedSlideshowPlugin
