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

#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <qgroupbox.h>
#include <qfileinfo.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qfile.h>

// KDE includes.

#include <kcursor.h>
#include <kdebug.h>
#include <klistview.h>
#include <klocale.h>
#include <kurl.h>
#include <kiconloader.h>
#include <kprogress.h>
#include <kio/renamedlg.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>

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
           : KDialogBase(0, 0, false, i18n("Raw Images Batch Converter"),
                         Help|Default|User1|User2|Close, Close, true,
                         i18n("Con&vert"), i18n("&Abort"))
{
    m_currentConvertItem = 0;
    m_thread             = 0;
    m_page = new QWidget( this );
    setMainWidget( m_page );
    QGridLayout *mainLayout = new QGridLayout(m_page, 3, 1, 0, spacingHint());

    //---------------------------------------------

    m_listView = new KListView(m_page);
    m_listView->addColumn( i18n("Thumbnail") );
    m_listView->addColumn( i18n("Raw File") );
    m_listView->addColumn( i18n("Target File") );
    m_listView->addColumn( i18n("Camera") );
    m_listView->setResizeMode(QListView::AllColumns);
    m_listView->setAllColumnsShowFocus(true);
    m_listView->setSorting(-1);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setSelectionMode(QListView::Single);
    m_listView->setMinimumWidth(450);
    mainLayout->addMultiCellWidget(m_listView, 0, 3, 0, 0);

    // ---------------------------------------------------------------

    m_rawSettingsGroup    = new QGroupBox(1, Qt::Vertical, i18n("RAW Decoding Settings"), m_page);
    m_decodingSettingsBox = new KDcrawIface::DcrawSettingsWidget(m_rawSettingsGroup, false, true, true);
    m_saveSettingsBox     = new SaveSettingsWidget(m_page);

    mainLayout->addMultiCellWidget(m_rawSettingsGroup, 0, 0, 1, 1);
    mainLayout->addMultiCellWidget(m_saveSettingsBox, 1, 1, 1, 1);

    m_progressBar = new KProgress(m_page);
    m_progressBar->setMaximumHeight( fontMetrics().height()+2 );
    m_progressBar->setEnabled(false);
    mainLayout->addMultiCellWidget(m_progressBar, 2, 2, 1, 1);

    mainLayout->setColStretch(0, 10);
    mainLayout->setRowStretch(3, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("RAW Images Batch Converter"),
                                           NULL,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin to convert Raw images\n"
                                                     "Using KDcraw library"),
                                           "(c) 2003-2005, Renchi Raju\n"
                                           "(c) 2006-2007, Gilles Caulier");

    m_about->addAuthor("Renchi Raju", I18N_NOOP("Original author"),
                       "renchi@pooh.tam.uiuc.edu");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Maintainer"),
                       "caulier dot gilles at gmail dot com");

    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Raw Converter Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    setButtonTip( User1, i18n("<p>Start converting the Raw images from current settings"));
    setButtonTip( User2, i18n("<p>Abort the current Raw files conversion"));
    setButtonTip( Close, i18n("<p>Exit Raw Converter"));

    m_blinkConvertTimer = new QTimer(this);
    m_thread            = new ActionThread(this);

    // ---------------------------------------------------------------

    connect(m_blinkConvertTimer, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

    connect(m_saveSettingsBox, SIGNAL(signalSaveFormatChanged()),
            this, SLOT(slotSaveFormatChanged()));

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
    KDialogBase::slotClose();
}

void BatchDialog::slotDefault()
{
    m_decodingSettingsBox->setDefaultSettings();
    m_saveSettingsBox->setDefaultSettings();
}

void BatchDialog::readSettings()
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
    m_decodingSettingsBox->setNRThreshold(config.readNumEntry("NR Threshold", 100));
    m_decodingSettingsBox->setUseCACorrection(config.readBoolEntry("EnableCACorrection", false));
    m_decodingSettingsBox->setcaRedMultiplier(config.readDoubleNumEntry("caRedMultiplier", 1.0));
    m_decodingSettingsBox->setcaBlueMultiplier(config.readDoubleNumEntry("caBlueMultiplier", 1.0));
    m_decodingSettingsBox->setUseColorMultipliers(config.readBoolEntry("Use Color Multipliers", false));
    m_decodingSettingsBox->setcolorMultiplier1(config.readDoubleNumEntry("Color Multiplier1", 1.0));
    m_decodingSettingsBox->setcolorMultiplier2(config.readDoubleNumEntry("Color Multiplier2", 1.0));
    m_decodingSettingsBox->setcolorMultiplier3(config.readDoubleNumEntry("Color Multiplier3", 1.0));
    m_decodingSettingsBox->setcolorMultiplier4(config.readDoubleNumEntry("Color Multiplier4", 1.0));

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

    resize(configDialogSize(config, QString("Batch Raw Converter Dialog")));
}

