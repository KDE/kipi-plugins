/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-09
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
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

#define ICONSIZE 256

#include "maindialog.moc"

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QHeaderView>
#include <QPainter>
#include <QVBoxLayout>
#include <QSvgRenderer>

// KDE includes

#include <kdebug.h>
#include <kicon.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kstandarddirs.h>

// Libkipi includes

#include <libkipi/imagecollection.h>

// Local includes

#include "commoncontainer.h"
#include "advanceddialog.h"
#include "slideshow.h"
#include "slideshowgl.h"
#include "slideshowkb.h"
#include "kpimagedialog.h"
#include "kpimageslist.h"

using namespace KIPIPlugins;

namespace KIPIAdvancedSlideshowPlugin
{

MainDialog::MainDialog(QWidget* const parent, SharedContainer* const sharedData)
    : QWidget(parent)
{
    setupUi(this);

    m_sharedData = sharedData;
    m_thumbJob   = 0L;

    // --------------------------------------------------------

    QVBoxLayout* listBoxContainerLayout = new QVBoxLayout;
    m_ImagesFilesListBox                = new KPImagesList(m_ImagesFilesListBoxContainer, KIconLoader::SizeMedium);
    m_ImagesFilesListBox->listView()->header()->hide();

    listBoxContainerLayout->addWidget(m_ImagesFilesListBox);
    listBoxContainerLayout->setSpacing(0);
    listBoxContainerLayout->setMargin(0);
    m_ImagesFilesListBoxContainer->setLayout(listBoxContainerLayout);

    // --------------------------------------------------------

    m_previewLabel->setMinimumWidth(ICONSIZE);
    m_previewLabel->setMinimumHeight(ICONSIZE);

    // --------------------------------------------------------

    // Prepare a preview pixmap (KIPI logo) for no image selection
    QSvgRenderer svgRenderer( KStandardDirs::locate("data", "kipi/data/kipi-icon.svg") );
    m_noPreviewPixmap = QPixmap(ICONSIZE, ICONSIZE);
    m_noPreviewPixmap.fill(Qt::transparent);
    QPaintDevice* pdp = &m_noPreviewPixmap;
    QPainter painter(pdp);
    svgRenderer.render(&painter);
}

MainDialog::~MainDialog()
{
}

void MainDialog::readSettings()
{
    m_openglCheckBox->setChecked(m_sharedData->opengl);
    m_openGlFullScale->setChecked(m_sharedData->openGlFullScale);
    m_openGlFullScale->setEnabled(m_sharedData->opengl);
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
        m_printCommentsCheckBox->setEnabled(false);
        m_printCommentsCheckBox->setChecked(false);
    }

    // Switch to selected files only (it depends on showSelectedFilesOnly)

    m_selectedFilesButton->setEnabled( m_sharedData->showSelectedFilesOnly );

    m_delaySpinBox->setValue(m_sharedData->useMilliseconds ? m_sharedData->delay
                                                           : m_sharedData->delay / 1000 );

    slotUseMillisecondsToggled();

    // --------------------------------------------------------

    setupConnections();
    slotOpenGLToggled();
    slotPrintCommentsToggled();
    slotEffectChanged();
    slotSelection();
}

