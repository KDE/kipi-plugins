/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "CanvasSizeDialog.moc"

// KDE
#include <kcombobox.h>
#include <klocalizedstring.h>
#include <kpushbutton.h>

// Qt
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QButtonGroup>
#include <QDebug>

#include "CanvasSize.h"
#include "global.h"

using namespace KIPIPhotoLayoutsEditor;

class KIPIPhotoLayoutsEditor::CanvasSizeDialogPrivate
{
    public:
        CanvasSizeDialogPrivate() :
            sizeWidget(0),
            advancedWidget(0),
            paperSize(0),
            xSize(0),
            ySize(0),
            orientationGroup(0),
            verticalButton(0),
            sizeLabel(0),
            horizontalButton(0),
            sizeUnitsWidget(0),
            xResolution(0),
            yResolution(0),
            resolutionUnitsWidget(0)
        {
            paperSizes.insert("A0 (841 x 1189 mm)",QPrinter::A0);
            paperSizes.insert("A1 (594 x 841 mm)",QPrinter::A1);
            paperSizes.insert("A2 (420 x 594 mm)",QPrinter::A2);
            paperSizes.insert("A3 (297 x 420 mm)",QPrinter::A3);
            paperSizes.insert("A4 (210 x 297 mm, 8.26 x 11.69 inches)",QPrinter::A4);
            paperSizes.insert("A5 (148 x 210 mm)",QPrinter::A5);
            paperSizes.insert("A6 (105 x 148 mm)",QPrinter::A6);
            paperSizes.insert("A7 (74 x 105 mm)",QPrinter::A7);
            paperSizes.insert("A8 (52 x 74 mm)",QPrinter::A8);
            paperSizes.insert("A9 (37 x 52 mm)",QPrinter::A9);
            paperSizes.insert("B0 (1000 x 1414 mm)",QPrinter::B0);
            paperSizes.insert("B1 (707 x 1000 mm)",QPrinter::B1);
            paperSizes.insert("B2 (500 x 707 mm)",QPrinter::B2);
            paperSizes.insert("B3 (353 x 500 mm)",QPrinter::B3);
            paperSizes.insert("B4 (250 x 353 mm)",QPrinter::B4);
            paperSizes.insert("B5 (176 x 250 mm, 6.93 x 9.84 inches)",QPrinter::B5);
            paperSizes.insert("B6 (125 x 176 mm)",QPrinter::B6);
            paperSizes.insert("B7 (88 x 125 mm)",QPrinter::B7);
            paperSizes.insert("B8 (62 x 88 mm)",QPrinter::B8);
            paperSizes.insert("B9 (33 x 62 mm)",QPrinter::B9);
            paperSizes.insert("B10 (31 x 44 mm)",QPrinter::B10);
            paperSizes.insert("C5E (163 x 229 mm)",QPrinter::C5E);
            paperSizes.insert("U.S. Common 10 Envelope (105 x 241 mm)",QPrinter::Comm10E);
            paperSizes.insert("DLE (110 x 220 mm)",QPrinter::DLE);
            paperSizes.insert("Executive (7.5 x 10 inches, 190.5 x 254 mm)",QPrinter::Executive);
            paperSizes.insert("Folio (210 x 330 mm)",QPrinter::Folio);
            paperSizes.insert("Ledger (431.8 x 279.4 mm)",QPrinter::Ledger);
            paperSizes.insert("Legal (8.5 x 14 inches, 215.9 x 355.6 mm)",QPrinter::Legal);
            paperSizes.insert("Letter (8.5 x 11 inches, 215.9 x 279.4 mm)",QPrinter::Letter);
            paperSizes.insert("Tabloid (279.4 x 431.8 mm)",QPrinter::Tabloid);
        }

        ~CanvasSizeDialogPrivate()
        {
        }

        void swapSizes();
        void updateSizeLabel();
        void setPaper(QPrinter::PageSize pageSize);

        QWidget *   sizeWidget;
        QWidget *   advancedWidget;
        KComboBox * paperSize;
        QDoubleSpinBox *  xSize;
        QDoubleSpinBox *  ySize;
        QButtonGroup *  orientationGroup;
        KPushButton *   verticalButton;
        QLabel *    sizeLabel;
        KPushButton *   horizontalButton;
        KComboBox * sizeUnitsWidget;
        QDoubleSpinBox *  xResolution;
        QDoubleSpinBox *  yResolution;
        KComboBox * resolutionUnitsWidget;

        static int WIDTH;
        static int HEIGHT;
        static QString currentSizeUnit;

