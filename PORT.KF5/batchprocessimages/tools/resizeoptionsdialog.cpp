/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 18.10.2009
 * Description : option dialogs for resizing
 *
 * Copyright (C) 2009 by Johannes Wienke <languitar at semipol dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "resizeoptionsdialog.moc"

// Qt includes

#include <QGroupBox>

// KDE includes

#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>

// Local inclues

#include "resizecommandbuilder.h"

namespace KIPIBatchProcessImagesPlugin
{

const QString ResizeOptionsBaseDialog::OPTION_QUALITY_NAME = "Quality";
const QString ResizeOptionsBaseDialog::OPTION_FILTER_NAME  = "ResizeFilter";

ResizeOptionsBaseDialog::ResizeOptionsBaseDialog(QWidget* const parent,
                                                 ResizeCommandBuilder* const commandBuilder,
                                                 const QString& settingsPrefix)
                       : KDialog(parent),
                         m_settingsPrefix(settingsPrefix), m_commandBuilder(commandBuilder),
                         m_mainWidget(new QWidget(this)), m_resizeFilterLabel(0),
                         m_resizeFilterComboBox(0), m_qualityInput(0)
{
    // general dialog settings
    setCaption(i18n("Image-Resize Options"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);

    // setup main widget for the dialog
    setMainWidget(m_mainWidget);
    m_mainWidgetLayout = new QVBoxLayout(m_mainWidget);
    m_mainWidgetLayout->setSpacing(spacingHint());
    m_mainWidgetLayout->setMargin(spacingHint());

    // connections
    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));
}

ResizeOptionsBaseDialog::~ResizeOptionsBaseDialog()
{
}

void ResizeOptionsBaseDialog::layout()
{
    // call template method to add widgets before the quality settings
    prependWidgets();

    // setup content that every dialog has
    QGroupBox *qualityGroupBox         = new QGroupBox(i18n("Quality Settings"), this);
    QGridLayout *qualityGroupBoxLayout = new QGridLayout(qualityGroupBox);
    qualityGroupBox->setLayout(qualityGroupBoxLayout);

    m_resizeFilterLabel    = new QLabel(i18n("Filter:"), qualityGroupBox);
    m_resizeFilterComboBox = new KComboBox();
    m_defaultFilterName    = i18nc("Filter name", "&lt;default&gt;");
    m_resizeFilterComboBox->addItem(m_defaultFilterName);
    // we really don't need to translate these filter names
    m_resizeFilterComboBox->insertItems(1, ResizeCommandBuilder::getAllowedFilters());
    m_resizeFilterComboBox->setWhatsThis(i18n("Select here the filter name for the resize-image process. "
                                      "This filter will be used like a kernel convolution process "
                                      "during the increased image size rendering."));
    m_resizeFilterLabel->setBuddy(m_resizeFilterComboBox);

    m_qualityInput = new KIntNumInput(75);
    m_qualityInput->setRange(1, ResizeCommandBuilder::MAX_QUALITY);
    m_qualityInput->setSliderEnabled(true);
    m_qualityInput->setLabel(i18n("Image quality (percent):"));
    m_qualityInput->setWhatsThis(i18n("Quality for compressed images."));

    qualityGroupBoxLayout->addWidget(m_qualityInput,            1, 0, 1, -1);
    qualityGroupBoxLayout->addWidget(m_resizeFilterLabel,       2, 0, 1, 1);
    qualityGroupBoxLayout->addWidget(m_resizeFilterComboBox,    2, 1, 1, 1);

    addOptionWidget(qualityGroupBox);

    // call template method to append options at the end of the dialog
    appendWidgets();
}

void ResizeOptionsBaseDialog::readSettings(const QString& rcname, const QString& groupName)
{
    kDebug() << "reading settings";

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    m_resizeFilterComboBox->setCurrentIndex(group.readEntry(m_settingsPrefix
                    + OPTION_FILTER_NAME, 0));
    m_commandBuilder->setFilterName(m_resizeFilterComboBox->currentText());
    m_qualityInput->setValue(group.readEntry(m_settingsPrefix
                    + OPTION_QUALITY_NAME, 75));
    m_commandBuilder->setQuality(m_qualityInput->value());
}

