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
#include <qgroupbox.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
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
#include <klistview.h>
#include <klocale.h>
#include <kurl.h>
#include <kiconloader.h>
#include <kprogress.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kio/previewjob.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <knuminput.h>

// KIPI include files

#include <libkipi/version.h>

// Local includes.

#include "batchdialog.h"
#include "processcontroller.h"
#include "clistviewitem.h"

namespace KIPIRawConverterPlugin
{

BatchDialog::BatchDialog(QWidget *parent)
           : KDialogBase(parent, 0, false, i18n("Raw Images Batch Converter"),
                         Help|User1|User2|Close, Close, true,
                         i18n("Con&vert"), i18n("&Abort"))
{
    currentConvertItem_ = 0;
    page_ = new QWidget( this );
    setMainWidget( page_ );
    QGridLayout *mainLayout = new QGridLayout(page_, 5, 1, 0, marginHint());

    //---------------------------------------------

    QFrame *headerFrame = new QFrame( page_ );
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

    listView_ = new KListView(page_);
    listView_->addColumn( i18n("Thumbnail") );
    listView_->addColumn( i18n("Raw File") );
    listView_->addColumn( i18n("Target File") );
    listView_->addColumn( i18n("Camera") );
    listView_->setResizeMode(QListView::AllColumns);
    listView_->setAllColumnsShowFocus(true);
    listView_->setSorting(-1);
    listView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listView_->setSelectionMode(QListView::Single);
    listView_->setMinimumWidth(450);
    mainLayout->addMultiCellWidget(listView_, 1, 5, 0, 0);

    // ---------------------------------------------------------------

    settingsBox_ = new QGroupBox(0, Qt::Vertical, i18n("Settings"), page_);
    QGridLayout* settingsBoxLayout = new QGridLayout(settingsBox_->layout(), 4, 1, KDialog::spacingHint());

    cameraWBCheckBox_ = new QCheckBox(i18n("Use camera white balance"), settingsBox_);
    QToolTip::add(cameraWBCheckBox_, i18n("<p>Use the camera's custom white-balance settings. "
                                          "The default  is to use fixed daylight values, "
                                          "calculated from sample images."));
    settingsBoxLayout->addMultiCellWidget(cameraWBCheckBox_, 0, 0, 0, 1);    

    // ---------------------------------------------------------------

    fourColorCheckBox_ = new QCheckBox(i18n("Four color RGBG"), settingsBox_);
    QToolTip::add(fourColorCheckBox_, i18n("<p>Interpolate RGB as four colors. "
                                      "The default is to assume that all green "
                                      "pixels are the same. If even-row green "
                                      "pixels are more sensitive to ultraviolet light "
                                      "than odd-row this difference causes a mesh "
                                      "pattern in the output; using this option solves "
                                      "this problem with minimal loss of detail."));
    settingsBoxLayout->addMultiCellWidget(fourColorCheckBox_, 1, 1, 0, 1);    

    // ---------------------------------------------------------------

    brightnessLabel_   = new QLabel(i18n("Brightness:"), settingsBox_);
    brightnessSpinBox_ = new KDoubleNumInput(settingsBox_);
    brightnessSpinBox_->setPrecision(2);
    brightnessSpinBox_->setRange(0.0, 10.0, 0.01, true);
    QToolTip::add(brightnessSpinBox_, i18n("<p>Specify the output brightness"));
    settingsBoxLayout->addMultiCellWidget(brightnessLabel_, 2, 2, 0, 0);    
    settingsBoxLayout->addMultiCellWidget(brightnessSpinBox_, 2, 2, 1, 1);    

    // ---------------------------------------------------------------

    redLabel_   = new QLabel(i18n("Red multiplier:"), settingsBox_);
    redSpinBox_ = new KDoubleNumInput(settingsBox_);
    redSpinBox_->setPrecision(2);
    redSpinBox_->setRange(0.0, 10.0, 0.01, true);
    QToolTip::add(redSpinBox_, i18n("<p>After all other color adjustments, "
                                    "multiply the red channel by this value"));
    settingsBoxLayout->addMultiCellWidget(redLabel_, 3, 3, 0, 0);    
    settingsBoxLayout->addMultiCellWidget(redSpinBox_, 3, 3, 1, 1);    

    // ---------------------------------------------------------------

    blueLabel_   = new QLabel(i18n("Blue multiplier:"), settingsBox_);
    blueSpinBox_ = new KDoubleNumInput(settingsBox_);
    blueSpinBox_->setPrecision(2);
    blueSpinBox_->setRange(0.0, 10.0, 0.01, true);
    QToolTip::add(blueSpinBox_, i18n("<p>After all other color adjustments, "
                                     "multiply the blue channel by this value"));
    settingsBoxLayout->addMultiCellWidget(blueLabel_, 4, 4, 0, 0);    
    settingsBoxLayout->addMultiCellWidget(blueSpinBox_, 4, 4, 1, 1);   

    // ---------------------------------------------------------------

    saveButtonGroup_ = new QVButtonGroup(i18n("Save Format"),page_);
    saveButtonGroup_->setRadioButtonExclusive(true);

    jpegButton_ = new QRadioButton("JPEG", saveButtonGroup_);
    QToolTip::add(jpegButton_, i18n("<p>Output the processed image in JPEG Format. "
                                    "Warning!!! JPEG is a lossy format, but will give "
                                    "smaller-sized files. Minimum JPEG compression "
                                    "will be used during conversion."));

    tiffButton_ = new QRadioButton("TIFF", saveButtonGroup_);
    QToolTip::add(tiffButton_, i18n("<p>Output the processed image in TIFF Format. "
                                    "This generates larges, without "
                                    "losing quality. Adobe Deflate compression "
                                    "will be used during conversion."));

    ppmButton_ = new QRadioButton("PPM", saveButtonGroup_);
    QToolTip::add(ppmButton_, i18n("<p>Output the processed image in PPM Format. "
                                   "This generates the largest files, without "
                                   "losing quality"));

    pngButton_ = new QRadioButton("PNG", saveButtonGroup_);
    QToolTip::add(pngButton_, i18n("<p>Output the processed image in PNG Format. "
                                   "This generates larges, without "
                                   "losing quality. Maximum PNG compression "
                                   "will be used during conversion."));

    pngButton_->setChecked(true);

    // ---------------------------------------------------------------

    conflictButtonGroup_ = new QVButtonGroup(i18n("If Target File Exists"), page_);
    conflictButtonGroup_->setRadioButtonExclusive(true);

    overwriteButton_ = new QRadioButton(i18n("Overwrite"),conflictButtonGroup_);
    overwriteButton_->setChecked(true);
    promptButton_ = new QRadioButton(i18n("Open file dialog"),conflictButtonGroup_);

    // ---------------------------------------------------------------

    mainLayout->addMultiCellWidget(settingsBox_, 1, 1, 1, 1);
    mainLayout->addMultiCellWidget(saveButtonGroup_, 2, 2, 1, 1);
    mainLayout->addMultiCellWidget(conflictButtonGroup_, 3, 3, 1, 1);

    progressBar_ = new KProgress(page_);
    progressBar_->setMaximumHeight( fontMetrics().height() );
    progressBar_->setEnabled(false);
    mainLayout->addMultiCellWidget(progressBar_, 4, 4, 1, 1);

    mainLayout->setColStretch(0, 10);
    mainLayout->setRowStretch(5, 10);
    
    // ---------------------------------------------------------------
    // About data and help button.

    QPushButton *helpButton = actionButton( Help );

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Raw Images Batch Converter"),
                                       kipi_version,
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

    blinkConvertTimer_ = new QTimer(this);
    controller_        = new ProcessController(this);

    // ---------------------------------------------------------------

    connect(blinkConvertTimer_, SIGNAL(timeout()),
            this, SLOT(slotConvertBlinkTimerDone()));

    connect(saveButtonGroup_, SIGNAL(clicked(int)),
            this, SLOT(slotSaveFormatChanged()));

    connect(controller_, SIGNAL(signalIdentified(const QString&, const QString&)),
            this, SLOT(slotIdentified(const QString&, const QString&)));

    connect(controller_, SIGNAL(signalIdentifyFailed(const QString&, const QString&)),
            this, SLOT(slotIdentifyFailed(const QString&, const QString&)));

    connect(controller_, SIGNAL(signalProcessing(const QString&)),
            this, SLOT(slotProcessing(const QString&)));

    connect(controller_, SIGNAL(signalProcessed(const QString&, const QString&)),
            this, SLOT(slotProcessed(const QString&, const QString&)));

    connect(controller_, SIGNAL(signalProcessingFailed(const QString&)),
            this, SLOT(slotProcessingFailed(const QString&)));

    connect(controller_, SIGNAL(signalBusy(bool)), 
            this, SLOT(slotBusy(bool)));

    // ---------------------------------------------------------------

    itemDict_.setAutoDelete(true);
    slotBusy(false);
    readSettings();
}

BatchDialog::~BatchDialog()
{
    blinkConvertTimer_->stop();
    saveSettings();
}

void BatchDialog::addItems(const QStringList& itemList)
{
    QString ext;

    if (saveButtonGroup_->selected() == jpegButton_)
        ext = "JPEG";
    else if (saveButtonGroup_->selected() == tiffButton_)
        ext = "TIFF";
    else if (saveButtonGroup_->selected() == pngButton_)
        ext = "PNG";
    else
        ext = "PPM";

    KURL::List urlList;

    QPixmap pix(SmallIcon( "file_broken", KIcon::SizeLarge, KIcon::DisabledState ));
    
    for (QStringList::const_iterator  it = itemList.begin();
         it != itemList.end(); ++it) 
    {
        QFileInfo fi(*it);
        if (fi.exists() && !itemDict_.find(fi.fileName())) 
        {
            RawItem *item = new RawItem;
            item->directory = fi.dirPath();
            item->src  = fi.fileName();
            item->dest = fi.baseName() + QString(".") + ext;
            new CListViewItem(listView_, pix, item);
            itemDict_.insert(item->src, item);
            urlList.append(fi.absFilePath());
        }
    }

    if (!urlList.empty()) 
    {
        KIO::PreviewJob* thumbnailJob = KIO::filePreview(urlList, 48);

        connect(thumbnailJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
                this, SLOT(slotGotThumbnail(const KFileItem*, const QPixmap&)));
    }

    QTimer::singleShot(0, this, SLOT(slotIdentify()));
}

void BatchDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("RawConverter Settings");

