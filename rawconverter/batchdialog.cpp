/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-24
 * Description : Raw converter batch dialog
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C Ansi includes.

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

// C++ includes.

#include <cstdio> 

// Qt includes.

#include <Q3Dict>
#include <QTimer>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QFileInfo>
#include <QCloseEvent>
#include <QEvent>
#include <QPushButton>
#include <QFile>
#include <QPixmap>
#include <QProgressBar>

// KDE includes.

#include <kmenu.h>
#include <kcursor.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kio/renamedialog.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <ktoolinvocation.h>
#include <kpushbutton.h>

// LibKDcraw includes.

#include <libkdcraw/dcrawsettingswidget.h>

// Local includes.

#include "actions.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "rawdecodingiface.h"
#include "savesettingswidget.h"
#include "actionthread.h"
#include "clistviewitem.h"
#include "batchdialog.h"
#include "batchdialog.moc"

namespace KIPIRawConverterPlugin
{

class BatchDialogPriv
{
public:

    BatchDialogPriv()
    {
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
    }

    bool                              convertBlink;

    QTimer                           *blinkConvertTimer;

    QWidget                          *page;

    Q3Dict<RawItem>                   itemDict;

    QStringList                       fileList;

    QProgressBar                     *progressBar;

    K3ListView                       *listView;

    RawItem                          *currentConvertItem;

    ActionThread                     *thread;

    SaveSettingsWidget               *saveSettingsBox;

    KDcrawIface::DcrawSettingsWidget *decodingSettingsBox;

