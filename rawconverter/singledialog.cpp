/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-22
 * Description : Raw converter single dialog
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

// LibKipi includes.

#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>

// Local includes.

#include "actions.h"
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "previewwidget.h"
#include "rawdecodingiface.h"
#include "savesettingswidget.h"
#include "actionthread.h"
#include "singledialog.h"
#include "singledialog.moc"

namespace KIPIRawConverterPlugin
{

class SingleDialogPriv
{
public:

    SingleDialogPriv()
    {
        previewBlink        = false;
        convertBlink        = false;
        blinkPreviewTimer   = 0;
        blinkConvertTimer   = 0;
        previewWidget       = 0;
        thread              = 0;
        saveSettingsBox     = 0;
        decodingSettingsBox = 0;
        about               = 0;
        iface               = 0;
    }

    bool                              previewBlink;
    bool                              convertBlink;

    QString                           inputFile;
    QString                           inputFileName;
    
    QTimer                           *blinkPreviewTimer;
    QTimer                           *blinkConvertTimer;

    PreviewWidget                    *previewWidget;

    ActionThread                     *thread;

    SaveSettingsWidget               *saveSettingsBox;

    KDcrawIface::DcrawSettingsWidget *decodingSettingsBox;

    KIPIPlugins::KPAboutData         *about; 