    brightnessSpinBox_->setValue(config.readDoubleNumEntry("Brightness Multiplier", 1.0));
    redSpinBox_->setValue(config.readDoubleNumEntry("Red Multiplier", 1.0));
    blueSpinBox_->setValue(config.readDoubleNumEntry("Blue Multiplier", 1.0));
    cameraWBCheckBox_->setChecked(config.readBoolEntry("Use Camera WB", true));
    fourColorCheckBox_->setChecked(config.readBoolEntry("Four Color RGB", false));
    saveButtonGroup_->setButton(config.readNumEntry("Output Format", 3));   // PNG by default.
    conflictButtonGroup_->setButton(config.readNumEntry("Conflict", 0));
}

void BatchDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("RawConverter Settings");

    config.writeEntry("Brightness Multiplier", brightnessSpinBox_->value());
    config.writeEntry("Red Multiplier", redSpinBox_->value());
    config.writeEntry("Blue Multiplier", blueSpinBox_->value());
    config.writeEntry("Use Camera WB", cameraWBCheckBox_->isChecked());
    config.writeEntry("Four Color RGB", fourColorCheckBox_->isChecked());
    config.writeEntry("Output Format", saveButtonGroup_->id(saveButtonGroup_->selected()));
    config.writeEntry("Conflict", conflictButtonGroup_->id(conflictButtonGroup_->selected()));
    config.sync();
}

