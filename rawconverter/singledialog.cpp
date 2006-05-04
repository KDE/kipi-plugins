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
#include <knuminput.h>

// KIPI include files

#include <libkipi/version.h>

// Local includes.

#include "singledialog.h"
#include "previewwidget.h"
#include "processcontroller.h"

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

    QGroupBox *settingsBox = new QGroupBox(0, Qt::Vertical, i18n("Settings"), page);
    QGridLayout* settingsBoxLayout = new QGridLayout(settingsBox->layout(), 4, 1, spacingHint());

    cameraWBCheckBox_ = new QCheckBox(i18n("Use camera white balance"), settingsBox);
    QToolTip::add(cameraWBCheckBox_, i18n("<p>Use the camera's custom white-balance settings. "
                                          "The default  is to use fixed daylight values, "
                                          "calculated from sample images."));
    settingsBoxLayout->addMultiCellWidget(cameraWBCheckBox_, 0, 0, 0, 1);    

    // ---------------------------------------------------------------

    fourColorCheckBox_ = new QCheckBox(i18n("Four color RGBG"), settingsBox);
    QToolTip::add(fourColorCheckBox_, i18n("<p>Interpolate RGB as four colors. "
                                      "The default is to assume that all green "
                                      "pixels are the same. If even-row green "
                                      "pixels are more sensitive to ultraviolet light "
                                      "than odd-row this difference causes a mesh "
                                      "pattern in the output; using this option solves "
                                      "this problem with minimal loss of detail."));
    settingsBoxLayout->addMultiCellWidget(fourColorCheckBox_, 1, 1, 0, 1);    

    // ---------------------------------------------------------------

    QLabel *label1 = new QLabel(i18n("Brightness:"), settingsBox);
    brightnessSpinBox_ = new KDoubleNumInput(settingsBox);
    brightnessSpinBox_->setPrecision(2);
    brightnessSpinBox_->setRange(0.0, 10.0, 0.01, true);
    QToolTip::add(brightnessSpinBox_, i18n("<p>Specify the output brightness"));
    settingsBoxLayout->addMultiCellWidget(label1, 2, 2, 0, 0);    
    settingsBoxLayout->addMultiCellWidget(brightnessSpinBox_, 2, 2, 1, 1);    

    // ---------------------------------------------------------------

    QLabel *label2 = new QLabel(i18n("Red multiplier:"), settingsBox);
    redSpinBox_ = new KDoubleNumInput(settingsBox);
    redSpinBox_->setPrecision(2);
    redSpinBox_->setRange(0.0, 10.0, 0.01, true);
    QToolTip::add(redSpinBox_, i18n("<p>After all other color adjustments, "
                                    "multiply the red channel by this value"));
    settingsBoxLayout->addMultiCellWidget(label2, 3, 3, 0, 0);    
    settingsBoxLayout->addMultiCellWidget(redSpinBox_, 3, 3, 1, 1);    

    // ---------------------------------------------------------------

    QLabel *label3 = new QLabel(i18n("Blue multiplier:"), settingsBox);
    blueSpinBox_ = new KDoubleNumInput(settingsBox);
    blueSpinBox_->setPrecision(2);
    blueSpinBox_->setRange(0.0, 10.0, 0.01, true);
    QToolTip::add(blueSpinBox_, i18n("<p>After all other color adjustments, "
                                     "multiply the blue channel by this value"));
    settingsBoxLayout->addMultiCellWidget(label3, 4, 4, 0, 0);    
    settingsBoxLayout->addMultiCellWidget(blueSpinBox_, 4, 4, 1, 1);   

    // ---------------------------------------------------------------

    saveButtonGroup_ = new QVButtonGroup(i18n("Save Format"), page);
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

    mainLayout->addMultiCellWidget(settingsBox, 1, 1, 1, 1);
    mainLayout->addMultiCellWidget(saveButtonGroup_, 2, 2, 1, 1);
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
    controller_        = new ProcessController(this);

    // ---------------------------------------------------------------

    connect( blinkPreviewTimer_, SIGNAL(timeout()),
             this, SLOT(slotPreviewBlinkTimerDone()) );
    
    connect( blinkConvertTimer_, SIGNAL(timeout()),
             this, SLOT(slotConvertBlinkTimerDone()) );

    connect(controller_, SIGNAL(signalIdentified(const QString&, const QString&)),
            this, SLOT(slotIdentified(const QString&, const QString&)));

    connect(controller_, SIGNAL(signalIdentifyFailed(const QString&, const QString&)),
            this, SLOT(slotIdentifyFailed(const QString&, const QString&)));

    connect(controller_, SIGNAL(signalPreviewing(const QString&)),
            this, SLOT(slotPreviewing(const QString&)));

    connect(controller_, SIGNAL(signalPreviewed(const QString&, const QString&)),
            this, SLOT(slotPreviewed(const QString&, const QString&)));

    connect(controller_, SIGNAL(signalPreviewFailed(const QString&)),
            this, SLOT(slotPreviewFailed(const QString&)));

    connect(controller_, SIGNAL(signalProcessing(const QString&)),
            this, SLOT(slotProcessing(const QString&)));

    connect(controller_, SIGNAL(signalProcessed(const QString&, const QString&)),
            this, SLOT(slotProcessed(const QString&, const QString&)));

    connect(controller_, SIGNAL(signalProcessingFailed(const QString&)),
            this, SLOT(slotProcessingFailed(const QString&)));

    connect(controller_, SIGNAL(signalBusy(bool)), 
            this, SLOT(slotBusy(bool)));

    // ---------------------------------------------------------------

    slotBusy(false);
    readSettings();
    QTimer::singleShot(0, this, SLOT( slotIdentify() ) );
}

