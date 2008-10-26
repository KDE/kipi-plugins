/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-09-09
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

#include "maindialog.h"
#include "maindialog.moc"

// Qt includes.

#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTime>

// KDE includes.

#include <kdebug.h>
#include <kicon.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kurl.h>

// Local includes.

#include "imagedialog.h"
#include "imageviewer.h"
#include "listimageitems.h"
#include "slideshow.h"
#include "slideshowgl.h"
#include "slideshowkb.h"

namespace KIPISlideShowPlugin
{

MainDialog::MainDialog( QWidget* parent, SharedData* sharedData)
        : QWidget(parent)
{
    setupUi(this);

    m_sharedData = sharedData;
    m_thumbJob = 0L;

    m_ImagesFilesButtonUp->setIcon(KIcon("arrow-up"));
    m_ImagesFilesButtonDown->setIcon(KIcon("arrow-down"));
    m_ImagesFilesButtonAdd->setIcon(KIcon("list-add"));
    m_ImagesFilesButtonDelete->setIcon(KIcon("list-remove"));

    m_ImagesFilesButtonUp->setText("");
    m_ImagesFilesButtonDown->setText("");
    m_ImagesFilesButtonAdd->setText("");
    m_ImagesFilesButtonDelete->setText("");

}

MainDialog::~MainDialog()
{
//    if (m_thumbJob) delete m_thumbJob;
}

void MainDialog::readSettings()
{
    connect(m_sharedData->advancedPage, SIGNAL(useMillisecondsToggled()), this, SLOT(slotUseMillisecondsToggled()));
    connect(m_printCommentsCheckBox, SIGNAL(toggled(bool)), this, SLOT(slotPrintCommentsToggled()));

    connect(m_allFilesButton, SIGNAL(toggled(bool)), this, SLOT(slotSelection()));

    connect(m_delaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotDelayChanged()));
    connect(m_effectsComboBox, SIGNAL(activated(int)), this,  SLOT(slotEffectChanged()));

    connect( m_ImagesFilesListBox, SIGNAL( currentRowChanged( int ) ),
             this, SLOT( slotImagesFilesSelected( int ) ) );
    connect( m_ImagesFilesListBox, SIGNAL( addedDropItems(KUrl::List) ),
             this, SLOT( slotAddDropItems(KUrl::List)));
    connect( m_ImagesFilesButtonAdd, SIGNAL( clicked() ),
             this, SLOT( slotImagesFilesButtonAdd() ) );
    connect( m_ImagesFilesButtonDelete, SIGNAL( clicked() ),
             this, SLOT( slotImagesFilesButtonDelete() ) );
    connect( m_ImagesFilesButtonUp, SIGNAL( clicked() ),
             this, SLOT( slotImagesFilesButtonUp() ) );
    connect( m_ImagesFilesButtonDown, SIGNAL( clicked() ),
             this, SLOT( slotImagesFilesButtonDown() ) );

    m_openglCheckBox->setChecked(m_sharedData->opengl);
    m_delaySpinBox->setValue(m_sharedData->delay);
    m_printNameCheckBox->setChecked(m_sharedData->printFileName);
    m_printProgressCheckBox->setChecked(m_sharedData->printProgress);
    m_printCommentsCheckBox->setChecked(m_sharedData->printFileComments);
    m_loopCheckBox->setChecked(m_sharedData->loop);
    m_shuffleCheckBox->setChecked(m_sharedData->shuffle);

    if (m_sharedData->showSelectedFilesOnly && m_selectedFilesButton->isEnabled() )
        m_selectedFilesButton->setChecked(true);
    else
        m_allFilesButton->setChecked(true);

    // Host application images has comments
    if ( ! m_sharedData->ImagesHasComments )
    {
        m_printCommentsCheckBox->setEnabled(FALSE);
        m_printCommentsCheckBox->setChecked(FALSE);
    }

    // -----------------------
    // Disable normal effects
    // TODO: find a way to make them work again.

    m_openglCheckBox->setChecked(true);

    m_openglCheckBox->setEnabled(false);

    m_openglCheckBox->setToolTip(i18n("In this version of kipi-plugins, normal SlideShow's effects "
                                      "still under porting.<br/>"
                                      "<b>They will be available again in the future.</b>"));

    //----------------------

    // Switch to selected files only (it depends on showSelectedFilesOnly)

    m_selectedFilesButton->setEnabled( m_sharedData->showSelectedFilesOnly );


    m_delaySpinBox->setValue(m_sharedData->useMilliseconds ?
                             m_sharedData->delay : m_sharedData->delay / 1000 );

    slotUseMillisecondsToggled();

    // ---------------------

    if ( m_sharedData->showSelectedFilesOnly )
        connect(m_selectedFilesButton, SIGNAL(toggled(bool)), this, SLOT(slotSelection()));