void MainDialog::saveSettings()
{
    m_sharedData->opengl                = m_openglCheckBox->isChecked();
    m_sharedData->openGlFullScale       = m_openGlFullScale->isChecked();
    m_sharedData->delay                 = m_sharedData->useMilliseconds ? m_delaySpinBox->value()
                                                                        : m_delaySpinBox->value() * 1000;

    m_sharedData->printFileName         = m_printNameCheckBox->isChecked();
    m_sharedData->printProgress         = m_printProgressCheckBox->isChecked();
    m_sharedData->printFileComments     = m_printCommentsCheckBox->isChecked();
    m_sharedData->loop                  = m_loopCheckBox->isChecked();
    m_sharedData->shuffle               = m_shuffleCheckBox->isChecked();
    m_sharedData->showSelectedFilesOnly = m_selectedFilesButton->isChecked();

    if (!m_openglCheckBox->isChecked())
    {

        QString effect;
        QMap<QString, QString> effectNames = SlideShow::effectNamesI18N();
        QMap<QString, QString>::ConstIterator it;

        for (it = effectNames.constBegin(); it != effectNames.constEnd(); ++it)
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
        QMap<QString, QString>::ConstIterator it;

        // Load slideshowgl effects
        effectNames = SlideShowGL::effectNamesI18N();

        for (it = effectNames.constBegin(); it != effectNames.constEnd(); ++it)
        {
            effects.insert(it.key(), it.value());
        }

        // Load Ken Burns effect
        effectNames = SlideShowKB::effectNamesI18N();

        for (it = effectNames.constBegin(); it != effectNames.constEnd(); ++it)
        {
            effects.insert(it.key(), it.value());
        }

        QString effect;

        for (it = effects.constBegin(); it != effects.constEnd(); ++it)
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

void MainDialog::showNumberImages()
{
    int numberOfImages = m_ImagesFilesListBox->imageUrls().count();
    QTime totalDuration (0, 0, 0);

    int transitionDuration = 2000;

    if ( m_openglCheckBox->isChecked() )
        transitionDuration += 500;

    if (numberOfImages != 0)
    {
        if ( m_sharedData->useMilliseconds )
            totalDuration = totalDuration.addMSecs(numberOfImages * m_delaySpinBox->text().toInt());
        else
            totalDuration = totalDuration.addSecs(numberOfImages * m_delaySpinBox->text().toInt());

        totalDuration = totalDuration.addMSecs((numberOfImages - 1) * transitionDuration);
    }

    m_totalTime = totalDuration;

    // Notify total time is changed
    emit signalTotalTimeChanged(m_totalTime);

    m_label6->setText(i18np("%1 image [%2]", "%1 images [%2]", numberOfImages, totalDuration.toString()));
}

void MainDialog::loadEffectNames()
{
    m_effectsComboBox->clear();

    QMap<QString, QString> effectNames = SlideShow::effectNamesI18N();
    QStringList effects;

    QMap<QString, QString>::Iterator it;

    for (it = effectNames.begin(); it != effectNames.end(); ++it)
    {
        effects.append(it.value());
    }

    m_effectsComboBox->insertItems(0, effects);

    for (int i = 0; i < m_effectsComboBox->count(); ++i)
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

    // Add Ken Burns effect
    effectNames.unite(SlideShowKB::effectNamesI18N());

    for (it = effectNames.begin(); it != effectNames.end(); ++it)
    {
        effects.append(it.value());
    }

    // Update GUI

    effects.sort();

    m_effectsComboBox->insertItems(0, effects);

    for (int i = 0; i < m_effectsComboBox->count(); ++i)
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
    m_sharedData->urlList.clear();
    QTreeWidgetItemIterator it(m_ImagesFilesListBox->listView());

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (!item)
            continue;

        QString url = item->url().toLocalFile();

        if (!QFile::exists(url))
        {
            KMessageBox::error(this, i18n("Cannot access file %1. Please check the path is correct.", url));
            return false;
        }

        m_sharedData->urlList.append(url);  // Input images files.
        ++it;
    }

    return true;
}

void MainDialog::slotImagesFilesSelected(QTreeWidgetItem* item)
{
    if (!item || m_ImagesFilesListBox->imageUrls().isEmpty())
    {
        m_label7->setText("");
        m_previewLabel->setPixmap(m_noPreviewPixmap);
        return;
    }

    KPImagesListViewItem* const pitem = dynamic_cast<KPImagesListViewItem*>(item);

    if (!pitem)
        return;

    KUrl url;
    url.setPath(pitem->url().path());

    connect(m_sharedData->iface(), SIGNAL(gotThumbnail(KUrl,QPixmap)),
            this, SLOT(slotThumbnail(KUrl,QPixmap)));

    m_sharedData->iface()->thumbnail(url, ICONSIZE);

    QModelIndex index = m_ImagesFilesListBox->listView()->currentIndex();

    if (index.isValid())
    {
        int rowindex = index.row();
        m_label7->setText(i18nc("Image number %1", "Image #%1", rowindex + 1));
    }
}

void MainDialog::addItems(const KUrl::List& fileList)
{
    if (fileList.isEmpty())
        return;

    KUrl::List files = fileList;

    m_ImagesFilesListBox->slotAddImages(files);
    slotImagesFilesSelected(m_ImagesFilesListBox->listView()->currentItem());
}

void MainDialog::slotOpenGLToggled()
{
    if (m_openglCheckBox->isChecked())
    {
        loadEffectNamesGL();
    }
    else
    {
        loadEffectNames();
    }

    showNumberImages();
    slotEffectChanged();
}

void MainDialog::slotEffectChanged()
{
    bool isKB = m_effectsComboBox->currentText() == i18n("Ken Burns");

    m_printNameCheckBox->setEnabled(!isKB);
    m_printProgressCheckBox->setEnabled(!isKB);
    m_printCommentsCheckBox->setEnabled(!isKB);
    m_openGlFullScale->setEnabled(!isKB && m_openglCheckBox->isChecked());
    m_sharedData->page_caption->setEnabled((!isKB) && m_printCommentsCheckBox->isChecked());
}

void MainDialog::slotDelayChanged( int delay )
{
    m_sharedData->delay = m_sharedData->useMilliseconds ? delay : delay * 1000;
    showNumberImages();
}

void MainDialog::slotUseMillisecondsToggled()
{
    int delay = m_sharedData->delay;

    if ( m_sharedData->useMilliseconds )
    {
        m_delayLabel->setText(i18n("Delay between images (ms):"));

        m_delaySpinBox->setRange(m_sharedData->delayMsMinValue, m_sharedData->delayMsMaxValue);
        m_delaySpinBox->setSingleStep(m_sharedData->delayMsLineStep);
    }
    else
    {
        m_delayLabel->setText(i18n("Delay between images (s):"));

        m_delaySpinBox->setRange(m_sharedData->delayMsMinValue / 100, m_sharedData->delayMsMaxValue / 1000  );
        m_delaySpinBox->setSingleStep(m_sharedData->delayMsLineStep / 100);
        delay /= 1000;

    }

    m_delaySpinBox->setValue(delay);
}

void MainDialog::slotSelection()
{
    KUrl::List urlList;

    if (m_selectedFilesButton->isChecked())
    {
        m_ImagesFilesListBox->listView()->clear();
        urlList = m_sharedData->iface()->currentSelection().images();
    }
    else if (m_allFilesButton->isChecked())
    {
        KUrl currentPath = m_sharedData->iface()->currentAlbum().path();
        QList<KIPI::ImageCollection> albumList;
        albumList        = m_sharedData->iface()->allAlbums();

        m_ImagesFilesListBox->listView()->clear();
        urlList = m_sharedData->iface()->currentAlbum().images();

        QList<KIPI::ImageCollection>::iterator it;

        for (it = albumList.begin(); it != albumList.end(); ++it)
        {
            if (currentPath.isParentOf((*it).path()) && !((*it).path() == currentPath))
            {
                urlList += (*it).images();
            }
        }
    }

    bool customize = m_customButton->isChecked();

    if (!urlList.isEmpty() && !customize)
    {
        addItems(urlList);
    }

    m_ImagesFilesListBox->enableControlButtons(customize);
    m_ImagesFilesListBox->enableDragAndDrop(customize);
}

void MainDialog::slotPortfolioDurationChanged(int)
{
    showNumberImages();
    emit signalTotalTimeChanged( m_totalTime );
}

void MainDialog::slotThumbnail(const KUrl& /*url*/, const QPixmap& pix)
{
    if (pix.isNull())
        m_previewLabel->setPixmap(SmallIcon("image-x-generic", ICONSIZE, KIconLoader::DisabledState));
    else
        m_previewLabel->setPixmap(pix.scaled(ICONSIZE, ICONSIZE, Qt::KeepAspectRatio));

    disconnect(m_sharedData->iface(), 0,
               this, 0);
}

void MainDialog::slotPrintCommentsToggled()
{
    m_sharedData->printFileComments =  m_printCommentsCheckBox->isChecked();
    m_sharedData->page_caption->setEnabled(m_printCommentsCheckBox->isChecked());
}

void MainDialog::slotImageListChanged()
{
    showNumberImages();
    slotImagesFilesSelected(m_ImagesFilesListBox->listView()->currentItem());
}

void MainDialog::setupConnections()
{
    connect(m_sharedData->advancedPage, SIGNAL(useMillisecondsToggled()), this,
            SLOT(slotUseMillisecondsToggled()));

    connect(m_printCommentsCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(slotPrintCommentsToggled()));

    connect(m_openglCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(slotOpenGLToggled()));

    connect(m_allFilesButton, SIGNAL(toggled(bool)),
            this, SLOT(slotSelection()));

    connect(m_delaySpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(slotDelayChanged(int)));

    connect(m_effectsComboBox, SIGNAL(activated(int)),
            this, SLOT(slotEffectChanged()));

    connect(m_ImagesFilesListBox, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    connect(m_ImagesFilesListBox, SIGNAL(signalItemClicked(QTreeWidgetItem*)),
            this, SLOT(slotImagesFilesSelected(QTreeWidgetItem*)));

    if (m_sharedData->showSelectedFilesOnly)
    {
        connect(m_selectedFilesButton, SIGNAL(toggled(bool)),
                this, SLOT(slotSelection()));
    }
}

}  // namespace KIPIAdvancedSlideshowPlugin
