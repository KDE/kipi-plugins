/* ============================================================
 * File  : batchdialog.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-10-24
 * Description :
 *
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <klistview.h>
#include <klocale.h>
#include <kurl.h>
#include <kiconloader.h>
#include <kprogress.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kapplication.h>

#include <qpushbutton.h>
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

#include <libkipi/thumbnailjob.h>

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

#include "batchdialog.h"
#include "cspinbox.h"
#include "processcontroller.h"
#include "clistviewitem.h"
#include "dmessagebox.h"

namespace RawConverter
{

BatchDialog::BatchDialog()
    : QDialog(0,0,false,Qt::WDestructiveClose)
{
    setCaption(i18n("Digikam Raw Image Batch Converter"));

    QGridLayout *mainLayout = new QGridLayout(this,5,2,6,11);

    // --------------------------------------------------------------

    listView_ = new KListView(this);
    listView_->addColumn( i18n("Thumbnail") );
    listView_->addColumn( i18n("Raw Image") );
    listView_->addColumn( i18n("Target Image") );
    listView_->addColumn( i18n("Camera") );
    listView_->setResizeMode(QListView::AllColumns);
    listView_->setAllColumnsShowFocus(true);
    listView_->setSorting(-1);
    listView_->setSizePolicy(QSizePolicy::Expanding,
                             QSizePolicy::Expanding);
    listView_->setSelectionMode(QListView::Single);

    mainLayout->addMultiCellWidget(listView_, 0, 3, 0, 0);

    // ---------------------------------------------------------------

    QGroupBox *settingsBox = new QGroupBox(i18n("Settings"), this);
    settingsBox->setColumnLayout(0, Qt::Vertical);
    settingsBox->layout()->setSpacing( 6 );
    settingsBox->layout()->setMargin( 11 );
    QVBoxLayout* settingsBoxLayout =
        new QVBoxLayout(settingsBox->layout());

    // ---------------------------------------------------------------

    cameraWBCheckBox_ = new QCheckBox(i18n("Use Camera White Balance"), settingsBox);
    QToolTip::add(cameraWBCheckBox_,
                    i18n("Use the camera's custom white-balance settings.\n"
                         "The default  is to use fixed daylight values,\n"
                         "calculated from sample images."));
    settingsBoxLayout->addWidget(cameraWBCheckBox_);

    fourColorCheckBox_ = new QCheckBox(i18n("Four Color RGBG"), settingsBox);
    QToolTip::add(fourColorCheckBox_,
                    i18n("Interpolate RGB as four colors. \n"
                         "The default is to  assume  that all  green \n"
                         "pixels  are  the same.  If even-row green \n"
                         "pixels are more sensitive to ultraviolet light \n"
                         "than odd-row this difference causes a mesh \n"
                         "pattern in the output. Using this option solves \n"
                         "this problem with minimal loss of detail.\n"));
    settingsBoxLayout->addWidget(fourColorCheckBox_);

    QHBoxLayout *hboxLayout;

    // ---------------------------------------------------------------

    hboxLayout = new QHBoxLayout(0,0,6,"layout1");
    gammaSpinBox_ = new CSpinBox(settingsBox);
    gammaSpinBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hboxLayout->addWidget(gammaSpinBox_);
    hboxLayout->addWidget(new QLabel(i18n("Gamma"), settingsBox));
    QToolTip::add(gammaSpinBox_,
                    i18n("Specify the gamma value"));
    settingsBoxLayout->addLayout(hboxLayout);

    // ---------------------------------------------------------------

    hboxLayout = new QHBoxLayout(0,0,6,"layout2");
    brightnessSpinBox_ = new CSpinBox(settingsBox);
    brightnessSpinBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hboxLayout->addWidget(brightnessSpinBox_);
    hboxLayout->addWidget(new QLabel(i18n("Brightness"), settingsBox));
    QToolTip::add(brightnessSpinBox_,
                    i18n("Specify the output brightness"));
    settingsBoxLayout->addLayout(hboxLayout);

    // ---------------------------------------------------------------

    hboxLayout = new QHBoxLayout(0,0,6,"layout3");
    redSpinBox_ = new CSpinBox(settingsBox);
    redSpinBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QToolTip::add(redSpinBox_,
                    i18n("After all other color adjustments,\n"
                         "multiply the red channel by this value"));

    hboxLayout->addWidget(redSpinBox_);
    hboxLayout->addWidget(new QLabel(i18n("Red Multiplier"), settingsBox));
    settingsBoxLayout->addLayout(hboxLayout);

    // ---------------------------------------------------------------

    hboxLayout = new QHBoxLayout(0,0,6,"layout4");
    blueSpinBox_ = new CSpinBox(settingsBox);
    blueSpinBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QToolTip::add(blueSpinBox_,
                    i18n("After all other color adjustments,\n"
                         "multiply the blue channel by this value"));

    hboxLayout->addWidget(blueSpinBox_);
    hboxLayout->addWidget(new QLabel(i18n("Blue Multiplier"), settingsBox));
    settingsBoxLayout->addLayout(hboxLayout);

    // ---------------------------------------------------------------

    saveButtonGroup_ = new QVButtonGroup(i18n("Save Format"),this);
    saveButtonGroup_->setRadioButtonExclusive(true);

    QRadioButton *radioButton;
    radioButton = new QRadioButton("JPEG",saveButtonGroup_);
    QToolTip::add(radioButton,
                    i18n("Output the processed image in JPEG Format.\n"
                         "This is a lossy format, but will give smaller\n"
                         "sized files"));
    radioButton->setChecked(true);

    radioButton = new QRadioButton("TIFF",saveButtonGroup_);
    QToolTip::add(radioButton,
                    i18n("Output the processed image in TIFF Format.\n"
                         "This is generates big sized files without \n"
                         "losing quality"));

    radioButton = new QRadioButton("PPM",saveButtonGroup_);
    QToolTip::add(radioButton,
                    i18n("Output the processed image in PPM Format.\n"
                         "This is generates biggest sized files without\n"
                         "losing quality"));

    connect(saveButtonGroup_, SIGNAL(clicked(int)),
            SLOT(slotSaveFormatChanged()));

    // ---------------------------------------------------------------

    conflictButtonGroup_ = new QVButtonGroup(i18n("If Target File Exists"),this);
    conflictButtonGroup_->setRadioButtonExclusive(true);

    radioButton = new QRadioButton(i18n("Overwrite"),conflictButtonGroup_);
    radioButton->setChecked(true);
    radioButton = new QRadioButton(i18n("Open File Dialog"),conflictButtonGroup_);

    // ---------------------------------------------------------------

    mainLayout->addWidget(settingsBox, 0, 1);
    mainLayout->addWidget(saveButtonGroup_, 1, 1);
    mainLayout->addWidget(conflictButtonGroup_, 2, 1);
    mainLayout->addItem(new QSpacerItem(10,10,QSizePolicy::Minimum,
                                        QSizePolicy::Expanding), 3, 1);

    // ---------------------------------------------------------------

    QFrame *hline = new QFrame(this);
    hline->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    mainLayout->addMultiCellWidget(hline, 4, 4, 0, 1);

    // ---------------------------------------------------------------

    hboxLayout = new QHBoxLayout(0,0,6);

    progressBar_ = new KProgress(this);
    hboxLayout->addWidget(progressBar_);

    hboxLayout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,
                                        QSizePolicy::Minimum));

    helpButton_ = new QPushButton(i18n("&Help"), this);
    hboxLayout->addWidget(helpButton_);

    aboutButton_ = new QPushButton(i18n("About"), this);
    hboxLayout->addWidget(aboutButton_);

    processButton_ = new QPushButton(i18n("P&rocess"), this);
    QToolTip::add(processButton_,
                  i18n("Start converting the Raw Images from current settings."));
    hboxLayout->addWidget(processButton_);

    abortButton_ = new QPushButton(i18n("&Abort"), this);
    QToolTip::add(abortButton_, i18n("Abort processing images"));
    hboxLayout->addWidget(abortButton_);

    closeButton_ = new QPushButton(i18n("&Close"), this);
    QToolTip::add(closeButton_, i18n("Exit Raw Converter"));
    hboxLayout->addWidget(closeButton_);


    mainLayout->addMultiCellLayout(hboxLayout, 5, 5, 0, 1);

    // ---------------------------------------------------------------

    connect(helpButton_, SIGNAL(clicked()),
            SLOT(slotHelp()));
    connect(aboutButton_, SIGNAL(clicked()),
            SLOT(slotAbout()));
    connect(processButton_, SIGNAL(clicked()),
            SLOT(slotProcess()));
    connect(closeButton_, SIGNAL(clicked()),
            SLOT(close()));
    connect(abortButton_, SIGNAL(clicked()),
            SLOT(slotAbort()));

    // ---------------------------------------------------------------

    controller_ = new ProcessController(this);
    connect(controller_,
            SIGNAL(signalIdentified(const QString&, const QString&)),
            SLOT(slotIdentified(const QString&, const QString&)));
    connect(controller_,
            SIGNAL(signalIdentifyFailed(const QString&, const QString&)),
            SLOT(slotIdentifyFailed(const QString&, const QString&)));
    connect(controller_,
            SIGNAL(signalProcessing(const QString&)),
            SLOT(slotProcessing(const QString&)));
    connect(controller_,
            SIGNAL(signalProcessed(const QString&, const QString&)),
            SLOT(slotProcessed(const QString&, const QString&)));
    connect(controller_,
            SIGNAL(signalProcessingFailed(const QString&)),
            SLOT(slotProcessingFailed(const QString&)));
    connect(controller_,
            SIGNAL(signalBusy(bool)), SLOT(slotBusy(bool)));

    // ---------------------------------------------------------------

    itemDict_.setAutoDelete(true);
    thumbnailJob_ = 0;
    slotBusy(false);

    readSettings();
}

BatchDialog::~BatchDialog()
{
    if (thumbnailJob_)
        delete thumbnailJob_;
    saveSettings();
}

void BatchDialog::addItems(const QStringList& itemList)
{
    QString ext;

    QButton *btn = saveButtonGroup_->selected();
    if (btn) ext = btn->text().lower();

    KURL::List urlList;

    QPixmap pix(SmallIcon( "file_broken", KIcon::SizeLarge,
                           KIcon::DisabledState ));
    for (QStringList::const_iterator  it = itemList.begin();
         it != itemList.end(); ++it) {

        QFileInfo fi(*it);
        if (fi.exists() && !itemDict_.find(fi.fileName())) {
            RawItem *item = new RawItem;
            item->directory = fi.dirPath();
            item->src  = fi.fileName();
            item->dest = fi.baseName() + QString(".") + ext;
            new CListViewItem(listView_, pix, item);
            itemDict_.insert(item->src, item);
            urlList.append(fi.absFilePath());
        }
    }

    if (!urlList.empty()) {
        thumbnailJob_ = new KIPI::ThumbnailJob(urlList, 48, true);
        connect(thumbnailJob_, SIGNAL(signalThumbnail(const KURL&, const QPixmap&)),
                SLOT(slotGotThumbnail(const KURL&, const QPixmap&)));
    }

    QTimer::singleShot(0, this, SLOT(slotIdentify()));
}

void BatchDialog::readSettings()
{
    KConfig* config=kapp->config();

    config->setGroup("RawConverter Settings");

    gammaSpinBox_->setValue(config->readNumEntry("Gamma", 8));
    brightnessSpinBox_->setValue(config->readNumEntry("Brightness",10));

    redSpinBox_->setValue(config->readNumEntry("Red Scale",10));
    blueSpinBox_->setValue(config->readNumEntry("Blue Scale",10));

    cameraWBCheckBox_->setChecked(config->readBoolEntry("Use Camera WB", true));
    fourColorCheckBox_->setChecked(config->readBoolEntry("Four Color RGB", false));

    saveButtonGroup_->setButton(config->readNumEntry("Output Format", 0));
    conflictButtonGroup_->setButton(config->readNumEntry("Conflict", 0));
}

void BatchDialog::saveSettings()
{
    KConfig* config=kapp->config();

    config->setGroup("RawConverter Settings");

    config->writeEntry("Gamma", gammaSpinBox_->value());
    config->writeEntry("Brightness", brightnessSpinBox_->value());

    config->writeEntry("Red Scale", redSpinBox_->value());
    config->writeEntry("Blue Scale", blueSpinBox_->value());

    config->writeEntry("Use Camera WB", cameraWBCheckBox_->isChecked());
    config->writeEntry("Four Color RGB", fourColorCheckBox_->isChecked());

    config->writeEntry("Output Format",
                       saveButtonGroup_->id(saveButtonGroup_->selected()));
    config->writeEntry("Conflict",
                       conflictButtonGroup_->id(conflictButtonGroup_->selected()));

    config->sync();
}

void BatchDialog::slotSaveFormatChanged()
{
    QString ext = saveButtonGroup_->selected()->text().lower();
    if (ext.isEmpty()) return;

    QListViewItemIterator it( listView_ );
    while ( it.current() ) {
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
    KApplication::kApplication()->invokeHelp("plugin-rawconverter.anchor",
                                             "digikam");
}

void BatchDialog::slotAbout()
{
    KMessageBox::about(this, i18n("A Digikam plugin for batch RAW images converting\n\n"
                                "Author: Renchi Raju\n\n"
                                "Email: renchi@pooh.tam.uiuc.edu\n\n"
                                "This plugin uses the Dave Coffin RAW photo decoder program \"dcraw\""),
                                i18n("About batch RAW images converter"));
}

void BatchDialog::slotProcess()
{
    fileList_.clear();

    QListViewItemIterator it( listView_ );
    while ( it.current() ) {
        CListViewItem *item = (CListViewItem*) it.current();
        item->setPixmap(1, 0);
        fileList_.append(item->rawItem->directory +
                         QString("/") + item->rawItem->src);
        ++it;
    }

    progressBar_->setTotalSteps(fileList_.count());
    progressBar_->setProgress(0);

    Settings& s      = controller_->settings;
    s.cameraWB       = cameraWBCheckBox_->isChecked();
    s.fourColorRGB   = fourColorCheckBox_->isChecked();
    s.gamma          = gammaSpinBox_->value()/10.0;
    s.brightness     = brightnessSpinBox_->value()/10.0;
    s.redMultiplier  = redSpinBox_->value()/10.0;
    s.blueMultiplier = blueSpinBox_->value()/10.0;
    s.outputFormat   = saveButtonGroup_->selected()->text();

    processOne();
}

void BatchDialog::processOne()
{
    if (fileList_.empty()) {
        return;
    }

    QString file(fileList_.first());
    fileList_.pop_front();

    controller_->process(file);
}

void BatchDialog::slotAbort()
{
    fileList_.clear();
    controller_->abort();
    slotBusy(false);
    QTimer::singleShot(500, progressBar_,
                       SLOT(reset()));
}

void BatchDialog::slotBusy(bool busy)
{
    abortButton_->setEnabled(busy);
    closeButton_->setEnabled(!busy);
    processButton_->setEnabled(!busy);
    saveButtonGroup_->setEnabled(!busy);
    conflictButtonGroup_->setEnabled(!busy);
    cameraWBCheckBox_->setEnabled(!busy);
    fourColorCheckBox_->setEnabled(!busy);
    gammaSpinBox_->setEnabled(!busy);
    brightnessSpinBox_->setEnabled(!busy);
    redSpinBox_->setEnabled(!busy);
    blueSpinBox_->setEnabled(!busy);
}

void BatchDialog::slotIdentify()
{
    QStringList fileList;

    QDictIterator<RawItem> it( itemDict_ );
    for( ; it.current(); ++it ) {
        RawItem *item = it.current();
        fileList.append(item->directory +
                        QString("/") +
                        item->src);
    }

    controller_->identify(fileList);
}

void BatchDialog::slotIdentified(const QString& file,
                                 const QString& identity)
{
    RawItem *item = itemDict_.find(QFileInfo(file).fileName());
    if (item) {
        item->identity = identity;
        item->viewItem->setText(3, identity);
    }
}

void BatchDialog::slotIdentifyFailed(const QString& file,
                                     const QString& identity)
{
    QString filename = QFileInfo(file).fileName();
    RawItem *item = itemDict_.find(filename);
    if (item) {
        DMessageBox::
            showMsg(identity,
                    i18n("Raw Converter Cannot Handle Following Items"),
                    this);
        delete ((CListViewItem*) item->viewItem);
        itemDict_.remove(filename);
    }
}

void BatchDialog::slotProcessing(const QString& file)
{
    QString filename = QFileInfo(file).fileName();
    RawItem *item = itemDict_.find(filename);
    if (item) {
        item->viewItem->setPixmap(1,SmallIcon("player_play"));
        listView_->setSelected(item->viewItem, true);
    }
}

void BatchDialog::slotProcessed(const QString& file,
                                const QString& tmpFile)
{
    QString filename = QFileInfo(file).fileName();
    RawItem *rawItem = itemDict_.find(filename);
    if (rawItem) {
        rawItem->viewItem->setPixmap(1,SmallIcon("ok"));
    }

    QString destFile(rawItem->directory + QString("/") +
                     rawItem->dest);

    if (conflictButtonGroup_->selected()->text() != i18n("Overwrite"))
    {
        struct stat statBuf;
        if (::stat(destFile.latin1(), &statBuf) == 0) {
            QString filter("*.");
            filter += saveButtonGroup_->selected()->text().lower();
            destFile = KFileDialog::
                       getSaveFileName(rawItem->directory,
                                       filter, this,
                                       i18n("Save Raw Image converted "
                                            "from '%1' as").arg(rawItem->src));
        }
    }

    if (!destFile.isEmpty()) {
        if (::rename(tmpFile.latin1(), destFile.latin1()) != 0)
        {
            KMessageBox::error(this, i18n("Failed to save image ")
                               + destFile);
        }
        else {
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
    if (item) {
        item->viewItem->setPixmap(1,SmallIcon("no"));
    }
    progressBar_->advance(1);
    processOne();
}

void BatchDialog::slotGotThumbnail(const KURL& url,
                                   const QPixmap& pix)
{
    RawItem *item = itemDict_.find(url.filename());
    if (item) {
        item->viewItem->setThumbnail(pix);
    }
}

}


#include "batchdialog.moc"
