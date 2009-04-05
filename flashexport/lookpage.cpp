/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-21
 * Description : look settings page.
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Look
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "lookpage.h"
#include "lookpage.moc"

// Qt includes

#include <QLabel>
#include <QGroupBox>
#include <QLayout>

// KDE includes

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kdialog.h>
#include <khbox.h>
#include <klocale.h>
#include <knuminput.h>

namespace KIPIFlashExportPlugin
{

class LookPagePriv
{
public:

    LookPagePriv()
    {
        thumbnailPosition = 0;
        navDirection      = 0;
        textColor         = 0;
        backgroundColor   = 0;
        frameColor        = 0;
        frameWidth        = 0;
        stagePadding      = 0;
        thumbnailColumns  = 0;
        thumbnailRows     = 0;
    }

    KComboBox    *thumbnailPosition;
    KComboBox    *navDirection;

    KColorButton *textColor;
    KColorButton *backgroundColor;
    KColorButton *frameColor;

    KIntNumInput *frameWidth;
    KIntNumInput *stagePadding;
    KIntNumInput *thumbnailColumns;
    KIntNumInput *thumbnailRows;
};

LookPage::LookPage(QWidget* parent)
        : QWidget(parent), d(new LookPagePriv)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ------------------------------------------------------------------------

    QGroupBox *box    = new QGroupBox(i18nc("Settings for flash export navigation", "Navigation"), this);
    QVBoxLayout *vlay = new QVBoxLayout(box);

    d->thumbnailRows = new KIntNumInput(this);
    d->thumbnailRows->setRange(1, 10, 1);
    d->thumbnailRows->setValue(3);
    d->thumbnailRows->setLabel(i18n("Thumbnail &Rows:"), Qt::AlignVCenter);
    d->thumbnailRows->setWhatsThis(i18n("Number of thumbnails rows"));

    d->thumbnailColumns = new KIntNumInput(this);
    d->thumbnailColumns->setRange(1, 10, 1);
    d->thumbnailColumns->setValue(3);
    d->thumbnailColumns->setLabel(i18n("Thumbnail &Columns:"), Qt::AlignVCenter);
    d->thumbnailColumns->setWhatsThis(i18n("Number of thumbnails columns"));

    KHBox *hbox          = new KHBox;
    QLabel *label        = new QLabel(i18n("Thumbnail &Position:"), hbox);
    d->thumbnailPosition = new KComboBox(hbox);
    QString pos_right  = i18nc("thumbnail position: right",  "Right");
    QString pos_left   = i18nc("thumbnail position: left",   "Left");
    QString pos_top    = i18nc("thumbnail position: top",    "Top");
    QString pos_bottom = i18nc("thumbnail position: bottom", "Bottom");
    d->thumbnailPosition->insertItem(SimpleViewerSettingsContainer::RIGHT,  pos_right);
    d->thumbnailPosition->insertItem(SimpleViewerSettingsContainer::LEFT,   pos_left);
    d->thumbnailPosition->insertItem(SimpleViewerSettingsContainer::TOP,    pos_top);
    d->thumbnailPosition->insertItem(SimpleViewerSettingsContainer::BOTTOM, pos_bottom);
    d->thumbnailPosition->setCurrentIndex(SimpleViewerSettingsContainer::RIGHT);
    label->setBuddy(d->thumbnailPosition);

    KHBox *hbox2    = new KHBox;
    QLabel *label2  = new QLabel(i18n("&Direction of Navigation:"), hbox2);
    d->navDirection = new KComboBox(hbox2);
    d->navDirection->insertItem(SimpleViewerSettingsContainer::LEFT2RIGHT, i18n("Left to Right"));
    d->navDirection->insertItem(SimpleViewerSettingsContainer::RIGHT2LEFT, i18n("Right to Left"));
    d->navDirection->setCurrentIndex(SimpleViewerSettingsContainer::LEFT2RIGHT);
    label2->setBuddy(d->navDirection);

    vlay->setMargin(KDialog::spacingHint());
    vlay->setSpacing(KDialog::spacingHint());
    vlay->addWidget(d->thumbnailRows);
    vlay->addWidget(d->thumbnailColumns);
    vlay->addWidget(hbox);
    vlay->addWidget(hbox2);

    // ------------------------------------------------------------------------