    KIPI::Interface                  *iface;
};

SingleDialog::SingleDialog(const QString& file, KIPI::Interface* iface)
            : KDialog(0)
{
    d = new SingleDialogPriv;
    d->iface         = iface;
    d->inputFile     = file;
    d->inputFileName = QFileInfo(file).fileName();

    setButtons(Help | Default | User1 | User2 | User3 | Close);
    setDefaultButton(KDialog::Close);
    setButtonText(User1, i18n("&Preview"));
    setButtonText(User2, i18n("Con&vert"));
    setButtonText(User3, i18n("&Abort"));
    setCaption(i18n("Raw Image Converter"));
    setModal(false);

    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QGridLayout *mainLayout = new QGridLayout(page);

    d->previewWidget = new PreviewWidget(page);

    // ---------------------------------------------------------------

    d->decodingSettingsBox = new KDcrawIface::DcrawSettingsWidget(page, true, true, true);
    d->saveSettingsBox     = new SaveSettingsWidget(d->decodingSettingsBox);
    d->decodingSettingsBox->addTab(d->saveSettingsBox, i18n("Save settings"));

    mainLayout->addWidget(d->previewWidget, 0, 0, 2, 1);
    mainLayout->addWidget(d->decodingSettingsBox, 0, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(1, 10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("RAW Image Converter"),
                   QByteArray(),
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to convert a Raw image"),
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

    setButtonToolTip(User1, i18n("<p>Generate a Preview from current settings. "
                                 "Uses a simple bilinear interpolation for "
                                 "quick results."));

    setButtonToolTip(User2, i18n("<p>Convert the Raw Image from current settings. "
                                 "This uses a high-quality adaptive algorithm."));

    setButtonToolTip(User3, i18n("<p>Abort the current Raw file conversion"));
    
    setButtonToolTip(Close, i18n("<p>Exit Raw Converter"));

    d->blinkPreviewTimer = new QTimer(this);
    d->blinkConvertTimer = new QTimer(this);
    d->thread            = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(d->blinkPreviewTimer, SIGNAL(timeout()),
            this, SLOT(slotPreviewBlinkTimerDone()));
    
    connect(d->blinkConvertTimer, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

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

    connect(this, SIGNAL(user3Clicked()),
            this, SLOT(slotUser3()));

    connect(d->thread, SIGNAL(starting(const ActionData&)),
            this, SLOT(slotAction(const ActionData&)));

    connect(d->thread, SIGNAL(finished(const ActionData&)),
            this, SLOT(slotAction(const ActionData&)));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
    QTimer::singleShot(0, this, SLOT( slotIdentify() ) );
}

SingleDialog::~SingleDialog()
{
    delete d->about;
    delete d->thread;
    delete d;
}

void SingleDialog::slotHelp()
{
    KToolInvocation::invokeHelp("rawconverter", "kipi-plugins");
}

void SingleDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    d->blinkPreviewTimer->stop();
    d->blinkConvertTimer->stop();
    d->thread->cancel();
    saveSettings();
    e->accept();
}

void SingleDialog::slotClose()
{
    d->blinkPreviewTimer->stop();
    d->blinkConvertTimer->stop();
    d->thread->cancel();
    saveSettings();
    done(Close);
}

void SingleDialog::slotDefault()
{
    d->decodingSettingsBox->setDefaultSettings();
    d->saveSettingsBox->setDefaultSettings();
}

void SingleDialog::readSettings()
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

    KConfigGroup group2 = config.group(QString("Single Raw Converter Dialog"));
    restoreDialogSize(group2);
}

void SingleDialog::saveSettings()
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

    KConfigGroup group2 = config.group(QString("Single Raw Converter Dialog"));
    saveDialogSize(group2);
    config.sync();
}

// 'Preview' dialog button.
void SingleDialog::slotUser1()
{
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
    
    d->thread->setRawDecodingSettings(rawDecodingSettings, SaveSettingsWidget::OUTPUT_PNG);
    d->thread->processHalfRawFile(KUrl(d->inputFile));
    if (!d->thread->isRunning())
        d->thread->start();
}

// 'Convert' dialog button.
void SingleDialog::slotUser2()
{
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
    d->thread->processRawFile(KUrl(d->inputFile));
    if (!d->thread->isRunning())
        d->thread->start();
}

// 'Abort' dialog button.
void SingleDialog::slotUser3()
{
    d->thread->cancel();
}

void SingleDialog::slotIdentify()
{
    d->thread->identifyRawFile(KUrl(d->inputFile), true);
    d->thread->thumbRawFile(KUrl(d->inputFile));
    if (!d->thread->isRunning())
        d->thread->start();
}

void SingleDialog::busy(bool val)
{   
    d->decodingSettingsBox->setEnabled(!val);
    d->saveSettingsBox->setEnabled(!val);
    enableButton(User1, !val);
    enableButton(User2, !val);
    enableButton(User3, val);
    enableButton(Close, !val);
}

void SingleDialog::setIdentity(const QString&, const QString& identity)
{
    d->previewWidget->setIdentity(d->inputFileName + QString(" :\n") + identity, Qt::white);
}

void SingleDialog::setThumbnail(const QString&, const QPixmap& thumbnail)
{
    d->previewWidget->setThumbnail(thumbnail);
}

void SingleDialog::previewing(const QString&)
{
    d->previewBlink = false;
    d->previewWidget->setCursor( Qt::WaitCursor );
    d->blinkPreviewTimer->start(200);
}

void SingleDialog::previewed(const QString&, const QString& tmpFile)
{
    d->previewWidget->unsetCursor();
    d->blinkPreviewTimer->stop();
    d->previewWidget->load(tmpFile);
    ::remove(QFile::encodeName(tmpFile));
}

void SingleDialog::previewFailed(const QString&)
{
    d->previewWidget->unsetCursor();
    d->blinkPreviewTimer->stop();
    d->previewWidget->setIdentity(i18n("Failed to generate preview"), Qt::red);
}

void SingleDialog::processing(const QString&)
{
    d->convertBlink = false;
    d->previewWidget->setCursor( Qt::WaitCursor );
    d->blinkConvertTimer->start(200);
}

void SingleDialog::processed(const QString& file, const QString& tmpFile)
{
    d->previewWidget->unsetCursor();
    d->blinkConvertTimer->stop();
    d->previewWidget->load(tmpFile);
    QString filter("*.");
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

    filter += ext;
    QFileInfo fi(d->inputFile);
    QString destFile = fi.absolutePath() + QString("/") + fi.baseName() + QString(".") + ext;

    if (d->saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0) 
        {
            KIO::RenameDialog dlg(this, i18n("Save Raw Image converted from '%1' as", fi.fileName()),
                                  tmpFile, destFile,
                                  KIO::RenameDialog_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

            switch (dlg.exec())
            {
                case KIO::R_CANCEL:
                case KIO::R_SKIP:
                {
                    destFile = QString();
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
        }
        else
        {
            // Assign Kipi host attributes from original RAW image.

            KIPI::ImageInfo orgInfo = d->iface->info(KUrl(file));
            KIPI::ImageInfo newInfo = d->iface->info(KUrl(destFile));
            newInfo.cloneData(orgInfo);
        }
    }
}

void SingleDialog::processingFailed(const QString&)
{
    d->previewWidget->unsetCursor();
    d->blinkConvertTimer->stop();
    d->previewWidget->setIdentity(i18n("Failed to convert Raw image"), Qt::red);
}

void SingleDialog::slotPreviewBlinkTimerDone()
{
    QString preview = i18n("Generating Preview...");

    if (d->previewBlink)
        d->previewWidget->setIdentity(preview, Qt::green);
    else
        d->previewWidget->setIdentity(preview, Qt::darkGreen);

    d->previewBlink = !d->previewBlink;
    d->blinkPreviewTimer->start(200);
}

void SingleDialog::slotConvertBlinkTimerDone()
{
    QString convert = i18n("Converting Raw Image...");

    if (d->convertBlink)
        d->previewWidget->setIdentity(convert, Qt::green);
    else
        d->previewWidget->setIdentity(convert, Qt::darkGreen);

    d->convertBlink = !d->convertBlink;
    d->blinkConvertTimer->start(200);
}

void SingleDialog::slotAction(const ActionData& ad)
{
    QString text;

    if (ad.starting)            // Something have been started...
    {
        switch (ad.action) 
        {
            case(IDENTIFY_FULL): 
            case(THUMBNAIL): 
                break;
            case(PREVIEW):
            {
                busy(true);
                previewing(ad.filePath);
                break;
            }
            case(PROCESS):
            {
                busy(true);
                processing(ad.filePath);
                break;
            }
            default: 
            {
                kWarning(51000) << "KIPIRawConverterPlugin: Unknown action";
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
                case(IDENTIFY_FULL): 
                case(THUMBNAIL): 
                    break;
                case(PREVIEW):
                {
                    previewFailed(ad.filePath);
                    busy(false);
                    break;
                }
                case(PROCESS):
                {
                    processingFailed(ad.filePath);
                    busy(false);
                    break;
                }
                default: 
                {
                    kWarning(51000) << "KIPIRawConverterPlugin: Unknown action";
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case(IDENTIFY_FULL): 
                {
                    setIdentity(ad.filePath, ad.message);
                    busy(false);
                    break;
                }
                case(THUMBNAIL): 
                {
                    QPixmap pix = QPixmap::fromImage(ad.image.scaled(256, 256, Qt::KeepAspectRatio));
                    setThumbnail(ad.filePath, pix);
                    busy(false);
                    break;
                }
                case(PREVIEW):
                {
                    previewed(ad.filePath, ad.destPath);
                    busy(false);
                    break;
                }
                case(PROCESS):
                {
                    processed(ad.filePath, ad.destPath);
                    busy(false);
                    break;
                }
                default: 
                {
                    kWarning(51000) << "KIPIRawConverterPlugin: Unknown action";
                    break;
                }
            }
        }
    }
}

} // NameSpace KIPIRawConverterPlugin
