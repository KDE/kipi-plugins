/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-22
 * Description : Raw converter single dialog
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

#include "singledialog.h"
#include "singledialog.moc"

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
#include <QPushButton>
#include <QTimer>

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
#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "previewwidget.h"
#include "rawdecodingiface.h"
#include "savesettingswidget.h"

using namespace KDcrawIface;

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

    bool                      previewBlink;
    bool                      convertBlink;

    QString                   inputFileName;

    QTimer                   *blinkPreviewTimer;
    QTimer                   *blinkConvertTimer;

    KUrl                      inputFile;

    PreviewWidget            *previewWidget;

    ActionThread             *thread;

    SaveSettingsWidget       *saveSettingsBox;

    DcrawSettingsWidget      *decodingSettingsBox;

    KIPIPlugins::KPAboutData *about;

    KIPI::Interface          *iface;
};

SingleDialog::SingleDialog(const QString& file, KIPI::Interface* iface)
            : KDialog(0), d(new SingleDialogPriv)
{
    d->iface = iface;
    setButtons(Help | Default | User1 | User2 | User3 | Close);
    setDefaultButton(KDialog::Close);
    setButtonText(User1, i18n("&Preview"));
    setButtonText(User2, i18n("Con&vert"));
    setButtonText(User3, i18n("&Abort"));
    setCaption(i18n("RAW Image Converter"));
    setModal(false);

    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QGridLayout *mainLayout = new QGridLayout(page);

    d->previewWidget = new PreviewWidget(page);

    // ---------------------------------------------------------------

    d->decodingSettingsBox = new DcrawSettingsWidget(page, DcrawSettingsWidget::SIXTEENBITS |
                                                           DcrawSettingsWidget::COLORSPACE |
                                                           DcrawSettingsWidget::POSTPROCESSING |
                                                           DcrawSettingsWidget::BLACKWHITEPOINTS);
    d->saveSettingsBox     = new SaveSettingsWidget(d->decodingSettingsBox);

#if KDCRAW_VERSION <= 0x000500
    d->decodingSettingsBox->addItem(d->saveSettingsBox, i18n("Save settings"));
    d->decodingSettingsBox->updateMinimumWidth();
#else
    d->decodingSettingsBox->addItem(d->saveSettingsBox, i18n("Save settings"), QString("savesettings"), false);
#endif

    mainLayout->addWidget(d->previewWidget,       0, 0, 2, 1);
    mainLayout->addWidget(d->decodingSettingsBox, 0, 1, 1, 1);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());
    mainLayout->setColumnStretch(0, 10);
#if KDCRAW_VERSION <= 0x000500
    mainLayout->setRowStretch(1, 10);
#endif

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("RAW Image Converter"),
                   0,
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to convert RAW images"),
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

    setButtonToolTip(User1, i18n("Generate a preview using current settings. "
                                 "Uses a simple bilinear interpolation for "
                                 "quick results."));

    setButtonToolTip(User2, i18n("Convert the RAW Image using current settings. "
                                 "This uses a high-quality adaptive algorithm."));

    setButtonToolTip(User3, i18n("Abort the current RAW file conversion"));

    setButtonToolTip(Close, i18n("Exit RAW Converter"));

    d->blinkPreviewTimer = new QTimer(this);
    d->blinkConvertTimer = new QTimer(this);
    d->thread            = new ActionThread(this, d->iface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());

    // ---------------------------------------------------------------

    connect(d->blinkPreviewTimer, SIGNAL(timeout()),
            this, SLOT(slotPreviewBlinkTimerDone()));

    connect(d->blinkConvertTimer, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

    connect(d->decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            d->saveSettingsBox, SLOT(slotPopulateImageFormat(bool)));

    connect(d->decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            this, SLOT(slotSixteenBitsImageToggled(bool)));

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

    connect(d->thread, SIGNAL(starting(const KIPIRawConverterPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIRawConverterPlugin::ActionData&)));

    connect(d->thread, SIGNAL(finished(const KIPIRawConverterPlugin::ActionData&)),
            this, SLOT(slotAction(const KIPIRawConverterPlugin::ActionData&)));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
    setFile(file);
}

SingleDialog::~SingleDialog()
{
    delete d->about;
    delete d->thread;
    delete d;
}

void SingleDialog::setFile(const QString& file)
{
    d->inputFile     = file;
    d->inputFileName = QFileInfo(file).fileName();
    QTimer::singleShot(0, this, SLOT( slotIdentify() ));
}

void SingleDialog::slotHelp()
{
    KToolInvocation::invokeHelp("rawconverter", "kipi-plugins");
}

void SingleDialog::slotSixteenBitsImageToggled(bool)
{
    // Dcraw do not provide a way to set brigness of image in 16 bits color depth.
    // We always set on this option. We drive brightness adjustment as post processing.
    d->decodingSettingsBox->setEnabledBrightnessSettings(true);
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

    KConfigGroup group2 = config.group(QString("Single Raw Converter Dialog"));
    restoreDialogSize(group2);
}

