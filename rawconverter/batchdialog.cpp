/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-24
 * Description : Raw converter batch dialog
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "batchdialog.h"
#include "batchdialog.moc"

// C ANSI includes

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

// C++ includes

#include <cstdio>

// Qt includes

#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QHeaderView>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidgetItemIterator>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kio/renamedialog.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/dcrawsettingswidget.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>

// Local includes

#include "actions.h"
#include "actionthread.h"
#include "clistviewitem.h"
#include "imagedialog.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "rawdecodingiface.h"
#include "savesettingswidget.h"

using namespace KDcrawIface;

namespace KIPIRawConverterPlugin
{

class BatchDialogPriv
{
public:

    BatchDialogPriv()
    {
        busy                = false;
        convertBlink        = false;
        blinkConvertTimer   = 0;
        page                = 0;
        progressBar         = 0;
        listView            = 0;
        currentConvertItem  = 0;
        thread              = 0;
        saveSettingsBox     = 0;
        decodingSettingsBox = 0;
        about               = 0;
        iface               = 0;
    }

    bool                      busy;
    bool                      convertBlink;

    QTimer                   *blinkConvertTimer;

    QWidget                  *page;

    QStringList               fileList;

    QProgressBar             *progressBar;

    QTreeWidget              *listView;

    CListViewItem            *currentConvertItem;

    ActionThread             *thread;

    SaveSettingsWidget       *saveSettingsBox;

    DcrawSettingsWidget      *decodingSettingsBox;

    KIPIPlugins::KPAboutData *about;

    KIPI::Interface          *iface;
};

BatchDialog::BatchDialog(KIPI::Interface* iface)
           : KDialog(0), d(new BatchDialogPriv)
{
    d->iface = iface;

    setButtons(Help | Default | Apply | Close | User1 | User2);
    setDefaultButton(KDialog::Close);
    setButtonToolTip(Close, i18n("Exit RAW Converter"));
    setCaption(i18n("RAW Image Batch Converter"));
    setModal(false);
    setButtonIcon(User1, KIcon("list-add"));
    setButtonText(User1, i18n("&Add"));
    setButtonToolTip(User1, i18n("Add new RAW files to the list"));
    setButtonIcon(User2, KIcon("list-remove"));
    setButtonText(User2, i18n("&Remove"));
    setButtonToolTip(User2, i18n("Remove selected RAW files from the list"));

    d->page = new QWidget( this );
    setMainWidget( d->page );
    QGridLayout *mainLayout = new QGridLayout(d->page);

    //---------------------------------------------

    d->listView = new QTreeWidget(d->page);
    d->listView->setColumnCount(3);
    d->listView->setIconSize(QSize(64, 64));
    d->listView->setRootIsDecorated(false);
    d->listView->setSortingEnabled(false);
    d->listView->setSelectionMode(QAbstractItemView::MultiSelection);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setMinimumWidth(450);

    QStringList labels;
    labels.append( i18n("Thumbnail") );
    labels.append( i18n("RAW File") );
    labels.append( i18n("Target File") );
    labels.append( i18n("Camera") );
    d->listView->setHeaderLabels(labels);
    d->listView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    d->listView->header()->setResizeMode(1, QHeaderView::Stretch);
    d->listView->header()->setResizeMode(2, QHeaderView::Stretch);
    d->listView->header()->setResizeMode(3, QHeaderView::Stretch);

    // ---------------------------------------------------------------

    d->decodingSettingsBox = new DcrawSettingsWidget(d->page, DcrawSettingsWidget::SIXTEENBITS |
                                                              DcrawSettingsWidget::COLORSPACE |
                                                              DcrawSettingsWidget::POSTPROCESSING |
                                                              DcrawSettingsWidget::BLACKWHITEPOINTS);
    d->decodingSettingsBox->setObjectName("RawSettingsBox Expander");
    d->saveSettingsBox     = new SaveSettingsWidget(d->page);

#if KDCRAW_VERSION <= 0x000500
    d->decodingSettingsBox->addItem(d->saveSettingsBox, i18n("Save settings"));
    d->decodingSettingsBox->updateMinimumWidth();
#else
    d->decodingSettingsBox->insertItem(DcrawSettingsWidget::COLORMANAGEMENT+1, d->saveSettingsBox,
                                       SmallIcon("document-save"), i18n("Save settings"),
                                       QString("savesettings"), false);
#endif

    d->progressBar = new QProgressBar(d->page);
    d->progressBar->setMaximumHeight( fontMetrics().height()+2 );
    d->progressBar->hide();

    mainLayout->addWidget(d->listView,            0, 0, 3, 1);
    mainLayout->addWidget(d->decodingSettingsBox, 0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,         1, 1, 1, 1);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());
    mainLayout->setColumnStretch(0, 10);
#if KDCRAW_VERSION <= 0x000500
    mainLayout->setRowStretch(2, 10);
#endif

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("RAW Image Converter"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to batch convert RAW images"),
                   ki18n("(c) 2003-2005, Renchi Raju\n"
                         "(c) 2006-2009, Gilles Caulier"));