    QGroupBox *box2    = new QGroupBox(i18n("Colors"), this);
    QVBoxLayout *vlay2 = new QVBoxLayout(box2);

    KHBox *hbox3    = new KHBox;
    QLabel *label3  = new QLabel(i18n("&Text Color:"), hbox3);
    d->textColor    = new KColorButton(hbox3);
    d->textColor->setColor(QColor("#FFFFFF"));
    d->textColor->setWhatsThis(i18n("Color of title and caption text"));
    label3->setBuddy(d->textColor);

    KHBox *hbox4       = new KHBox;
    QLabel *label4     = new QLabel(i18n("&Background Color:"), hbox4);
    d->backgroundColor = new KColorButton(hbox4);
    d->backgroundColor->setColor(QColor("#181818"));
    label4->setBuddy(d->backgroundColor);

    KHBox *hbox5    = new KHBox;
    QLabel *label5  = new QLabel(i18n("&Frame Color:"), hbox5);
    d->frameColor   = new KColorButton(QColor("#FFFFFF"), hbox5);
    d->frameColor->setColor(QColor("#FFFFFF"));
    d->frameColor->setWhatsThis(i18n("Color of image frame, viewed icon, load bars, thumbnail arrows"));
    label5->setBuddy(d->frameColor);

    vlay2->setMargin(KDialog::spacingHint());
    vlay2->setSpacing(KDialog::spacingHint());
    vlay2->addWidget(hbox3);
    vlay2->addWidget(hbox4);
    vlay2->addWidget(hbox5);

    // ------------------------------------------------------------------------

    QGroupBox *box3    = new QGroupBox(i18n("Style"), this);
    QVBoxLayout *vlay3 = new QVBoxLayout(box3);

    d->frameWidth = new KIntNumInput(this);
    d->frameWidth->setRange(0, 10, 1);
    d->frameWidth->setValue(1);
    d->frameWidth->setLabel(i18n("Frame &Width:"), Qt::AlignVCenter);
    d->frameWidth->setWhatsThis(i18n("Width of image frame in pixels."));

    d->stagePadding = new KIntNumInput(this);
    d->stagePadding->setRange(10, 100, 1);
    d->stagePadding->setValue(20);
    d->stagePadding->setLabel(i18n("Stage &Padding:"), Qt::AlignVCenter);
    d->stagePadding->setWhatsThis(i18n("Distance between image and thumbnails in pixels."));

    vlay3->setMargin(KDialog::spacingHint());
    vlay3->setSpacing(KDialog::spacingHint());
    vlay3->addWidget(d->frameWidth);
    vlay3->addWidget(d->stagePadding);

    // ------------------------------------------------------------------------

    mainLayout->addWidget(box);
    mainLayout->addWidget(box2);
    mainLayout->addWidget(box3);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->addStretch(10);
}

LookPage::~LookPage()
{
    delete d;
}

void LookPage::setSettings(const SimpleViewerSettingsContainer& settings)
{
    d->thumbnailRows->setValue(settings.thumbnailRows);
    d->thumbnailColumns->setValue(settings.thumbnailColumns);
    d->thumbnailPosition->setCurrentIndex(settings.thumbnailPosition);
    d->navDirection->setCurrentIndex(settings.navDirection);
    d->textColor->setColor(settings.textColor);
    d->backgroundColor->setColor(settings.backgroundColor);
    d->frameColor->setColor(settings.frameColor);
    d->frameWidth->setValue(settings.frameWidth);
    d->stagePadding->setValue(settings.stagePadding);
}

void LookPage::settings(SimpleViewerSettingsContainer& settings)
{
    settings.thumbnailRows     = d->thumbnailRows->value();
    settings.thumbnailColumns  = d->thumbnailColumns->value();
    settings.thumbnailPosition = (SimpleViewerSettingsContainer::ThumbPosition)d->thumbnailPosition->currentIndex();
    settings.navDirection      = (SimpleViewerSettingsContainer::NavDir)d->navDirection->currentIndex();
    settings.textColor         = d->textColor->color();
    settings.backgroundColor   = d->backgroundColor->color();
    settings.frameColor        = d->frameColor->color();
    settings.frameWidth        = d->frameWidth->value();
    settings.stagePadding      = d->stagePadding->value();
}

}  // namespace KIPIFlashExportPlugin
