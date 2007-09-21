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

// Local includes.

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

SingleDialog::SingleDialog(const QString& file, QWidget */*parent*/)
            : KDialog(0)
{
    setButtons(Help | Default | User1 | User2 | User3 | Close);
    setDefaultButton(KDialog::Close);
    setButtonText(User1, i18n("&Preview"));
    setButtonText(User2, i18n("Con&vert"));
    setButtonText(User3, i18n("&Abort"));
    setCaption(i18n("Raw Image Converter"));
    setModal(false);

    m_inputFile     = file;
    m_inputFileName = QFileInfo(file).fileName();
    
    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QGridLayout *mainLayout = new QGridLayout(page);

    m_previewWidget = new PreviewWidget(page);

    // ---------------------------------------------------------------

    m_decodingSettingsBox = new KDcrawIface::DcrawSettingsWidget(page, true, true, true);
    m_saveSettingsBox     = new SaveSettingsWidget(m_decodingSettingsBox);
    m_decodingSettingsBox->addTab(m_saveSettingsBox, i18n("Save settings"));

    mainLayout->addWidget(m_previewWidget, 0, 0, 2, 1);
    mainLayout->addWidget(m_decodingSettingsBox, 0, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(1, 10);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());

    // ---------------------------------------------------------------
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(ki18n("RAW Image Converter"),
                   QByteArray(),
                   KAboutData::License_GPL,
                   ki18n("A Kipi plugin to convert Raw images using KDcraw library"),
                   ki18n("(c) 2003-2005, Renchi Raju\n"
                         "(c) 2006-2007, Gilles Caulier"));

    m_about->addAuthor(ki18n("Renchi Raju"), 
                       ki18n("Author"),
                             "renchi at pooh dot tam dot uiuc dot edu");

    m_about->addAuthor(ki18n("Gilles Caulier"), 
                       ki18n("Developper and maintainer"),
                             "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KPushButton *helpButton = button( Help );
    KHelpMenu* helpMenu     = new KHelpMenu(this, m_about, false);
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

    m_blinkPreviewTimer = new QTimer(this);
    m_blinkConvertTimer = new QTimer(this);
    m_thread            = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(m_blinkPreviewTimer, SIGNAL(timeout()),
            this, SLOT(slotPreviewBlinkTimerDone()));
    
    connect(m_blinkConvertTimer, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

    connect(m_decodingSettingsBox, SIGNAL(signalSixteenBitsImageToggled(bool)),
            m_saveSettingsBox, SLOT(slotPopulateImageFormat(bool)));

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

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
    QTimer::singleShot(0, this, SLOT( slotIdentify() ) );
}

SingleDialog::~SingleDialog()
{
    delete m_about;
    delete m_thread;
}

void SingleDialog::slotHelp()
{
    KToolInvocation::invokeHelp("rawconverter", "kipi-plugins");
}

void SingleDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    m_blinkPreviewTimer->stop();
    m_blinkConvertTimer->stop();
    m_thread->cancel();
    saveSettings();
    e->accept();
}

void SingleDialog::slotClose()
{
    m_blinkPreviewTimer->stop();
    m_blinkConvertTimer->stop();
    m_thread->cancel();
    saveSettings();
    done(Close);
}

void SingleDialog::slotDefault()
{
    m_decodingSettingsBox->setDefaultSettings();
    m_saveSettingsBox->setDefaultSettings();
}

void SingleDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("RawConverter Settings"));

    m_decodingSettingsBox->setSixteenBits(group.readEntry("Sixteen Bits", false));
    m_decodingSettingsBox->setCameraWB(group.readEntry("Use Camera WB", true));
    m_decodingSettingsBox->setAutoColorBalance(group.readEntry("Use Auto Color Balance", true));
    m_decodingSettingsBox->setFourColor(group.readEntry("Four Color RGB", false));
    m_decodingSettingsBox->setUnclipColor(group.readEntry("Unclip Color", 0));
    m_decodingSettingsBox->setDontStretchPixels(group.readEntry("Dont Stretch Pixels", false));
    m_decodingSettingsBox->setNoiseReduction(group.readEntry("Use Noise Reduction", false));
    m_decodingSettingsBox->setBrightness(group.readEntry("Brightness Multiplier", 1.0));
    m_decodingSettingsBox->setUseBlackPoint(group.readEntry("Use Black Point", false));
    m_decodingSettingsBox->setBlackPoint(group.readEntry("Black Point", 0));
    m_decodingSettingsBox->setNRThreshold(group.readEntry("NR Threshold", 100));
    m_decodingSettingsBox->setUseCACorrection(group.readEntry("EnableCACorrection", false));
    m_decodingSettingsBox->setcaRedMultiplier(group.readEntry("caRedMultiplier", 1.0));
    m_decodingSettingsBox->setcaBlueMultiplier(group.readEntry("caBlueMultiplier", 1.0));
    m_decodingSettingsBox->setUseColorMultipliers(group.readEntry("Use Color Multipliers", false));
    m_decodingSettingsBox->setcolorMultiplier1(group.readEntry("Color Multiplier1", 1.0));
    m_decodingSettingsBox->setcolorMultiplier2(group.readEntry("Color Multiplier2", 1.0));
    m_decodingSettingsBox->setcolorMultiplier3(group.readEntry("Color Multiplier3", 1.0));
    m_decodingSettingsBox->setcolorMultiplier4(group.readEntry("Color Multiplier4", 1.0));

    m_decodingSettingsBox->setQuality(
        (KDcrawIface::RawDecodingSettings::DecodingQuality)group.readEntry("Decoding Quality", 
            (int)(KDcrawIface::RawDecodingSettings::BILINEAR))); 

    m_decodingSettingsBox->setOutputColorSpace(
        (KDcrawIface::RawDecodingSettings::OutputColorSpace)group.readEntry("Output Color Space", 
            (int)(KDcrawIface::RawDecodingSettings::SRGB))); 

    m_saveSettingsBox->setFileFormat(
        (SaveSettingsWidget::OutputFormat)group.readEntry("Output Format", 
            (int)(SaveSettingsWidget::OUTPUT_PNG))); 

    m_saveSettingsBox->setConflictRule(
        (SaveSettingsWidget::ConflictRule)group.readEntry("Conflict",
            (int)(SaveSettingsWidget::OVERWRITE)));

    m_saveSettingsBox->slotPopulateImageFormat(m_decodingSettingsBox->sixteenBits());

    KConfigGroup group2 = config.group(QString("Single Raw Converter Dialog"));
    restoreDialogSize(group2);
}

void SingleDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("RawConverter Settings"));

    group.writeEntry("Sixteen Bits", m_decodingSettingsBox->sixteenBits());
    group.writeEntry("Use Camera WB", m_decodingSettingsBox->useCameraWB());
    group.writeEntry("Use Auto Color Balance", m_decodingSettingsBox->useAutoColorBalance());
    group.writeEntry("Four Color RGB", m_decodingSettingsBox->useFourColor());
    group.writeEntry("Unclip Color", m_decodingSettingsBox->unclipColor());
    group.writeEntry("Dont Stretch Pixels", m_decodingSettingsBox->useDontStretchPixels());
    group.writeEntry("Use Noise Reduction", m_decodingSettingsBox->useNoiseReduction());
    group.writeEntry("Brightness Multiplier", m_decodingSettingsBox->brightness());
    group.writeEntry("Use Black Point", m_decodingSettingsBox->useBlackPoint());
    group.writeEntry("Black Point", m_decodingSettingsBox->blackPoint());
    group.writeEntry("NR Threshold", m_decodingSettingsBox->NRThreshold());
    group.writeEntry("EnableCACorrection", m_decodingSettingsBox->useCACorrection());
    group.writeEntry("caRedMultiplier", m_decodingSettingsBox->caRedMultiplier());
    group.writeEntry("caBlueMultiplier", m_decodingSettingsBox->caBlueMultiplier());
    group.writeEntry("Decoding Quality", (int)m_decodingSettingsBox->quality());
    group.writeEntry("Output Color Space", (int)m_decodingSettingsBox->outputColorSpace());
    group.writeEntry("Use Color Multipliers", m_decodingSettingsBox->useColorMultipliers());
    group.writeEntry("Color Multiplier1", m_decodingSettingsBox->colorMultiplier1());
    group.writeEntry("Color Multiplier2", m_decodingSettingsBox->colorMultiplier2());
    group.writeEntry("Color Multiplier3", m_decodingSettingsBox->colorMultiplier3());
    group.writeEntry("Color Multiplier4", m_decodingSettingsBox->colorMultiplier4());

    group.writeEntry("Output Format", (int)m_saveSettingsBox->fileFormat());
    group.writeEntry("Conflict", (int)m_saveSettingsBox->conflictRule());

    KConfigGroup group2 = config.group(QString("Single Raw Converter Dialog"));
    saveDialogSize(group2);
    config.sync();
}

