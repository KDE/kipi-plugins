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
#include "dmessagebox.h"

namespace KIPIRawConverterPlugin
{

BatchDialog::BatchDialog(QWidget *parent)
           : KDialogBase(parent, 0, false, i18n("Raw Images Batch Converter"),
                         Help|User1|User2|Close, Close, true,
                         i18n("Con&vert"), i18n("&Abort"))
{
    QWidget *page = new QWidget( this );
    setMainWidget( page );
    QGridLayout *mainLayout = new QGridLayout(page, 5, 1, 0, marginHint());

    //---------------------------------------------

    QFrame *headerFrame = new QFrame( page );
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

    listView_ = new KListView(page);
    listView_->addColumn( i18n("Thumbnail") );
    listView_->addColumn( i18n("Raw Image") );
    listView_->addColumn( i18n("Target Image") );
    listView_->addColumn( i18n("Camera") );
    listView_->setResizeMode(QListView::AllColumns);
    listView_->setAllColumnsShowFocus(true);
    listView_->setSorting(-1);
    listView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listView_->setSelectionMode(QListView::Single);
    listView_->setMinimumWidth(450);
    mainLayout->addMultiCellWidget(listView_, 1, 5, 0, 0);

    // ---------------------------------------------------------------

    QGroupBox *settingsBox = new QGroupBox(0, Qt::Vertical, i18n("Settings"), page);
    QGridLayout* settingsBoxLayout = new QGridLayout(settingsBox->layout(), 4, 1, KDialog::spacingHint());

    cameraWBCheckBox_ = new QCheckBox(i18n("Use camera white balance"), settingsBox);
    QToolTip::add(cameraWBCheckBox_, i18n("Use the camera's custom white-balance settings.\n"
                                          "The default  is to use fixed daylight values,\n"
                                          "calculated from sample images."));
    settingsBoxLayout->addMultiCellWidget(cameraWBCheckBox_, 0, 0, 0, 1);    

    // ---------------------------------------------------------------

    fourColorCheckBox_ = new QCheckBox(i18n("Four color RGBG"), settingsBox);
    QToolTip::add(fourColorCheckBox_, i18n("Interpolate RGB as four colors.\n"
                                      "The default is to assume that all green\n"
                                      "pixels are the same. If even-row green\n"
                                      "pixels are more sensitive to ultraviolet light\n"
                                      "than odd-row this difference causes a mesh\n"
                                      "pattern in the output; using this option solves\n"
                                      "this problem with minimal loss of detail."));
    settingsBoxLayout->addMultiCellWidget(fourColorCheckBox_, 1, 1, 0, 1);    

    // ---------------------------------------------------------------

    QLabel *label1 = new QLabel(i18n("Brightness:"), settingsBox);
    brightnessSpinBox_ = new KDoubleNumInput(settingsBox);
    brightnessSpinBox_->setPrecision(2);
    brightnessSpinBox_->setRange(0.0, 10.0, 0.01, true);
    QToolTip::add(brightnessSpinBox_, i18n("Specify the output brightness"));
    settingsBoxLayout->addMultiCellWidget(label1, 2, 2, 0, 0);    
    settingsBoxLayout->addMultiCellWidget(brightnessSpinBox_, 2, 2, 1, 1);    

    // ---------------------------------------------------------------

    QLabel *label2 = new QLabel(i18n("Red multiplier:"), settingsBox);
    redSpinBox_ = new KDoubleNumInput(settingsBox);
    redSpinBox_->setPrecision(2);
    redSpinBox_->setRange(0.0, 10.0, 0.01, true);
    QToolTip::add(redSpinBox_, i18n("After all other color adjustments,\n"
                                    "multiply the red channel by this value"));
    settingsBoxLayout->addMultiCellWidget(label2, 3, 3, 0, 0);    
    settingsBoxLayout->addMultiCellWidget(redSpinBox_, 3, 3, 1, 1);    

    // ---------------------------------------------------------------

    QLabel *label3 = new QLabel(i18n("Blue multiplier:"), settingsBox);
    blueSpinBox_ = new KDoubleNumInput(settingsBox);
    blueSpinBox_->setPrecision(2);
    blueSpinBox_->setRange(0.0, 10.0, 0.01, true);
    QToolTip::add(blueSpinBox_, i18n("After all other color adjustments,\n"
                                     "multiply the blue channel by this value"));
    settingsBoxLayout->addMultiCellWidget(label3, 4, 4, 0, 0);    
    settingsBoxLayout->addMultiCellWidget(blueSpinBox_, 4, 4, 1, 1);   

    // ---------------------------------------------------------------

    saveButtonGroup_ = new QVButtonGroup(i18n("Save Format"),page);
    saveButtonGroup_->setRadioButtonExclusive(true);

    jpegButton_ = new QRadioButton("JPEG",saveButtonGroup_);
    QToolTip::add(jpegButton_, i18n("Output the processed images in JPEG Format.\n"
                               "This is a lossy format, but will give\n"
                               "smaller-sized files"));
    jpegButton_->setChecked(true);

    tiffButton_ = new QRadioButton("TIFF",saveButtonGroup_);
    QToolTip::add(tiffButton_, i18n("Output the processed images in TIFF Format.\n"
                               "This generates large files, without\n"
                               "losing quality"));

    ppmButton_ = new QRadioButton("PPM",saveButtonGroup_);
    QToolTip::add(ppmButton_, i18n("Output the processed images in PPM Format.\n"
                              "This generates the largest files, without\n"
                              "losing quality"));

    // ---------------------------------------------------------------

    conflictButtonGroup_ = new QVButtonGroup(i18n("If Target File Exists"), page);
    conflictButtonGroup_->setRadioButtonExclusive(true);

    overwriteButton_ = new QRadioButton(i18n("Overwrite"),conflictButtonGroup_);
    overwriteButton_->setChecked(true);
    promptButton_ = new QRadioButton(i18n("Open file dialog"),conflictButtonGroup_);

    // ---------------------------------------------------------------

    mainLayout->addMultiCellWidget(settingsBox, 1, 1, 1, 1);
    mainLayout->addMultiCellWidget(saveButtonGroup_, 2, 2, 1, 1);
    mainLayout->addMultiCellWidget(conflictButtonGroup_, 3, 3, 1, 1);

    progressBar_ = new KProgress(page);
    mainLayout->addMultiCellWidget(progressBar_, 4, 4, 1, 1);

    mainLayout->setColStretch(0, 10);
    mainLayout->setRowStretch(5, 10);
    
    // ---------------------------------------------------------------
    // About data and help button.

    QPushButton *helpButton = actionButton( Help );

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("RAW Images Batch Converter"),
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin for RAW images conversion\n"
                                                 "This plugin uses the Dave Coffin RAW photo "
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
    helpMenu->menu()->insertItem(i18n("RAW Images Batch Converter Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    helpButton->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    setButtonTip( User1, i18n("Start converting the raw images from current settings."));

    setButtonTip( User2, i18n("Abort the current RAW files conversion"));
    
    setButtonTip( Close, i18n("Exit Raw Converter"));

    // ---------------------------------------------------------------

    controller_ = new ProcessController(this);

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
    saveSettings();
}

void BatchDialog::addItems(const QStringList& itemList)
{
    QString ext;

    if (saveButtonGroup_->selected() == jpegButton_)
        ext  = "JPEG";
    else if (saveButtonGroup_->selected() == tiffButton_)
        ext  = "TIFF";
    else
        ext  = "PPM";

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
        KIO::PreviewJob* thumbnailJob = KIO::filePreview(urlList, 48 );

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
    saveButtonGroup_->setButton(config.readNumEntry("Output Format", 0));
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
    else
        ext  = "PPM";
    
    QListViewItemIterator it( listView_ );
    while ( it.current() ) 
    {
        CListViewItem *item = (CListViewItem*) it.current();
        RawItem *rawItem = item->rawItem;
        QFileInfo fi(rawItem->directory + QString("/") + rawItem->src);
        rawItem->dest = fi.baseName() + QString(".") + ext;
        item->setText(2,rawItem->dest);
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
        item->setPixmap(1, 0);
        fileList_.append(item->rawItem->directory + QString("/") + item->rawItem->src);
        ++it;
    }

    progressBar_->setTotalSteps(fileList_.count());
    progressBar_->setProgress(0);

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
    else
        s.outputFormat  = "PPM";

    processOne();
}

void BatchDialog::slotUser2()
{
    fileList_.clear();
    controller_->abort();
    slotBusy(false);
    QTimer::singleShot(1000, progressBar_, SLOT(reset()));
}

void BatchDialog::processOne()
{
    if (fileList_.empty()) 
        return;
    
    QString file(fileList_.first());
    fileList_.pop_front();

    controller_->process(file);
}

void BatchDialog::slotBusy(bool busy)
{
    enableButton (User1, !busy);
    enableButton (User2, busy);
    enableButton (Close, !busy);

    saveButtonGroup_->setEnabled(!busy);
    conflictButtonGroup_->setEnabled(!busy);
    cameraWBCheckBox_->setEnabled(!busy);
    fourColorCheckBox_->setEnabled(!busy);
    brightnessSpinBox_->setEnabled(!busy);
    redSpinBox_->setEnabled(!busy);
    blueSpinBox_->setEnabled(!busy);
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

void BatchDialog::slotIdentifyFailed(const QString& file, const QString& identity)
{
    QString filename = QFileInfo(file).fileName();
    RawItem *item = itemDict_.find(filename);
    if (item) 
    {
        DMessageBox::showMsg(identity, i18n("Raw Converter Cannot Handle Following Items"), this);
        delete ((CListViewItem*) item->viewItem);
        itemDict_.remove(filename);
    }
}

void BatchDialog::slotProcessing(const QString& file)
{
    QString filename = QFileInfo(file).fileName();
    RawItem *item = itemDict_.find(filename);
    if (item) 
    {
        item->viewItem->setPixmap(1,SmallIcon("player_play"));
        listView_->setSelected(item->viewItem, true);
    }
}

void BatchDialog::slotProcessed(const QString& file, const QString& tmpFile)
{
    QString filename = QFileInfo(file).fileName();
    RawItem *rawItem = itemDict_.find(filename);
    if (rawItem) 
    {
        rawItem->viewItem->setPixmap(1,SmallIcon("ok"));
    }

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
    QString filename = QFileInfo(file).fileName();
    RawItem *item = itemDict_.find(filename);
    if (item) 
    {
        item->viewItem->setPixmap(1,SmallIcon("no"));
    }
    
    progressBar_->advance(1);
    processOne();
}

void BatchDialog::slotGotThumbnail(const KFileItem* url, const QPixmap& pix)
{
    RawItem *item = itemDict_.find(url->url().filename());
    if (item) 
    {
        item->viewItem->setThumbnail(pix);
    }
}

} // NameSpace KIPIRawConverterPlugin

#include "batchdialog.moc"
