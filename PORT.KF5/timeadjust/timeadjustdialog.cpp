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
 * Copyright (C) 2006-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "timeadjustdialog.moc"

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

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kvbox.h>

// Local includes

#include "actionthread.h"
#include "clockphotodialog.h"
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

    QMap<KUrl, QDateTime> itemsUsedMap;           // Map of item urls and Used Timestamps.
    QMap<KUrl, QDateTime> itemsUpdatedMap;        // Map of item urls and Updated Timestamps.
    QMap<KUrl, int>       itemsStatusMap;         // Map of item urls and status flag.

    KPProgressWidget*     progressBar;
    MyImageList*          listView;

    ActionThread*         thread;
};

TimeAdjustDialog::TimeAdjustDialog(QWidget* const /*parent*/)
    : KPToolDialog(0), d(new Private)
{
    setButtons(Help | Apply | Close);
    setDefaultButton(Apply);
    setCaption(i18n("Adjust Time & Date"));
    setModal(false);
    setMinimumSize(900, 500);

    setMainWidget(new QWidget(this));
    QGridLayout* const mainLayout = new QGridLayout(mainWidget());
    d->listView                   = new MyImageList(mainWidget());
    d->settingsView               = new SettingsWidget(mainWidget());
    d->settingsView->setImageList(d->listView);
    d->progressBar                = new KPProgressWidget(mainWidget());
    d->progressBar->reset();
    d->progressBar->hide();

    // ----------------------------------------------------------------------------

    mainLayout->addWidget(d->listView,     0, 0, 2, 1);
    mainLayout->addWidget(d->settingsView, 0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,  1, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(0, 10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());

    // -- About data and help button ----------------------------------------

    KPAboutData* const about = new KPAboutData(ki18n("Time Adjust"),
                                   0,
                                   KAboutData::License_GPL,
                                   ki18n("A Kipi plugin for adjusting the timestamp of picture files"),
                                   ki18n("(c) 2003-2005, Jesper K. Pedersen\n"
                                         "(c) 2006-2014, Gilles Caulier\n"
                                         "(c) 2012, Smit Mehta"));

    about->addAuthor(ki18n("Jesper K. Pedersen"),
                     ki18n("Author"),
                     "blackie at kde dot org");

    about->addAuthor(ki18n("Gilles Caulier"),
                     ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Smit Mehta"),
                     ki18n("Developer"),
                     "smit dot meh at gmail dot com");

    about->addAuthor(ki18n("Pieter Edelman"),
                     ki18n("Developer"),
                     "p dot edelman at gmx dot net");

    about->setHandbookEntry("timeadjust");
    setAboutData(about);

    // -- Thread Slots/Signals ----------------------------------------------

    d->thread = new ActionThread(this);

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()));

    connect(d->thread, SIGNAL(signalProcessStarted(KUrl)),
            this, SLOT(slotProcessStarted(KUrl)));

    connect(d->thread, SIGNAL(signalProcessEnded(KUrl,int)),
            this, SLOT(slotProcessEnded(KUrl,int)));

    connect(d->progressBar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotCancelThread()));

    // -- Dialog Slots/Signals -----------------------------------------------

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotApplyClicked()));

    connect(this, SIGNAL(signalMyCloseClicked()),
            this, SLOT(slotCloseClicked()));

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
    if (!e) return;

    saveSettings();
    e->accept();
}

void TimeAdjustDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group   = config.group(QString("Time Adjust Settings"));

    d->settingsView->readSettings(group);

    KConfigGroup group2 = config.group(QString("Time Adjust Dialog"));
    restoreDialogSize(group2);
}

void TimeAdjustDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Time Adjust Settings"));

    d->settingsView->saveSettings(group);

    KConfigGroup group2 = config.group(QString("Time Adjust Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void TimeAdjustDialog::disableApplTimestamp()
{
    d->settingsView->disableApplTimestamp();
}

void TimeAdjustDialog::addItems(const KUrl::List& imageUrls)
{
    d->itemsStatusMap.clear();
    d->listView->listView()->clear();
    d->itemsUsedMap.clear();
    d->itemsUpdatedMap.clear();

    foreach (const KUrl& url, imageUrls)
    {
        d->itemsUsedMap.insert(url, QDateTime());
    }

    d->listView->slotAddImages(imageUrls);
    slotReadTimestamps();
}

void TimeAdjustDialog::slotReadTimestamps()
{
    foreach (const KUrl& url, d->itemsUsedMap.keys())
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
            foreach (const KUrl& url, d->itemsUsedMap.keys())
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
    KUrl::List floatingDateItems;

    foreach (const KUrl& url, d->itemsUsedMap.keys())
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
    foreach (const KUrl& url, d->itemsUsedMap.keys())
    {
        QFileInfo fileInfo(url.toLocalFile());
        d->itemsUsedMap.insert(url, fileInfo.lastModified());
    }
}

void TimeAdjustDialog::readMetadataTimestamps()
{
    foreach (const KUrl& url, d->itemsUsedMap.keys())
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
                                                         "yyyy:MM:dd hh:mm:ss");
                break;
            case TimeAdjustSettings::EXIFORIGINAL:
                curImageDateTime = QDateTime::fromString(meta.getExifTagString("Exif.Photo.DateTimeOriginal"),
                                                         "yyyy:MM:dd hh:mm:ss");
                break;
            case TimeAdjustSettings::EXIFDIGITIZED:
                curImageDateTime = QDateTime::fromString(meta.getExifTagString("Exif.Photo.DateTimeDigitized"),
                                                         "yyyy:MM:dd hh:mm:ss");
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
                                                         "yyyy:MM:dd hh:mm:ss");
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
        d->progressBar->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));
        d->progressBar->setMaximum(d->itemsUsedMap.keys().size());
        d->thread->setSettings(prm);
        d->thread->setUpdatedDates(d->itemsUpdatedMap);

        if (!d->thread->isRunning())
        {
            d->thread->start();
        }

        enableButton(Apply, false);
        setBusy(true);
    }
    else
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Select at least one option"),
                           i18n("Adjust Time & Date"));
    }
}

void TimeAdjustDialog::slotCancelThread()
{
    if (d->thread->isRunning())
    {
        d->thread->cancel();
    }
}

void TimeAdjustDialog::slotButtonClicked(int button)
{
    emit buttonClicked(static_cast<KDialog::ButtonCode>(button));

    switch (button)
    {
        case Apply:
            emit applyClicked();
            break;
        case Close:
            emit signalMyCloseClicked();
            break;
        default:
            break;
    }
}

void TimeAdjustDialog::setBusy(bool busy)
{
    if (busy)
    {
        disconnect(this, SIGNAL(signalMyCloseClicked()),
                   this, SLOT(slotCloseClicked()));

        setButtonGuiItem(Close, KStandardGuiItem::cancel());
        enableButton(Apply, false);

        connect(this, SIGNAL(signalMyCloseClicked()),
                this, SLOT(slotCancelThread()));
    }
    else
    {
        disconnect(this, SIGNAL(signalMyCloseClicked()),
                   this, SLOT(slotCancelThread()));

        setButtonGuiItem(Close, KStandardGuiItem::close());
        enableButton(Apply, true);

        connect(this, SIGNAL(signalMyCloseClicked()),
                this, SLOT(slotCloseClicked()));
     }
}

void TimeAdjustDialog::slotCloseClicked()
{
    saveSettings();
    done(Close);
}

void TimeAdjustDialog::slotProcessStarted(const KUrl& url)
{
    d->listView->processing(url);
}

void TimeAdjustDialog::slotProcessEnded(const KUrl& url, int status)
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
    enableButton(Apply, true);
    saveSettings();
}

void TimeAdjustDialog::updateListView()
{
    kapp->setOverrideCursor(Qt::WaitCursor);

    TimeAdjustSettings prm = d->settingsView->settings();

    d->listView->setItemDates(d->itemsUsedMap, MyImageList::TIMESTAMP_USED, prm);

    // TODO : this loop can take a while, especially when items mist is huge.
    //        Moving this loop code to ActionThread is the right way for the future.

    foreach (const KUrl& url, d->itemsUsedMap.keys())
    {
        d->itemsUpdatedMap.insert(url, d->settingsView->calculateAdjustedDate(d->itemsUsedMap.value(url)));
    }

    d->listView->setItemDates(d->itemsUpdatedMap, MyImageList::TIMESTAMP_UPDATED, prm);

    kapp->restoreOverrideCursor();
}

}  // namespace KIPITimeAdjustPlugin
