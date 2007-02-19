/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at gmail dot com>
 * Date   : 2003-10-22
 * Description : Raw converter single dialog
 *
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006-2007 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
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

// Qt includes.

#include <qtimer.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qfileinfo.h>
#include <qgroupbox.h>
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
    QGridLayout *mainLayout = new QGridLayout(page, 3, 1, 0, spacingHint());

    // --------------------------------------------------------------

    QFrame *headerFrame = new QFrame( page );
    headerFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QHBoxLayout* layout = new QHBoxLayout( headerFrame );
    layout->setMargin( 2 ); // to make sure the frame gets displayed
    layout->setSpacing( 0 );
    QLabel *pixmapLabelLeft = new QLabel( headerFrame, "pixmapLabelLeft" );
    pixmapLabelLeft->setScaledContents( false );
    layout->addWidget( pixmapLabelLeft );
    QLabel *labelTitle = new QLabel( i18n("Raw Image Converter"), headerFrame, "labelTitle" );
    layout->addWidget( labelTitle );
    layout->setStretchFactor( labelTitle, 1 );
    mainLayout->addMultiCellWidget(headerFrame, 0, 0, 0, 1);

    QString directory;
    KGlobal::dirs()->addResourceType("kipi_banner_left", KGlobal::dirs()->kde_default("data") + "kipi/data");
    directory = KGlobal::dirs()->findResourceDir("kipi_banner_left", "banner_left.png");

    pixmapLabelLeft->setPaletteBackgroundColor( QColor(201, 208, 255) );
    pixmapLabelLeft->setPixmap( QPixmap( directory + "banner_left.png" ) );
    labelTitle->setPaletteBackgroundColor( QColor(201, 208, 255) );

    // --------------------------------------------------------------

    m_previewWidget = new PreviewWidget(page);
    mainLayout->addMultiCellWidget(m_previewWidget, 1, 3, 0, 0);

    // ---------------------------------------------------------------

    QGroupBox *rawSettingsGroup = new QGroupBox(1, Qt::Vertical, i18n("RAW Decoding Settings"), page);
    m_decodingSettingsBox       = new KDcrawIface::DcrawSettingsWidget(rawSettingsGroup, false, true, true);
    m_saveSettingsBox           = new SaveSettingsWidget(page);

    mainLayout->addMultiCellWidget(rawSettingsGroup, 1, 1, 1, 1);
    mainLayout->addMultiCellWidget(m_saveSettingsBox, 2, 2, 1, 1);

    mainLayout->setColStretch(0, 10);
    mainLayout->setRowStretch(3, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("RAW Image Converter"),
                                           NULL,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to convert RAW images\n"
                                                     "Using KDcraw library"),
                                           "(c) 2003-2005, Renchi Raju\n"
                                           "(c) 2006-2007, Gilles Caulier");

    m_about->addAuthor("Renchi Raju", I18N_NOOP("Original author"),
                       "renchi@pooh.tam.uiuc.edu");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Maintainer"),
                       "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("RAW Converter Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    setButtonTip( User1, i18n("<p>Generate a Preview from current settings. "
                              "Uses a simple bilinear interpolation for "
                              "quick results."));

    setButtonTip( User2, i18n("<p>Convert the Raw Image from current settings. "
                              "This uses a high-quality adaptive algorithm."));

    setButtonTip( User3, i18n("<p>Abort the current RAW file conversion"));
    
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

    m_decodingSettingsBox->setCameraWB(config.readBoolEntry("Use Camera WB", true));
    m_decodingSettingsBox->setAutoColorBalance(config.readBoolEntry("Use Auto Color Balance", true));
    m_decodingSettingsBox->setFourColor(config.readBoolEntry("Four Color RGB", false));
    m_decodingSettingsBox->setUnclipColor(config.readNumEntry("Unclip Color", 0));
    m_decodingSettingsBox->setDontStretchPixels(config.readBoolEntry("Dont Stretch Pixels", false));
    m_decodingSettingsBox->setNoiseReduction(config.readBoolEntry("Use Noise Reduction", false));
    m_decodingSettingsBox->setBrightness(config.readDoubleNumEntry("Brightness Multiplier", 1.0));
    m_decodingSettingsBox->setUseBlackPoint(config.readBoolEntry("Use Black Point", false));
    m_decodingSettingsBox->setBlackPoint(config.readNumEntry("Black Point", 0));
    m_decodingSettingsBox->setSigmaDomain(config.readDoubleNumEntry("Sigma Domain", 2.0));
    m_decodingSettingsBox->setSigmaRange(config.readDoubleNumEntry("Sigma Range", 4.0));

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

    config.writeEntry("Use Camera WB", m_decodingSettingsBox->useCameraWB());
    config.writeEntry("Use Auto Color Balance", m_decodingSettingsBox->useAutoColorBalance());
    config.writeEntry("Four Color RGB", m_decodingSettingsBox->useFourColor());
    config.writeEntry("Unclip Color", m_decodingSettingsBox->unclipColor());
    config.writeEntry("Dont Stretch Pixels", m_decodingSettingsBox->useDontStretchPixels());
    config.writeEntry("Use Noise Reduction", m_decodingSettingsBox->useNoiseReduction());
    config.writeEntry("Brightness Multiplier", m_decodingSettingsBox->brightness());
    config.writeEntry("Use Black Point", m_decodingSettingsBox->useBlackPoint());
    config.writeEntry("Black Point", m_decodingSettingsBox->blackPoint());
    config.writeEntry("Sigma Domain", m_decodingSettingsBox->sigmaDomain());
    config.writeEntry("Sigma Range", m_decodingSettingsBox->sigmaRange());
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
    rawDecodingSettings.cameraColorBalance      = m_decodingSettingsBox->useCameraWB();
    rawDecodingSettings.automaticColorBalance   = m_decodingSettingsBox->useAutoColorBalance();
    rawDecodingSettings.RGBInterpolate4Colors   = m_decodingSettingsBox->useFourColor();
    rawDecodingSettings.unclipColors            = m_decodingSettingsBox->unclipColor();
    rawDecodingSettings.DontStretchPixels       = m_decodingSettingsBox->useDontStretchPixels();
    rawDecodingSettings.enableNoiseReduction    = m_decodingSettingsBox->useNoiseReduction();
    rawDecodingSettings.brightness              = m_decodingSettingsBox->brightness();
    rawDecodingSettings.enableBlackPoint        = m_decodingSettingsBox->useBlackPoint();
    rawDecodingSettings.blackPoint              = m_decodingSettingsBox->blackPoint();
    rawDecodingSettings.NRSigmaDomain           = m_decodingSettingsBox->sigmaDomain();
    rawDecodingSettings.NRSigmaRange            = m_decodingSettingsBox->sigmaRange();
    rawDecodingSettings.RAWQuality              = m_decodingSettingsBox->quality();
    rawDecodingSettings.outputColorSpace        = m_decodingSettingsBox->outputColorSpace();
    
    m_thread->setRawDecodingSettings(rawDecodingSettings, SaveSettingsWidget::OUTPUT_PNG);
    m_thread->processHalfRawFile(KURL(m_inputFile));
    if (!m_thread->running())
        m_thread->start();
}