SingleDialog::~SingleDialog()
{
    blinkPreviewTimer_->stop();
    blinkConvertTimer_->stop();
    saveSettings();
}

void SingleDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    if (actionButton( User3 )->isEnabled()) 
    {
        kdWarning( 51000 ) << "KIPIRAWConverter:single dialog closed" << endl;
    }
    e->accept();
}

void SingleDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("RawConverter Settings");

    brightnessSpinBox_->setValue(config.readDoubleNumEntry("Brightness Multiplier", 1.0));
    redSpinBox_->setValue(config.readDoubleNumEntry("Red Multiplier", 1.0));
    blueSpinBox_->setValue(config.readDoubleNumEntry("Blue Multiplier", 1.0));
    cameraWBCheckBox_->setChecked(config.readBoolEntry("Use Camera WB", true));
    fourColorCheckBox_->setChecked(config.readBoolEntry("Four Color RGB", false));
    saveButtonGroup_->setButton(config.readNumEntry("Output Format", 3));  // PNG by default
}

void SingleDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("RawConverter Settings");

    config.writeEntry("Brightness Multiplier", brightnessSpinBox_->value());
    config.writeEntry("Red Multiplier", redSpinBox_->value());
    config.writeEntry("Blue Multiplier", blueSpinBox_->value());
    config.writeEntry("Use Camera WB", cameraWBCheckBox_->isChecked());
    config.writeEntry("Four Color RGB", fourColorCheckBox_->isChecked());
    config.writeEntry("Output Format", saveButtonGroup_->id(saveButtonGroup_->selected()));
    config.sync();
}

void SingleDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("rawconverter", "kipi-plugins");
}

void SingleDialog::slotUser1()
{
    Settings& s      = controller_->settings;
    s.cameraWB       = cameraWBCheckBox_->isChecked();
    s.fourColorRGB   = fourColorCheckBox_->isChecked();
    s.brightness     = brightnessSpinBox_->value();
    s.redMultiplier  = redSpinBox_->value();
    s.blueMultiplier = blueSpinBox_->value();
    s.outputFormat   = "PPM";

    controller_->preview(inputFile_);
}

void SingleDialog::slotUser2()
{
    Settings& s      = controller_->settings;
    s.cameraWB       = cameraWBCheckBox_->isChecked();
    s.fourColorRGB   = fourColorCheckBox_->isChecked();
    s.brightness     = brightnessSpinBox_->value();
    s.redMultiplier  = redSpinBox_->value();
    s.blueMultiplier = blueSpinBox_->value();
    
    if (saveButtonGroup_->selected() == jpegButton_)
        s.outputFormat = "JPEG";
    else if (saveButtonGroup_->selected() == tiffButton_)
        s.outputFormat = "TIFF";
    else if (saveButtonGroup_->selected() == pngButton_)
        s.outputFormat = "PNG";
    else
        s.outputFormat = "PPM";

    controller_->process(inputFile_);
}

void SingleDialog::slotUser3()
{
    controller_->abort();
}

void SingleDialog::slotIdentify()
{
    controller_->identify(inputFile_);
}

void SingleDialog::slotBusy(bool val)
{   
    enableButton (User1, !val);
    enableButton (User2, !val);
    enableButton (User3, val);
    enableButton (Close, !val);
}

void SingleDialog::slotIdentified(const QString&, const QString& identity)
{
    previewWidget_->setText(inputFileName_ + QString(" : ") + identity);
}

void SingleDialog::slotIdentifyFailed(const QString&, const QString& identity)
{
    previewWidget_->setText(i18n("Failed to identify Raw image\n") + identity);
}

void SingleDialog::slotPreviewing(const QString&)
{
    previewBlink_ = false;
    previewWidget_->setCursor( KCursor::waitCursor() );
    blinkPreviewTimer_->start(200);
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

void SingleDialog::slotPreviewed(const QString&, const QString& tmpFile_)
{
    previewWidget_->unsetCursor();
    blinkPreviewTimer_->stop();
    previewWidget_->load(tmpFile_);
}

void SingleDialog::slotPreviewFailed(const QString&)
{
    previewWidget_->unsetCursor();
    previewWidget_->setText(i18n("Failed to generate preview"), Qt::red);
}

void SingleDialog::slotProcessing(const QString&)
{
    convertBlink_ = false;
    previewWidget_->setCursor( KCursor::waitCursor() );
    blinkConvertTimer_->start(200);
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

void SingleDialog::slotProcessed(const QString&, const QString& tmpFile_)
{
    previewWidget_->unsetCursor();
    blinkConvertTimer_->stop();
    previewWidget_->load(tmpFile_);
    QString filter("*.");
    QString ext;

    if (saveButtonGroup_->selected() == ppmButton_)
	ext = QString("ppm");
    else if (saveButtonGroup_->selected() == tiffButton_)
	ext = QString("tif");
    else if (saveButtonGroup_->selected() == pngButton_)
	ext = QString("png");
    else
	ext = QString("jpg");

    filter += ext;
    QFileInfo fi(inputFile_);
    QString saveFile = fi.dirPath(true) + QString("/") + fi.baseName() + QString(".") + ext;
    saveFile = KFileDialog::getSaveFileName(saveFile, filter, this);
    
    if (saveFile.isEmpty()) 
        return;

    if (::rename(QFile::encodeName( tmpFile_ ), QFile::encodeName( saveFile )) != 0)
    {
        KMessageBox::error(this, i18n("Failed to save image %1").arg( saveFile ));
    }
}

void SingleDialog::slotProcessingFailed(const QString&)
{
    previewWidget_->unsetCursor();
    previewWidget_->setText(i18n("Failed to convert Raw image"), Qt::red);
}

} // NameSpace KIPIRawConverterPlugin

#include "singledialog.moc"