        static qreal WIDTH_RES;
        static qreal HEIGHT_RES;
        static QString currentResolutionUnit;

        QMap<QString,QPrinter::PaperSize> paperSizes;
};

int CanvasSizeDialogPrivate::WIDTH = 800;
int CanvasSizeDialogPrivate::HEIGHT = 800;
QString CanvasSizeDialogPrivate::currentSizeUnit = "";
qreal CanvasSizeDialogPrivate::WIDTH_RES = 72;
qreal CanvasSizeDialogPrivate::HEIGHT_RES = 72;
QString CanvasSizeDialogPrivate::currentResolutionUnit = "";

void CanvasSizeDialogPrivate::swapSizes()
{
    // swap dimensions
    qreal temp = WIDTH;
    WIDTH = HEIGHT;
    HEIGHT = temp;
    temp = xSize->value();
    xSize->setValue( ySize->value() );
    ySize->setValue( temp );

    // Swap resolutions
    temp = WIDTH_RES;
    WIDTH_RES = HEIGHT_RES;
    HEIGHT_RES = temp;
    temp = xResolution->value();
    xResolution->setValue(yResolution->value());
    yResolution->setValue(temp);
}

void CanvasSizeDialogPrivate::updateSizeLabel()
{
    sizeLabel->setText(QString::number(WIDTH).append(" x ").append(QString::number(HEIGHT).append(" px")));
}

void CanvasSizeDialogPrivate::setPaper(QPrinter::PageSize pageSize)
{
    QSizeF result;
    switch (pageSize)
    {
    case QPrinter::A0:
        result = QSizeF(841,1189);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::A1:
        result = QSizeF(594,841);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::A2:
        result = QSizeF(420,594);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::A3:
        result = QSizeF(297,420);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::A4:
        result = QSizeF(210,297);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::A5:
        result = QSizeF(148,210);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::A6:
        result = QSizeF(105,148);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::A7:
        result = QSizeF(74,105);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::A8:
        result = QSizeF(52,74);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::A9:
        result = QSizeF(37,52);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B0:
        result = QSizeF(1030,1456);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B1:
        result = QSizeF(728,1030);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B10:
        result = QSizeF(32,45);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B2:
        result = QSizeF(515,728);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B3:
        result = QSizeF(364,515);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B4:
        result = QSizeF(257,364);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B5:
        result = QSizeF(182,257);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B6:
        result = QSizeF(128,182);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B7:
        result = QSizeF(91,128);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B8:
        result = QSizeF(64,91);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::B9:
        result = QSizeF(45,64);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::C5E:
        result = QSizeF(163,229);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::Comm10E:
        result = QSizeF(105,241);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::DLE:
        result = QSizeF(110,220);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::Executive:
        result = QSizeF(7.5,10);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Inches) );
        break;
    case QPrinter::Folio:
        result = QSizeF(210,330);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::Ledger:
        result = QSizeF(432,279);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::Legal:
        result = QSizeF(8.5,14);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Inches) );
        break;
    case QPrinter::Letter:
        result = QSizeF(8.5,11);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Inches) );
        break;
    case QPrinter::Tabloid:
        result = QSizeF(279,432);
        sizeUnitsWidget->setCurrentItem( CanvasSize::sizeUnitName(CanvasSize::Milimeters) );
        break;
    case QPrinter::Custom:
        return;
    }

    xSize->setValue(result.width());
    ySize->setValue(result.height());
}

CanvasSizeDialog::CanvasSizeDialog(QWidget * parent) :
    KDialog(parent),
    d(new CanvasSizeDialogPrivate)
{
    setupDialog(QSize(d->WIDTH, d->HEIGHT),
                d->currentSizeUnit,
                QSize(d->WIDTH_RES, d->HEIGHT_RES),
                d->currentResolutionUnit);
}

CanvasSizeDialog::CanvasSizeDialog(const CanvasSize & canvasSize, QWidget * parent) :
    KDialog(parent),
    d(new CanvasSizeDialogPrivate)
{
    setupDialog(canvasSize.size(),
                CanvasSize::sizeUnitName(canvasSize.sizeUnit()),
                canvasSize.resolution(),
                CanvasSize::resolutionUnitName(canvasSize.resolutionUnit()));
}

CanvasSizeDialog::~CanvasSizeDialog()
{
    delete d;
}

