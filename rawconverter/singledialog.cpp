/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-22
 * Description : Raw converter single dialog
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <qtimer.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qfileinfo.h>
#include <qevent.h>
#include <qpushbutton.h>
#include <qfile.h>

// KDE includes.

#include <kcursor.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kio/renamedlg.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <kdebug.h>

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
            : KDialogBase(0, 0, false, i18n("Raw Image Converter"),
                          Help|Default|User1|User2|User3|Close, Close, true,
                          i18n("&Preview"), i18n("Con&vert"), i18n("&Abort"))
{
    m_inputFile     = file;
    m_inputFileName = QFileInfo(file).fileName();

    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QGridLayout *mainLayout = new QGridLayout(page, 1, 1, 0, spacingHint());

    m_previewWidget = new PreviewWidget(page);

    // ---------------------------------------------------------------

    m_decodingSettingsBox = new KDcrawIface::DcrawSettingsWidget(page, false, true, true);
    m_saveSettingsBox     = new SaveSettingsWidget(m_decodingSettingsBox);
    m_decodingSettingsBox->insertTab(m_saveSettingsBox, i18n("Save settings"));

    mainLayout->addMultiCellWidget(m_previewWidget, 0, 1, 0, 0);
    mainLayout->addMultiCellWidget(m_decodingSettingsBox, 0, 0, 1, 1);
    mainLayout->setColStretch(0, 10);
    mainLayout->setRowStretch(1, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("RAW Image Converter"),
                                           0,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to convert a Raw image"),
                                           "(c) 2003-2005, Renchi Raju\n"
                                           "(c) 2006-2008, Gilles Caulier");

    m_about->addAuthor("Renchi Raju", I18N_NOOP("Original author"),
                       "renchi at pooh dot tam dot uiuc dot edu");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Maintainer"),
                       "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    setButtonTip( User1, i18n("<p>Generate a Preview from current settings. "
                              "Uses a simple bilinear interpolation for "
                              "quick results."));

    setButtonTip( User2, i18n("<p>Convert the Raw Image from current settings. "
                              "This uses a high-quality adaptive algorithm."));

    setButtonTip( User3, i18n("<p>Abort the current Raw file conversion"));

    setButtonTip( Close, i18n("<p>Exit Raw Converter"));

    m_blinkPreviewTimer = new QTimer(this);
    m_blinkConvertTimer = new QTimer(this);
    m_thread            = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(m_blinkPreviewTimer, SIGNAL(timeout()),
            this, SLOT(slotPreviewBlinkTimerDone()));

    connect(m_blinkConvertTimer, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

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
    KDialogBase::slotClose();
}

void SingleDialog::slotDefault()
{
    m_decodingSettingsBox->setDefaultSettings();
    m_saveSettingsBox->setDefaultSettings();
}

void SingleDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("RawConverter Settings");

    m_decodingSettingsBox->setWhiteBalance((KDcrawIface::RawDecodingSettings::WhiteBalance)
                                            config.readNumEntry("White Balance",
                                            KDcrawIface::RawDecodingSettings::CAMERA));
    m_decodingSettingsBox->setCustomWhiteBalance(config.readNumEntry("Custom White Balance", 6500));
    m_decodingSettingsBox->setCustomWhiteBalanceGreen(config.readDoubleNumEntry("Custom White Balance Green", 1.0));
    m_decodingSettingsBox->setFourColor(config.readBoolEntry("Four Color RGB", false));
    m_decodingSettingsBox->setUnclipColor(config.readNumEntry("Unclip Color", 0));
    m_decodingSettingsBox->setDontStretchPixels(config.readBoolEntry("Dont Stretch Pixels", false));
    m_decodingSettingsBox->setNoiseReduction(config.readBoolEntry("Use Noise Reduction", false));
    m_decodingSettingsBox->setBrightness(config.readDoubleNumEntry("Brightness Multiplier", 1.0));
    m_decodingSettingsBox->setUseBlackPoint(config.readBoolEntry("Use Black Point", false));
    m_decodingSettingsBox->setBlackPoint(config.readNumEntry("Black Point", 0));
#if KDCRAW_VERSION >= 0x000105
    m_decodingSettingsBox->setUseWhitePoint(config.readBoolEntry("Use White Point", false));
    m_decodingSettingsBox->setWhitePoint(config.readNumEntry("White Point", 0));