// 'Preview' dialog button.
void SingleDialog::slotUser1()
{
    KDcrawIface::RawDecodingSettings rawDecodingSettings;
    rawDecodingSettings.sixteenBitsImage           = m_decodingSettingsBox->sixteenBits();
    rawDecodingSettings.cameraColorBalance         = m_decodingSettingsBox->useCameraWB();
    rawDecodingSettings.automaticColorBalance      = m_decodingSettingsBox->useAutoColorBalance();
    rawDecodingSettings.RGBInterpolate4Colors      = m_decodingSettingsBox->useFourColor();
    rawDecodingSettings.unclipColors               = m_decodingSettingsBox->unclipColor();
    rawDecodingSettings.DontStretchPixels          = m_decodingSettingsBox->useDontStretchPixels();
    rawDecodingSettings.enableNoiseReduction       = m_decodingSettingsBox->useNoiseReduction();
    rawDecodingSettings.brightness                 = m_decodingSettingsBox->brightness();
    rawDecodingSettings.enableBlackPoint           = m_decodingSettingsBox->useBlackPoint();
    rawDecodingSettings.blackPoint                 = m_decodingSettingsBox->blackPoint();
    rawDecodingSettings.NRThreshold                = m_decodingSettingsBox->NRThreshold();
    rawDecodingSettings.enableCACorrection         = m_decodingSettingsBox->useCACorrection();
    rawDecodingSettings.caMultiplier[0]            = m_decodingSettingsBox->caRedMultiplier();
    rawDecodingSettings.caMultiplier[1]            = m_decodingSettingsBox->caBlueMultiplier();
    rawDecodingSettings.RAWQuality                 = m_decodingSettingsBox->quality();
    rawDecodingSettings.outputColorSpace           = m_decodingSettingsBox->outputColorSpace();
    rawDecodingSettings.enableColorMultipliers     = m_decodingSettingsBox->useColorMultipliers();
    rawDecodingSettings.colorBalanceMultipliers[0] = m_decodingSettingsBox->colorMultiplier1();
    rawDecodingSettings.colorBalanceMultipliers[1] = m_decodingSettingsBox->colorMultiplier2();
    rawDecodingSettings.colorBalanceMultipliers[2] = m_decodingSettingsBox->colorMultiplier3();
    rawDecodingSettings.colorBalanceMultipliers[3] = m_decodingSettingsBox->colorMultiplier4();
    
    m_thread->setRawDecodingSettings(rawDecodingSettings, SaveSettingsWidget::OUTPUT_PNG);
    m_thread->processHalfRawFile(KUrl(m_inputFile));
    if (!m_thread->isRunning())
        m_thread->start();
}