void CanvasSizeDialog::setupDialog(const QSizeF & size, const QString & sizeUnits, const QSizeF & resolution, const QString & resolutionUnits)
{
    this->setCaption(i18n("Canvas size"));

    QString tempSizeUnits = sizeUnits;
    QString tempResolutionUnits = resolutionUnits;
    if (tempSizeUnits.isEmpty() ||
            CanvasSize::sizeUnit(tempSizeUnits) == CanvasSize::UnknownSizeUnit)
        tempSizeUnits = CanvasSize::sizeUnitName(CanvasSize::Pixels);;
    if (tempResolutionUnits.isEmpty() ||
            CanvasSize::resolutionUnit(tempResolutionUnits) == CanvasSize::UnknownResolutionUnit)
        tempResolutionUnits = CanvasSize::resolutionUnitName(CanvasSize::PixelsPerInch);;

    QWidget * main = new QWidget(this);
    setMainWidget(main);
    QVBoxLayout * vLayout = new  QVBoxLayout(main);
    main->setLayout(vLayout);


    /// ----------------------- TEMPLATES PART -----------------------
    QGridLayout * gridLayout = new QGridLayout();
    d->paperSize = new KComboBox(main);
    d->paperSize->addItem( i18n("Custom") );
    d->paperSize->insertSeparator(1);
    d->paperSize->addItems(d->paperSizes.keys());
    d->paperSize->setCurrentIndex(0);
    gridLayout->addWidget(new QLabel( i18n("Template"), main),0,0);
    gridLayout->addWidget(d->paperSize,0,1);
    vLayout->addLayout(gridLayout);

    /// ----------------------- CANVAS SIZE PART -----------------------
    d->sizeWidget = new QGroupBox(i18n("Canvas size"), main);
    vLayout->addWidget(d->sizeWidget);

    gridLayout = new QGridLayout(d->sizeWidget);
    d->sizeWidget->setLayout(gridLayout);

    // Width widget
    d->xSize = new QDoubleSpinBox(d->sizeWidget);
    d->xSize->setMinimum(0.00001);
    d->xSize->setMaximum(999999);
    d->xSize->setValue(size.width());
    d->WIDTH = CanvasSize::toPixels(size.width(),
                                    resolution.width(),
                                    CanvasSize::sizeUnit(tempSizeUnits),
                                    CanvasSize::resolutionUnit(tempResolutionUnits));
    gridLayout->addWidget(new QLabel(i18n("Width"), d->sizeWidget),0,0);
    gridLayout->addWidget(d->xSize,0,1);

    // Height widget
    d->ySize = new QDoubleSpinBox(d->sizeWidget);
    d->ySize->setMinimum(0.00001);
    d->ySize->setMaximum(999999);
    d->ySize->setValue(size.height());
    d->HEIGHT = CanvasSize::toPixels(size.height(),
                                     resolution.height(),
                                     CanvasSize::sizeUnit(tempSizeUnits),
                                     CanvasSize::resolutionUnit(tempResolutionUnits));
    gridLayout->addWidget(new QLabel(i18n("Height"), d->sizeWidget),1,0);
    gridLayout->addWidget(d->ySize,1,1);

    // Unit widget
    d->sizeUnitsWidget = new KComboBox(d->sizeWidget);
    d->sizeUnitsWidget->addItems(CanvasSize::sizeUnitsNames());
    d->sizeUnitsWidget->setCurrentItem(tempSizeUnits);
    d->currentSizeUnit = tempSizeUnits;
    gridLayout->addWidget(d->sizeUnitsWidget,1,2);

    // Orientation buttons
    d->horizontalButton = new KPushButton(KIcon(":horizontal_orientation.png"),"",d->sizeWidget);
    d->horizontalButton->setCheckable(true);
    d->horizontalButton->setFlat(true);
    d->horizontalButton->setIconSize(QSize(24,24));
    d->verticalButton = new KPushButton(KIcon(":vertical_orientation.png"),"",d->sizeWidget);
    d->verticalButton->setCheckable(true);
    d->verticalButton->setFlat(true);
    d->verticalButton->setIconSize(QSize(24,24));
    QHBoxLayout * hLayout = new QHBoxLayout();
    hLayout->addWidget(d->horizontalButton);
    hLayout->addWidget(d->verticalButton);
    gridLayout->addWidget(new QLabel(i18n("Orientation"), d->sizeWidget),2,0);
    gridLayout->addLayout(hLayout,2,1);
    d->sizeLabel = new QLabel(d->sizeWidget);
    gridLayout->addWidget(d->sizeLabel,2, 2);

    /// ----------------------- ADVANCED PART -----------------------
    d->advancedWidget = new QGroupBox(i18n("Advanced"), main);
    vLayout->addWidget(d->advancedWidget);
    gridLayout = new QGridLayout(d->advancedWidget);
    d->advancedWidget->setLayout(gridLayout);

    // x resolution widget
    d->xResolution = new QDoubleSpinBox(d->advancedWidget);
    d->xResolution->setMinimum(0);
    d->xResolution->setMaximum(999999);
    d->xResolution->setValue(resolution.width());
    d->xResolution->setDecimals(3);
    d->WIDTH_RES = resolution.width() * CanvasSize::resolutionUnitFactor(tempResolutionUnits);
    gridLayout->addWidget(new QLabel(i18n("Resolution X"), d->advancedWidget),0,0);
    gridLayout->addWidget(d->xResolution,0,1);

    // y resolution widget
    d->yResolution = new QDoubleSpinBox(d->advancedWidget);
    d->yResolution->setMinimum(0);
    d->yResolution->setMaximum(999999);
    d->yResolution->setValue(resolution.height());
    d->yResolution->setDecimals(3);
    d->HEIGHT_RES = resolution.height() * CanvasSize::resolutionUnitFactor(tempResolutionUnits);
    gridLayout->addWidget(new QLabel(i18n("Resolution Y"), d->advancedWidget),1,0);
    gridLayout->addWidget(d->yResolution,1,1);

    // Unit widget
    d->resolutionUnitsWidget = new KComboBox(d->sizeWidget);
    d->resolutionUnitsWidget->addItems(CanvasSize::resolutionUnitsNames());
    d->resolutionUnitsWidget->setCurrentItem(tempResolutionUnits);
    d->currentResolutionUnit = tempResolutionUnits;
    gridLayout->addWidget(d->resolutionUnitsWidget,1,2);

    this->prepareSignalsConnections();

    d->updateSizeLabel();
}

