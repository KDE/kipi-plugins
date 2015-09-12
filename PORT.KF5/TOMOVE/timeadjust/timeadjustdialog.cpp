/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright (C) 2006-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "timeadjustdialog.h"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QCursor>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMap>
#include <QPointer>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QTimeEdit>
#include <QToolButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QMessageBox>

// KDE includes

#include <kconfig.h>
#include <klocalizedstring.h>
#include <kwindowconfig.h>

// Local includes

#include "actionthread.h"
#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kpmetadata.h"
#include "kpprogresswidget.h"
#include "kpversion.h"
#include "myimagelist.h"
#include "settingswidget.h"

namespace KIPITimeAdjustPlugin
{

class TimeAdjustDialog::Private
{

public:

    Private()
    {
        settingsView = 0;
        progressBar  = 0;
        listView     = 0;
        thread       = 0;
    }

    SettingsWidget*       settingsView;

    QMap<QUrl, QDateTime> itemsUsedMap;           // Map of item urls and Used Timestamps.
    QMap<QUrl, QDateTime> itemsUpdatedMap;        // Map of item urls and Updated Timestamps.
    QMap<QUrl, int>       itemsStatusMap;         // Map of item urls and status flag.

    KPProgressWidget*     progressBar;
    MyImageList*          listView;

    ActionThread*         thread;
};

TimeAdjustDialog::TimeAdjustDialog(QWidget* const /*parent*/)
    : KPToolDialog(0), d(new Private)
{
    setWindowTitle(i18n("Adjust Time & Date"));
    setModal(false);
    setMinimumSize(900, 500);

    startButton()->setText(i18nc("@action:button", "&Apply"));
    startButton()->setToolTip(i18nc("@info:tooltip", "Write the corrected date and time for each image"));
    startButton()->setIcon(QIcon::fromTheme(QLatin1String("dialog-ok-apply")));

    QWidget* const mainWidget = new QWidget(this);
    setMainWidget(mainWidget);
    QGridLayout* const mainLayout = new QGridLayout(mainWidget);
    d->listView                   = new MyImageList(mainWidget);
    d->settingsView               = new SettingsWidget(mainWidget);
    d->settingsView->setImageList(d->listView);
    d->progressBar                = new KPProgressWidget(mainWidget);
    d->progressBar->reset();
    d->progressBar->hide();

    // ----------------------------------------------------------------------------

    mainLayout->addWidget(d->listView,     0, 0, 2, 1);
    mainLayout->addWidget(d->settingsView, 0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,  1, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(0, 10);
    mainLayout->setMargin(0);

    // -- About data and help button ----------------------------------------

    KPAboutData* const about = new KPAboutData(ki18n("Time Adjust"),
                                   0,
                                   KAboutLicense::GPL,
                                   ki18n("A Kipi plugin for adjusting the timestamp of picture files"),
                                   ki18n("(c) 2003-2005, Jesper K. Pedersen\n"
                                         "(c) 2006-2014, Gilles Caulier\n"
                                         "(c) 2012, Smit Mehta"));

    about->addAuthor(ki18n("Jesper K. Pedersen").toString(),
                     ki18n("Author").toString(),
                     QLatin1String("blackie at kde dot org"));

    about->addAuthor(ki18n("Gilles Caulier").toString(),
                     ki18n("Developer").toString(),
                     QLatin1String("caulier dot gilles at gmail dot com"));

    about->addAuthor(ki18n("Smit Mehta").toString(),
                     ki18n("Developer").toString(),
                     QLatin1String("smit dot meh at gmail dot com"));

    about->addAuthor(ki18n("Pieter Edelman").toString(),
                     ki18n("Developer").toString(),
                     QLatin1String("p dot edelman at gmx dot net"));

    about->setHandbookEntry(QLatin1String("timeadjust"));
    setAboutData(about);

    // -- Thread Slots/Signals ----------------------------------------------

    d->thread = new ActionThread(this);

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()));

    connect(d->thread, SIGNAL(signalProcessStarted(QUrl)),
            this, SLOT(slotProcessStarted(QUrl)));

    connect(d->thread, SIGNAL(signalProcessEnded(QUrl,int)),
            this, SLOT(slotProcessEnded(QUrl,int)));

