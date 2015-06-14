/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-06-24
 * Description : Video SlideShow Export Dialog
 *
 * Copyright (C) 2012 by A Janardhan Reddy <annapareddyjanardhanreddy at gmail dot com>
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

#include "exportdialog.moc"

// Qt includes

#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>
#include <QDir>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kio/renamedialog.h>
#include <kde_file.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "kpaboutdata.h"
#include "kpversion.h"
#include "kpimageinfo.h"
#include "kpprogresswidget.h"
#include "actionthread.h"
#include "slideshowsettingswidget.h"

namespace KIPIVideoSlideShowPlugin
{

class ExportDialog::Private
{
public:

    Private()
    {
        busy        = false;
        page        = 0;
        progressBar = 0;
        listView    = 0;
        thread      = 0;
        settingsBox = 0;
    }

    bool                     busy;

    QWidget*                 page;

    KPProgressWidget*        progressBar;

    MyImageList*             listView;

    ActionThread*            thread;

    SlideShowSettingsWidget* settingsBox;
};

ExportDialog::ExportDialog(const ImageCollection& images)
    : KPToolDialog(0), d(new Private)
{
    setButtons(Help | Default | Apply | Close );
    setDefaultButton(Close);
    setButtonToolTip(Close, i18n("Exit"));
    setCaption(i18n("Video Slide Show"));
    setModal(false);
    setMinimumSize(700, 500);

    d->page                 = new QWidget(this);
    setMainWidget(d->page);
    QGridLayout* mainLayout = new QGridLayout(d->page);

    //---------------------------------------------

    d->listView    = new MyImageList(d->page);

    // ---------------------------------------------------------------

    d->settingsBox = new SlideShowSettingsWidget(d->page);

    d->progressBar = new KPProgressWidget(d->page);
    d->progressBar->setMaximumHeight( fontMetrics().height()+2 );
    d->progressBar->hide();

    mainLayout->addWidget(d->listView,            0, 0, 3, 1);
    mainLayout->addWidget(d->settingsBox,         0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,         1, 1, 1, 1);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());
    mainLayout->setColumnStretch(0, 10);

    // ---------------------------------------------------------------

    KPAboutData* const about = new KPAboutData(ki18n("Video Slide Show"),
                                   0,
                                   KAboutData::License_GPL,
                                   ki18n("A Kipi plugin to export images as video slideshow"),
                                   ki18n("(c) 2012, A Janardhan Reddy\n"));

    about->addAuthor(ki18n("A Janardhan Reddy"),
                     ki18n("Developer"),
                           "annapareddyjanardhanreddy at gmail dot com");

    about->setHandbookEntry("videoslideshow");
    setAboutData(about);

    // ---------------------------------------------------------------

    d->thread = new ActionThread(this);
    addItems(images.images());

    connect(d->listView->listView(), SIGNAL(itemSelectionChanged()),
            this, SLOT(updateSettingWidget()));

    connect(d->settingsBox, SIGNAL(timeDataChanged(int)),
            this, SLOT(updateImageTime(int)));

    connect(d->settingsBox, SIGNAL(effectDataChanged(QString,EFFECT)),
            this, SLOT(updateImageEffect(QString,EFFECT)));

    connect(d->settingsBox, SIGNAL(transDataChanged(QString,TRANSITION_TYPE)),
            this, SLOT(updateImageTransition(QString,TRANSITION_TYPE)));

    connect(d->settingsBox, SIGNAL(transSpeedDataChanged(QString,TRANSITION_SPEED)),
            this, SLOT(updateImageTransSpeed(QString,TRANSITION_SPEED)));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(slotDefault()));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotStartStop()));

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()));

    connect(d->thread, SIGNAL(signalProcessError(QString)),
            this, SLOT(slotShowError(QString)));

    connect(d->thread, SIGNAL(frameCompleted(KIPIVideoSlideShowPlugin::ActionData)),
            this, SLOT(slotProcessedFrame(KIPIVideoSlideShowPlugin::ActionData)));

    connect(d->progressBar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotStartStop()));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
}