// 'Convert' dialog button.
void SingleDialog::slotUser2()
{
    KDcrawIface::RawDecodingSettings rawDecodingSettings;
    rawDecodingSettings.cameraColorBalance      = m_decodingSettingsBox->useCameraWB();
    rawDecodingSettings.automaticColorBalance   = m_decodingSettingsBox->useAutoColorBalance();
    rawDecodingSettings.RGBInterpolate4Colors   = m_decodingSettingsBox->useFourColor();
    rawDecodingSettings.unclipColors            = m_decodingSettingsBox->unclipColor();
    rawDecodingSettings.DontStretchPixels       = m_decodingSettingsBox->useDontStretchPixels();
    rawDecodingSettings.enableNoiseReduction    = m_decodingSettingsBox->useNoiseReduction();
    rawDecodingSettings.brightness              = m_decodingSettingsBox->brightness();
    rawDecodingSettings.enableBlackPoint        = m_decodingSettingsBox->useBlackPoint();
    rawDecodingSettings.blackPoint              = m_decodingSettingsBox->blackPoint();
    rawDecodingSettings.NRSigmaDomain           = m_decodingSettingsBox->sigmaDomain();
    rawDecodingSettings.NRSigmaRange            = m_decodingSettingsBox->sigmaRange();
    rawDecodingSettings.RAWQuality              = m_decodingSettingsBox->quality();
    rawDecodingSettings.outputColorSpace        = m_decodingSettingsBox->outputColorSpace();

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
    m_thread->identifyRawFile(KURL(m_inputFile));
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

void SingleDialog::identified(const QString&, const QString& identity)
{
    m_previewWidget->setText(m_inputFileName + QString(" : ") + identity);
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
}

void SingleDialog::previewFailed(const QString&)
{
    m_previewWidget->unsetCursor();
    m_blinkPreviewTimer->stop();
    m_previewWidget->setText(i18n("Failed to generate preview"), Qt::red);
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
    m_previewWidget->setText(i18n("Failed to convert Raw image"), Qt::red);
}

void SingleDialog::slotPreviewBlinkTimerDone()
{
    QString preview = i18n("Generating Preview...");

    if (m_previewBlink)
        m_previewWidget->setText(preview, Qt::green);
    else
        m_previewWidget->setText(preview, Qt::darkGreen);

    m_previewBlink = !m_previewBlink;
    m_blinkPreviewTimer->start(200);
}

void SingleDialog::slotConvertBlinkTimerDone()
{
    QString convert = i18n("Converting Raw Image...");

    if (m_convertBlink)
        m_previewWidget->setText(convert, Qt::green);
    else
        m_previewWidget->setText(convert, Qt::darkGreen);

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
            case(IDENTIFY): 
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
                case(IDENTIFY): 
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
                case(IDENTIFY): 
                {
                    identified(d->filePath, d->message);
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