    d->about->addAuthor(ki18n("Renchi Raju"),
                       ki18n("Author"),
                             "renchi dot raju at gmail dot com");

    d->about->addAuthor(ki18n("Gilles Caulier"),
                       ki18n("Developer and maintainer"),
                             "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ---------------------------------------------------------------

    d->blinkConvertTimer = new QTimer(this);
    d->thread            = new ActionThread(this, d->iface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());

    // ---------------------------------------------------------------

    connect(d->blinkConvertTimer, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

    connect(d->saveSettingsBox, SIGNAL(signalSaveFormatChanged()),
            this, SLOT(slotSaveFormatChanged()));

    connect(d->decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            d->saveSettingsBox, SLOT(slotPopulateImageFormat(bool)));

    connect(d->decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            this, SLOT(slotSixteenBitsImageToggled(bool)));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(slotDefault()));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotStartStop()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotAddItems()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotRemoveItems()));

    connect(d->thread, SIGNAL(starting(const KIPIRawConverterPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIRawConverterPlugin::ActionData&)));

    connect(d->thread, SIGNAL(finished(const KIPIRawConverterPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIRawConverterPlugin::ActionData&)));

    connect(d->iface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
            this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
}

BatchDialog::~BatchDialog()
{
    delete d->about;
    delete d;
}

void BatchDialog::slotHelp()
{
    KToolInvocation::invokeHelp("rawconverter", "kipi-plugins");
}

void BatchDialog::slotSixteenBitsImageToggled(bool)
{
    // Dcraw do not provide a way to set brigness of image in 16 bits color depth.
    // We always set on this option. We drive brightness adjustment as post processing.
    d->decodingSettingsBox->setEnabledBrightnessSettings(true);
}

void BatchDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;

    // Stop current conversion if necessary
    if (d->busy) slotStartStop();
    saveSettings();
    d->listView->clear();
    e->accept();
}

void BatchDialog::slotClose()
{
    // Stop current conversion if necessary
    if (d->busy) slotStartStop();
    saveSettings();
    d->listView->clear();
    done(Close);
}

void BatchDialog::slotDefault()
{
    d->decodingSettingsBox->setDefaultSettings();
    d->saveSettingsBox->setDefaultSettings();
}

void BatchDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("RawConverter Settings"));

    d->decodingSettingsBox->setSixteenBits(group.readEntry("Sixteen Bits", false));
    d->decodingSettingsBox->setWhiteBalance((KDcrawIface::RawDecodingSettings::WhiteBalance)
                                            group.readEntry("White Balance",
                                            (int)KDcrawIface::RawDecodingSettings::CAMERA));
    d->decodingSettingsBox->setCustomWhiteBalance(group.readEntry("Custom White Balance", 6500));
    d->decodingSettingsBox->setCustomWhiteBalanceGreen(group.readEntry("Custom White Balance Green", 1.0));
    d->decodingSettingsBox->setFourColor(group.readEntry("Four Color RGB", false));
    d->decodingSettingsBox->setUnclipColor(group.readEntry("Unclip Color", 0));
    d->decodingSettingsBox->setDontStretchPixels(group.readEntry("Dont Stretch Pixels", false));
    d->decodingSettingsBox->setNoiseReduction(group.readEntry("Use Noise Reduction", false));
    d->decodingSettingsBox->setBrightness(group.readEntry("Brightness Multiplier", 1.0));
    d->decodingSettingsBox->setUseBlackPoint(group.readEntry("Use Black Point", false));
    d->decodingSettingsBox->setBlackPoint(group.readEntry("Black Point", 0));
    d->decodingSettingsBox->setUseWhitePoint(group.readEntry("Use White Point", false));
    d->decodingSettingsBox->setWhitePoint(group.readEntry("White Point", 0));
    d->decodingSettingsBox->setMedianFilterPasses(group.readEntry("Median Filter Passes", 0));
    d->decodingSettingsBox->setNRThreshold(group.readEntry("NR Threshold", 100));
    d->decodingSettingsBox->setUseCACorrection(group.readEntry("EnableCACorrection", false));
    d->decodingSettingsBox->setcaRedMultiplier(group.readEntry("caRedMultiplier", 1.0));
    d->decodingSettingsBox->setcaBlueMultiplier(group.readEntry("caBlueMultiplier", 1.0));