void ResizeOptionsBaseDialog::saveSettings(const QString& rcname, const QString& groupName)
{
    kDebug() << "saving settings";

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    group.writeEntry(m_settingsPrefix + OPTION_FILTER_NAME,
                    m_resizeFilterComboBox->currentIndex());
    group.writeEntry(m_settingsPrefix + OPTION_QUALITY_NAME,
                    m_qualityInput->value());
}

void ResizeOptionsBaseDialog::addOptionWidget(QWidget *widget)
{
    m_mainWidgetLayout->addWidget(widget);
}

void ResizeOptionsBaseDialog::slotOk()
{
    // first ensure that the gui handling of the subclass was ok
    bool subclassOk = handleOk();
    if (!subclassOk)
    {
        kDebug() << "subclass indicated an error in gui handling, "
                      << "stopping own handling here.";
        return;
    }

    // then handle own
    m_commandBuilder->setQuality(m_qualityInput->value());
    if (m_resizeFilterComboBox->currentText() == m_defaultFilterName)
    {
        m_commandBuilder->setFilterName("");
    }
    else
    {
        m_commandBuilder->setFilterName(m_resizeFilterComboBox->currentText());
    }

    accept();
}

// --- one dimensional resizing ---

const QString OneDimResizeOptionsDialog::OPTION_SIZE_NAME = "OneDimSize";

OneDimResizeOptionsDialog::OneDimResizeOptionsDialog(QWidget *parent,
                           OneDimResizeCommandBuilder *commandBuilder)
                         : ResizeOptionsBaseDialog(parent, commandBuilder, "OneDim"),
                           m_commandBuilder(commandBuilder), m_sizeInput(0)
{
}

OneDimResizeOptionsDialog::~OneDimResizeOptionsDialog()
{
}

void OneDimResizeOptionsDialog::readSettings(const QString& rcname, const QString& groupName)
{
    ResizeOptionsBaseDialog::readSettings(rcname, groupName);

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    m_sizeInput->setValue(group.readEntry(OPTION_SIZE_NAME, 600));
    m_commandBuilder->setSize(m_sizeInput->value());
}

void OneDimResizeOptionsDialog::saveSettings(const QString& rcname, const QString& groupName)
{
    ResizeOptionsBaseDialog::saveSettings(rcname, groupName);

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    group.writeEntry(OPTION_SIZE_NAME, m_sizeInput->value());
}

void OneDimResizeOptionsDialog::prependWidgets()
{

    // prepend size options
    QGroupBox *sizeGroupBox = new QGroupBox(i18n("Size Settings"), this);
    QGridLayout *sizeGroupBoxLayout = new QGridLayout(sizeGroupBox);
    sizeGroupBox->setLayout(sizeGroupBoxLayout);

    m_sizeInput = new KIntNumInput(sizeGroupBox);
    m_sizeInput->setRange(ResizeCommandBuilder::MIN_SIZE, 10000);
    m_sizeInput->setSliderEnabled(true);
    m_sizeInput->setLabel(i18n("New size (pixels):"));
    m_sizeInput->setWhatsThis(i18n("The new images' size in pixels."));

    sizeGroupBoxLayout->addWidget(m_sizeInput, 0, 0, 1, -1);

    addOptionWidget(sizeGroupBox);
}

void OneDimResizeOptionsDialog::appendWidgets()
{
}

bool OneDimResizeOptionsDialog::handleOk()
{
    m_commandBuilder->setSize(m_sizeInput->value());
    return true;
}

QString OneDimResizeOptionsDialog::getWhatsThis()
{
    return i18n("<p><b>Proportional (1 dim.)</b>: "
                "standard auto-resizing using one dimension. "
                "The width or the height of the images will be automatically "
                "selected, depending on the images' orientations. "
                "The images' aspect ratios are preserved.</p>");
}

// --- two dimensional resizing ---

const QString TwoDimResizeOptionsDialog::OPTION_WIDTH_NAME = "TwoDimWidth";
const QString TwoDimResizeOptionsDialog::OPTION_HEIGHT_NAME = "TwoDimHeight";
const QString TwoDimResizeOptionsDialog::OPTION_FILL_NAME = "TwoDimFill";
const QString TwoDimResizeOptionsDialog::OPTION_FILL_COLOR_NAME = "TwoDimFillColor";