#endif
    m_decodingSettingsBox->setNRThreshold(config.readNumEntry("NR Threshold", 100));
    m_decodingSettingsBox->setUseCACorrection(config.readBoolEntry("EnableCACorrection", false));
    m_decodingSettingsBox->setcaRedMultiplier(config.readDoubleNumEntry("caRedMultiplier", 1.0));
    m_decodingSettingsBox->setcaBlueMultiplier(config.readDoubleNumEntry("caBlueMultiplier", 1.0));

    m_decodingSettingsBox->setQuality(
        (KDcrawIface::RawDecodingSettings::DecodingQuality)config.readNumEntry("Decoding Quality", 
            (int)(KDcrawIface::RawDecodingSettings::BILINEAR))); 

    m_decodingSettingsBox->setOutputColorSpace(
        (KDcrawIface::RawDecodingSettings::OutputColorSpace)config.readNumEntry("Output Color Space", 
            (int)(KDcrawIface::RawDecodingSettings::SRGB))); 

    m_saveSettingsBox->setFileFormat(
        (SaveSettingsWidget::OutputFormat)config.readNumEntry("Output Format", 
            (int)(SaveSettingsWidget::OUTPUT_PNG))); 

    m_saveSettingsBox->setConflictRule(
        (SaveSettingsWidget::ConflictRule)config.readNumEntry("Conflict",
            (int)(SaveSettingsWidget::OVERWRITE)));

    resize(configDialogSize(config, QString("Single Raw Converter Dialog")));
}

void SingleDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("RawConverter Settings");

    config.writeEntry("White Balance", m_decodingSettingsBox->whiteBalance());
    config.writeEntry("Custom White Balance", m_decodingSettingsBox->customWhiteBalance());
    config.writeEntry("Custom White Balance Green", m_decodingSettingsBox->customWhiteBalanceGreen());
    config.writeEntry("Four Color RGB", m_decodingSettingsBox->useFourColor());
    config.writeEntry("Unclip Color", m_decodingSettingsBox->unclipColor());
    config.writeEntry("Dont Stretch Pixels", m_decodingSettingsBox->useDontStretchPixels());
    config.writeEntry("Use Noise Reduction", m_decodingSettingsBox->useNoiseReduction());
    config.writeEntry("Brightness Multiplier", m_decodingSettingsBox->brightness());
    config.writeEntry("Use Black Point", m_decodingSettingsBox->useBlackPoint());
    config.writeEntry("Black Point", m_decodingSettingsBox->blackPoint());
#if KDCRAW_VERSION >= 0x000105
    config.writeEntry("Use White Point", m_decodingSettingsBox->useWhitePoint());
    config.writeEntry("White Point", m_decodingSettingsBox->whitePoint());
#endif
    config.writeEntry("NR Threshold", m_decodingSettingsBox->NRThreshold());
    config.writeEntry("EnableCACorrection", m_decodingSettingsBox->useCACorrection());
    config.writeEntry("caRedMultiplier", m_decodingSettingsBox->caRedMultiplier());
    config.writeEntry("caBlueMultiplier", m_decodingSettingsBox->caBlueMultiplier());
    config.writeEntry("Decoding Quality", (int)m_decodingSettingsBox->quality());
    config.writeEntry("Output Color Space", (int)m_decodingSettingsBox->outputColorSpace());

    config.writeEntry("Output Format", (int)m_saveSettingsBox->fileFormat());
    config.writeEntry("Conflict", (int)m_saveSettingsBox->conflictRule());

    saveDialogSize(config, QString("Single Raw Converter Dialog"));
    config.sync();
}

void SingleDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("rawconverter", "kipi-plugins");
}