#if KDCRAW_VERSION >= 0x000500
    d->decodingSettingsBox->setAutoBrightness(group.readEntry("AutoBrightness", true));
#endif

    d->decodingSettingsBox->setQuality(
        (KDcrawIface::RawDecodingSettings::DecodingQuality)group.readEntry("Decoding Quality",
            (int)(KDcrawIface::RawDecodingSettings::BILINEAR)));

    d->decodingSettingsBox->setOutputColorSpace(
        (KDcrawIface::RawDecodingSettings::OutputColorSpace)group.readEntry("Output Color Space",
            (int)(KDcrawIface::RawDecodingSettings::SRGB)));

    d->saveSettingsBox->setFileFormat(
        (SaveSettingsWidget::OutputFormat)group.readEntry("Output Format",
            (int)(SaveSettingsWidget::OUTPUT_PNG)));

    d->saveSettingsBox->setConflictRule(
        (SaveSettingsWidget::ConflictRule)group.readEntry("Conflict",
            (int)(SaveSettingsWidget::OVERWRITE)));

    d->saveSettingsBox->slotPopulateImageFormat(d->decodingSettingsBox->sixteenBits());

    KConfigGroup group2 = config.group(QString("Batch Raw Converter Dialog"));
#if KDCRAW_VERSION > 0x000500
    d->decodingSettingsBox->readSettings();
#endif
    restoreDialogSize(group2);
}

void BatchDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("RawConverter Settings"));

    group.writeEntry("Sixteen Bits", d->decodingSettingsBox->sixteenBits());
    group.writeEntry("White Balance", (int)d->decodingSettingsBox->whiteBalance());
    group.writeEntry("Custom White Balance", d->decodingSettingsBox->customWhiteBalance());
    group.writeEntry("Custom White Balance Green", d->decodingSettingsBox->customWhiteBalanceGreen());
    group.writeEntry("Four Color RGB", d->decodingSettingsBox->useFourColor());
    group.writeEntry("Unclip Color", d->decodingSettingsBox->unclipColor());
    group.writeEntry("Dont Stretch Pixels", d->decodingSettingsBox->useDontStretchPixels());
    group.writeEntry("Use Noise Reduction", d->decodingSettingsBox->useNoiseReduction());
    group.writeEntry("Brightness Multiplier", d->decodingSettingsBox->brightness());
    group.writeEntry("Use Black Point", d->decodingSettingsBox->useBlackPoint());
    group.writeEntry("Black Point", d->decodingSettingsBox->blackPoint());
    group.writeEntry("Use White Point", d->decodingSettingsBox->useWhitePoint());
    group.writeEntry("White Point", d->decodingSettingsBox->whitePoint());
    group.writeEntry("Median Filter Passes", d->decodingSettingsBox->medianFilterPasses());
    group.writeEntry("NR Threshold", d->decodingSettingsBox->NRThreshold());
    group.writeEntry("EnableCACorrection", d->decodingSettingsBox->useCACorrection());
    group.writeEntry("caRedMultiplier", d->decodingSettingsBox->caRedMultiplier());
    group.writeEntry("caBlueMultiplier", d->decodingSettingsBox->caBlueMultiplier());
    group.writeEntry("Decoding Quality", (int)d->decodingSettingsBox->quality());
    group.writeEntry("Output Color Space", (int)d->decodingSettingsBox->outputColorSpace());
#if KDCRAW_VERSION >= 0x000500
    group.writeEntry("AutoBrightness", d->decodingSettingsBox->useAutoBrightness());