TwoDimResizeOptionsDialog::TwoDimResizeOptionsDialog(QWidget *parent,
                           TwoDimResizeCommandBuilder *commandBuilder)
                         : ResizeOptionsBaseDialog(parent, commandBuilder, "TwoDim"),
                           m_commandBuilder(commandBuilder), m_widthInput(0),
                           m_heightInput(0), m_fillCheckBox(0), m_fillColorLabel(0),
                           m_fillColorButton(0)
{
}

TwoDimResizeOptionsDialog::~TwoDimResizeOptionsDialog()
{
}

void TwoDimResizeOptionsDialog::readSettings(const QString& rcname, const QString& groupName)
{
    ResizeOptionsBaseDialog::readSettings(rcname, groupName);

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    m_widthInput->setValue(group.readEntry(OPTION_WIDTH_NAME, 640));
    m_commandBuilder->setWidth(m_widthInput->value());
    m_heightInput->setValue(group.readEntry(OPTION_HEIGHT_NAME, 480));
    m_commandBuilder->setHeight(m_heightInput->value());
    m_fillCheckBox->setChecked(group.readEntry(OPTION_FILL_NAME, false));
    m_commandBuilder->setFill(m_fillCheckBox->isChecked());
    m_fillColorButton->setColor(group.readEntry(OPTION_FILL_COLOR_NAME, QColor(Qt::white)));
    m_commandBuilder->setFillColor(m_fillColorButton->color());
}

void TwoDimResizeOptionsDialog::saveSettings(const QString& rcname, const QString& groupName)
{
    ResizeOptionsBaseDialog::saveSettings(rcname, groupName);

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    group.writeEntry(OPTION_WIDTH_NAME, m_widthInput->value());
    group.writeEntry(OPTION_HEIGHT_NAME, m_heightInput->value());
    group.writeEntry(OPTION_FILL_NAME, m_fillCheckBox->isChecked());
    group.writeEntry(OPTION_FILL_COLOR_NAME, m_fillColorButton->color());
}

void TwoDimResizeOptionsDialog::prependWidgets()
{
    // prepend size options
    QGroupBox *sizeGroupBox = new QGroupBox(i18n("Size Settings"), this);
    QGridLayout *sizeGroupBoxLayout = new QGridLayout(sizeGroupBox);
    sizeGroupBox->setLayout(sizeGroupBoxLayout);

    m_widthInput = new KIntNumInput(sizeGroupBox);
    m_widthInput->setRange(ResizeCommandBuilder::MIN_SIZE, 10000);
    m_widthInput->setSliderEnabled(true);
    m_widthInput->setLabel(i18n("Width (pixels):"));
    m_widthInput->setWhatsThis(i18n("The new images' width in pixels."));

    m_heightInput = new KIntNumInput(sizeGroupBox);
    m_heightInput->setRange(ResizeCommandBuilder::MIN_SIZE, 10000);
    m_heightInput->setSliderEnabled(true);
    m_heightInput->setLabel(i18n("Height (pixels):"));
    m_heightInput->setWhatsThis(i18n("The new images' height in pixels."));

    m_fillCheckBox = new QCheckBox(i18n("Fill image to specified size"), sizeGroupBox);
    m_fillCheckBox->setWhatsThis(i18n("If this box is set, the resulting image is "
                    "filled up with the specified background color to the desired dimension, "
                    "otherwise it is only as big as needed to fit the original image "
                    "into the specified dimensions."));

    m_fillColorLabel  = new QLabel(i18n("Background color:"), sizeGroupBox);
    m_fillColorButton = new KColorButton(QColor(Qt::white), sizeGroupBox);
    m_fillColorButton->setWhatsThis(i18n("You can select here the background color to "
                                        "be used when adapting the images' sizes."));
    m_fillColorLabel->setBuddy(m_fillColorButton);

    sizeGroupBoxLayout->addWidget(m_widthInput, 0, 0, 1, -1);
    sizeGroupBoxLayout->addWidget(m_heightInput, 1, 0, 1, -1);
    sizeGroupBoxLayout->addWidget(m_fillCheckBox, 2, 0, 1, -1);
    sizeGroupBoxLayout->addWidget(m_fillColorLabel, 3, 0, 1, 1);
    sizeGroupBoxLayout->addWidget(m_fillColorButton, 3, 1, 1, 1);

    addOptionWidget(sizeGroupBox);
}