void BatchDialog::slotSaveFormatChanged()
{
    QString ext;

    if (saveButtonGroup_->selected() == jpegButton_)
        ext  = "JPEG";
    else if (saveButtonGroup_->selected() == tiffButton_)
        ext  = "TIFF";
    else if (saveButtonGroup_->selected() == pngButton_)
        ext  = "PNG";
    else
        ext  = "PPM";
    
    QListViewItemIterator it( listView_ );
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

void BatchDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("rawconverter", "kipi-plugins");
}

void BatchDialog::slotUser1()
{
    fileList_.clear();

    QListViewItemIterator it( listView_ );
    while ( it.current() ) 
    {
        CListViewItem *item = (CListViewItem*) it.current();
        if (item->isEnabled())
        {
            item->setPixmap(1, 0);
            fileList_.append(item->rawItem->directory + QString("/") + item->rawItem->src);
        }
        ++it;
    }

    if (fileList_.empty()) 
    {
        KMessageBox::error(this, i18n("There is no Raw file to process in the list!"));
        enableButton (User1, false);
        enableButton (User2, false);
        slotAborted();
        return;
    }

    progressBar_->setTotalSteps(fileList_.count());
    progressBar_->setProgress(0);
    progressBar_->setEnabled(true);

    Settings& s      = controller_->settings;
    s.cameraWB       = cameraWBCheckBox_->isChecked();
    s.fourColorRGB   = fourColorCheckBox_->isChecked();
    s.brightness     = brightnessSpinBox_->value();
    s.redMultiplier  = redSpinBox_->value();
    s.blueMultiplier = blueSpinBox_->value();

    if (saveButtonGroup_->selected() == jpegButton_)
        s.outputFormat  = "JPEG";
    else if (saveButtonGroup_->selected() == tiffButton_)
        s.outputFormat  = "TIFF";
    else if (saveButtonGroup_->selected() == pngButton_)
        s.outputFormat  = "PNG";
    else
        s.outputFormat  = "PPM";

    processOne();
}

void BatchDialog::slotUser2()
{
    fileList_.clear();
    controller_->abort();
    slotBusy(false);
    QTimer::singleShot(500, this, SLOT(slotAborted()));
}