ExportDialog::~ExportDialog()
{
    slotAborted();
    delete d;
}

void ExportDialog::setImages(const ImageCollection& images)
{
    addItems(images.images());
}

void ExportDialog::closeEvent(QCloseEvent* e)
{
    if (!e) return;

    // Stop current conversion if necessary
    if (d->busy)
        slotStartStop();

    saveSettings();
    d->listView->listView()->clear();
    e->accept();
}

void ExportDialog::slotClose()
{
    // Stop current conversion if necessary
    if (d->busy)
        slotStartStop();

    saveSettings();
    d->listView->listView()->clear();
    done(Close);
}

void ExportDialog::addItems(const KUrl::List& itemList)
{
    d->listView->slotAddImages(itemList);
}

void ExportDialog::slotDefault()
{
    d->settingsBox->resetToDefault();
}

void ExportDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("VideoSlideShow Settings"));
    QString path       = group.readEntry("Temp Dir", QString());
    d->settingsBox->setTempDirPath(path);
    restoreDialogSize(group);
}

void ExportDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group  = config.group(QString("VideoSlideShow Settings"));

    group.writeEntry("Temp Dir", d->settingsBox->getTempDirPath());
    saveDialogSize(group);

    config.sync();
}

void ExportDialog::slotAborted()
{
    d->progressBar->setValue(0);
    d->progressBar->hide();
    d->progressBar->progressCompleted();
}

void ExportDialog::slotThreadFinished()
{
    busy(false);
    slotAborted();
}

void ExportDialog::processAll(MyImageListViewItem* const item)
{
    int frameHeight            = d->settingsBox->getFrameHeight();
    int frameWidth             = d->settingsBox->getFrameWidth();
    QString path               = d->settingsBox->getTempDirPath();
    QString audioPath          = d->settingsBox->getAudioFile();
    ASPECTCORRECTION_TYPE type = d->settingsBox->getAspectCorrection();
    ASPECT_RATIO aspectRatio   = d->settingsBox->getAspectRatio();
    VIDEO_FORMAT format        = d->settingsBox->getVideoFormat();
    VIDEO_TYPE videotype       = d->settingsBox->getVideoType();
    QString savePath           = d->settingsBox->getSaveFile();

    if(!KUrl(path).isValid())
    {
        path = QDir::tempPath();
        d->settingsBox->setTempDirPath(path);
    }

    d->thread->doPreProcessing(type, aspectRatio, frameWidth, frameHeight, path, item, format, videotype, audioPath, savePath);
}

void ExportDialog::busy(bool busy)
{
    d->busy = busy;

    if (d->busy)
    {
        setButtonIcon(Apply,    KIcon("process-stop"));
        setButtonText(Apply,    i18n("&Abort"));
        setButtonToolTip(Apply, i18n("Abort the current conversion"));
    }
    else
    {
        setButtonIcon(Apply,    KIcon("system-run"));
        setButtonText(Apply,    i18n("Con&vert"));
        setButtonToolTip(Apply, i18n("Start converting the images using current settings."));
    }

    d->settingsBox->setEnabled(!d->busy);
    d->listView->listView()->viewport()->setEnabled(!d->busy);

    d->busy ? d->page->setCursor(Qt::WaitCursor)
            : d->page->unsetCursor();
}

void ExportDialog::slotStartStop()
{
    if (!d->busy)
    {
        if (d->listView->imageUrls().isEmpty())
        {
            KMessageBox::error(this, i18n("There are no images in the list to process."));
            busy(false);
            slotAborted();
            return;
        }

        MyImageListViewItem* const item = setUpImageItems();

        processAll(item);

        d->progressBar->setMaximum(d->thread->getTotalFrames(item));
        d->progressBar->setValue(0);
        d->progressBar->show();
        d->progressBar->progressScheduled(i18n("Video SlideShow"), true, true);

        busy(true);

        if (!d->thread->isRunning())
            d->thread->start();
    }
    else
    {
        d->thread->cancel();
        busy(false);

        d->listView->cancelProcess();

        QTimer::singleShot(500, this, SLOT(slotAborted()));
    }
}