void TwoDimResizeOptionsDialog::appendWidgets()
{
}

bool TwoDimResizeOptionsDialog::handleOk()
{
    m_commandBuilder->setWidth(m_widthInput->value());
    m_commandBuilder->setHeight(m_heightInput->value());
    m_commandBuilder->setFill(m_fillCheckBox->isChecked());
    m_commandBuilder->setFillColor(m_fillColorButton->color());
    return true;
}

QString TwoDimResizeOptionsDialog::getWhatsThis()
{
    return i18n("<p><b>Proportional (2 dim.)</b>: auto-resizing using two dimensions. "
                "The images' aspect ratio are preserved. You can use this, for example, "
                "to adapt your images' sizes to your screen size.</p>");
}

// --- non-proportional resizing ---

const QString NonProportionalResizeOptionsDialog::OPTION_WIDTH_NAME = "NonPropWidth";
const QString NonProportionalResizeOptionsDialog::OPTION_HEIGHT_NAME = "NonPropHeight";

NonProportionalResizeOptionsDialog::NonProportionalResizeOptionsDialog(QWidget *parent,
                                    NonProportionalResizeCommandBuilder *commandBuilder)
                                  : ResizeOptionsBaseDialog(parent, commandBuilder, "NonProp"),
                                    m_commandBuilder(commandBuilder), m_widthInput(0),
                                    m_heightInput(0)
{
}

NonProportionalResizeOptionsDialog::~NonProportionalResizeOptionsDialog()
{
}

void NonProportionalResizeOptionsDialog::readSettings(const QString& rcname, const QString& groupName)
{
    ResizeOptionsBaseDialog::readSettings(rcname, groupName);

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    m_widthInput->setValue(group.readEntry(OPTION_WIDTH_NAME, 640));
    m_commandBuilder->setWidth(m_widthInput->value());
    m_heightInput->setValue(group.readEntry(OPTION_HEIGHT_NAME, 480));
    m_commandBuilder->setHeight(m_heightInput->value());
}

void NonProportionalResizeOptionsDialog::saveSettings(const QString& rcname, const QString& groupName)
{
    ResizeOptionsBaseDialog::saveSettings(rcname, groupName);

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    group.writeEntry(OPTION_WIDTH_NAME, m_widthInput->value());
    group.writeEntry(OPTION_HEIGHT_NAME, m_heightInput->value());
}

void NonProportionalResizeOptionsDialog::prependWidgets()
{
    // prepend size options
    QGroupBox *sizeGroupBox = new QGroupBox(i18n("Size Settings"), this);
    QGridLayout *sizeGroupBoxLayout = new QGridLayout(sizeGroupBox);
    sizeGroupBox->setLayout(sizeGroupBoxLayout);

    m_widthInput = new KIntNumInput(sizeGroupBox);
    m_widthInput->setRange(ResizeCommandBuilder::MIN_SIZE, 10000);
    m_widthInput->setSliderEnabled(true);
    m_widthInput->setLabel(i18n("Width (pixels):"));
    m_widthInput->setWhatsThis(i18n("The new images' width in pixels."));

    m_heightInput = new KIntNumInput(sizeGroupBox);
    m_heightInput->setRange(ResizeCommandBuilder::MIN_SIZE, 10000);
    m_heightInput->setSliderEnabled(true);
    m_heightInput->setLabel(i18n("Height (pixels):"));
    m_heightInput->setWhatsThis(i18n("The new images' height in pixels."));

    sizeGroupBoxLayout->addWidget(m_widthInput, 0, 0, 1, -1);
    sizeGroupBoxLayout->addWidget(m_heightInput, 1, 0, 1, -1);

    addOptionWidget(sizeGroupBox);
}

