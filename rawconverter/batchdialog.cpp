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

BatchDialog::BatchDialog(QWidget* /*parent*/)
           : KDialog(0)
{
    setButtons(Help | Default | User1 | User2 | Close);
    setDefaultButton(KDialog::Close);
    setButtonText(User1, i18n("Con&vert"));
    setButtonText(User2, i18n("&Abort"));
    setCaption(i18n("Raw Images Batch Converter"));
    setModal(false);

    m_currentConvertItem = 0;
    m_thread             = 0;

    m_page = new QWidget( this );
    setMainWidget( m_page );
    QGridLayout *mainLayout = new QGridLayout(m_page);

    //---------------------------------------------

    m_listView = new K3ListView(m_page);
    m_listView->addColumn( i18n("Thumbnail") );
    m_listView->addColumn( i18n("Raw File") );
    m_listView->addColumn( i18n("Target File") );
    m_listView->addColumn( i18n("Camera") );
    m_listView->setResizeMode(Q3ListView::AllColumns);
    m_listView->setAllColumnsShowFocus(true);
    m_listView->setSorting(-1);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setSelectionMode(Q3ListView::Single);
    m_listView->setMinimumWidth(450);

    // ---------------------------------------------------------------

    m_decodingSettingsBox = new KDcrawIface::DcrawSettingsWidget(m_page, true, true, true);
    m_saveSettingsBox     = new SaveSettingsWidget(m_page);
    m_decodingSettingsBox->addTab(m_saveSettingsBox, i18n("Save settings"));

    m_progressBar = new QProgressBar(m_page);
    m_progressBar->setMaximumHeight( fontMetrics().height()+2 );
    m_progressBar->setEnabled(false);

    mainLayout->addWidget(m_listView, 0, 0, 3, 1);
    mainLayout->addWidget(m_decodingSettingsBox, 0, 1, 1, 1);
    mainLayout->addWidget(m_progressBar, 1, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(2, 10);
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

    setButtonToolTip( User1, i18n("<p>Start converting the Raw images from current settings"));
    setButtonToolTip( User2, i18n("<p>Abort the current Raw files conversion"));
    setButtonToolTip( Close, i18n("<p>Exit Raw Converter"));

    m_blinkConvertTimer = new QTimer(this);
    m_thread            = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(m_blinkConvertTimer, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

    connect(m_saveSettingsBox, SIGNAL(signalSaveFormatChanged()),
            this, SLOT(slotSaveFormatChanged()));

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

    // ---------------------------------------------------------------

    m_itemDict.setAutoDelete(true);
    busy(false);
    readSettings();
}

BatchDialog::~BatchDialog()
{
    delete m_about;
    delete m_thread;
}

void BatchDialog::slotHelp()
{
    KToolInvocation::invokeHelp("rawconverter", "kipi-plugins");
}

void BatchDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    m_blinkConvertTimer->stop();
    m_thread->cancel();
    saveSettings();
    e->accept();
}

void BatchDialog::slotClose()
{
    m_blinkConvertTimer->stop();
    m_thread->cancel();
    saveSettings();
    done(Close);
}

void BatchDialog::slotDefault()
{
    m_decodingSettingsBox->setDefaultSettings();
    m_saveSettingsBox->setDefaultSettings();
}

void BatchDialog::readSettings()
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

    KConfigGroup group2 = config.group(QString("Batch Raw Converter Dialog"));
    restoreDialogSize(group2);
}