void ExportDialog::slotProcessedFrame(const ActionData& ad)
{
    switch(ad.action)
    {
        case TYPE_TRANSITION:
            d->progressBar->progressStatusChanged(i18n("Processing transition: %1, total frames: %2",
               ad.fileUrl.path(), ad.totalFrames));
            break;
        case TYPE_IMAGE:
            d->progressBar->progressStatusChanged(i18n("Processing image: %1, total frames: %2",
               ad.fileUrl.path(), ad.totalFrames));
            d->listView->processed(ad.fileUrl, true);
            break;
        default:
            slotShowError(i18n("Undefined action"));
            break;
    }

    d->progressBar->setValue(d->progressBar->value() + ad.totalFrames);
}

void ExportDialog::slotShowError(const QString& err)
{
    KMessageBox::error(this, err);
}

void ExportDialog::updateSettingWidget()
{
    // When more than one image is selected, only the first images data will be shown in settings Widget
    if(!d->listView->listView()->selectedItems().isEmpty())
    {
        MyImageListViewItem* const item = dynamic_cast<MyImageListViewItem*>(d->listView->listView()->selectedItems().at(0));
        d->settingsBox->updateData(item->getTime(), item->getTransition(), item->getTransitionSpeed(), item->EffectName());
    }
}

void ExportDialog::updateImageEffect(const QString& data, EFFECT effect)
{
    QList<QTreeWidgetItem*>           imgLst = d->listView->listView()->selectedItems();
    QList<QTreeWidgetItem*>::iterator it;

    for(it = imgLst.begin(); it != imgLst.end(); ++it)
        dynamic_cast<MyImageListViewItem*>((*it))->setEffectName(data, effect);
}

void ExportDialog::updateImageTime(int time)
{
    QList<QTreeWidgetItem*>           imgLst = d->listView->listView()->selectedItems();
    QList<QTreeWidgetItem*>::iterator it;

    for(it = imgLst.begin(); it != imgLst.end(); ++it)
        dynamic_cast<MyImageListViewItem*>((*it))->setTime(time);;
}

void ExportDialog::updateImageTransition(const QString& data, TRANSITION_TYPE type)
{
    QList<QTreeWidgetItem*>           imgLst = d->listView->listView()->selectedItems();
    QList<QTreeWidgetItem*>::iterator it;

    for(it = imgLst.begin(); it != imgLst.end(); ++it)
        dynamic_cast<MyImageListViewItem*>((*it))->setTransition(data, type);
}

void ExportDialog::updateImageTransSpeed(const QString& data, TRANSITION_SPEED speed)
{
    QList<QTreeWidgetItem*>           imgLst = d->listView->listView()->selectedItems();
    QList<QTreeWidgetItem*>::iterator it;

    for(it = imgLst.begin(); it != imgLst.end(); ++it)
        dynamic_cast<MyImageListViewItem*>((*it))->setTransitionSpeed(data, speed);
}

MyImageListViewItem* ExportDialog::setUpImageItems() const
{
    KPImagesListView* const view = d->listView->listView();
    MyImageListViewItem* prev    = 0;
    MyImageListViewItem* next    = 0;
    int total                    = view->topLevelItemCount();

    for(int i = 0; i < total; i++)
    {
        dynamic_cast<MyImageListViewItem*>(view->topLevelItem(i))->setPrevImageItem(prev);
        prev = dynamic_cast<MyImageListViewItem*>(view->topLevelItem(i));
        next = (i != total - 1) ? dynamic_cast<MyImageListViewItem*>(view->topLevelItem(i + 1)) : 0;
        dynamic_cast<MyImageListViewItem*>(view->topLevelItem(i))->setNextImageItem(next);
    }

    return dynamic_cast<MyImageListViewItem*>(view->topLevelItem(0));
}

} // namespace KIPIVideoSlideShowPlugin