void NonProportionalResizeOptionsDialog::appendWidgets()
{
}

bool NonProportionalResizeOptionsDialog::handleOk()
{
    m_commandBuilder->setWidth(m_widthInput->value());
    m_commandBuilder->setHeight(m_heightInput->value());
    return true;
}

QString NonProportionalResizeOptionsDialog::getWhatsThis()
{
    return i18n("<p><b>Non proportional</b>: non-proportional resizing using two dimensions. "
                "The images' aspect ratios are not preserved.</p>");
}

// --- print prepare resizing ---

const QString PrintPrepareResizeOptionsDialog::OPTION_PAPER_SIZE_NAME = "PaperSize";
const QString PrintPrepareResizeOptionsDialog::OPTION_DPI_NAME = "DPI";
const QString PrintPrepareResizeOptionsDialog::OPTION_CUSTOM_PAPER_WIDTH_NAME = "CustomPaperWidth";
const QString PrintPrepareResizeOptionsDialog::OPTION_CUSTOM_PAPTER_HEIGHT_NAME = "CustomPaperHeight";
const QString PrintPrepareResizeOptionsDialog::OPTION_CUSTOM_DPI_NAME = "CustomDPI";
const QString PrintPrepareResizeOptionsDialog::OPTION_STRETCH_NAME = "Stretch";
const QString PrintPrepareResizeOptionsDialog::OPTION_CUSTOM_SETTINGS_NAME = "CustomSettings";

PrintPrepareResizeOptionsDialog::PrintPrepareResizeOptionsDialog(QWidget *parent,
                                 PrintPrepareResizeCommandBuilder *commandBuilder)
                               : ResizeOptionsBaseDialog(parent, commandBuilder, "Print"),
                                 m_commandBuilder(commandBuilder), m_paperSizeLabel(0),
                                 m_dpiLabel(0), m_customPaperWidthLabel(0),
                                 m_customPaperHeightLabel(0), m_customDpiLabel(0),
                                 m_customPaperWidthInput(0), m_customPaperHeightInput(0),
                                 m_customDpiInput(0), m_paperSizeComboBox(0),
                                 m_dpiComboBox(0), m_customSettingsCheckBox(0),
                                 m_stretchCheckBox(0)
{
}

PrintPrepareResizeOptionsDialog::~PrintPrepareResizeOptionsDialog()
{
}

void PrintPrepareResizeOptionsDialog::readSettings(const QString& rcname, const QString& groupName)
{
    ResizeOptionsBaseDialog::readSettings(rcname, groupName);

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    m_paperSizeComboBox->setCurrentIndex(group.readEntry(OPTION_PAPER_SIZE_NAME, 0));
    m_dpiComboBox->setCurrentIndex(group.readEntry(OPTION_DPI_NAME, 0));
    m_customPaperWidthInput->setValue(group.readEntry(OPTION_CUSTOM_PAPER_WIDTH_NAME, 13));
    m_customPaperHeightInput->setValue(group.readEntry(OPTION_CUSTOM_PAPTER_HEIGHT_NAME, 9));
    m_customDpiInput->setValue(group.readEntry(OPTION_CUSTOM_DPI_NAME, 300));
    m_stretchCheckBox->setChecked(group.readEntry(OPTION_STRETCH_NAME, false));
    m_customSettingsCheckBox->setChecked(group.readEntry(OPTION_CUSTOM_SETTINGS_NAME, false));

    handleOk();
}

void PrintPrepareResizeOptionsDialog::saveSettings(const QString& rcname, const QString& groupName)
{
    ResizeOptionsBaseDialog::saveSettings(rcname, groupName);

    KConfig config(rcname);
    KConfigGroup group = config.group(groupName);

    group.writeEntry(OPTION_PAPER_SIZE_NAME, m_paperSizeComboBox->currentIndex());
    group.writeEntry(OPTION_DPI_NAME, m_dpiComboBox->currentIndex());
    group.writeEntry(OPTION_CUSTOM_PAPER_WIDTH_NAME, m_customPaperWidthInput->value());
    group.writeEntry(OPTION_CUSTOM_PAPTER_HEIGHT_NAME, m_customPaperHeightInput->value());
    group.writeEntry(OPTION_CUSTOM_DPI_NAME, m_customDpiInput->value());
    group.writeEntry(OPTION_STRETCH_NAME, m_stretchCheckBox->isChecked());
    group.writeEntry(OPTION_CUSTOM_SETTINGS_NAME, m_customSettingsCheckBox->isChecked());
}