void BatchDialog::slotAborted()
{
    progressBar_->setProgress(0);
    progressBar_->setEnabled(false);
}

void BatchDialog::processOne()
{
    if (fileList_.empty()) 
    {
        enableButton (User1, false);
        enableButton (User2, false);
        slotAborted();
        return;
    }
    
    QString file(fileList_.first());
    fileList_.pop_front();
    controller_->process(file);
}

void BatchDialog::slotBusy(bool busy)
{
    enableButton (User1, !busy);
    enableButton (User2, busy);
    enableButton (Close, !busy);

    settingsBox_->setEnabled(!busy);
    saveButtonGroup_->setEnabled(!busy);
    conflictButtonGroup_->setEnabled(!busy);
    cameraWBCheckBox_->setEnabled(!busy);
    fourColorCheckBox_->setEnabled(!busy);
    brightnessSpinBox_->setEnabled(!busy);
    redSpinBox_->setEnabled(!busy);
    blueSpinBox_->setEnabled(!busy);
    brightnessLabel_->setEnabled(!busy);
    redLabel_->setEnabled(!busy);
    blueLabel_->setEnabled(!busy);
    listView_->setEnabled(!busy);

    busy ? page_->setCursor(KCursor::waitCursor()) : page_->unsetCursor();
}

void BatchDialog::slotIdentify()
{
    QStringList fileList;

    QDictIterator<RawItem> it( itemDict_ );
    for( ; it.current(); ++it ) 
    {
        RawItem *item = it.current();
        fileList.append(item->directory + QString("/") + item->src);
    }

    controller_->identify(fileList);
}

void BatchDialog::slotIdentified(const QString& file, const QString& identity)
{
    RawItem *item = itemDict_.find(QFileInfo(file).fileName());
    if (item) 
    {
        item->identity = identity;
        item->viewItem->setText(3, identity);
    }
}

void BatchDialog::slotIdentifyFailed(const QString& file, const QString& /*identity*/)
{
    QString filename = QFileInfo(file).fileName();
    RawItem *item = itemDict_.find(filename);
    if (item) 
    {
        item->viewItem->setText(2, i18n("None!"));
        item->viewItem->setText(3, i18n("Not a Raw file!"));
        item->viewItem->setSelectable(false);
        item->viewItem->setEnabled(false);
    }
}

void BatchDialog::slotProcessing(const QString& file)
{
    QString filename    = QFileInfo(file).fileName();
    currentConvertItem_ = itemDict_.find(filename);
    if (currentConvertItem_) 
        listView_->setSelected(currentConvertItem_->viewItem, true);

    convertBlink_ = false;
    blinkConvertTimer_->start(500);
}

void BatchDialog::slotConvertBlinkTimerDone()
{
    if(convertBlink_)
    {
        if (currentConvertItem_)
            currentConvertItem_->viewItem->setPixmap(1, SmallIcon("1rightarrow"));
    }
    else
    {
        if (currentConvertItem_)
            currentConvertItem_->viewItem->setPixmap(1, SmallIcon("2rightarrow"));
    }

    convertBlink_ = !convertBlink_;
    blinkConvertTimer_->start(500);
}

void BatchDialog::slotProcessed(const QString& file, const QString& tmpFile)
{
    currentConvertItem_ = 0;
    QString filename = QFileInfo(file).fileName();
    RawItem *rawItem = itemDict_.find(filename);
    if (rawItem) 
        rawItem->viewItem->setPixmap(1, SmallIcon("ok"));

    QString destFile(rawItem->directory + QString("/") + rawItem->dest);

    if (conflictButtonGroup_->selected() != overwriteButton_)
    {
        struct stat statBuf;
        if (::stat(QFile::encodeName(destFile), &statBuf) == 0) 
        {
            destFile = KFileDialog::getSaveFileName(rawItem->directory, QString(), this,
                                       i18n("Save Raw Image converted from '%1' as").arg(rawItem->src));
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

    progressBar_->advance(1);
    processOne();
}

void BatchDialog::slotProcessingFailed(const QString& file)
{
    currentConvertItem_ = 0;
    QString filename = QFileInfo(file).fileName();
    RawItem *rawItem = itemDict_.find(filename);
    if (rawItem) 
        rawItem->viewItem->setPixmap(1, SmallIcon("no"));
    
    progressBar_->advance(1);
    processOne();
}

void BatchDialog::slotGotThumbnail(const KFileItem* url, const QPixmap& pix)
{
    RawItem *rawItem = itemDict_.find(url->url().filename());
    if (rawItem) 
        rawItem->viewItem->setThumbnail(pix);
}

} // NameSpace KIPIRawConverterPlugin

#include "batchdialog.moc"