void BatchDialog::saveSettings()
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
    config.writeEntry("NR Threshold", m_decodingSettingsBox->NRThreshold());
    config.writeEntry("EnableCACorrection", m_decodingSettingsBox->useCACorrection());
    config.writeEntry("caRedMultiplier", m_decodingSettingsBox->caRedMultiplier());
    config.writeEntry("caBlueMultiplier", m_decodingSettingsBox->caBlueMultiplier());
    config.writeEntry("Decoding Quality", (int)m_decodingSettingsBox->quality());
    config.writeEntry("Output Color Space", (int)m_decodingSettingsBox->outputColorSpace());
    config.writeEntry("Use Color Multipliers", m_decodingSettingsBox->useColorMultipliers());
    config.writeEntry("Color Multiplier1", m_decodingSettingsBox->colorMultiplier1());
    config.writeEntry("Color Multiplier2", m_decodingSettingsBox->colorMultiplier2());
    config.writeEntry("Color Multiplier3", m_decodingSettingsBox->colorMultiplier3());
    config.writeEntry("Color Multiplier4", m_decodingSettingsBox->colorMultiplier4());

    config.writeEntry("Output Format", (int)m_saveSettingsBox->fileFormat());
    config.writeEntry("Conflict", (int)m_saveSettingsBox->conflictRule());

    saveDialogSize(config, QString("Batch Raw Converter Dialog"));
    config.sync();
}

void BatchDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("rawconverter", "kipi-plugins");
}

void BatchDialog::slotUser1()
{
    m_fileList.clear();

    QListViewItemIterator it( m_listView );
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

    m_progressBar->setTotalSteps(m_fileList.count());
    m_progressBar->setProgress(0);
    m_progressBar->setEnabled(true);

    KDcrawIface::RawDecodingSettings rawDecodingSettings;
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
    m_progressBar->setProgress(0);
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

    KURL::List urlList;

    QPixmap pix(SmallIcon( "file_broken", KIcon::SizeLarge, KIcon::DisabledState ));
    
    for (QStringList::const_iterator  it = itemList.begin();
         it != itemList.end(); ++it) 
    {
        QFileInfo fi(*it);
        if (fi.exists() && !m_itemDict.find(fi.fileName())) 
        {
            RawItem *item = new RawItem;
            item->directory = fi.dirPath();
            item->src  = fi.fileName();
            item->dest = fi.baseName() + QString(".") + ext;
            new CListViewItem(m_listView, pix, item, m_listView->lastItem());
            m_itemDict.insert(item->src, item);
            urlList.append(fi.absFilePath());
        }
    }

    if (!urlList.empty()) 
    {
        m_thread->identifyRawFiles(urlList);
        if (!m_thread->running())
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
    
    QListViewItemIterator it( m_listView );
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

    m_thread->processRawFile(KURL(file));
    if (!m_thread->running())
        m_thread->start();
}

void BatchDialog::busy(bool busy)
{
    enableButton(User1, !busy);
    enableButton(User2, busy);
    enableButton(Close, !busy);

    m_rawSettingsGroup->setEnabled(!busy);
    m_saveSettingsBox->setEnabled(!busy);
    m_listView->setEnabled(!busy);

    busy ? m_page->setCursor(KCursor::waitCursor()) : m_page->unsetCursor();
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
            KIO::RenameDlg dlg(this, i18n("Save Raw Image converted from '%1' as")
                                     .arg(m_currentConvertItem->src),
                               tmpFile, destFile,
                               KIO::RenameDlg_Mode(KIO::M_SINGLE | KIO::M_OVERWRITE | KIO::M_SKIP));

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
            m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("cancel"));
        }
        else 
        {
            m_currentConvertItem->dest = QFileInfo(destFile).fileName();
            m_currentConvertItem->viewItem->setText(2, m_currentConvertItem->dest);
            m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("ok"));
        }
    }

    m_progressBar->advance(1);
    m_currentConvertItem = 0;
}

void BatchDialog::processingFailed(const QString& file)
{
    QString filename = QFileInfo(file).fileName();
    m_currentConvertItem->viewItem->setPixmap(1, SmallIcon("no"));
    m_progressBar->advance(1);
    m_currentConvertItem = 0;
}

void BatchDialog::customEvent(QCustomEvent *event)
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
                case(PROCESS):
                {
                    processingFailed(d->filePath);
                    processOne();
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
                    QFileInfo fi(d->filePath);
                    RawItem *rawItem = m_itemDict.find(fi.fileName());
                    if (rawItem) 
                    {
                        if (!d->image.isNull())
                        {
                            QPixmap pix = QPixmap(d->image.scale(64, 64, QImage::ScaleMin));
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
                    kdWarning( 51000 ) << "KIPIRawConverterPlugin: Unknown event" << endl;
                    break;
                }
            }
        }
    }

    delete d;
}

} // NameSpace KIPIRawConverterPlugin