void BatchDialog::saveSettings()
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

    KConfigGroup group2 = config.group(QString("Batch Raw Converter Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void BatchDialog::slotUser1()
{
    m_fileList.clear();

    Q3ListViewItemIterator it( m_listView );
    while ( it.current() ) 
    {
        CListViewItem *item = (CListViewItem*) it.current();
        if (item->isEnabled())
        {
            item->setPixmap(1, 0);
            m_fileList.append(item->rawItem->directory + QString("/") + item->rawItem->src);
        }
        ++it;
    }

    if (m_fileList.empty()) 
    {
        KMessageBox::error(this, i18n("There is no Raw file to process in the list!"));
        busy(false);
        slotAborted();
        return;
    }

    m_progressBar->setMaximum(m_fileList.count());
    m_progressBar->setValue(0);
    m_progressBar->setEnabled(true);

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
    processOne();
}

void BatchDialog::slotUser2()
{
    m_blinkConvertTimer->stop();
    m_fileList.clear();
    m_thread->cancel();
    busy(false);

    if (m_currentConvertItem)
        m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("cancel"));

    QTimer::singleShot(500, this, SLOT(slotAborted()));
}

void BatchDialog::slotAborted()
{
    m_progressBar->setValue(0);
    m_progressBar->setEnabled(false);
}

void BatchDialog::addItems(const QStringList& itemList)
{
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

    KUrl::List urlList;

    QPixmap pix(SmallIcon("file_broken", K3Icon::SizeLarge, K3Icon::DisabledState ));
    
    for (QStringList::const_iterator  it = itemList.begin();
         it != itemList.end(); ++it) 
    {
        QFileInfo fi(*it);
        if (fi.exists() && !m_itemDict.find(fi.fileName())) 
        {
            RawItem *item = new RawItem;
            item->directory = fi.path();
            item->src  = fi.fileName();
            item->dest = fi.baseName() + QString(".") + ext;
            new CListViewItem(m_listView, pix, item, m_listView->lastItem());
            m_itemDict.insert(item->src, item);
            urlList.append(fi.absoluteFilePath());
        }
    }

    if (!urlList.empty()) 
    {
        m_thread->identifyRawFiles(urlList);
        if (!m_thread->isRunning())
            m_thread->start();
    }
}

void BatchDialog::slotSaveFormatChanged()
{
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
    
    Q3ListViewItemIterator it( m_listView );
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
    if (m_fileList.empty()) 
    {
        busy(false);
        slotAborted();
        return;
    }
    
    QString file(m_fileList.first());
    m_fileList.pop_front();

    m_thread->processRawFile(KUrl(file));
    if (!m_thread->isRunning())
        m_thread->start();
}

void BatchDialog::busy(bool busy)
{
    enableButton(User1, !busy);
    enableButton(User2, busy);
    enableButton(Close, !busy);

    m_decodingSettingsBox->setEnabled(!busy);
    m_saveSettingsBox->setEnabled(!busy);
    m_listView->setEnabled(!busy);

    busy ? m_page->setCursor(Qt::WaitCursor) : m_page->unsetCursor();
}

void BatchDialog::slotConvertBlinkTimerDone()
{
    if(m_convertBlink)
    {
        if (m_currentConvertItem)
            m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("1rightarrow"));
    }
    else
    {
        if (m_currentConvertItem)
            m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("2rightarrow"));
    }

    m_convertBlink = !m_convertBlink;
    m_blinkConvertTimer->start(500);
}

void BatchDialog::processing(const QString& file)
{
    QString filename     = QFileInfo(file).fileName();
    m_currentConvertItem = m_itemDict.find(filename);
    if (m_currentConvertItem) 
    {
        m_listView->setSelected(m_currentConvertItem->viewItem, true);
        m_listView->ensureItemVisible(m_currentConvertItem->viewItem);
    }

    m_convertBlink = false;
    m_blinkConvertTimer->start(500);
}

void BatchDialog::processed(const QString& file, const QString& tmpFile)
{
    m_blinkConvertTimer->stop();
    QString filename = QFileInfo(file).fileName();
    QString destFile(m_currentConvertItem->directory + QString("/") + m_currentConvertItem->dest);

    if (m_saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0) 
        {
            KIO::RenameDialog dlg(this, i18n("Save Raw Image converted from '%1' as",
                                  m_currentConvertItem->src),
                                  tmpFile, destFile,
                                  KIO::RenameDialog_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

            switch (dlg.exec())
            {
                case KIO::R_CANCEL:
                case KIO::R_SKIP:
                {
                    destFile = QString();
                    m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("cancel"));
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
            m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("cancel"));
        }
        else 
        {
            m_currentConvertItem->dest = QFileInfo(destFile).fileName();
            m_currentConvertItem->viewItem->setText(2, m_currentConvertItem->dest);
            m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("ok"));
        }
    }

    m_progressBar->setValue(m_progressBar->value()+1);
    m_currentConvertItem = 0;
}

void BatchDialog::processingFailed(const QString& file)
{
    QString filename = QFileInfo(file).fileName();
    m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("no"));
    m_progressBar->setValue(m_progressBar->value()+1);
    m_currentConvertItem = 0;
}

void BatchDialog::customEvent(QEvent *event)
{
    if (!event) return;

    EventData *d = (EventData*)event;
    if (!d) return;

    QString text;

    if (d->starting)            // Something have been started...
    {
        switch (d->action) 
        {
            case(IDENTIFY): 
                break;
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
                case(IDENTIFY): 
                    break;
                case(PROCESS):
                {
                    processingFailed(d->filePath);
                    processOne();
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
                case(IDENTIFY): 
                {
                    QFileInfo fi(d->filePath);
                    RawItem *rawItem = m_itemDict.find(fi.fileName());
                    if (rawItem) 
                    {
                        if (!d->image.isNull())
                        {
                            QPixmap pix = QPixmap::fromImage(d->image.scaled(64, 64, Qt::KeepAspectRatio));
                            rawItem->viewItem->setThumbnail(pix);
                        }
                        rawItem->viewItem->setText(3, d->message);
                        rawItem->identity = d->message;
                    }
                    break;
                }
                case(PROCESS):
                {
                    processed(d->filePath, d->destPath);
                    processOne();
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

//    delete d;
}

} // NameSpace KIPIRawConverterPlugin