#endif
    group.writeEntry("Output Format", (int)d->saveSettingsBox->fileFormat());
    group.writeEntry("Conflict", (int)d->saveSettingsBox->conflictRule());

    KConfigGroup group2 = config.group(QString("Batch Raw Converter Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void BatchDialog::slotStartStop()
{
    if (!d->busy)
    {
        d->fileList.clear();

        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            CListViewItem *lvItem = dynamic_cast<CListViewItem*>(*it);
            if (lvItem)
            {
                if (lvItem->isEnabled())
                {
                    lvItem->setIcon(1, QIcon());
                    d->fileList.append(lvItem->url().path());
                }
            }
            ++it;
        }

        if (d->fileList.empty())
        {
            KMessageBox::error(this, i18n("There is no RAW file in the list to process."));
            busy(false);
            slotAborted();
            return;
        }

        d->progressBar->setMaximum(d->fileList.count());
        d->progressBar->setValue(0);
        d->progressBar->show();

        KDcrawIface::RawDecodingSettings rawDecodingSettings;
        rawDecodingSettings.sixteenBitsImage           = d->decodingSettingsBox->sixteenBits();
        rawDecodingSettings.whiteBalance               = d->decodingSettingsBox->whiteBalance();
        rawDecodingSettings.customWhiteBalance         = d->decodingSettingsBox->customWhiteBalance();
        rawDecodingSettings.customWhiteBalanceGreen    = d->decodingSettingsBox->customWhiteBalanceGreen();
        rawDecodingSettings.RGBInterpolate4Colors      = d->decodingSettingsBox->useFourColor();
        rawDecodingSettings.unclipColors               = d->decodingSettingsBox->unclipColor();
        rawDecodingSettings.DontStretchPixels          = d->decodingSettingsBox->useDontStretchPixels();
        rawDecodingSettings.enableNoiseReduction       = d->decodingSettingsBox->useNoiseReduction();
        rawDecodingSettings.brightness                 = d->decodingSettingsBox->brightness();
        rawDecodingSettings.enableBlackPoint           = d->decodingSettingsBox->useBlackPoint();
        rawDecodingSettings.blackPoint                 = d->decodingSettingsBox->blackPoint();
        rawDecodingSettings.enableWhitePoint           = d->decodingSettingsBox->useWhitePoint();
        rawDecodingSettings.whitePoint                 = d->decodingSettingsBox->whitePoint();
        rawDecodingSettings.medianFilterPasses         = d->decodingSettingsBox->medianFilterPasses();
        rawDecodingSettings.NRThreshold                = d->decodingSettingsBox->NRThreshold();
        rawDecodingSettings.enableCACorrection         = d->decodingSettingsBox->useCACorrection();
        rawDecodingSettings.caMultiplier[0]            = d->decodingSettingsBox->caRedMultiplier();
        rawDecodingSettings.caMultiplier[1]            = d->decodingSettingsBox->caBlueMultiplier();
        rawDecodingSettings.RAWQuality                 = d->decodingSettingsBox->quality();
        rawDecodingSettings.outputColorSpace           = d->decodingSettingsBox->outputColorSpace();
#if KDCRAW_VERSION >= 0x000500
        rawDecodingSettings.autoBrightness             = d->decodingSettingsBox->useAutoBrightness();
#endif

        d->thread->setRawDecodingSettings(rawDecodingSettings, d->saveSettingsBox->fileFormat());
        processOne();
    }
    else
    {
        d->blinkConvertTimer->stop();
        d->fileList.clear();
        d->thread->cancel();
        busy(false);

        if (d->currentConvertItem)
            d->currentConvertItem->setIcon(1, SmallIcon("dialog-cancel"));

        QTimer::singleShot(500, this, SLOT(slotAborted()));
    }
}

void BatchDialog::slotAddItems()
{
    KIPIPlugins::ImageDialog dlg(this, d->iface, false, true);
    KUrl::List urls = dlg.urls();
    if (!urls.isEmpty())
    {
        addItems(urls);
    }
}

void BatchDialog::slotRemoveItems()
{
    bool find;
    do
    {
        find = false;
        QTreeWidgetItemIterator it(d->listView);
        while (*it)
        {
            CListViewItem* item = dynamic_cast<CListViewItem*>(*it);
            if (item->isSelected())
            {
                delete item;
                find = true;
                break;
            }
        ++it;
        }
    }
    while(find);
}

void BatchDialog::slotAborted()
{
    d->progressBar->setValue(0);
    d->progressBar->hide();
}

void BatchDialog::addItems(const KUrl::List& itemList)
{
    QString ext;

    switch(d->saveSettingsBox->fileFormat())
    {
        case SaveSettingsWidget::OUTPUT_JPEG:
            ext = "jpg";
            break;
        case SaveSettingsWidget::OUTPUT_TIFF:
            ext = "tif";
            break;
        case SaveSettingsWidget::OUTPUT_PPM:
            ext = "ppm";
            break;
        case SaveSettingsWidget::OUTPUT_PNG:
            ext = "png";
            break;
    }

    KUrl::List urlList;

    QPixmap pix(SmallIcon("image-x-generic", KIconLoader::SizeLarge, KIconLoader::DisabledState));

    for (KUrl::List::const_iterator  it = itemList.constBegin();
         it != itemList.constEnd(); ++it)
    {
        KUrl url = *it;
        QFileInfo fi(url.path());
        if (fi.exists() && !findItem(url))
        {
            QString dest = fi.completeBaseName() + QString(".") + ext;
            new CListViewItem(d->listView, pix, url, dest);
            urlList.append(url);
        }
    }

    if (!urlList.empty())
    {
        if (!d->iface->hasFeature(KIPI::HostSupportsThumbnails))
            d->thread->thumbRawFiles(urlList);
        else
            d->iface->thumbnails(urlList, 256);

        d->thread->identifyRawFiles(urlList);
        if (!d->thread->isRunning())
            d->thread->start();
    }
}

void BatchDialog::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    CListViewItem *item = findItem(url);
    if (item)
    {
        if (!pix.isNull())
        {
            QPixmap pixmap = pix.scaled(64, 64, Qt::KeepAspectRatio);
            item->setThumbnail(pixmap);
        }
    }
}

