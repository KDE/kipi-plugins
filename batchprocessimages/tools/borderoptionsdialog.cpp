/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "borderoptionsdialog.moc"

// Qt includes

#include <QColor>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

// KDE includes

#include <kcolorbutton.h>
#include <klocale.h>
#include <knuminput.h>

namespace KIPIBatchProcessImagesPlugin
{

BorderOptionsDialog::BorderOptionsDialog(QWidget *parent, int BorderType)
                   : KDialog(parent),
                   m_solidBorderWidth(0),
                   m_button_solidBorderColor(0),
                   m_lineNiepceBorderWidth(0),
                   m_button_lineNiepceBorderColor(0),
                   m_NiepceBorderWidth(0),
                   m_button_NiepceBorderColor(0),
                   m_raiseBorderWidth(0),
                   m_frameBorderWidth(0),
                   m_frameBevelBorderWidth(0),
                   m_button_frameBorderColor(0)
{
    setCaption(i18n("Border Options"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    QWidget* box       = new QWidget(this);
    QVBoxLayout *dvlay = new QVBoxLayout(box);
    dvlay->setSpacing(spacingHint());
    dvlay->setMargin(spacingHint());
    setMainWidget(box);
    QString whatsThis;

    if (BorderType == 0)
    { // Solid
        QLabel *m_label_solidBorderWidth = new QLabel(i18n("Border width:"), box);
        dvlay->addWidget(m_label_solidBorderWidth);
        m_solidBorderWidth               = new KIntNumInput(25, box);
        m_solidBorderWidth->setRange(1, 1000);
        m_solidBorderWidth->setSliderEnabled(true);
        m_solidBorderWidth->setWhatsThis(i18n("Select here the border width in pixels."));
        m_label_solidBorderWidth->setBuddy(m_solidBorderWidth);
        dvlay->addWidget(m_solidBorderWidth);

        QLabel *m_label_solidColor = new QLabel(i18n("Border color:"), box);
        dvlay->addWidget(m_label_solidColor);
        QColor solidBorderColor    = QColor(0, 0, 0);                           // Black per default.
        m_button_solidBorderColor  = new KColorButton(solidBorderColor, box);
        m_button_solidBorderColor->setWhatsThis(i18n("You can select here the border color."));
        dvlay->addWidget(m_button_solidBorderColor);
    }
    else if (BorderType == 1)
    { // Niepce
        QLabel *m_label_lineNiepceBorderWidth = new QLabel(i18n("Line border width:"), box);
        dvlay->addWidget(m_label_lineNiepceBorderWidth);
        m_lineNiepceBorderWidth               = new KIntNumInput(10, box);
        m_lineNiepceBorderWidth->setRange(1, 500);
        m_lineNiepceBorderWidth->setSliderEnabled(true);
        m_lineNiepceBorderWidth->setWhatsThis(i18n("Select here the line border width in pixels."));
        m_label_lineNiepceBorderWidth->setBuddy(m_lineNiepceBorderWidth);
        dvlay->addWidget(m_lineNiepceBorderWidth);

        QLabel *m_label_lineNiepceColor = new QLabel(i18n("Line border color:"), box);
        dvlay->addWidget(m_label_lineNiepceColor);
        QColor lineNiepceBorderColor    = QColor(0, 0, 0);                           // Black per default.
        m_button_lineNiepceBorderColor  = new KColorButton(lineNiepceBorderColor, box);
        m_button_lineNiepceBorderColor->setWhatsThis(i18n("You can select here the line border color."));
        dvlay->addWidget(m_button_lineNiepceBorderColor);

        QLabel *m_label_NiepceBorderWidth = new QLabel(i18n("Border width:"), box);
        dvlay->addWidget(m_label_NiepceBorderWidth);
        m_NiepceBorderWidth               = new KIntNumInput(100, box);
        m_NiepceBorderWidth->setRange(1, 500);
        m_NiepceBorderWidth->setSliderEnabled(true);
        m_NiepceBorderWidth->setWhatsThis(i18n("Select here the border width in pixels."));
        m_label_NiepceBorderWidth->setBuddy(m_NiepceBorderWidth);
        dvlay->addWidget(m_NiepceBorderWidth);

        QLabel *m_label_NiepceColor = new QLabel(i18n("Border color:"), box);
        dvlay->addWidget(m_label_NiepceColor);
        QColor NiepceBorderColor    = QColor(255, 255, 255);                           // White per default.
        m_button_NiepceBorderColor  = new KColorButton(NiepceBorderColor, box);
        m_button_NiepceBorderColor->setWhatsThis(i18n("You can select here the border color."));
        dvlay->addWidget(m_button_NiepceBorderColor);
    }
    else if (BorderType == 2)
    { // Raise
        QLabel *m_label_raiseBorderWidth = new QLabel(i18n("Border width:"), box);
        dvlay->addWidget(m_label_raiseBorderWidth);
        m_raiseBorderWidth               = new KIntNumInput(50, box);
        m_raiseBorderWidth->setRange(1, 500);
        m_raiseBorderWidth->setSliderEnabled(true);
        m_raiseBorderWidth->setWhatsThis(i18n("Select here the border width in pixels."));
        m_label_raiseBorderWidth->setBuddy(m_raiseBorderWidth);
        dvlay->addWidget(m_raiseBorderWidth);
    }
    else if (BorderType == 3)
    { // Frame
        QLabel *m_label_frameBorderWidth = new QLabel(i18n("Border width:"), box);
        dvlay->addWidget(m_label_frameBorderWidth);
        m_frameBorderWidth               = new KIntNumInput(25, box);
        m_frameBorderWidth->setRange(0, 500);
        m_frameBorderWidth->setSliderEnabled(true);
        m_frameBorderWidth->setWhatsThis(i18n("Select here the border width in pixels."));
        m_label_frameBorderWidth->setBuddy(m_frameBorderWidth);
        dvlay->addWidget(m_frameBorderWidth);

        QLabel *m_label_frameBevelBorderWidth = new QLabel(i18n("Bevel width:"), box);
        dvlay->addWidget(m_label_frameBevelBorderWidth);
        m_frameBevelBorderWidth = new KIntNumInput(10, box);
        m_frameBevelBorderWidth->setRange(0, 250);
        m_frameBevelBorderWidth->setSliderEnabled(true);
        m_frameBevelBorderWidth->setWhatsThis(i18n("Select here the bevel width in pixels. "
                                              "This value must be &lt;= Border width / 2"));
        m_label_frameBevelBorderWidth->setBuddy(m_frameBevelBorderWidth);
        dvlay->addWidget(m_frameBevelBorderWidth);

        QLabel *m_label_frameColor = new QLabel(i18n("Border color:"), box);
        dvlay->addWidget(m_label_frameColor);
        QColor frameBorderColor = QColor(0, 0, 0);                           // Black per default.
        m_button_frameBorderColor = new KColorButton(frameBorderColor, box);
        m_button_frameBorderColor->setWhatsThis(i18n("You can select here the border color."));
        dvlay->addWidget(m_button_frameBorderColor);

        connect(m_frameBorderWidth, SIGNAL(valueChanged(int)),
                this, SLOT(slotFrameBorderWidthChanged(int)));
    }
}

BorderOptionsDialog::~BorderOptionsDialog()
{
}

void BorderOptionsDialog::slotFrameBorderWidthChanged(int value)
{
    m_frameBevelBorderWidth->setMaximum((int)(value / 2));
}

}  // namespace KIPIBatchProcessImagesPlugin