    slotOpenGLToggled();

    slotPrintCommentsToggled();

    slotSelection();

    slotEffectChanged();

}

void MainDialog::saveSettings()
{
    m_sharedData->opengl  = m_openglCheckBox->isChecked();

    m_sharedData->delay   = m_sharedData->useMilliseconds ?
                            m_delaySpinBox->value() :
                            m_delaySpinBox->value() * 1000;

    m_sharedData->printFileName     = m_printNameCheckBox->isChecked();
    m_sharedData->printProgress     = m_printProgressCheckBox->isChecked();
    m_sharedData->printFileComments = m_printCommentsCheckBox->isChecked();

    m_sharedData->loop    = m_loopCheckBox->isChecked();
    m_sharedData->shuffle = m_shuffleCheckBox->isChecked();

    m_sharedData->showSelectedFilesOnly = m_selectedFilesButton->isChecked();

    if (!m_openglCheckBox->isChecked())
    {

        QString effect;
        QMap<QString, QString> effectNames = SlideShow::effectNamesI18N();
        QMap<QString, QString>::Iterator it;

        for (it = effectNames.begin(); it != effectNames.end(); ++it)
        {
            if (it.value() == m_effectsComboBox->currentText())
            {
                effect = it.key();
                break;
            }
        }

        m_sharedData->effectName = effect;
    }
    else
    {
        QMap<QString, QString> effects;
        QMap<QString, QString> effectNames;
        QMap<QString, QString>::Iterator it;

        // Load slideshowgl effects
        effectNames = SlideShowGL::effectNamesI18N();

        for (it = effectNames.begin(); it != effectNames.end(); ++it)
            effects.insert(it.key(), it.value());

        // Load Ken Burns effect
        effectNames = SlideShowKB::effectNamesI18N();

        for (it = effectNames.begin(); it != effectNames.end(); ++it)
            effects.insert(it.key(), it.value());

        QString effect;

        for (it = effects.begin(); it != effects.end(); ++it)
        {
            if ( it.value() == m_effectsComboBox->currentText())
            {
                effect = it.key();
                break;
            }
        }

        m_sharedData->effectNameGL = effect;
    }

}

void MainDialog::ShowNumberImages( int Number )
{
    QTime TotalDuration (0, 0, 0);

    int TransitionDuration = 2000;

    if ( m_openglCheckBox->isChecked() )
        TransitionDuration += 500;

    if ( m_sharedData->useMilliseconds )
        TotalDuration = TotalDuration.addMSecs(Number * m_delaySpinBox->text().toInt());
    else
        TotalDuration = TotalDuration.addSecs(Number * m_delaySpinBox->text().toInt());

    TotalDuration = TotalDuration.addMSecs((Number - 1) * TransitionDuration);

    m_totalTime = TotalDuration;

    // Notify total time is changed
    emit totalTimeChanged(m_totalTime);

    if ( Number == 1 )
        m_label6->setText(i18n("%1 image [%2]", Number, TotalDuration.toString()));
    else
        m_label6->setText(i18n("%1 images [%2]", Number, TotalDuration.toString()));
}

void MainDialog::loadEffectNames()
{
    m_effectsComboBox->clear();

    QMap<QString, QString> effectNames = SlideShow::effectNamesI18N();
    QStringList effects;

    QMap<QString, QString>::Iterator it;

    for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.value());

    m_effectsComboBox->insertItems(0, effects);

    for (int i = 0; i < m_effectsComboBox->count(); i++)
    {
        if (effectNames[m_sharedData->effectName] == m_effectsComboBox->itemText(i))
        {
            m_effectsComboBox->setCurrentIndex(i);
            break;
        }
    }
}

void MainDialog::loadEffectNamesGL()
{
    m_effectsComboBox->clear();

    QStringList effects;
    QMap<QString, QString> effectNames;
    QMap<QString, QString>::Iterator it;

    // Load slideshowgl effects
    effectNames = SlideShowGL::effectNamesI18N();

    for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.value());

    // Load Ken Burns effect
    effectNames = SlideShowKB::effectNamesI18N();

    for (it = effectNames.begin(); it != effectNames.end(); ++it)
        effects.append(it.value());

    // Update GUI

    effects.sort();

    m_effectsComboBox->insertItems(0, effects);

    for (int i = 0; i < m_effectsComboBox->count(); i++)
    {
        if (effectNames[m_sharedData->effectNameGL] == m_effectsComboBox->itemText(i))
        {
            m_effectsComboBox->setCurrentIndex(i);
            break;
        }
    }
}

