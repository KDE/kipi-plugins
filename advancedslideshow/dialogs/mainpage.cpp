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
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "mainpage.h"

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
#include <QUrl>
#include <QIcon>
#include <QMessageBox>

// Libkipi includes

#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_debug.h"
#include "presentationcontainer.h"
#include "advancedpage.h"
#include "captionpage.h"
#include "kpimageslist.h"
#include "presentation.h"

#ifdef HAVE_OPENGL
#   include "presentationgl.h"
#   include "presentationkb.h"
#endif

using namespace KIPIPlugins;

namespace KIPIAdvancedSlideshowPlugin
{

class MainPage::Private
{

public:

    Private()
        : noPreviewPixmap(ICONSIZE, ICONSIZE)
    {
        sharedData         = 0;
        imagesFilesListBox = 0;
    }

    PresentationContainer*           sharedData;
    QTime                      totalTime;
    KPSvgPixmapRenderer        noPreviewPixmap;
    KIPIPlugins::KPImagesList* imagesFilesListBox;
};

MainPage::MainPage(QWidget* const parent, PresentationContainer* const sharedData)
    : QWidget(parent),
      d(new Private)
{
    setupUi(this);

    d->sharedData = sharedData;

    // --------------------------------------------------------

    QVBoxLayout* const listBoxContainerLayout = new QVBoxLayout;
    d->imagesFilesListBox                     = new KPImagesList(m_ImagesFilesListBoxContainer, 32);
    d->imagesFilesListBox->listView()->header()->hide();

    listBoxContainerLayout->addWidget(d->imagesFilesListBox);
    listBoxContainerLayout->setContentsMargins(QMargins());
    listBoxContainerLayout->setSpacing(0);
    m_ImagesFilesListBoxContainer->setLayout(listBoxContainerLayout);

    // --------------------------------------------------------

    m_previewLabel->setMinimumWidth(ICONSIZE);
    m_previewLabel->setMinimumHeight(ICONSIZE);

#ifdef HAVE_OPENGL
    m_openglCheckBox->setEnabled(true);
    m_openGlFullScale->setEnabled(true);
#else
    m_openglCheckBox->setEnabled(false);
    m_openGlFullScale->setEnabled(false);
#endif
}

MainPage::~MainPage()
{
    delete d;
}

void MainPage::readSettings()
{
#ifdef HAVE_OPENGL
    m_openglCheckBox->setChecked(d->sharedData->opengl);
    m_openGlFullScale->setChecked(d->sharedData->openGlFullScale);
    m_openGlFullScale->setEnabled(d->sharedData->opengl);
#endif
    m_delaySpinBox->setValue(d->sharedData->delay);
    m_printNameCheckBox->setChecked(d->sharedData->printFileName);
    m_printProgressCheckBox->setChecked(d->sharedData->printProgress);
    m_printCommentsCheckBox->setChecked(d->sharedData->printFileComments);
    m_loopCheckBox->setChecked(d->sharedData->loop);
    m_shuffleCheckBox->setChecked(d->sharedData->shuffle);

    if (d->sharedData->showSelectedFilesOnly && m_selectedFilesButton->isEnabled() )
        m_selectedFilesButton->setChecked(true);
    else
        m_allFilesButton->setChecked(true);

    // Host application images has comments
    if ( ! d->sharedData->ImagesHasComments )
    {
        m_printCommentsCheckBox->setEnabled(false);
        m_printCommentsCheckBox->setChecked(false);
    }

    // Switch to selected files only (it depends on showSelectedFilesOnly)

    m_selectedFilesButton->setEnabled( d->sharedData->showSelectedFilesOnly );

    m_delaySpinBox->setValue(d->sharedData->useMilliseconds ? d->sharedData->delay
                                                            : d->sharedData->delay / 1000 );

    slotUseMillisecondsToggled();

    // --------------------------------------------------------

    setupConnections();
    slotOpenGLToggled();
    slotPrintCommentsToggled();
    slotEffectChanged();
    slotSelection();
}

void MainPage::saveSettings()
{
#ifdef HAVE_OPENGL
    d->sharedData->opengl                = m_openglCheckBox->isChecked();
    d->sharedData->openGlFullScale       = m_openGlFullScale->isChecked();
#endif
    d->sharedData->delay                 = d->sharedData->useMilliseconds ? m_delaySpinBox->value()
                                                                          : m_delaySpinBox->value() * 1000;

    d->sharedData->printFileName         = m_printNameCheckBox->isChecked();
    d->sharedData->printProgress         = m_printProgressCheckBox->isChecked();
    d->sharedData->printFileComments     = m_printCommentsCheckBox->isChecked();
    d->sharedData->loop                  = m_loopCheckBox->isChecked();
    d->sharedData->shuffle               = m_shuffleCheckBox->isChecked();
    d->sharedData->showSelectedFilesOnly = m_selectedFilesButton->isChecked();

    if (!m_openglCheckBox->isChecked())
    {

        QString effect;
        QMap<QString, QString> effectNames = Presentation::effectNamesI18N();
        QMap<QString, QString>::ConstIterator it;

        for (it = effectNames.constBegin(); it != effectNames.constEnd(); ++it)
        {
            if (it.value() == m_effectsComboBox->currentText())
            {
                effect = it.key();
                break;
            }
        }

        d->sharedData->effectName = effect;
    }
#ifdef HAVE_OPENGL
    else
    {
        QMap<QString, QString> effects;
        QMap<QString, QString> effectNames;
        QMap<QString, QString>::ConstIterator it;

        // Load slideshowgl effects
        effectNames = PresentationGL::effectNamesI18N();

        for (it = effectNames.constBegin(); it != effectNames.constEnd(); ++it)
        {
            effects.insert(it.key(), it.value());
        }

        // Load Ken Burns effect
        effectNames = PresentationKB::effectNamesI18N();

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

        d->sharedData->effectNameGL = effect;
    }
#endif
}

void MainPage::showNumberImages()
{
    int numberOfImages = d->imagesFilesListBox->imageUrls().count();
    QTime totalDuration(0, 0, 0);

    int transitionDuration = 2000;

#ifdef HAVE_OPENGL
    if ( m_openglCheckBox->isChecked() )
        transitionDuration += 500;
#endif

    if (numberOfImages != 0)
    {
        if ( d->sharedData->useMilliseconds )
            totalDuration = totalDuration.addMSecs(numberOfImages * m_delaySpinBox->text().toInt());
        else
            totalDuration = totalDuration.addSecs(numberOfImages * m_delaySpinBox->text().toInt());

        totalDuration = totalDuration.addMSecs((numberOfImages - 1) * transitionDuration);
    }

    d->totalTime = totalDuration;

    // Notify total time is changed
    emit signalTotalTimeChanged(d->totalTime);

    m_label6->setText(i18np("%1 image [%2]", "%1 images [%2]", numberOfImages, totalDuration.toString()));
}

void MainPage::loadEffectNames()
{
    m_effectsComboBox->clear();

    QMap<QString, QString> effectNames = Presentation::effectNamesI18N();
    QStringList effects;

    QMap<QString, QString>::Iterator it;

    for (it = effectNames.begin(); it != effectNames.end(); ++it)
    {
        effects.append(it.value());
    }

    m_effectsComboBox->insertItems(0, effects);

    for (int i = 0; i < m_effectsComboBox->count(); ++i)
    {
        if (effectNames[d->sharedData->effectName] == m_effectsComboBox->itemText(i))
        {
            m_effectsComboBox->setCurrentIndex(i);
            break;
        }
    }
}

void MainPage::loadEffectNamesGL()
{
#ifdef HAVE_OPENGL
    m_effectsComboBox->clear();

    QStringList effects;
    QMap<QString, QString> effectNames;
    QMap<QString, QString>::Iterator it;

    // Load slideshowgl effects
    effectNames = PresentationGL::effectNamesI18N();

    // Add Ken Burns effect
    effectNames.unite(PresentationKB::effectNamesI18N());

    for (it = effectNames.begin(); it != effectNames.end(); ++it)
    {
        effects.append(it.value());
    }

    // Update GUI

    effects.sort();

    m_effectsComboBox->insertItems(0, effects);

    for (int i = 0; i < m_effectsComboBox->count(); ++i)
    {
        if (effectNames[d->sharedData->effectNameGL] == m_effectsComboBox->itemText(i))
        {
            m_effectsComboBox->setCurrentIndex(i);
            break;
        }
    }
#endif
}

bool MainPage::updateUrlList()
{
    d->sharedData->urlList.clear();
    QTreeWidgetItemIterator it(d->imagesFilesListBox->listView());

    while (*it)
    {
        KPImagesListViewItem* const item = dynamic_cast<KPImagesListViewItem*>(*it);

        if (!item)
            continue;

        QString url = item->url().toLocalFile();

        if (!QFile::exists(url))
        {
            QMessageBox::critical(this, i18n("Error"), i18n("Cannot access file %1. Please check the path is correct.", url));
            return false;
        }

        d->sharedData->urlList.append(QUrl::fromLocalFile(url));  // Input images files.
        ++it;
    }

    return true;
}

void MainPage::slotImagesFilesSelected(QTreeWidgetItem* item)
{
    if (!item || d->imagesFilesListBox->imageUrls().isEmpty())
    {
        m_label7->setText(QString::fromLatin1(""));
        m_previewLabel->setPixmap(d->noPreviewPixmap.getPixmap());
        return;
    }

    KPImagesListViewItem* const pitem = dynamic_cast<KPImagesListViewItem*>(item);

    if (!pitem)
        return;

    QUrl url;
    url.setPath(pitem->url().path());

    connect(d->sharedData->iface(), SIGNAL(gotThumbnail(QUrl,QPixmap)),
            this, SLOT(slotThumbnail(QUrl,QPixmap)));

    d->sharedData->iface()->thumbnail(url, ICONSIZE);

    QModelIndex index = d->imagesFilesListBox->listView()->currentIndex();

    if (index.isValid())
    {
        int rowindex = index.row();
        m_label7->setText(i18nc("Image number %1", "Image #%1", rowindex + 1));
    }
}

void MainPage::addItems(const QList<QUrl>& fileList)
{
    if (fileList.isEmpty())
        return;

    QList<QUrl> files = fileList;

    d->imagesFilesListBox->slotAddImages(files);
    slotImagesFilesSelected(d->imagesFilesListBox->listView()->currentItem());
}

void MainPage::slotOpenGLToggled()
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

void MainPage::slotEffectChanged()
{
    bool isKB = m_effectsComboBox->currentText() == i18n("Ken Burns");

    m_printNameCheckBox->setEnabled(!isKB);
    m_printProgressCheckBox->setEnabled(!isKB);
    m_printCommentsCheckBox->setEnabled(!isKB);
#ifdef HAVE_OPENGL
    m_openGlFullScale->setEnabled(!isKB && m_openglCheckBox->isChecked());
#endif
    d->sharedData->captionPage->setEnabled((!isKB) && m_printCommentsCheckBox->isChecked());
}

void MainPage::slotDelayChanged( int delay )
{
    d->sharedData->delay = d->sharedData->useMilliseconds ? delay : delay * 1000;
    showNumberImages();
}

void MainPage::slotUseMillisecondsToggled()
{
    int delay = d->sharedData->delay;

    if ( d->sharedData->useMilliseconds )
    {
        m_delayLabel->setText(i18n("Delay between images (ms):"));

        m_delaySpinBox->setRange(d->sharedData->delayMsMinValue, d->sharedData->delayMsMaxValue);
        m_delaySpinBox->setSingleStep(d->sharedData->delayMsLineStep);
    }
    else
    {
        m_delayLabel->setText(i18n("Delay between images (s):"));

        m_delaySpinBox->setRange(d->sharedData->delayMsMinValue / 100, d->sharedData->delayMsMaxValue / 1000  );
        m_delaySpinBox->setSingleStep(d->sharedData->delayMsLineStep / 100);
        delay /= 1000;

    }

    m_delaySpinBox->setValue(delay);
}

void MainPage::slotSelection()
{
    QList<QUrl> urlList;

    if (m_selectedFilesButton->isChecked())
    {
        d->imagesFilesListBox->listView()->clear();
        urlList = d->sharedData->iface()->currentSelection().images();
    }
    else if (m_allFilesButton->isChecked())
    {
        QUrl currentPath = d->sharedData->iface()->currentAlbum().url();
        QList<KIPI::ImageCollection> albumList;
        albumList        = d->sharedData->iface()->allAlbums();

        d->imagesFilesListBox->listView()->clear();
        urlList          = d->sharedData->iface()->currentAlbum().images();

        QList<KIPI::ImageCollection>::iterator it;

        for (it = albumList.begin(); it != albumList.end(); ++it)
        {
            if (currentPath.isParentOf((*it).url()) && !((*it).url() == currentPath))
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

    d->imagesFilesListBox->enableControlButtons(customize);
    d->imagesFilesListBox->enableDragAndDrop(customize);
}

void MainPage::slotPortfolioDurationChanged(int)
{
    showNumberImages();
    emit signalTotalTimeChanged( d->totalTime );
}

void MainPage::slotThumbnail(const QUrl& /*url*/, const QPixmap& pix)
{
    if (pix.isNull())
    {
        m_previewLabel->setPixmap(QIcon::fromTheme(QString::fromLatin1("image-x-generic")).pixmap(ICONSIZE, QIcon::Disabled));
    }
    else
    {
        m_previewLabel->setPixmap(pix.scaled(ICONSIZE, ICONSIZE, Qt::KeepAspectRatio));
    }

    disconnect(d->sharedData->iface(), 0,
               this, 0);
}

void MainPage::slotPrintCommentsToggled()
{
    d->sharedData->printFileComments =  m_printCommentsCheckBox->isChecked();
    d->sharedData->captionPage->setEnabled(m_printCommentsCheckBox->isChecked());
}

void MainPage::slotImageListChanged()
{
    showNumberImages();
    slotImagesFilesSelected(d->imagesFilesListBox->listView()->currentItem());
}

void MainPage::setupConnections()
{
    connect(d->sharedData->advancedPage, SIGNAL(useMillisecondsToggled()), this,
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

    connect(d->imagesFilesListBox, SIGNAL(signalImageListChanged()),
            this, SLOT(slotImageListChanged()));

    connect(d->imagesFilesListBox, SIGNAL(signalItemClicked(QTreeWidgetItem*)),
            this, SLOT(slotImagesFilesSelected(QTreeWidgetItem*)));

    if (d->sharedData->showSelectedFilesOnly)
    {
        connect(m_selectedFilesButton, SIGNAL(toggled(bool)),
                this, SLOT(slotSelection()));
    }
}

}  // namespace KIPIAdvancedSlideshowPlugin