void BatchDialog::slotSaveFormatChanged()
{
    QString ext;

    switch(d->saveSettingsBox->fileFormat())
    {
        case SaveSettingsWidget::OUTPUT_JPEG:
            ext = "jpg";
            break;
        case SaveSettingsWidget::OUTPUT_TIFF:
            ext = "tif";
            break;
        case SaveSettingsWidget::OUTPUT_PPM:
            ext = "ppm";
            break;
        case SaveSettingsWidget::OUTPUT_PNG:
            ext = "png";
            break;
    }

    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        CListViewItem *lvItem = dynamic_cast<CListViewItem*>(*it);
        if (lvItem)
        {
            if (lvItem->isEnabled())
            {
                QFileInfo fi(lvItem->url().path());
                QString dest = fi.completeBaseName() + QString(".") + ext;
                lvItem->setDestFileName(dest);
            }
        }
        ++it;
    }
}

void BatchDialog::processOne()
{
    if (d->fileList.empty())
    {
        busy(false);
        slotAborted();
        return;
    }

    QString file(d->fileList.first());
    d->fileList.pop_front();

    d->thread->processRawFile(KUrl(file));
    if (!d->thread->isRunning())
        d->thread->start();
}

void BatchDialog::busy(bool busy)
{
    d->busy = busy;
    enableButton(User1, !d->busy);
    enableButton(User2, !d->busy);

    if (d->busy)
    {
        setButtonIcon(Apply, KIcon("process-stop"));
        setButtonText(Apply, i18n("&Abort"));
        setButtonToolTip(Apply, i18n("Abort the current RAW file conversion"));
    }
    else
    {
        setButtonIcon(Apply, KIcon("system-run"));
        setButtonText(Apply, i18n("Con&vert"));
        setButtonToolTip(Apply, i18n("Start converting the RAW images using current settings."));
    }

    d->decodingSettingsBox->setEnabled(!d->busy);
    d->saveSettingsBox->setEnabled(!d->busy);
    d->listView->viewport()->setEnabled(!d->busy);

    d->busy ? d->page->setCursor(Qt::WaitCursor) : d->page->unsetCursor();
}

void BatchDialog::slotConvertBlinkTimerDone()
{
    if(d->convertBlink)
    {
        if (d->currentConvertItem)
            d->currentConvertItem->setProgressIcon(SmallIcon("arrow-right"));
    }
    else
    {
        if (d->currentConvertItem)
            d->currentConvertItem->setProgressIcon(SmallIcon("arrow-right-double"));
    }

    d->convertBlink = !d->convertBlink;
    d->blinkConvertTimer->start(500);
}