    connect(d->progressBar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotCancelThread()));

    // -- Dialog Slots/Signals -----------------------------------------------

    connect(startButton(), &QPushButton::clicked,
            this, &TimeAdjustDialog::slotApplyClicked);

    connect(this, &KPToolDialog::cancelClicked,
            this, &TimeAdjustDialog::slotCancelThread);

    connect(this, &QDialog::finished,
            this, &TimeAdjustDialog::slotDialogFinished);

    connect(d->settingsView, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotReadTimestamps()));

    // -----------------------------------------------------------------------

    setBusy(false);
    readSettings();
}

TimeAdjustDialog::~TimeAdjustDialog()
{
    delete d;
}

void TimeAdjustDialog::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotDialogFinished();
    e->accept();
}

void TimeAdjustDialog::slotDialogFinished()
{
    saveSettings();
}

void TimeAdjustDialog::readSettings()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group   = config.group(QLatin1String("Time Adjust Settings"));

    d->settingsView->readSettings(group);

    KConfigGroup group2 = config.group(QLatin1String("Time Adjust Dialog"));
    KWindowConfig::restoreWindowSize(windowHandle(), group2);
}

void TimeAdjustDialog::saveSettings()
{
    KConfig config(QLatin1String("kipirc"));
    KConfigGroup group = config.group(QLatin1String("Time Adjust Settings"));

    d->settingsView->saveSettings(group);

    KConfigGroup group2 = config.group(QLatin1String("Time Adjust Dialog"));
    KWindowConfig::saveWindowSize(windowHandle(), group2);
    config.sync();
}

void TimeAdjustDialog::disableApplTimestamp()
{
    d->settingsView->disableApplTimestamp();
}

void TimeAdjustDialog::addItems(const QList<QUrl>& imageUrls)
{
    d->itemsStatusMap.clear();
    d->listView->listView()->clear();
    d->itemsUsedMap.clear();
    d->itemsUpdatedMap.clear();

    foreach (const QUrl& url, imageUrls)
    {
        d->itemsUsedMap.insert(url, QDateTime());
    }

    d->listView->slotAddImages(imageUrls);
    slotReadTimestamps();
}

void TimeAdjustDialog::slotReadTimestamps()
{
    foreach (const QUrl& url, d->itemsUsedMap.keys())
    {
        d->itemsUsedMap.insert(url, QDateTime());
    }

    TimeAdjustSettings prm = d->settingsView->settings();

    switch (prm.dateSource)
    {
        case TimeAdjustSettings::APPDATE:
        {
            readApplicationTimestamps();
            break;
        }

        case TimeAdjustSettings::FILEDATE:
        {
            readFileTimestamps();
            break;
        }

        case TimeAdjustSettings::METADATADATE:
        {
            readMetadataTimestamps();
            break;
        }

        default:  // CUSTOMDATE
        {
            foreach (const QUrl& url, d->itemsUsedMap.keys())
            {
                d->itemsUsedMap.insert(url, d->settingsView->customDate());
            }
            break;
        }
    }

    updateListView();
}

void TimeAdjustDialog::readApplicationTimestamps()
{
    QList<QUrl> floatingDateItems;

    foreach (const QUrl& url, d->itemsUsedMap.keys())
    {
        KPImageInfo info(url);

        if (info.isExactDate())
        {
            d->itemsUsedMap.insert(url, info.date());
        }
        else
        {
            floatingDateItems.append(url);
            d->itemsUsedMap.insert(url, QDateTime());
        }
    }

    // TODO (blackie): handle all items in listview with inexact timestamp through floatingDateItems.
}

void TimeAdjustDialog::readFileTimestamps()
{
    foreach (const QUrl& url, d->itemsUsedMap.keys())
    {
        QFileInfo fileInfo(url.toLocalFile());
        d->itemsUsedMap.insert(url, fileInfo.lastModified());
    }
}

