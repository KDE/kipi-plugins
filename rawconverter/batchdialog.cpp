/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date  : 2003-10-24
 * Description : Raw converter batch dialog
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

// C Ansi includes.

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

// Qt includes.

#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qtimer.h>
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
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>

// Local includes.

#include "pluginsversion.h"
#include "dcrawutils.h"
#include "dcrawsettingswidget.h"
#include "savesettingswidget.h"
#include "actionthread.h"
#include "clistviewitem.h"
#include "batchdialog.h"
#include "batchdialog.moc"

namespace KIPIRawConverterPlugin
{

BatchDialog::BatchDialog(QWidget* /*parent*/)
           : KDialogBase(0, 0, false, i18n("Raw Images Batch Converter"),
                         Help|User1|User2|Close, Close, true,
                         i18n("Con&vert"), i18n("&Abort"))
{
    m_currentConvertItem = 0;
    m_thread             = 0;
    m_page = new QWidget( this );
    setMainWidget( m_page );
    QGridLayout *mainLayout = new QGridLayout(m_page, 5, 1, 0, marginHint());

    //---------------------------------------------

    QFrame *headerFrame = new QFrame( m_page );
    headerFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QHBoxLayout* layout = new QHBoxLayout( headerFrame );
    layout->setMargin( 2 ); // to make sure the frame gets displayed
    layout->setSpacing( 0 );
    QLabel *pixmapLabelLeft = new QLabel( headerFrame, "pixmapLabelLeft" );
    pixmapLabelLeft->setScaledContents( false );
    layout->addWidget( pixmapLabelLeft );
    QLabel *labelTitle = new QLabel( i18n("Raw Images Batch Converter"), headerFrame, "labelTitle" );
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
    mainLayout->addMultiCellWidget(m_listView, 1, 5, 0, 0);

    // ---------------------------------------------------------------

    m_decodingSettingsBox = new DcrawSettingsWidget(m_page);
    m_saveSettingsBox     = new SaveSettingsWidget(m_page);

    mainLayout->addMultiCellWidget(m_decodingSettingsBox, 1, 1, 1, 1);
    mainLayout->addMultiCellWidget(m_saveSettingsBox, 2, 3, 1, 1);

    m_progressBar = new KProgress(m_page);
    m_progressBar->setMaximumHeight( fontMetrics().height() );
    m_progressBar->setEnabled(false);
    mainLayout->addMultiCellWidget(m_progressBar, 4, 4, 1, 1);

    mainLayout->setColStretch(0, 10);
    mainLayout->setRowStretch(5, 10);
    
    // ---------------------------------------------------------------
    // About data and help button.

    QPushButton *helpButton = actionButton( Help );

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Raw Images Batch Converter"),
                                       kipiplugins_version,
                                       I18N_NOOP("A Kipi plugin for Raw images conversion\n"
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
    helpMenu->menu()->insertItem(i18n("Raw Converter Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    helpButton->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    setButtonTip( User1, i18n("<p>Start converting the raw images from current settings"));
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

void BatchDialog::readSettings()
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

    resize(configDialogSize(config, QString("Batch Raw Converter Dialog")));
}

void BatchDialog::saveSettings()
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

    KURL::List oneFile;
    oneFile.append(file);
    m_thread->processRawFiles(oneFile);
    if (!m_thread->running())
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
        m_listView->setSelected(m_currentConvertItem->viewItem, true);

    m_convertBlink = false;
    m_blinkConvertTimer->start(500);
}

void BatchDialog::processed(const QString& file, const QString& tmpFile)
{
    m_currentConvertItem = 0;
    QString filename = QFileInfo(file).fileName();
    RawItem *rawItem = m_itemDict.find(filename);
    if (rawItem) 
        rawItem->viewItem->setPixmap(1, SmallIcon("ok"));

    QString destFile(rawItem->directory + QString("/") + rawItem->dest);

    if (m_saveSettingsBox->conflictRule() != SaveSettingsWidget::OVERWRITE)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0) 
        {
            destFile = KFileDialog::getSaveFileName(rawItem->directory, QString(), this,
                                       i18n("Save Raw Image converted from "
                                            "'%1' as").arg(rawItem->src));
        }
    }

    if (!destFile.isEmpty()) 
    {
        if (::rename(QFile::encodeName(tmpFile), QFile::encodeName(destFile)) != 0)
        {
            KMessageBox::error(this, i18n("Failed to save image %1").arg( destFile ));
        }
        else 
        {
            rawItem->dest = QFileInfo(destFile).fileName();
            rawItem->viewItem->setText(2, rawItem->dest);
        }
    }

    m_progressBar->advance(1);
}

void BatchDialog::processingFailed(const QString& file)
{
    m_currentConvertItem = 0;
    QString filename    = QFileInfo(file).fileName();
    RawItem *rawItem    = m_itemDict.find(filename);
    if (rawItem) 
        rawItem->viewItem->setPixmap(1, SmallIcon("no"));
    
    m_progressBar->advance(1);
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