void PrintPrepareResizeOptionsDialog::prependWidgets()
{

    m_customSettingsCheckBox = new QCheckBox(i18n("Use custom settings"), this);
    m_customSettingsCheckBox->setWhatsThis(i18n("If this option is enabled, "
                                        "all printing settings can be customized."));
    addOptionWidget(m_customSettingsCheckBox);

    connect(m_customSettingsCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(slotCustomSettingsEnabled(bool)));

    QGroupBox *sizeGroupBox         = new QGroupBox(i18n("Printing Standard Settings"), this);
    QGridLayout *sizeGroupBoxLayout = new QGridLayout(sizeGroupBox);

    m_paperSizeLabel = new QLabel(i18n("Paper size (cm):"), sizeGroupBox);
    m_paperSizeComboBox = new KComboBox(sizeGroupBox);
    m_paperSizeComboBox->addItem("9x13");
    m_paperSizeComboBox->addItem("10x15");
    m_paperSizeComboBox->addItem("13x19");
    m_paperSizeComboBox->addItem("15x21");
    m_paperSizeComboBox->addItem("18x24");
    m_paperSizeComboBox->addItem("20x30");
    m_paperSizeComboBox->addItem("21x30");
    m_paperSizeComboBox->addItem("30x40");
    m_paperSizeComboBox->addItem("30x45");
    m_paperSizeComboBox->addItem("40x50");
    m_paperSizeComboBox->addItem("50x75");
    m_paperSizeComboBox->setWhatsThis(i18n("The standard photographic paper sizes in centimeters."));
    m_paperSizeLabel->setBuddy(m_paperSizeComboBox);

    m_dpiLabel = new QLabel(i18n("Print resolution (dpi):"), sizeGroupBox);
    m_dpiComboBox = new KComboBox(sizeGroupBox);
    m_dpiComboBox->addItem("75");
    m_dpiComboBox->addItem("150");
    m_dpiComboBox->addItem("300");
    m_dpiComboBox->addItem("600");
    m_dpiComboBox->addItem("1200");
    m_dpiComboBox->addItem("1400");
    m_dpiComboBox->addItem("2400");
    m_dpiComboBox->setWhatsThis(i18n("The standard print resolutions in dots per inch."));
    m_dpiLabel->setBuddy(m_dpiComboBox);

    m_stretchCheckBox = new QCheckBox(i18n("Stretch Image"), sizeGroupBox);
    m_stretchCheckBox->setWhatsThis(i18n("If this is selected, the image will be stretched "
                    "to fit the paper dimensions. Otherwise it will be centered "
                    "on the canvas and the borders will get cropped "
                    "to achieve the desired paper size."));

    sizeGroupBoxLayout->addWidget(m_paperSizeLabel,    0, 0, 1, 1);
    sizeGroupBoxLayout->addWidget(m_paperSizeComboBox, 0, 1, 1, 1);
    sizeGroupBoxLayout->addWidget(m_dpiLabel,          1, 0, 1, 1);
    sizeGroupBoxLayout->addWidget(m_dpiComboBox,       1, 1, 1, 1);
    sizeGroupBoxLayout->addWidget(m_stretchCheckBox,   2, 0, 1, -1);
    sizeGroupBox->setLayout(sizeGroupBoxLayout);

    addOptionWidget(sizeGroupBox);

    // ----------------------------------------------------

    QGroupBox *customGroupBox = new QGroupBox(i18n("Printing Custom Settings"), this);
    QGridLayout *customGroupBoxLayout = new QGridLayout(customGroupBox);

    m_customPaperWidthLabel = new QLabel(i18n("Paper width (cm):"), customGroupBox);
    m_customPaperWidthInput = new KIntNumInput(10, customGroupBox);
    m_customPaperWidthInput->setRange(1, 100);
    m_customPaperWidthInput->setSliderEnabled(true);
    m_customPaperWidthInput->setWhatsThis(i18n("The customized width of the photographic paper size "
                                     "in centimeters."));
    m_customPaperWidthLabel->setBuddy(m_customPaperWidthInput);

    m_customPaperHeightLabel = new QLabel(i18n("Paper height (cm):"), customGroupBox);
    m_customPaperHeightInput = new KIntNumInput(15, customGroupBox);
    m_customPaperHeightInput->setRange(1, 100);
    m_customPaperHeightInput->setSliderEnabled(true);
    m_customPaperHeightInput->setWhatsThis(i18n("The customized height of the photographic paper size "
                                     "in centimeters."));
    m_customPaperHeightLabel->setBuddy(m_customPaperHeightInput);

    m_customDpiLabel = new QLabel(i18n("Print resolution (dpi):"), customGroupBox);
    m_customDpiInput = new KIntNumInput(300);
    m_customDpiInput->setRange(10, 5000, 10);
    m_customDpiInput->setSliderEnabled(true);
    m_customDpiInput->setWhatsThis(i18n("The customized print resolution in dots per inch."));
    m_customDpiLabel->setBuddy(m_customDpiInput);

    customGroupBoxLayout->addWidget(m_customPaperWidthLabel,  0, 0, 1, -1);
    customGroupBoxLayout->addWidget(m_customPaperWidthInput,  1, 0, 1, -1);
    customGroupBoxLayout->addWidget(m_customPaperHeightLabel, 2, 0, 1, -1);
    customGroupBoxLayout->addWidget(m_customPaperHeightInput, 3, 0, 1, -1);
    customGroupBoxLayout->addWidget(m_customDpiLabel,         4, 0, 1, -1);
    customGroupBoxLayout->addWidget(m_customDpiInput,         5, 0, 1, -1);
    customGroupBox->setLayout(customGroupBoxLayout);

    addOptionWidget(customGroupBox);

    slotCustomSettingsEnabled(m_customSettingsCheckBox->isChecked());
}