    KIPIPlugins::KPAboutData         *about;
};

BatchDialog::BatchDialog(QWidget* /*parent*/)
           : KDialog(0)
{
    d = new BatchDialogPriv;
    setButtons(Help | Default | User1 | User2 | Close);
    setDefaultButton(KDialog::Close);
    setButtonText(User1, i18n("Con&vert"));
    setButtonText(User2, i18n("&Abort"));
    setCaption(i18n("Raw Images Batch Converter"));
    setModal(false);

    d->currentConvertItem = 0;
    d->thread             = 0;

    d->page = new QWidget( this );
    setMainWidget( d->page );
    QGridLayout *mainLayout = new QGridLayout(d->page);

    //---------------------------------------------

    d->listView = new K3ListView(d->page);
    d->listView->addColumn( i18n("Thumbnail") );
    d->listView->addColumn( i18n("Raw File") );
    d->listView->addColumn( i18n("Target File") );
    d->listView->addColumn( i18n("Camera") );
    d->listView->setResizeMode(Q3ListView::AllColumns);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setSorting(-1);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setSelectionMode(Q3ListView::Single);
    d->listView->setMinimumWidth(450);

    // ---------------------------------------------------------------

    d->decodingSettingsBox = new KDcrawIface::DcrawSettingsWidget(d->page, true, true, true);
    d->saveSettingsBox     = new SaveSettingsWidget(d->page);
    d->decodingSettingsBox->addTab(d->saveSettingsBox, i18n("Save settings"));

    d->progressBar = new QProgressBar(d->page);
    d->progressBar->setMaximumHeight( fontMetrics().height()+2 );
    d->progressBar->hide();

    mainLayout->addWidget(d->listView, 0, 0, 3, 1);
    mainLayout->addWidget(d->decodingSettingsBox, 0, 1, 1, 1);
    mainLayout->addWidget(d->progressBar, 1, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(2, 10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("RAW Image Converter"),
                   QByteArray(),
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to batch convert Raw images"),
                   ki18n("(c) 2003-2005, Renchi Raju\n"
                         "(c) 2006-2007, Gilles Caulier"));

    d->about->addAuthor(ki18n("Renchi Raju"), 
                       ki18n("Author"),
                             "renchi at pooh dot tam dot uiuc dot edu");

    d->about->addAuthor(ki18n("Gilles Caulier"), 
                       ki18n("Developper and maintainer"),
                             "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KPushButton *helpButton = button( Help );
    KHelpMenu* helpMenu     = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook       = new QAction(i18n("Plugin Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    helpButton->setDelayedMenu( helpMenu->menu() );

    // ---------------------------------------------------------------

    setButtonToolTip( User1, i18n("<p>Start converting the Raw images from current settings"));
    setButtonToolTip( User2, i18n("<p>Abort the current Raw files conversion"));
    setButtonToolTip( Close, i18n("<p>Exit Raw Converter"));

    d->blinkConvertTimer = new QTimer(this);
    d->thread            = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(d->blinkConvertTimer, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

    connect(d->saveSettingsBox, SIGNAL(signalSaveFormatChanged()),
            this, SLOT(slotSaveFormatChanged()));

    connect(d->decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            d->saveSettingsBox, SLOT(slotPopulateImageFormat(bool)));

    connect(this, SIGNAL(closeClicked()),
            this, SLOT(slotClose()));

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(slotDefault()));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotUser1()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(slotUser2()));

    connect(d->thread, SIGNAL(starting(const ActionData&)),
            this, SLOT(slotAction(const ActionData&)));

    connect(d->thread, SIGNAL(finished(const ActionData&)),
            this, SLOT(slotAction(const ActionData&)));

    // ---------------------------------------------------------------

    d->itemDict.setAutoDelete(true);
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

void BatchDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    d->blinkConvertTimer->stop();
    d->thread->cancel();
    saveSettings();
    e->accept();
}

void BatchDialog::slotClose()
{
    d->blinkConvertTimer->stop();
    d->thread->cancel();
    saveSettings();
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
    d->decodingSettingsBox->setCameraWB(group.readEntry("Use Camera WB", true));
    d->decodingSettingsBox->setAutoColorBalance(group.readEntry("Use Auto Color Balance", true));
    d->decodingSettingsBox->setFourColor(group.readEntry("Four Color RGB", false));
    d->decodingSettingsBox->setUnclipColor(group.readEntry("Unclip Color", 0));
    d->decodingSettingsBox->setDontStretchPixels(group.readEntry("Dont Stretch Pixels", false));
    d->decodingSettingsBox->setNoiseReduction(group.readEntry("Use Noise Reduction", false));
    d->decodingSettingsBox->setBrightness(group.readEntry("Brightness Multiplier", 1.0));
    d->decodingSettingsBox->setUseBlackPoint(group.readEntry("Use Black Point", false));
    d->decodingSettingsBox->setBlackPoint(group.readEntry("Black Point", 0));
    d->decodingSettingsBox->setNRThreshold(group.readEntry("NR Threshold", 100));
    d->decodingSettingsBox->setUseCACorrection(group.readEntry("EnableCACorrection", false));
    d->decodingSettingsBox->setcaRedMultiplier(group.readEntry("caRedMultiplier", 1.0));
    d->decodingSettingsBox->setcaBlueMultiplier(group.readEntry("caBlueMultiplier", 1.0));
    d->decodingSettingsBox->setUseColorMultipliers(group.readEntry("Use Color Multipliers", false));
    d->decodingSettingsBox->setcolorMultiplier1(group.readEntry("Color Multiplier1", 1.0));
    d->decodingSettingsBox->setcolorMultiplier2(group.readEntry("Color Multiplier2", 1.0));
    d->decodingSettingsBox->setcolorMultiplier3(group.readEntry("Color Multiplier3", 1.0));
    d->decodingSettingsBox->setcolorMultiplier4(group.readEntry("Color Multiplier4", 1.0));

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
    restoreDialogSize(group2);
}

void BatchDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("RawConverter Settings"));

    group.writeEntry("Sixteen Bits", d->decodingSettingsBox->sixteenBits());
    group.writeEntry("Use Camera WB", d->decodingSettingsBox->useCameraWB());
    group.writeEntry("Use Auto Color Balance", d->decodingSettingsBox->useAutoColorBalance());
    group.writeEntry("Four Color RGB", d->decodingSettingsBox->useFourColor());
    group.writeEntry("Unclip Color", d->decodingSettingsBox->unclipColor());
    group.writeEntry("Dont Stretch Pixels", d->decodingSettingsBox->useDontStretchPixels());
    group.writeEntry("Use Noise Reduction", d->decodingSettingsBox->useNoiseReduction());
    group.writeEntry("Brightness Multiplier", d->decodingSettingsBox->brightness());
    group.writeEntry("Use Black Point", d->decodingSettingsBox->useBlackPoint());
    group.writeEntry("Black Point", d->decodingSettingsBox->blackPoint());
    group.writeEntry("NR Threshold", d->decodingSettingsBox->NRThreshold());
    group.writeEntry("EnableCACorrection", d->decodingSettingsBox->useCACorrection());
    group.writeEntry("caRedMultiplier", d->decodingSettingsBox->caRedMultiplier());
    group.writeEntry("caBlueMultiplier", d->decodingSettingsBox->caBlueMultiplier());
    group.writeEntry("Decoding Quality", (int)d->decodingSettingsBox->quality());
    group.writeEntry("Output Color Space", (int)d->decodingSettingsBox->outputColorSpace());
    group.writeEntry("Use Color Multipliers", d->decodingSettingsBox->useColorMultipliers());
    group.writeEntry("Color Multiplier1", d->decodingSettingsBox->colorMultiplier1());
    group.writeEntry("Color Multiplier2", d->decodingSettingsBox->colorMultiplier2());
    group.writeEntry("Color Multiplier3", d->decodingSettingsBox->colorMultiplier3());
    group.writeEntry("Color Multiplier4", d->decodingSettingsBox->colorMultiplier4());

    group.writeEntry("Output Format", (int)d->saveSettingsBox->fileFormat());
    group.writeEntry("Conflict", (int)d->saveSettingsBox->conflictRule());

    KConfigGroup group2 = config.group(QString("Batch Raw Converter Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void BatchDialog::slotUser1()
{
    d->fileList.clear();

    Q3ListViewItemIterator it( d->listView );
    while ( it.current() ) 
    {
        CListViewItem *item = (CListViewItem*) it.current();
        if (item->isEnabled())
        {
            item->setPixmap(1, 0);
            d->fileList.append(item->rawItem->directory + QString("/") + item->rawItem->src);
        }
        ++it;
    }

    if (d->fileList.empty()) 
    {
        KMessageBox::error(this, i18n("There is no Raw file to process in the list!"));
        busy(false);
        slotAborted();
        return;
    }

    d->progressBar->setMaximum(d->fileList.count());
    d->progressBar->setValue(0);
    d->progressBar->show();

    KDcrawIface::RawDecodingSettings rawDecodingSettings;
    rawDecodingSettings.sixteenBitsImage           = d->decodingSettingsBox->sixteenBits();
    rawDecodingSettings.cameraColorBalance         = d->decodingSettingsBox->useCameraWB();
    rawDecodingSettings.automaticColorBalance      = d->decodingSettingsBox->useAutoColorBalance();
    rawDecodingSettings.RGBInterpolate4Colors      = d->decodingSettingsBox->useFourColor();
    rawDecodingSettings.unclipColors               = d->decodingSettingsBox->unclipColor();
    rawDecodingSettings.DontStretchPixels          = d->decodingSettingsBox->useDontStretchPixels();
    rawDecodingSettings.enableNoiseReduction       = d->decodingSettingsBox->useNoiseReduction();
    rawDecodingSettings.brightness                 = d->decodingSettingsBox->brightness();
    rawDecodingSettings.enableBlackPoint           = d->decodingSettingsBox->useBlackPoint();
    rawDecodingSettings.blackPoint                 = d->decodingSettingsBox->blackPoint();
    rawDecodingSettings.NRThreshold                = d->decodingSettingsBox->NRThreshold();
    rawDecodingSettings.enableCACorrection         = d->decodingSettingsBox->useCACorrection();
    rawDecodingSettings.caMultiplier[0]            = d->decodingSettingsBox->caRedMultiplier();
    rawDecodingSettings.caMultiplier[1]            = d->decodingSettingsBox->caBlueMultiplier();
    rawDecodingSettings.RAWQuality                 = d->decodingSettingsBox->quality();
    rawDecodingSettings.outputColorSpace           = d->decodingSettingsBox->outputColorSpace();
    rawDecodingSettings.enableColorMultipliers     = d->decodingSettingsBox->useColorMultipliers();
    rawDecodingSettings.colorBalanceMultipliers[0] = d->decodingSettingsBox->colorMultiplier1();
    rawDecodingSettings.colorBalanceMultipliers[1] = d->decodingSettingsBox->colorMultiplier2();
    rawDecodingSettings.colorBalanceMultipliers[2] = d->decodingSettingsBox->colorMultiplier3();
    rawDecodingSettings.colorBalanceMultipliers[3] = d->decodingSettingsBox->colorMultiplier4();

    d->thread->setRawDecodingSettings(rawDecodingSettings, d->saveSettingsBox->fileFormat());
    processOne();
}

void BatchDialog::slotUser2()
{
    d->blinkConvertTimer->stop();
    d->fileList.clear();
    d->thread->cancel();
    busy(false);

    if (d->currentConvertItem)
        d->currentConvertItem->viewItem->setPixmap(1, SmallIcon("dialog-cancel"));

    QTimer::singleShot(500, this, SLOT(slotAborted()));
}

void BatchDialog::slotAborted()
{
    d->progressBar->setValue(0);
    d->progressBar->hide();
}

void BatchDialog::addItems(const QStringList& itemList)
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

    QPixmap pix(SmallIcon("empty", K3Icon::SizeLarge, K3Icon::DisabledState));
    
    for (QStringList::const_iterator  it = itemList.begin();
         it != itemList.end(); ++it) 
    {
        QFileInfo fi(*it);
        if (fi.exists() && !d->itemDict.find(fi.fileName())) 
        {
            RawItem *item = new RawItem;
            item->directory = fi.path();
            item->src  = fi.fileName();
            item->dest = fi.baseName() + QString(".") + ext;
            new CListViewItem(d->listView, pix, item, d->listView->lastItem());
            d->itemDict.insert(item->src, item);
            urlList.append(fi.absoluteFilePath());
        }
    }

    if (!urlList.empty()) 
    {
        d->thread->identifyRawFiles(urlList);
        if (!d->thread->isRunning())
            d->thread->start();
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
    
    Q3ListViewItemIterator it( d->listView );
    while ( it.current() ) 
    {
        CListViewItem *item = (CListViewItem*) it.current();
        if (item->isEnabled())
        {
            RawItem *rawItem = item->rawItem;
            QFileInfo fi(rawItem->directory + QString("/") + rawItem->src);
            rawItem->dest = fi.baseName() + QString(".") + ext;
            item->setText(2, rawItem->dest);
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
    enableButton(User1, !busy);
    enableButton(User2, busy);
    enableButton(Close, !busy);

    d->decodingSettingsBox->setEnabled(!busy);
    d->saveSettingsBox->setEnabled(!busy);
    d->listView->setEnabled(!busy);

    busy ? d->page->setCursor(Qt::WaitCursor) : d->page->unsetCursor();
}

void BatchDialog::slotConvertBlinkTimerDone()
{
    if(d->convertBlink)
    {
        if (d->currentConvertItem)
            d->currentConvertItem->viewItem->setPixmap(1, SmallIcon("arrow-right"));
    }
    else
    {
        if (d->currentConvertItem)
            d->currentConvertItem->viewItem->setPixmap(1, SmallIcon("arrow-right-double"));
    }

    d->convertBlink = !d->convertBlink;
    d->blinkConvertTimer->start(500);
}

void BatchDialog::processing(const QString& file)
{
    QString filename     = QFileInfo(file).fileName();
    d->currentConvertItem = d->itemDict.find(filename);
    if (d->currentConvertItem) 
    {
        d->listView->setSelected(d->currentConvertItem->viewItem, true);
        d->listView->ensureItemVisible(d->currentConvertItem->viewItem);
    }

    d->convertBlink = false;
    d->blinkConvertTimer->start(500);
}

void BatchDialog::processed(const QString& file, const QString& tmpFile)
{
    d->blinkConvertTimer->stop();
    QString filename = QFileInfo(file).fileName();
    QString destFile(d->currentConvertItem->directory + QString("/") + d->currentConvertItem->dest);

    if (d->saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0) 
        {
            KIO::RenameDialog dlg(this, i18n("Save Raw Image converted from '%1' as",
                                  d->currentConvertItem->src),
                                  tmpFile, destFile,
                                  KIO::RenameDialog_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

            switch (dlg.exec())
            {
                case KIO::R_CANCEL:
                case KIO::R_SKIP:
                {
                    destFile = QString();
                    d->currentConvertItem->viewItem->setPixmap(1, SmallIcon("dialog-cancel"));
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
            d->currentConvertItem->viewItem->setPixmap(1, SmallIcon("dialog-cancel"));
        }
        else 
        {
            d->currentConvertItem->dest = QFileInfo(destFile).fileName();
            d->currentConvertItem->viewItem->setText(2, d->currentConvertItem->dest);
            d->currentConvertItem->viewItem->setPixmap(1, SmallIcon("dialog-ok"));
        }
    }

    d->progressBar->setValue(d->progressBar->value()+1);
    d->currentConvertItem = 0;
}

void BatchDialog::processingFailed(const QString& file)
{
    QString filename = QFileInfo(file).fileName();
    d->currentConvertItem->viewItem->setPixmap(1, SmallIcon("dialog-cancel"));
    d->progressBar->setValue(d->progressBar->value()+1);
    d->currentConvertItem = 0;
}

void BatchDialog::slotAction(const ActionData& ad)
{
    QString text;

    if (ad.starting)            // Something have been started...
    {
        switch (ad.action) 
        {
            case(IDENTIFY): 
                break;
            case(PROCESS):
            {
                busy(true);
                processing(ad.filePath);
                break;
            }
            default: 
            {
                kWarning(51000) << "KIPIRawConverterPlugin: Unknown action" << endl;
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
                    break;
                case(PROCESS):
                {
                    processingFailed(ad.filePath);
                    processOne();
                    break;
                }
                default: 
                {
                    kWarning(51000) << "KIPIRawConverterPlugin: Unknown action" << endl;
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
                    QFileInfo fi(ad.filePath);
                    RawItem *rawItem = d->itemDict.find(fi.fileName());
                    if (rawItem) 
                    {
                        if (!ad.image.isNull())
                        {
                            QPixmap pix = QPixmap::fromImage(ad.image.scaled(64, 64, Qt::KeepAspectRatio));
                            rawItem->viewItem->setThumbnail(pix);
                        }
                        rawItem->viewItem->setText(3, ad.message);
                        rawItem->identity = ad.message;
                    }
                    break;
                }
                case(PROCESS):
                {
                    processed(ad.filePath, ad.destPath);
                    processOne();
                    break;
                }
                default: 
                {
                    kWarning(51000) << "KIPIRawConverterPlugin: Unknown action" << endl;
                    break;
                }
            }
        }
    }
}

} // NameSpace KIPIRawConverterPlugin