bool MainDialog::updateUrlList()
{
    for (int i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
    {
        ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->item(i) );

        if (!QFile::exists(pitem->path()))
        {
            KMessageBox::error(this,
                               i18n("Cannot access to file %1, please check the path is right.", pitem->path()));
            return false;
        }

        m_sharedData->urlList->append(pitem->path());                              // Input images files.
    }

    return true;
}

// --- Slots

void MainDialog::slotImagesFilesSelected( int row )
{
    QListWidgetItem* item = m_ImagesFilesListBox->item(row);

    if ( !item || m_ImagesFilesListBox->count() == 0 )
    {
        m_label7->setText("");
        m_ImageViewer->clear();
        return;
    }

    ImageItem *pitem = static_cast<ImageItem*>( item );

    if ( !pitem ) return;

    KUrl url;

    url.setPath(pitem->path());

    if ( m_thumbJob ) delete m_thumbJob;

    m_thumbJob = KIO::filePreview( url, m_ImageViewer->width() );

    connect(m_thumbJob, SIGNAL(gotPreview(const KFileItem&, const QPixmap&)),
            SLOT(slotGotPreview(const KFileItem&, const QPixmap&)));

    connect(m_thumbJob, SIGNAL(failed(const KFileItem&)),
            SLOT(slotFailedPreview(const KFileItem&)));

    int index = m_ImagesFilesListBox->row ( item );

    m_label7->setText(i18n("Image no. %1", QString::number(index + 1)));
}

void MainDialog::addItems(const KUrl::List& fileList)
{
    if (fileList.isEmpty()) return;

    KUrl::List Files = fileList;

    for ( KUrl::List::Iterator it = Files.begin() ; it != Files.end() ; ++it )
    {
        KUrl currentFile = *it;

        QFileInfo fi(currentFile.path());
        QString Temp = fi.path();
        QString albumName = Temp.section('/', -1);

        KIPI::ImageInfo info = m_sharedData->interface->info(currentFile);
        QString comments = info.description();

        ImageItem *item = new ImageItem( m_ImagesFilesListBox,
                                         currentFile.path().section('/', -1 ),   // File name with extension.
                                         comments,                               // Image comments.
                                         currentFile.path().section('/', 0, -1), // Complete path with file name.
                                         albumName                               // Album name.
                                       );

        item->setName( currentFile.path().section('/', -1) );
        m_ImagesFilesListBox->insertItem(m_ImagesFilesListBox->count() - 1, item);
    }

    ShowNumberImages( m_ImagesFilesListBox->count() );

    m_ImagesFilesListBox->setCurrentItem(m_ImagesFilesListBox->item(m_ImagesFilesListBox->count() - 1)) ;
    slotImagesFilesSelected(m_ImagesFilesListBox->currentRow());
    m_ImagesFilesListBox->scrollToItem(m_ImagesFilesListBox->currentItem());
}


void MainDialog::slotAddDropItems(KUrl::List filesUrl)
{
    addItems(filesUrl);
}

void MainDialog::slotImagesFilesButtonAdd( void )
{
    KIPIPlugins::ImageDialog dlg(this, m_sharedData->interface, false);
    KUrl::List urls = dlg.urls();

    if (!urls.isEmpty())
    {
        addItems(urls);
    }

}

void MainDialog::slotImagesFilesButtonDelete( void )
{
    int Index = m_ImagesFilesListBox->currentRow();
    ImageItem* pitem = static_cast<ImageItem*>(m_ImagesFilesListBox->takeItem(Index));
    delete pitem;

    slotImagesFilesSelected(m_ImagesFilesListBox->currentRow());
    ShowNumberImages( m_ImagesFilesListBox->count() );
}