// 'Convert' dialog button.
void SingleDialog::slotUser2()
{
    KDcrawIface::RawDecodingSettings rawDecodingSettings;
    rawDecodingSettings.sixteenBitsImage           = m_decodingSettingsBox->sixteenBits();
    rawDecodingSettings.cameraColorBalance         = m_decodingSettingsBox->useCameraWB();
    rawDecodingSettings.automaticColorBalance      = m_decodingSettingsBox->useAutoColorBalance();
    rawDecodingSettings.RGBInterpolate4Colors      = m_decodingSettingsBox->useFourColor();
    rawDecodingSettings.unclipColors               = m_decodingSettingsBox->unclipColor();
    rawDecodingSettings.DontStretchPixels          = m_decodingSettingsBox->useDontStretchPixels();
    rawDecodingSettings.enableNoiseReduction       = m_decodingSettingsBox->useNoiseReduction();
    rawDecodingSettings.brightness                 = m_decodingSettingsBox->brightness();
    rawDecodingSettings.enableBlackPoint           = m_decodingSettingsBox->useBlackPoint();
    rawDecodingSettings.blackPoint                 = m_decodingSettingsBox->blackPoint();
    rawDecodingSettings.NRThreshold                = m_decodingSettingsBox->NRThreshold();
    rawDecodingSettings.enableCACorrection         = m_decodingSettingsBox->useCACorrection();
    rawDecodingSettings.caMultiplier[0]            = m_decodingSettingsBox->caRedMultiplier();
    rawDecodingSettings.caMultiplier[1]            = m_decodingSettingsBox->caBlueMultiplier();
    rawDecodingSettings.RAWQuality                 = m_decodingSettingsBox->quality();
    rawDecodingSettings.outputColorSpace           = m_decodingSettingsBox->outputColorSpace();
    rawDecodingSettings.enableColorMultipliers     = m_decodingSettingsBox->useColorMultipliers();
    rawDecodingSettings.colorBalanceMultipliers[0] = m_decodingSettingsBox->colorMultiplier1();
    rawDecodingSettings.colorBalanceMultipliers[1] = m_decodingSettingsBox->colorMultiplier2();
    rawDecodingSettings.colorBalanceMultipliers[2] = m_decodingSettingsBox->colorMultiplier3();
    rawDecodingSettings.colorBalanceMultipliers[3] = m_decodingSettingsBox->colorMultiplier4();

    m_thread->setRawDecodingSettings(rawDecodingSettings, m_saveSettingsBox->fileFormat());
    m_thread->processRawFile(KUrl(m_inputFile));
    if (!m_thread->isRunning())
        m_thread->start();
}

// 'Abort' dialog button.
void SingleDialog::slotUser3()
{
    m_thread->cancel();
}

void SingleDialog::slotIdentify()
{
    m_thread->identifyRawFile(KUrl(m_inputFile), true);
    if (!m_thread->isRunning())
        m_thread->start();
}

void SingleDialog::busy(bool val)
{   
    m_decodingSettingsBox->setEnabled(!val);
    m_saveSettingsBox->setEnabled(!val);
    enableButton(User1, !val);
    enableButton(User2, !val);
    enableButton(User3, val);
    enableButton(Close, !val);
}

void SingleDialog::identified(const QString&, const QString& identity, const QPixmap& preview)
{
    m_previewWidget->setInfo(m_inputFileName + QString(" :\n") + identity, Qt::white, preview);
}

void SingleDialog::previewing(const QString&)
{
    m_previewBlink = false;
    m_previewWidget->setCursor( Qt::WaitCursor );
    m_blinkPreviewTimer->start(200);
}

void SingleDialog::previewed(const QString&, const QString& tmpFile)
{
    m_previewWidget->unsetCursor();
    m_blinkPreviewTimer->stop();
    m_previewWidget->load(tmpFile);
    ::remove(QFile::encodeName(tmpFile));
}

void SingleDialog::previewFailed(const QString&)
{
    m_previewWidget->unsetCursor();
    m_blinkPreviewTimer->stop();
    m_previewWidget->setInfo(i18n("Failed to generate preview"), Qt::red);
}

