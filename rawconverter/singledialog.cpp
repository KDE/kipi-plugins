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

    saveButtonGroup_ = new QVButtonGroup(i18n("Save Format"), page);
    saveButtonGroup_->setRadioButtonExclusive(true);

    jpegButton_ = new QRadioButton("JPEG",saveButtonGroup_);
    QToolTip::add(jpegButton_, i18n("Output the processed image in JPEG Format.\n"
                                    "This is a lossy format, but will give\n"
                                    "smaller-sized files"));
    jpegButton_->setChecked(true);

    tiffButton_ = new QRadioButton("TIFF",saveButtonGroup_);
    QToolTip::add(tiffButton_, i18n("Output the processed image in TIFF Format.\n"
                                    "This generates larges, without\n"
                                    "losing quality"));

    ppmButton_ = new QRadioButton("PPM",saveButtonGroup_);
    QToolTip::add(ppmButton_, i18n("Output the processed image in PPM Format.\n"
                                   "This generates the largest files, without\n"
                                   "losing quality"));

    // ---------------------------------------------------------------

    mainLayout->addMultiCellWidget(settingsBox, 1, 1, 1, 1);
    mainLayout->addMultiCellWidget(saveButtonGroup_, 2, 2, 1, 1);
    mainLayout->setColStretch(0, 10);
    mainLayout->setRowStretch(3, 10);

    // ---------------------------------------------------------------
    // About data and help button.

    QPushButton *helpButton = actionButton( Help );

    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("RAW Image Converter"),
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin for RAW image conversion\n"
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
    helpMenu->menu()->insertItem(i18n("RAW Image Converter Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    helpButton->setPopup( helpMenu->menu() );

    // ---------------------------------------------------------------

    setButtonTip( User1, i18n("Generate a Preview from current settings.\n"
                              "Uses a simple bilinear interpolation for\n"
                              "quick results."));

    setButtonTip( User2, i18n("Convert the Raw Image from current settings.\n"
                              "This uses a high-quality adaptive algorithm."));

    setButtonTip( User3, i18n("Abort the current RAW file conversion"));
    
    setButtonTip( Close, i18n("Exit Raw Converter"));

    // ---------------------------------------------------------------

    controller_ = new ProcessController(this);

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
    saveButtonGroup_->setButton(config.readNumEntry("Output Format", 0));
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
        s.outputFormat  = "JPEG";
    else if (saveButtonGroup_->selected() == tiffButton_)
        s.outputFormat  = "TIFF";
    else
        s.outputFormat  = "PPM";

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
    previewWidget_->setText(i18n("Failed to identify raw image\n") + identity);
}

void SingleDialog::slotPreviewing(const QString&)
{
    previewWidget_->setText(i18n("Generating Preview ..."));
}

void SingleDialog::slotPreviewed(const QString&, const QString& tmpFile_)
{
    previewWidget_->load(tmpFile_);
}

void SingleDialog::slotPreviewFailed(const QString&)
{
    previewWidget_->setText(i18n("Failed to generate preview"));
}

void SingleDialog::slotProcessing(const QString&)
{
    previewWidget_->setText(i18n("Converting Raw Image ..."));
}

void SingleDialog::slotProcessed(const QString&, const QString& tmpFile_)
{
    previewWidget_->load(tmpFile_);
    QString filter("*.");
    QString ext;

    if (saveButtonGroup_->selected() == ppmButton_)
	ext = QString("ppm");
    else if (saveButtonGroup_->selected() == tiffButton_)
	ext = QString("tif");
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
    previewWidget_->setText(i18n("Failed to convert raw image"));
}

} // NameSpace KIPIRawConverterPlugin

#include "singledialog.moc"
