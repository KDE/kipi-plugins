/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-10-22
 * Description : Raw converter single dialog
 *
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
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
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <kdebug.h>

// KIPI include files

#include <libkipi/version.h>

// Local includes.

#include "previewwidget.h"
#include "dcrawutils.h"
#include "dcrawsettingswidget.h"
#include "savesettingswidget.h"
#include "actionthread.h"
#include "singledialog.h"
#include "singledialog.moc"

namespace KIPIRawConverterPlugin
{

SingleDialog::SingleDialog(const QString& file, QWidget *parent)
            : KDialogBase(parent, 0, false, i18n("Raw Image Converter"),
                          Help|User1|User2|User3|Close, Close, true,
                          i18n("&Preview"), i18n("Con&vert"), i18n("&Abort"))
{
    inputFile_     = file;
    inputFileName_ = QFileInfo(file).fileName();
    
    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QGridLayout *mainLayout = new QGridLayout(page, 3, 1, 0, marginHint());

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

    QGroupBox *lbox = new QGroupBox(i18n("Image Preview"), page);
    lbox->setColumnLayout(0, Qt::Vertical);
    lbox->layout()->setSpacing( spacingHint() );
    lbox->layout()->setMargin( marginHint() );
    QVBoxLayout* lboxLayout = new QVBoxLayout(lbox->layout());

    previewWidget_ = new PreviewWidget(lbox);
    lboxLayout->addWidget(previewWidget_);

    mainLayout->addMultiCellWidget(lbox, 1, 3, 0, 0);

    // ---------------------------------------------------------------

    m_decodingSettingsBox = new DcrawSettingsWidget(page);
    m_saveSettingsBox     = new SaveSettingsWidget(page);

    mainLayout->addMultiCellWidget(m_decodingSettingsBox, 1, 1, 1, 1);
    mainLayout->addMultiCellWidget(m_saveSettingsBox, 2, 2, 1, 1);

    mainLayout->setColStretch(0, 10);
    mainLayout->setRowStretch(3, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    QPushButton *helpButton = actionButton( Help );

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Raw Image Converter"),
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin for Raw image conversion\n"
                                                 "This plugin uses the Dave Coffin Raw photo "
                                                 "decoder program \"dcraw\""),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2005, Renchi Raju\n"
                                       "(c) 2006, Gilles Caulier",
                                       0,
                                       "http://extragear.kde.org/apps/kipi");

    about->addAuthor("Renchi Raju", I18N_NOOP("Original author"),
                     "renchi@pooh.tam.uiuc.edu");

    about->addAuthor("Gilles Caulier", I18N_NOOP("Maintainer"),
                     "caulier dot gilles at kdemail dot net");

    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("RAW Converter Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    helpButton->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    setButtonTip( User1, i18n("<p>Generate a Preview from current settings. "
                              "Uses a simple bilinear interpolation for "
                              "quick results."));

    setButtonTip( User2, i18n("<p>Convert the Raw Image from current settings. "
                              "This uses a high-quality adaptive algorithm."));

    setButtonTip( User3, i18n("<p>Abort the current RAW file conversion"));
    
    setButtonTip( Close, i18n("<p>Exit Raw Converter"));

    blinkPreviewTimer_ = new QTimer(this);
    blinkConvertTimer_ = new QTimer(this);
    m_thread           = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(blinkPreviewTimer_, SIGNAL(timeout()),
            this, SLOT(slotPreviewBlinkTimerDone()));
    
    connect(blinkConvertTimer_, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

    // ---------------------------------------------------------------

    busy(false);
    readSettings();
    QTimer::singleShot(0, this, SLOT( slotIdentify() ) );
}

SingleDialog::~SingleDialog()
{
    delete m_thread;
}

void SingleDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    blinkPreviewTimer_->stop();
    blinkConvertTimer_->stop();
    m_thread->cancel();
    saveSettings();
    e->accept();
}

void SingleDialog::slotClose()
{
    blinkPreviewTimer_->stop();
    blinkConvertTimer_->stop();
    m_thread->cancel();
    saveSettings();
    KDialogBase::slotClose();
}

void SingleDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("RawConverter Settings");

    m_decodingSettingsBox->setCameraWB(config.readBoolEntry("Use Camera WB", true));
    m_decodingSettingsBox->setAutoColorBalance(config.readBoolEntry("Use Auto Color Balance", true));
    m_decodingSettingsBox->setFourColor(config.readBoolEntry("Four Color RGB", false));
    m_decodingSettingsBox->setUnclipColor(config.readNumEntry("Unclip Color", 0));
    m_decodingSettingsBox->setSecondarySensor(config.readBoolEntry("Use Secondary Sensor", false));
    m_decodingSettingsBox->setNoiseReduction(config.readBoolEntry("Use Noise Resuction", false));
    m_decodingSettingsBox->setBrightness(config.readDoubleNumEntry("Brightness Multiplier", 1.0));
    m_decodingSettingsBox->setSigmaDomain(config.readDoubleNumEntry("Sigma Domain", 2.0));
    m_decodingSettingsBox->setSigmaRange(config.readDoubleNumEntry("Sigma Range", 4.0));

    m_decodingSettingsBox->setQuality(
        (RawDecodingSettings::DecodingQuality)config.readNumEntry("Decoding Quality", 
            (int)(RawDecodingSettings::BILINEAR))); 

    m_saveSettingsBox->setFileFormat(
        (RawDecodingSettings::OutputFormat)config.readNumEntry("Output Format", 
            (int)(RawDecodingSettings::PNG))); 

    m_saveSettingsBox->setConflictRule(
        (SaveSettingsWidget::ConflictRule)config.readNumEntry("Conflict",
            (int)(SaveSettingsWidget::OVERWRITE)));

    m_decodingSettingsBox->setOutputColorSpace(
        (RawDecodingSettings::OutputColorSpace)config.readNumEntry("Output Color Space", 
            (int)(RawDecodingSettings::SRGB))); 

    resize(configDialogSize(config, QString("Single Raw Converter Dialog")));
}

void SingleDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("RawConverter Settings");

    config.writeEntry("Use Camera WB", m_decodingSettingsBox->useCameraWB());
    config.writeEntry("Use Auto Color Balance", m_decodingSettingsBox->useAutoColorBalance());
    config.writeEntry("Four Color RGB", m_decodingSettingsBox->usefourColor());
    config.writeEntry("Unclip Color", m_decodingSettingsBox->unclipColor());
    config.writeEntry("Use Secondary Sensor", m_decodingSettingsBox->useSecondarySensor());
    config.writeEntry("Use Noise Resuction", m_decodingSettingsBox->useNoiseReduction());
    config.writeEntry("Brightness Multiplier", m_decodingSettingsBox->brightness());
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

void SingleDialog::slotUser1()
{
    RawDecodingSettings rawDecodingSettings;
    rawDecodingSettings.cameraColorBalance      = m_decodingSettingsBox->useCameraWB();
    rawDecodingSettings.automaticColorBalance   = m_decodingSettingsBox->useAutoColorBalance();
    rawDecodingSettings.RGBInterpolate4Colors   = m_decodingSettingsBox->usefourColor();
    rawDecodingSettings.unclipColors            = m_decodingSettingsBox->unclipColor();
    rawDecodingSettings.SuperCCDsecondarySensor = m_decodingSettingsBox->useSecondarySensor();
    rawDecodingSettings.enableNoiseReduction    = m_decodingSettingsBox->useNoiseReduction();
    rawDecodingSettings.brightness              = m_decodingSettingsBox->brightness();
    rawDecodingSettings.NRSigmaDomain           = m_decodingSettingsBox->sigmaDomain();
    rawDecodingSettings.NRSigmaRange            = m_decodingSettingsBox->sigmaRange();
    rawDecodingSettings.RAWQuality              = m_decodingSettingsBox->quality();
    rawDecodingSettings.outputColorSpace        = m_decodingSettingsBox->outputColorSpace();
    
    m_thread->setRawDecodingSettings(rawDecodingSettings);

    KURL::List oneFile;
    oneFile.append(inputFile_);
    m_thread->processHalfRawFiles(oneFile);
    if (!m_thread->running())
        m_thread->start();
}