void MainDialog::slotImagesFilesButtonUp( void )
{
    int Cpt = 0;

    for (int i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
        if (m_ImagesFilesListBox->currentRow() == i)
            ++Cpt;

    if  (Cpt == 0)
        return;

    if  (Cpt > 1)
    {
        KMessageBox::error(this, i18n("You can only move up one image file at once."));
        return;
    }

    unsigned int Index = m_ImagesFilesListBox->currentRow();

    if (Index == 0)
        return;

    ImageItem* pitem = static_cast<ImageItem*>(m_ImagesFilesListBox->takeItem(Index));

    m_ImagesFilesListBox->insertItem(Index - 1, pitem);

    m_ImagesFilesListBox->setCurrentItem(pitem);
}

void MainDialog::slotImagesFilesButtonDown( void )
{
    int Cpt = 0;

    for (int i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
        if (m_ImagesFilesListBox->currentRow() == i)
            ++Cpt;

    if (Cpt == 0)
        return;

    if (Cpt > 1)
    {
        KMessageBox::error(this, i18n("You can only move down one image file at once."));
        return;
    }

    int Index = m_ImagesFilesListBox->currentRow();

    if (Index == m_ImagesFilesListBox->count())
        return;

    ImageItem* pitem = static_cast<ImageItem*>(m_ImagesFilesListBox->takeItem(Index));

    m_ImagesFilesListBox->insertItem(Index + 1, pitem);

    m_ImagesFilesListBox->setCurrentItem(pitem);
}

void MainDialog::slotOpenGLToggled( void )
{
    if (m_openglCheckBox->isChecked())
    {
        loadEffectNamesGL();
    }
    else
    {
        loadEffectNames();
    }

    ShowNumberImages( m_ImagesFilesListBox->count() );

    slotEffectChanged();
}

void MainDialog::slotEffectChanged( void )
{
    bool isKB = m_effectsComboBox->currentText() == i18n("Ken Burns");

    m_printNameCheckBox->setEnabled(!isKB);
    m_printProgressCheckBox->setEnabled(!isKB);
    m_printCommentsCheckBox->setEnabled(!isKB);
    m_sharedData->page_caption->setEnabled((!isKB) &&
                                           m_printCommentsCheckBox->isChecked());
}

void MainDialog::slotDelayChanged( void )
{
    ShowNumberImages( m_ImagesFilesListBox->count() );
}

void MainDialog::slotUseMillisecondsToggled( void )
{

    int delayValue = m_delaySpinBox->value();

    m_delaySpinBox->setValue(0);

    if ( m_sharedData->useMilliseconds )
    {
        m_delayLabel->setText(i18n("Delay between images (ms):"));

        m_delaySpinBox->setRange(m_sharedData->delayMsMinValue, m_sharedData->delayMsMaxValue);
        m_delaySpinBox->setSingleStep(m_sharedData->delayMsLineStep);

        m_delaySpinBox->setValue(delayValue*1000);
    }
    else
    {
        m_delayLabel->setText(i18n("Delay between images  (s):"));

        m_delaySpinBox->setRange(m_sharedData->delayMsMinValue / 100, m_sharedData->delayMsMaxValue / 10);
        m_delaySpinBox->setSingleStep(m_sharedData->delayMsLineStep / 100);

        m_delaySpinBox->setValue(delayValue / 1000);
    }
}

void MainDialog::slotSelection( void )
{
    KUrl::List urlList;

    if (m_selectedFilesButton->isChecked())
    {

        urlList = m_sharedData->interface->currentSelection().images();

        m_ImagesFilesButtonAdd->setEnabled(FALSE);
        m_ImagesFilesButtonDelete->setEnabled(FALSE);
        m_ImagesFilesButtonUp->setEnabled(FALSE);
        m_ImagesFilesButtonDown->setEnabled(FALSE);
    }
    else
        if (m_allFilesButton->isChecked())
        {

            KUrl currentPath = m_sharedData->interface->currentAlbum().path();
            Q3ValueList<KIPI::ImageCollection> albumList;

            albumList = m_sharedData->interface->allAlbums();
            Q3ValueList<KIPI::ImageCollection>::iterator it;

            urlList = m_sharedData->interface->currentAlbum().images();

            for ( it = albumList.begin(); it != albumList.end(); ++it )
                if (currentPath.isParentOf((*it).path()) && !((*it).path() == currentPath))
                    urlList += (*it).images();

            m_ImagesFilesButtonAdd->setEnabled(FALSE);

            m_ImagesFilesButtonDelete->setEnabled(FALSE);

            m_ImagesFilesButtonUp->setEnabled(FALSE);

            m_ImagesFilesButtonDown->setEnabled(FALSE);
        }

    if ( m_customButton->isChecked() )    // Custom selected
    {
        m_ImagesFilesButtonAdd->setEnabled(TRUE);
        m_ImagesFilesButtonDelete->setEnabled(TRUE);
        m_ImagesFilesButtonUp->setEnabled(TRUE);
        m_ImagesFilesButtonDown->setEnabled(TRUE);
    }
    else
    {
        if (!urlList.isEmpty())
        {
            m_ImagesFilesListBox->clear();
            addItems(urlList);
        }
    }
}

void MainDialog::SlotPortfolioDurationChanged ( int )
{
    ShowNumberImages( m_ImagesFilesListBox->count() );
    emit totalTimeChanged( m_totalTime );
}

void MainDialog::slotGotPreview(const KFileItem&, const QPixmap &pixmap)
{
    m_ImageViewer->setImage(pixmap.toImage());
    m_thumbJob = 0L;
}

void MainDialog::slotFailedPreview(const KFileItem&)
{
    m_thumbJob = 0L;
}

void MainDialog::slotPrintCommentsToggled( void )
{
    m_sharedData->printFileComments =  m_printCommentsCheckBox->isChecked();
    m_sharedData->page_caption->setEnabled(m_printCommentsCheckBox->isChecked());
}

}  // namespace KIPISlideShowPlugin