void CanvasSizeDialog::prepareSignalsConnections()
{
    connect(d->paperSize, SIGNAL(activated(QString)), this, SLOT(recalculatePaperSize(QString)));
    connect(d->xSize,SIGNAL(valueChanged(double)),this,SLOT(widthChanged(double)));
    connect(d->ySize,SIGNAL(valueChanged(double)),this,SLOT(heightChanged(double)));
    connect(d->sizeUnitsWidget, SIGNAL(activated(QString)), this, SLOT(sizeUnitsChanged(QString)));
    connect(d->horizontalButton, SIGNAL(toggled(bool)), this, SLOT(setHorizontal(bool)));
    connect(d->verticalButton, SIGNAL(toggled(bool)), this, SLOT(setVertical(bool)));
    connect(d->xResolution, SIGNAL(valueChanged(double)), this, SLOT(xResolutionChanged(double)));
    connect(d->yResolution, SIGNAL(valueChanged(double)), this, SLOT(yResolutionChanged(double)));
    connect(d->resolutionUnitsWidget, SIGNAL(currentIndexChanged(QString)), this, SLOT(resolutionUnitsChanged(QString)));
}

CanvasSize CanvasSizeDialog::canvasSize() const
{
    CanvasSize result(QSizeF(d->xSize->value(), d->ySize->value()),
                      CanvasSize::sizeUnit(d->sizeUnitsWidget->currentText()),
                      QSizeF(d->xResolution->value(), d->yResolution->value()),
                      CanvasSize::resolutionUnit(d->resolutionUnitsWidget->currentText()));
    return result;
}

void CanvasSizeDialog::recalculatePaperSize(const QString & paperSize)
{
    d->setPaper( d->paperSizes.value(paperSize, QPrinter::Custom) );
    d->updateSizeLabel();
    sizeUnitsChanged(d->sizeUnitsWidget->currentText());
}

void CanvasSizeDialog::sizeUnitsChanged(const QString & unitName)
{
    d->currentSizeUnit = unitName;
    CanvasSize::SizeUnits sizeUnit = CanvasSize::sizeUnit(unitName);
    if (sizeUnit == CanvasSize::Pixels)
    {
        d->xSize->setValue(d->WIDTH);
        d->ySize->setValue(d->HEIGHT);
        d->xSize->setDecimals(0);
        d->ySize->setDecimals(0);
        return;
    }
    d->xSize->setDecimals(5);
    d->ySize->setDecimals(5);
    CanvasSize::ResolutionUnits resolutionUnit = CanvasSize::resolutionUnit(d->resolutionUnitsWidget->currentText());
    qreal WIDTH = CanvasSize::fromPixels(d->WIDTH,
                                         d->xResolution->value(),
                                         sizeUnit,
                                         resolutionUnit);
    qreal HEIGHT = CanvasSize::fromPixels(d->HEIGHT,
                                          d->yResolution->value(),
                                          sizeUnit,
                                          resolutionUnit);
    d->xSize->setValue(WIDTH);
    d->ySize->setValue(HEIGHT);
}