void SingleDialog::slotUser2()
{
    RawDecodingSettings rawDecodingSettings;
    rawDecodingSettings.cameraColorBalance      = m_decodingSettingsBox->useCameraWB();
    rawDecodingSettings.automaticColorBalance   = m_decodingSettingsBox->useAutoColorBalance();
    rawDecodingSettings.RGBInterpolate4Colors   = m_decodingSettingsBox->usefourColor();
    rawDecodingSettings.unclipColors            = m_decodingSettingsBox->unclipColor();
    rawDecodingSettings.SuperCCDsecondarySensor = m_decodingSettingsBox->useSecondarySensor();
    rawDecodingSettings.enableNoiseReduction    = m_decodingSettingsBox->useNoiseReduction();
    rawDecodingSettings.brightness              = m_decodingSettingsBox->brightness();
    rawDecodingSettings.NRSigmaDomain           = m_decodingSettingsBox->sigmaDomain();
    rawDecodingSettings.NRSigmaRange            = m_decodingSettingsBox->sigmaRange();
    rawDecodingSettings.RAWQuality              = m_decodingSettingsBox->quality();
    rawDecodingSettings.outputFileFormat        = m_saveSettingsBox->fileFormat();
    rawDecodingSettings.outputColorSpace        = m_decodingSettingsBox->outputColorSpace();

    m_thread->setRawDecodingSettings(rawDecodingSettings);

    KURL::List oneFile;
    oneFile.append(inputFile_);
    m_thread->processRawFiles(oneFile);
    if (!m_thread->running())
        m_thread->start();
}

void SingleDialog::slotUser3()
{
    m_thread->cancel();
}

void SingleDialog::slotIdentify()
{
    KURL::List oneFile;
    oneFile.append(inputFile_);
    m_thread->identifyRawFiles(oneFile);
    if (!m_thread->running())
        m_thread->start();
}

void SingleDialog::busy(bool val)
{   
    enableButton (User1, !val);
    enableButton (User2, !val);
    enableButton (User3, val);
    enableButton (Close, !val);
}

void SingleDialog::identified(const QString&, const QString& identity)
{
    previewWidget_->setText(inputFileName_ + QString(" : ") + identity);
}

void SingleDialog::previewing(const QString&)
{
    previewBlink_ = false;
    previewWidget_->setCursor( KCursor::waitCursor() );
    blinkPreviewTimer_->start(200);
}

void SingleDialog::previewed(const QString&, const QString& tmpFile_)
{
    previewWidget_->unsetCursor();
    blinkPreviewTimer_->stop();
    previewWidget_->load(tmpFile_);
}

void SingleDialog::previewFailed(const QString&)
{
    previewWidget_->unsetCursor();
    previewWidget_->setText(i18n("Failed to generate preview"), Qt::red);
}

void SingleDialog::processing(const QString&)
{
    convertBlink_ = false;
    previewWidget_->setCursor( KCursor::waitCursor() );
    blinkConvertTimer_->start(200);
}

void SingleDialog::processed(const QString&, const QString& tmpFile_)
{
    previewWidget_->unsetCursor();
    blinkConvertTimer_->stop();
    previewWidget_->load(tmpFile_);
    QString filter("*.");
    QString ext;

    switch(m_saveSettingsBox->fileFormat())
    {
        case RawDecodingSettings::JPEG:
            ext = "jpg";
            break;
        case RawDecodingSettings::TIFF:
            ext = "tif";
            break;
        case RawDecodingSettings::PPM:
            ext = "ppm";
            break;
        case RawDecodingSettings::PNG:
            ext = "png";
            break;
    }

    filter += ext;
    QFileInfo fi(inputFile_);
    QString destFile = fi.dirPath(true) + QString("/") + fi.baseName() + QString(".") + ext;

    if (m_saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0) 
        {
            destFile = KFileDialog::getSaveFileName(destFile, QString(), this,
                                       i18n("Save Raw Image converted from "
                                            "'%1' as").arg(fi.fileName()));
        }
    }

    if (!destFile.isEmpty()) 
    {
        if (::rename(QFile::encodeName(tmpFile_), QFile::encodeName(destFile)) != 0)
        {
            KMessageBox::error(this, i18n("Failed to save image %1").arg( destFile ));
        }
    }
}

void SingleDialog::processingFailed(const QString&)
{
    previewWidget_->unsetCursor();
    previewWidget_->setText(i18n("Failed to convert Raw image"), Qt::red);
}

void SingleDialog::slotPreviewBlinkTimerDone()
{
    QString preview = i18n("Generating Preview...");

    if (previewBlink_)
        previewWidget_->setText(preview, Qt::green);
    else
        previewWidget_->setText(preview, Qt::darkGreen);

    previewBlink_ = !previewBlink_;
    blinkPreviewTimer_->start(200);
}

void SingleDialog::slotConvertBlinkTimerDone()
{
    QString convert = i18n("Converting Raw Image...");

    if (convertBlink_)
        previewWidget_->setText(convert, Qt::green);
    else
        previewWidget_->setText(convert, Qt::darkGreen);

    convertBlink_ = !convertBlink_;
    blinkConvertTimer_->start(200);
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