// 'Preview' dialog button.
void SingleDialog::slotUser1()
{
    KDcrawIface::RawDecodingSettings rawDecodingSettings;
    rawDecodingSettings.whiteBalance               = m_decodingSettingsBox->whiteBalance();
    rawDecodingSettings.customWhiteBalance         = m_decodingSettingsBox->customWhiteBalance();
    rawDecodingSettings.customWhiteBalanceGreen    = m_decodingSettingsBox->customWhiteBalanceGreen();
    rawDecodingSettings.RGBInterpolate4Colors      = m_decodingSettingsBox->useFourColor();
    rawDecodingSettings.unclipColors               = m_decodingSettingsBox->unclipColor();
    rawDecodingSettings.DontStretchPixels          = m_decodingSettingsBox->useDontStretchPixels();
    rawDecodingSettings.enableNoiseReduction       = m_decodingSettingsBox->useNoiseReduction();
    rawDecodingSettings.brightness                 = m_decodingSettingsBox->brightness();
    rawDecodingSettings.enableBlackPoint           = m_decodingSettingsBox->useBlackPoint();
    rawDecodingSettings.blackPoint                 = m_decodingSettingsBox->blackPoint();
#if KDCRAW_VERSION >= 0x000105
    rawDecodingSettings.enableWhitePoint           = m_decodingSettingsBox->useWhitePoint();
    rawDecodingSettings.whitePoint                 = m_decodingSettingsBox->whitePoint();
#endif
    rawDecodingSettings.NRThreshold                = m_decodingSettingsBox->NRThreshold();
    rawDecodingSettings.enableCACorrection         = m_decodingSettingsBox->useCACorrection();
    rawDecodingSettings.caMultiplier[0]            = m_decodingSettingsBox->caRedMultiplier();
    rawDecodingSettings.caMultiplier[1]            = m_decodingSettingsBox->caBlueMultiplier();
    rawDecodingSettings.RAWQuality                 = m_decodingSettingsBox->quality();
    rawDecodingSettings.outputColorSpace           = m_decodingSettingsBox->outputColorSpace();

    m_thread->setRawDecodingSettings(rawDecodingSettings, SaveSettingsWidget::OUTPUT_PPM);
    m_thread->processHalfRawFile(KURL(m_inputFile));
    if (!m_thread->running())
        m_thread->start();
}

// 'Convert' dialog button.
void SingleDialog::slotUser2()
{
    KDcrawIface::RawDecodingSettings rawDecodingSettings;
    rawDecodingSettings.whiteBalance               = m_decodingSettingsBox->whiteBalance();
    rawDecodingSettings.customWhiteBalance         = m_decodingSettingsBox->customWhiteBalance();
    rawDecodingSettings.customWhiteBalanceGreen    = m_decodingSettingsBox->customWhiteBalanceGreen();
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

    m_thread->setRawDecodingSettings(rawDecodingSettings, m_saveSettingsBox->fileFormat());
    m_thread->processRawFile(KURL(m_inputFile));
    if (!m_thread->running())
        m_thread->start();
}

// 'Abort' dialog button.
void SingleDialog::slotUser3()
{
    m_thread->cancel();
}

void SingleDialog::slotIdentify()
{
    m_thread->identifyRawFile(KURL(m_inputFile), true);
    if (!m_thread->running())
        m_thread->start();
}

void SingleDialog::busy(bool val)
{
    m_decodingSettingsBox->setEnabled(!val);
    m_saveSettingsBox->setEnabled(!val);
    enableButton (User1, !val);
    enableButton (User2, !val);
    enableButton (User3, val);
    enableButton (Close, !val);
}

void SingleDialog::identified(const QString&, const QString& identity, const QPixmap& preview)
{
    m_previewWidget->setInfo(m_inputFileName + QString(" :\n") + identity, Qt::white, preview);
}

void SingleDialog::previewing(const QString&)
{
    m_previewBlink = false;
    m_previewWidget->setCursor( KCursor::waitCursor() );
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
    m_previewWidget->setCursor( KCursor::waitCursor() );
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
    QString destFile = fi.dirPath(true) + QString("/") + fi.baseName() + QString(".") + ext;

    if (m_saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0) 
        {
            KIO::RenameDlg dlg(this, i18n("Save Raw Image converted from '%1' as").arg(fi.fileName()),
                               tmpFile, destFile,
                               KIO::RenameDlg_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

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
                    destFile = dlg.newDestURL().path();
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

void SingleDialog::customEvent(QCustomEvent *event)
{
    if (!event) return;

    EventData *d = (EventData*) event->data();
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
                kdWarning( 51000 ) << "KIPIRawConverterPlugin: Unknown event" << endl;
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
                    kdWarning( 51000 ) << "KIPIRawConverterPlugin: Unknown event" << endl;
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
                    QPixmap pix = QPixmap(d->image.scale(256, 256, QImage::ScaleMin));
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
                    kdWarning( 51000 ) << "KIPIRawConverterPlugin: Unknown event" << endl;
                    break;
                }
            }
        }
    }

    delete d;
}

} // NameSpace KIPIRawConverterPlugin