void CanvasSizeDialog::resolutionUnitsChanged(const QString & unitName)
{
    d->currentResolutionUnit = unitName;
    CanvasSize::ResolutionUnits unit = CanvasSize::resolutionUnit(unitName);
    if (unit == CanvasSize::PixelsPerInch)
    {
        d->xResolution->setValue(d->WIDTH_RES);
        d->yResolution->setValue(d->HEIGHT_RES);
        return;
    }
    qreal factor = CanvasSize::resolutionUnitFactor(unit);
    d->xResolution->setValue(d->WIDTH_RES / factor);
    d->yResolution->setValue(d->HEIGHT_RES / factor);
}

void CanvasSizeDialog::setHorizontal(bool isHorizontal)
{
    if (isHorizontal)
    {
        if (d->WIDTH < d->HEIGHT)
        {
            d->swapSizes();
            d->updateSizeLabel();
        }
    }
    d->horizontalButton->setChecked(d->WIDTH > d->HEIGHT);
    d->verticalButton->setChecked(d->WIDTH < d->HEIGHT);
}

void CanvasSizeDialog::setVertical(bool isVertical)
{
    if (isVertical)
    {
        if (d->HEIGHT < d->WIDTH)
        {
            d->swapSizes();
            d->updateSizeLabel();
        }
    }
    d->horizontalButton->setChecked(d->WIDTH > d->HEIGHT);
    d->verticalButton->setChecked(d->WIDTH < d->HEIGHT);
}

void CanvasSizeDialog::widthChanged(double width)
{
    width = CanvasSize::toPixels(width,
                                 d->xResolution->value(),
                                 CanvasSize::sizeUnit(d->sizeUnitsWidget->currentText()),
                                 CanvasSize::resolutionUnit(d->resolutionUnitsWidget->currentText()));
    d->WIDTH = width;
    d->horizontalButton->setChecked(d->WIDTH > d->HEIGHT);
    d->verticalButton->setChecked(d->WIDTH < d->HEIGHT);
    d->updateSizeLabel();
}

void CanvasSizeDialog::heightChanged(double height)
{
    height = CanvasSize::toPixels(height,
                                  d->yResolution->value(),
                                  CanvasSize::sizeUnit(d->sizeUnitsWidget->currentText()),
                                  CanvasSize::resolutionUnit(d->resolutionUnitsWidget->currentText()));
    d->HEIGHT = height;
    d->horizontalButton->setChecked(d->WIDTH > d->HEIGHT);
    d->verticalButton->setChecked(d->WIDTH < d->HEIGHT);
    d->updateSizeLabel();
}

void CanvasSizeDialog::xResolutionChanged(double xResolution)
{
    CanvasSize::SizeUnits sizeUnit = CanvasSize::sizeUnit(d->sizeUnitsWidget->currentText());
    if (sizeUnit == CanvasSize::Pixels)
        return;
    CanvasSize::ResolutionUnits resolutionUnit = CanvasSize::resolutionUnit(d->resolutionUnitsWidget->currentText());
    qreal resolutionFactor = CanvasSize::resolutionUnitFactor(resolutionUnit);
    int width = CanvasSize::toPixels(d->xSize->value(),
                                     xResolution,
                                     CanvasSize::sizeUnit(d->sizeUnitsWidget->currentText()),
                                     CanvasSize::resolutionUnit(d->resolutionUnitsWidget->currentText()));
    d->WIDTH = width;
    d->WIDTH_RES = xResolution * resolutionFactor;
    d->updateSizeLabel();
}

void CanvasSizeDialog::yResolutionChanged(double yResolution)
{
    CanvasSize::SizeUnits sizeUnit = CanvasSize::sizeUnit(d->sizeUnitsWidget->currentText());
    if (sizeUnit == CanvasSize::Pixels)
        return;
    CanvasSize::ResolutionUnits resolutionUnit = CanvasSize::resolutionUnit(d->resolutionUnitsWidget->currentText());
    qreal resolutionFactor = CanvasSize::resolutionUnitFactor(resolutionUnit);
    int height = CanvasSize::toPixels(d->ySize->value(),
                                      yResolution,
                                      CanvasSize::sizeUnit(d->sizeUnitsWidget->currentText()),
                                      CanvasSize::resolutionUnit(d->resolutionUnitsWidget->currentText()));
    d->HEIGHT = height;
    d->HEIGHT_RES = yResolution * resolutionFactor;
    d->updateSizeLabel();
}