void SingleDialog::saveSettings()
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

    KConfigGroup group2 = config.group(QString("Single Raw Converter Dialog"));
    saveDialogSize(group2);
    config.sync();
}

// 'Preview' dialog button.
void SingleDialog::slotUser1()
{
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
        rawDecodingSettings.autoBrightness         = d->decodingSettingsBox->useAutoBrightness();
#endif

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
        rawDecodingSettings.autoBrightness         = d->decodingSettingsBox->useAutoBrightness();
#endif

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
    if (!d->iface->hasFeature(KIPI::HostSupportsThumbnails))
    {
        d->thread->thumbRawFile(KUrl(d->inputFile));
    }
    else
    {
        connect(d->iface, SIGNAL(gotThumbnail( const KUrl&, const QPixmap& )),
                this, SLOT(slotThumbnail(const KUrl&, const QPixmap&)));

        d->iface->thumbnail(KUrl(d->inputFile), 256);
    }

    d->thread->identifyRawFile(KUrl(d->inputFile), true);
    if (!d->thread->isRunning())
        d->thread->start();
}

void SingleDialog::slotThumbnail(const KUrl& url, const QPixmap& pix)
{
    setThumbnail(url.path(), pix);
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

void SingleDialog::setIdentity(const KUrl& /*url*/, const QString& identity)
{
    d->previewWidget->setIdentity(d->inputFileName + QString(" :\n") + identity, Qt::white);
}

void SingleDialog::setThumbnail(const KUrl& url, const QPixmap& thumbnail)
{
    if (url == d->inputFile)
        d->previewWidget->setThumbnail(thumbnail);
}

void SingleDialog::previewing(const KUrl& /*url*/)
{
    d->previewBlink = false;
    d->previewWidget->setCursor( Qt::WaitCursor );
    d->blinkPreviewTimer->start(200);
}

void SingleDialog::previewed(const KUrl& /*url*/, const QString& tmpFile)
{
    d->previewWidget->unsetCursor();
    d->blinkPreviewTimer->stop();
    d->previewWidget->load(tmpFile);
    ::remove(QFile::encodeName(tmpFile));
}

void SingleDialog::previewFailed(const KUrl& /*url*/)
{
    d->previewWidget->unsetCursor();
    d->blinkPreviewTimer->stop();
    d->previewWidget->setIdentity(i18n("Failed to generate preview"), Qt::red);
}

void SingleDialog::processing(const KUrl& /*url*/)
{
    d->convertBlink = false;
    d->previewWidget->setCursor( Qt::WaitCursor );
    d->blinkConvertTimer->start(200);
}

void SingleDialog::processed(const KUrl& url, const QString& tmpFile)
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
    QFileInfo fi(d->inputFile.path());
    QString destFile = fi.absolutePath() + QString("/") + fi.completeBaseName() + QString(".") + ext;

    if (d->saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0)
        {
            KIO::RenameDialog dlg(this, i18n("Save RAW image converted from '%1' as", fi.fileName()),
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

            KIPI::ImageInfo orgInfo = d->iface->info(url);
            KIPI::ImageInfo newInfo = d->iface->info(KUrl(destFile));
            newInfo.cloneData(orgInfo);
        }
    }
}

void SingleDialog::processingFailed(const KUrl& /*url*/)
{
    d->previewWidget->unsetCursor();
    d->blinkConvertTimer->stop();
    d->previewWidget->setIdentity(i18n("Failed to convert RAW image"), Qt::red);
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
    QString convert = i18n("Converting RAW Image...");

    if (d->convertBlink)
        d->previewWidget->setIdentity(convert, Qt::green);
    else
        d->previewWidget->setIdentity(convert, Qt::darkGreen);

    d->convertBlink = !d->convertBlink;
    d->blinkConvertTimer->start(200);
}

void SingleDialog::slotAction(const KIPIRawConverterPlugin::ActionData& ad)
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
                previewing(ad.fileUrl);
                break;
            }
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
                case(IDENTIFY_FULL):
                case(THUMBNAIL):
                    break;
                case(PREVIEW):
                {
                    previewFailed(ad.fileUrl);
                    busy(false);
                    break;
                }
                case(PROCESS):
                {
                    processingFailed(ad.fileUrl);
                    busy(false);
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
                case(IDENTIFY_FULL):
                {
                    setIdentity(ad.fileUrl, ad.message);
                    busy(false);
                    break;
                }
                case(THUMBNAIL):
                {
                    QPixmap pix = QPixmap::fromImage(ad.image.scaled(256, 256, Qt::KeepAspectRatio));
                    setThumbnail(ad.fileUrl, pix);
                    busy(false);
                    break;
                }
                case(PREVIEW):
                {
                    previewed(ad.fileUrl, ad.destPath);
                    busy(false);
                    break;
                }
                case(PROCESS):
                {
                    processed(ad.fileUrl, ad.destPath);
                    busy(false);
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

} // namespace KIPIRawConverterPlugin