void SingleDialog::processing(const QString&)
{
    m_convertBlink = false;
    m_previewWidget->setCursor( Qt::WaitCursor );
    m_blinkConvertTimer->start(200);
}

void SingleDialog::processed(const QString&, const QString& tmpFile)
{
    m_previewWidget->unsetCursor();
    m_blinkConvertTimer->stop();
    m_previewWidget->load(tmpFile);
    QString filter("*.");
    QString ext;

    switch(m_saveSettingsBox->fileFormat())
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
    QFileInfo fi(m_inputFile);
    QString destFile = fi.absolutePath() + QString("/") + fi.baseName() + QString(".") + ext;

    if (m_saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0) 
        {
            KIO::RenameDialog dlg(this, i18n("Save Raw Image converted from '%1' as").arg(fi.fileName()),
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
            KMessageBox::error(this, i18n("Failed to save image %1").arg( destFile ));
        }
    }
}

void SingleDialog::processingFailed(const QString&)
{
    m_previewWidget->unsetCursor();
    m_blinkConvertTimer->stop();
    m_previewWidget->setInfo(i18n("Failed to convert Raw image"), Qt::red);
}

void SingleDialog::slotPreviewBlinkTimerDone()
{
    QString preview = i18n("Generating Preview...");

    if (m_previewBlink)
        m_previewWidget->setInfo(preview, Qt::green);
    else
        m_previewWidget->setInfo(preview, Qt::darkGreen);

    m_previewBlink = !m_previewBlink;
    m_blinkPreviewTimer->start(200);
}

void SingleDialog::slotConvertBlinkTimerDone()
{
    QString convert = i18n("Converting Raw Image...");

    if (m_convertBlink)
        m_previewWidget->setInfo(convert, Qt::green);
    else
        m_previewWidget->setInfo(convert, Qt::darkGreen);

    m_convertBlink = !m_convertBlink;
    m_blinkConvertTimer->start(200);
}

void SingleDialog::customEvent(QEvent *event)
{
    if (!event) return;

    EventData *d = (EventData*)event;
    if (!d) return;

    QString text;

    if (d->starting)            // Something have been started...
    {
        switch (d->action) 
        {
            case(IDENTIFY_FULL): 
                break;
            case(PREVIEW):
            {
                busy(true);
                previewing(d->filePath);
                break;
            }
            case(PROCESS):
            {
                busy(true);
                processing(d->filePath);
                break;
            }
            default: 
            {
                kWarning( 51000 ) << "KIPIRawConverterPlugin: Unknown event" << endl;
                break;
            }
        }
    }
    else                 
    {
        if (!d->success)        // Something is failed...
        {
            switch (d->action) 
            {
                case(IDENTIFY_FULL): 
                    break;
                case(PREVIEW):
                {
                    previewFailed(d->filePath);
                    busy(false);
                    break;
                }
                case(PROCESS):
                {
                    processingFailed(d->filePath);
                    busy(false);
                    break;
                }
                default: 
                {
                    kWarning( 51000 ) << "KIPIRawConverterPlugin: Unknown event" << endl;
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (d->action)
            {
                case(IDENTIFY_FULL): 
                {
                    QPixmap pix = QPixmap::fromImage(d->image.scaled(256, 256, Qt::KeepAspectRatio));
                    identified(d->filePath, d->message, pix);
                    busy(false);
                    break;
                }
                case(PREVIEW):
                {
                    previewed(d->filePath, d->destPath);
                    busy(false);
                    break;
                }
                case(PROCESS):
                {
                    processed(d->filePath, d->destPath);
                    busy(false);
                    break;
                }
                default: 
                {
                    kWarning( 51000 ) << "KIPIRawConverterPlugin: Unknown event" << endl;
                    break;
                }
            }
        }
    }

    delete d;
}

} // NameSpace KIPIRawConverterPlugin