void PrintPrepareResizeOptionsDialog::appendWidgets()
{
}

void PrintPrepareResizeOptionsDialog::slotCustomSettingsEnabled(bool enable)
{
    m_paperSizeLabel->setEnabled(!enable);
    m_paperSizeComboBox->setEnabled(!enable);
    m_dpiLabel->setEnabled(!enable);
    m_dpiComboBox->setEnabled(!enable);

    m_customPaperWidthLabel->setEnabled(enable);
    m_customPaperWidthInput->setEnabled(enable);
    m_customPaperHeightLabel->setEnabled(enable);
    m_customPaperHeightInput->setEnabled(enable);
    m_customDpiLabel->setEnabled(enable);
    m_customDpiInput->setEnabled(enable);
}

bool PrintPrepareResizeOptionsDialog::handleOk()
{
    if (m_customSettingsCheckBox->isChecked())
    {
        m_commandBuilder->setDpi(m_customDpiInput->value());
        m_commandBuilder->setPaperWidth(m_customPaperWidthInput->value() * 10);
        m_commandBuilder->setPaperHeight(m_customPaperHeightInput->value() * 10);
    }
    else
    {
        m_commandBuilder->setDpi(m_dpiComboBox->currentText().toInt());
        QString paperSize = m_paperSizeComboBox->currentText();
        m_commandBuilder->setPaperWidth(paperSize.right(paperSize.size()
                        - paperSize.indexOf('x') - 1).toInt() * 10);
        m_commandBuilder->setPaperHeight(
                paperSize.left(paperSize.indexOf('x')).toInt() * 10);
    }

    m_commandBuilder->setStretch(m_stretchCheckBox->isChecked());

    return true;
}

QString PrintPrepareResizeOptionsDialog::getWhatsThis()
{
    return i18n("<p><b>Prepare to print</b>: prepare the image for photographic printing. "
                "The user can set the print resolution and the photographic paper size. "
                "The target images will be adapted to the specified dimensions "
                "(included the background size, margin size, and background color).</p>");
}

} // namespace KIPIBatchProcessImagesPlugin
