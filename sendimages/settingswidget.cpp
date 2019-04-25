/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2006-10-18
 * Description : e-mail settings page.
 *
 * Copyright (C) 2006-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "settingswidget.h"

// Qt includes

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QSpinBox>
#include <QComboBox>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

namespace KIPISendimagesPlugin
{

class SettingsWidget::Private
{
public:

    Private()
    {
        labelMailAgent        = nullptr;
        mailAgentName         = nullptr;
        imagesResize          = nullptr;
        addComments           = nullptr;
        changeImagesProp      = nullptr;
        imageCompression      = nullptr;
        labelImagesResize     = nullptr;
        imagesFormat          = nullptr;
        labelImagesFormat     = nullptr;
        attachmentlimit       = nullptr;
        labelAttachmentLimit  = nullptr;
        labelImageCompression = nullptr;
    }

    QLabel*    labelMailAgent;
    QLabel*    labelImagesResize;
    QLabel*    labelImagesFormat;
    QLabel*    labelAttachmentLimit;
    QLabel*    labelImageCompression;

    QComboBox* mailAgentName;
    QComboBox* imagesResize;
    QComboBox* imagesFormat;

    QCheckBox* changeImagesProp;
    QCheckBox* addComments;

    QSpinBox*  imageCompression;
    QSpinBox*  attachmentlimit;
};

SettingsWidget::SettingsWidget(QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QGridLayout* const grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->labelMailAgent = new QLabel(i18n("Mail program:"), this);

    d->mailAgentName = new QComboBox(this);
    d->mailAgentName->insertItem(EmailSettings::DEFAULT,       i18nc("default desktop mail agent", "Default"));
#ifdef Q_OS_LINUX
    d->mailAgentName->insertItem(EmailSettings::BALSA,         QLatin1String("Balsa"));
    d->mailAgentName->insertItem(EmailSettings::CLAWSMAIL,     QLatin1String("Claws Mail"));
    d->mailAgentName->insertItem(EmailSettings::EVOLUTION,     QLatin1String("Evolution"));
    d->mailAgentName->insertItem(EmailSettings::KMAIL,         QLatin1String("KMail"));
    d->mailAgentName->insertItem(EmailSettings::NETSCAPE,      QLatin1String("Netscape"));
    d->mailAgentName->insertItem(EmailSettings::SYLPHEED,      QLatin1String("Sylpheed"));
    d->mailAgentName->insertItem(EmailSettings::SYLPHEEDCLAWS, QLatin1String("Sylpheed-Claws"));
    d->mailAgentName->insertItem(EmailSettings::THUNDERBIRD,   QLatin1String("Thunderbird"));
#endif
    d->mailAgentName->setCurrentIndex(EmailSettings::DEFAULT);
    d->mailAgentName->setWhatsThis(i18n("Select your preferred external email program here. "
                                        "<b>Default</b> is the current email program set in desktop "
                                        "system settings."));

    //---------------------------------------------

    d->addComments = new QCheckBox(i18n("Attach a file with image properties from %1",
                                   QApplication::applicationName()),
                                   this);
    d->addComments->setWhatsThis(i18n("If you enable this option, all image properties set by %1 "
                                      "as Comments, Rating, or Tags, will be added as an attached file.",
                                      QApplication::applicationName()));

    // --------------------------------------------------------

    d->attachmentlimit = new QSpinBox(this);
    d->attachmentlimit->setRange(1, 50);
    d->attachmentlimit->setSingleStep(1);
    d->attachmentlimit->setValue(17);
    d->attachmentlimit->setSuffix(i18n(" MB"));

    d->labelAttachmentLimit = new QLabel(i18n("Maximum email size limit:"), this);
    d->labelAttachmentLimit->setBuddy(d->attachmentlimit);

    //---------------------------------------------

    d->changeImagesProp  = new QCheckBox(i18n("Adjust image properties"), this);
    d->changeImagesProp->setChecked(true);
    d->changeImagesProp->setWhatsThis(i18n("If you enable this option, "
                                           "all images to be sent can be resized and recompressed."));

    QGroupBox* const groupBox = new QGroupBox(i18n("Image Properties"), this);
    QGridLayout* const grid2  = new QGridLayout(groupBox);

    d->imagesResize = new QComboBox(groupBox);
    d->imagesResize->insertItem(EmailSettings::VERYSMALL, i18n("Very Small (320 pixels)"));
    d->imagesResize->insertItem(EmailSettings::SMALL,     i18n("Small (640 pixels)"));
    d->imagesResize->insertItem(EmailSettings::MEDIUM,    i18n("Medium (800 pixels)"));
    d->imagesResize->insertItem(EmailSettings::BIG,       i18n("Big (1024 pixels)"));
    d->imagesResize->insertItem(EmailSettings::VERYBIG,   i18n("Very Big (1280 pixels)"));
    d->imagesResize->insertItem(EmailSettings::LARGE,     i18n("Large - for printing (1600 pixels)"));
    d->imagesResize->insertItem(EmailSettings::FULLHD,    i18n("Full HD (1920 pixels)"));
    d->imagesResize->insertItem(EmailSettings::ULTRAHD,   i18n("Ultra HD (3840 pixels)"));
    d->imagesResize->setCurrentIndex(EmailSettings::MEDIUM);
    QString whatsThis = i18n("<p>Select the size of the images that are to be sent:</p>"
                             "<p><b>%1</b>: use this if you have a very slow Internet "
                             "connection or the target mailbox size is very limited.</p>"
                             "<p><b>%2</b>: use this if you have a slow Internet connection "
                             "and the target mailbox size is limited.</p>"
                             "<p><b>%3</b>: this is the default value for medium Internet connection "
                             "speed and target mailbox size.</p>"
                             "<p><b>%4</b>: use this if you have a high-speed Internet connection "
                             "and the target mailbox size is not limited.</p>"
                             "<p><b>%5</b>: use this if you have no size or speed restrictions.</p>"
                             "<p><b>%6</b>: use this only for printing purposes.</p>"
                             "<p><b>%7</b>: use this for optimal viewing on Full HD displays. "
                             "Only if you have a high-speed Internet connection and the "
                             "target mailbox size is not limited.</p>"
                             "<p><b>%8</b>: use this for optimal viewing on Ultra HD displays. "
                             "Only if you have a high-speed Internet connection and the "
                             "target mailbox size is not limited.</p>",
                             i18n("Very Small (320 pixels)"),
                             i18n("Small (640 pixels)"),
                             i18n("Medium (800 pixels)"),
                             i18n("Big (1024 pixels)"),
                             i18n("Very Big (1280 pixels)"),
                             i18n("Large - for printing (1600 pixels)"),
                             i18n("Full HD (1920 pixels)"),
                             i18n("Ultra HD (3840 pixels)"));
    d->imagesResize->setWhatsThis(whatsThis);

    d->labelImagesResize = new QLabel( i18n("Image size:"), groupBox);
    d->labelImagesResize->setBuddy(d->imagesResize);

    //---------------------------------------------

    d->imagesFormat = new QComboBox(groupBox);
    d->imagesFormat->insertItem(EmailSettings::JPEG, QLatin1String("JPEG"));
    d->imagesFormat->insertItem(EmailSettings::PNG, QLatin1String("PNG"));
    d->imagesFormat->setCurrentIndex(EmailSettings::JPEG);
    whatsThis = i18n("<p>Select the file format of the image files to be sent.</p>");
    whatsThis = whatsThis + i18n("<p><b>JPEG</b>: The Joint Photographic Experts Group file format "
                "is a good web file format but it uses lossy compression.</p>"
                "<p><b>PNG</b>: the Portable Network Graphics format is an extensible file format for "
                "the lossless, portable, well-compressed storage of raster images. PNG provides a "
                "patent-free replacement for GIF and can also replace many common uses of TIFF. "
                "PNG is designed to work well in online viewing applications, such as the World Wide Web, "
                "so it is fully streamable with a progressive display option. Also, PNG can store gamma "
                "and chromaticity data for improved color-matching on heterogeneous platforms.</p>");
    d->imagesFormat->setWhatsThis(whatsThis);

    d->labelImagesFormat = new QLabel(i18n("Image format:"), groupBox);
    d->labelImagesFormat->setBuddy(d->imagesFormat);

    // --------------------------------------------------------

    d->imageCompression = new QSpinBox(groupBox);
    d->imageCompression->setRange(1, 100);
    d->imageCompression->setSingleStep(1);
    d->imageCompression->setValue(75);
    whatsThis = i18n("<p>The new compression value of JPEG images to be sent:</p>");
    whatsThis = whatsThis + i18n("<p><b>1</b>: very high compression<br/>"
                                 "<b>25</b>: high compression<br/>"
                                 "<b>50</b>: medium compression<br/>"
                                 "<b>75</b>: low compression (default value)<br/>"
                                 "<b>100</b>: no compression</p>");

    d->imageCompression->setWhatsThis(whatsThis);

    d->labelImageCompression = new QLabel(i18n("Image quality:"), this);
    d->labelImageCompression->setBuddy(d->imageCompression);

    //---------------------------------------------
    grid2->addWidget(d->labelImagesResize,     0, 0, 1, 1);
    grid2->addWidget(d->imagesResize,          0, 1, 1, 2);
    grid2->addWidget(d->labelImagesFormat,     1, 0, 1, 1);
    grid2->addWidget(d->imagesFormat,          1, 1, 1, 2);
    grid2->addWidget(d->labelImageCompression, 2, 0, 1, 1);
    grid2->addWidget(d->imageCompression,      2, 1, 1, 2);
    grid2->setRowStretch(4, 10);
    grid2->setColumnStretch(2, 10);
    grid2->setContentsMargins(spacing, spacing, spacing, spacing);
    grid2->setSpacing(spacing);
    grid2->setAlignment(Qt::AlignTop);

    // --------------------------------------------------------

    grid->addWidget(d->labelMailAgent,      0, 0, 1, 1);
    grid->addWidget(d->mailAgentName,       0, 1, 1, 2);
    grid->addWidget(d->labelAttachmentLimit,1, 0, 1, 1);
    grid->addWidget(d->attachmentlimit,     1, 1, 1, 4);
    grid->addWidget(d->addComments,         2, 0, 1, 4);
    grid->addWidget(d->changeImagesProp,    3, 0, 1, 4);
    grid->addWidget(groupBox,               4, 0, 1, 4);
    grid->setRowStretch(5, 10);
    grid->setColumnStretch(3, 10);
    grid->setContentsMargins(QMargins());
    grid->setSpacing(spacing);

    //---------------------------------------------

    connect(d->imagesFormat, SIGNAL(activated(int)),
            this, SLOT(slotImagesFormatChanged(int)));

    connect(d->changeImagesProp, SIGNAL(toggled(bool)),
            groupBox, SLOT(setEnabled(bool)));
}

SettingsWidget::~SettingsWidget()
{
    delete d;
}

void SettingsWidget::slotImagesFormatChanged(int i)
{
    if ( i == EmailSettings::JPEG )
        d->imageCompression->setEnabled(true);
    else
        d->imageCompression->setEnabled(false);
}

void SettingsWidget::setEmailSettings(const EmailSettings& settings)
{
    d->mailAgentName->setCurrentIndex((int)settings.emailProgram);
    d->imagesResize->setCurrentIndex((int)settings.imageSize);
    d->imagesFormat->setCurrentIndex((int)settings.imageFormat);

    d->changeImagesProp->setChecked(settings.imagesChangeProp);
    d->addComments->setChecked(settings.addCommentsAndTags);

    d->imageCompression->setValue(settings.imageCompression);
    d->attachmentlimit->setValue(settings.attachmentLimitInMbytes);

    slotImagesFormatChanged(d->imagesFormat->currentIndex());
}

EmailSettings SettingsWidget::emailSettings() const
{
    EmailSettings settings;
    settings.emailProgram            = EmailSettings::EmailClient(d->mailAgentName->currentIndex());
    settings.imageSize               = EmailSettings::ImageSize(d->imagesResize->currentIndex());
    settings.imageFormat             = EmailSettings::ImageFormat(d->imagesFormat->currentIndex());

    settings.imagesChangeProp        = d->changeImagesProp->isChecked();
    settings.addCommentsAndTags      = d->addComments->isChecked();

    settings.imageCompression        = d->imageCompression->value();
    settings.attachmentLimitInMbytes = d->attachmentlimit->value();

    return settings;
}

}  // namespace KIPISendimagesPlugin