void BatchDialog::processing(const KUrl& url)
{
    d->currentConvertItem = findItem(url);
    if (d->currentConvertItem)
    {
        d->listView->setCurrentItem(d->currentConvertItem, true);
        d->listView->scrollToItem(d->currentConvertItem);
    }

    d->convertBlink = false;
    d->blinkConvertTimer->start(500);
}

void BatchDialog::processed(const KUrl& url, const QString& tmpFile)
{
    d->blinkConvertTimer->stop();
    QString destFile(d->currentConvertItem->destPath());

    if (d->saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0)
        {
            KIO::RenameDialog dlg(this, i18n("Save RAW image converted from '%1' as",
                                  d->currentConvertItem->url().fileName()),
                                  tmpFile, destFile,
                                  KIO::RenameDialog_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

            switch (dlg.exec())
            {
                case KIO::R_CANCEL:
                case KIO::R_SKIP:
                {
                    destFile = QString();
                    d->currentConvertItem->setProgressIcon(SmallIcon("dialog-cancel"));
                    break;
                }
                case KIO::R_RENAME:
                {
                    destFile = dlg.newDestUrl().path();
                    break;
                }
                default:    // Overwrite.
                    break;
            }
        }
    }

    if (!destFile.isEmpty())
    {
        if (::rename(QFile::encodeName(tmpFile), QFile::encodeName(destFile)) != 0)
        {
            KMessageBox::error(this, i18n("Failed to save image %1", destFile));
            d->currentConvertItem->setProgressIcon(SmallIcon("dialog-error"));
        }
        else
        {
            d->currentConvertItem->setDestFileName(QFileInfo(destFile).fileName());
            d->currentConvertItem->setProgressIcon(SmallIcon("dialog-ok"));

            // Assign Kipi host attributes from original RAW image.

            KIPI::ImageInfo orgInfo = d->iface->info(url);
            KIPI::ImageInfo newInfo = d->iface->info(KUrl(destFile));
            newInfo.cloneData(orgInfo);
        }
    }

    d->progressBar->setValue(d->progressBar->value()+1);
    d->currentConvertItem = 0;
}

void BatchDialog::processingFailed(const KUrl& /*url*/)
{
    d->currentConvertItem->setProgressIcon(SmallIcon("dialog-cancel"));
    d->progressBar->setValue(d->progressBar->value()+1);
    d->currentConvertItem = 0;
}

void BatchDialog::slotAction(const KIPIRawConverterPlugin::ActionData& ad)
{
    QString text;

    if (ad.starting)            // Something have been started...
    {
        switch (ad.action)
        {
            case(IDENTIFY):
            case(THUMBNAIL):
                break;
            case(PROCESS):
            {
                busy(true);
                processing(ad.fileUrl);
                break;
            }
            default:
            {
                kWarning() << "KIPIRawConverterPlugin: Unknown action";
                break;
            }
        }
    }
    else
    {
        if (!ad.success)        // Something is failed...
        {
            switch (ad.action)
            {
                case(IDENTIFY):
                case(THUMBNAIL):
                    break;
                case(PROCESS):
                {
                    processingFailed(ad.fileUrl);
                    processOne();
                    break;
                }
                default:
                {
                    kWarning() << "KIPIRawConverterPlugin: Unknown action";
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case(IDENTIFY):
                {
                    CListViewItem *item = findItem(ad.fileUrl);
                    if (item)
                    {
                        item->setIdentity(ad.message);
                    }
                    break;
                }
                case(THUMBNAIL):
                {
                    CListViewItem *item = findItem(ad.fileUrl);
                    if (item)
                    {
                        if (!ad.image.isNull())
                        {
                            QPixmap pix = QPixmap::fromImage(ad.image.scaled(64, 64, Qt::KeepAspectRatio));
                            item->setThumbnail(pix);
                        }
                    }
                    break;
                }
                case(PROCESS):
                {
                    processed(ad.fileUrl, ad.destPath);
                    processOne();
                    break;
                }
                default:
                {
                    kWarning() << "KIPIRawConverterPlugin: Unknown action";
                    break;
                }
            }
        }
    }
}

CListViewItem* BatchDialog::findItem(const KUrl& url)
{
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        CListViewItem *lvItem = dynamic_cast<CListViewItem*>(*it);
        if (lvItem)
        {
            if (lvItem->url() == url)
            {
                return lvItem;
            }
        }
        ++it;
    }

    return 0;
}

} // namespace KIPIRawConverterPlugin