void TimeAdjustDialog::readMetadataTimestamps()
{
    foreach (const QUrl& url, d->itemsUsedMap.keys())
    {
        KPMetadata meta;

        if (!meta.load(url.toLocalFile()))
        {
            d->itemsUsedMap.insert(url, QDateTime());
            continue;
        }

        QDateTime curImageDateTime;
        TimeAdjustSettings prm = d->settingsView->settings();

        switch (prm.metadataSource)
        {
            case TimeAdjustSettings::EXIFIPTCXMP:
                curImageDateTime = meta.getImageDateTime();
                break;
            case TimeAdjustSettings::EXIFCREATED:
                curImageDateTime = QDateTime::fromString(meta.getExifTagString("Exif.Image.DateTime"),
                                                         QLatin1String("yyyy:MM:dd hh:mm:ss"));
                break;
            case TimeAdjustSettings::EXIFORIGINAL:
                curImageDateTime = QDateTime::fromString(meta.getExifTagString("Exif.Photo.DateTimeOriginal"),
                                                         QLatin1String("yyyy:MM:dd hh:mm:ss"));
                break;
            case TimeAdjustSettings::EXIFDIGITIZED:
                curImageDateTime = QDateTime::fromString(meta.getExifTagString("Exif.Photo.DateTimeDigitized"),
                                                         QLatin1String("yyyy:MM:dd hh:mm:ss"));
                break;
            case TimeAdjustSettings::IPTCCREATED:
                // we have to truncate the timezone from the time, otherwise it cannot be converted to a QTime
                curImageDateTime = QDateTime(QDate::fromString(meta.getIptcTagString("Iptc.Application2.DateCreated"), 
                                                               Qt::ISODate),
                                             QTime::fromString(meta.getIptcTagString("Iptc.Application2.TimeCreated").left(8),
                                                               Qt::ISODate));
                break;
            case TimeAdjustSettings::XMPCREATED:
                curImageDateTime = QDateTime::fromString(meta.getXmpTagString("Xmp.xmp.CreateDate"),
                                                         QLatin1String("yyyy:MM:dd hh:mm:ss"));
                break;
            default:
                // curImageDateTime stays invalid
                break;
        };

        d->itemsUsedMap.insert(url, curImageDateTime);
    }
}

void TimeAdjustDialog::slotApplyClicked()
{
    d->itemsStatusMap.clear();

    TimeAdjustSettings prm = d->settingsView->settings();

    if (prm.atLeastOneUpdateToProcess())
    {
        d->progressBar->show();
        d->progressBar->progressScheduled(i18n("Adjust Time and Date"), true, true);
        d->progressBar->progressThumbnailChanged(QIcon::fromTheme(QLatin1String("kipi")).pixmap(22, 22));
        d->progressBar->setMaximum(d->itemsUsedMap.keys().size());
        d->thread->setSettings(prm);
        d->thread->setUpdatedDates(d->itemsUpdatedMap);

        if (!d->thread->isRunning())
        {
            d->thread->start();
        }

        setBusy(true);
    }
    else
    {
        QMessageBox::critical(QApplication::activeWindow(),
                              i18n("Adjust Time & Date"),
                              i18n("Select at least one option"));
    }
}

void TimeAdjustDialog::slotCancelThread()
{
    if (d->thread->isRunning())
    {
        d->thread->cancel();
    }
}

void TimeAdjustDialog::setBusy(bool busy)
{
    setRejectButtonMode(busy ? QDialogButtonBox::Cancel : QDialogButtonBox::Close);
    startButton()->setEnabled(!busy);
}

void TimeAdjustDialog::slotProcessStarted(const QUrl& url)
{
    d->listView->processing(url);
}

void TimeAdjustDialog::slotProcessEnded(const QUrl& url, int status)
{
    d->listView->processed(url, (status == MyImageList::NOPROCESS_ERROR));
    d->itemsStatusMap.insert(url, status);
    d->progressBar->setValue(d->progressBar->value()+1);
}

void TimeAdjustDialog::slotThreadFinished()
{
    d->listView->setStatus(d->itemsStatusMap);
    setBusy(false);
    d->progressBar->hide();
    d->progressBar->progressCompleted();
    saveSettings();
}

void TimeAdjustDialog::updateListView()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    TimeAdjustSettings prm = d->settingsView->settings();

    d->listView->setItemDates(d->itemsUsedMap, MyImageList::TIMESTAMP_USED, prm);

    // TODO : this loop can take a while, especially when items mist is huge.
    //        Moving this loop code to ActionThread is the right way for the future.

    foreach (const QUrl& url, d->itemsUsedMap.keys())
    {
        d->itemsUpdatedMap.insert(url, d->settingsView->calculateAdjustedDate(d->itemsUsedMap.value(url)));
    }

    d->listView->setItemDates(d->itemsUpdatedMap, MyImageList::TIMESTAMP_UPDATED, prm);

    QApplication::restoreOverrideCursor();
}

}  // namespace KIPITimeAdjustPlugin
