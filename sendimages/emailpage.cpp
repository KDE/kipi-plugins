/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : e-mail settings page.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "emailpage.h"
#include "emailpage.moc"

// Qt includes.

#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>
#include <kiconloader.h>

namespace KIPISendimagesPlugin
{

class EmailPagePriv
{
public:

    EmailPagePriv()
    {
        labelMailAgent          = 0;
        mailAgentName           = 0;
        imagesResize            = 0;
        addComments             = 0;
        changeImagesProp        = 0;
        imageCompression        = 0;
        labelImagesResize       = 0;
        imagesFormat            = 0;
        labelImagesFormat       = 0;
        attachmentlimit         = 0;
    }
    
    QLabel        *labelMailAgent;
    QLabel        *labelImagesResize;
    QLabel        *labelImagesFormat;

    QComboBox     *mailAgentName;
    QComboBox     *imagesResize;
    QComboBox     *imagesFormat;

    QCheckBox     *changeImagesProp;
    QCheckBox     *addComments;

    KIntNumInput  *imageCompression;
    KIntNumInput  *attachmentlimit;
};

EmailPage::EmailPage(QWidget* parent)
         : QWidget(parent)
{
    d = new EmailPagePriv;

    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->labelMailAgent = new QLabel(i18n("Mail program:"), this);

    d->mailAgentName = new QComboBox(this);
    d->mailAgentName->insertItem(EmailSettingsContainer::DEFAULT,       i18n("Default"));
    d->mailAgentName->insertItem(EmailSettingsContainer::BALSA,         "Balsa");
    d->mailAgentName->insertItem(EmailSettingsContainer::CLAWSMAIL,     "Claws Mail");
    d->mailAgentName->insertItem(EmailSettingsContainer::EVOLUTION,     "Evolution");
    d->mailAgentName->insertItem(EmailSettingsContainer::GMAILAGENT,    "Gmail-Agent");
    d->mailAgentName->insertItem(EmailSettingsContainer::KMAIL,         "KMail");
    d->mailAgentName->insertItem(EmailSettingsContainer::MOZILLA,       "Mozilla");
    d->mailAgentName->insertItem(EmailSettingsContainer::NETSCAPE,      "Netscape");
    d->mailAgentName->insertItem(EmailSettingsContainer::SYLPHEED,      "Sylpheed");
    d->mailAgentName->insertItem(EmailSettingsContainer::SYLPHEEDCLAWS, "Sylpheed-Claws");
    d->mailAgentName->insertItem(EmailSettingsContainer::THUNDERBIRD,   "Thunderbird");
    d->mailAgentName->setCurrentIndex(EmailSettingsContainer::DEFAULT);
    d->mailAgentName->setWhatsThis(i18n("<p>Select here your preferred external e-mail program. "
                                        "<b>Default</b> is the current e-mail program set on KDE "
                                        "Control Panel."));

    //---------------------------------------------

    d->addComments = new QCheckBox(i18n("Attach a file with caption and tags"), this);
    d->addComments->setWhatsThis(i18n("<p>If you enable this option, all image captions and tags "
                                      "will be added as an attached file."));

    // --------------------------------------------------------

    d->attachmentlimit = new KIntNumInput(this);
    d->attachmentlimit->setRange(1, 50, 1);
    d->attachmentlimit->setValue(17);
    d->attachmentlimit->setLabel( i18n("Maximum e-mail size limit:"), Qt::AlignLeft | Qt::AlignVCenter);
    d->attachmentlimit->setSuffix(i18n("MB"));

    //---------------------------------------------

    d->changeImagesProp  = new QCheckBox(i18n("Adjust image properties"), this);
    d->changeImagesProp->setChecked(true);
    d->changeImagesProp->setWhatsThis(i18n("<p>If you enable this option, "
                                           "all images to send can be resized and recompressed."));

    QGroupBox *groupBox = new QGroupBox(i18n("Image Properties"), this);
    QGridLayout *grid2  = new QGridLayout(groupBox);

    d->imagesResize = new QComboBox(groupBox);
    d->imagesResize->insertItem(EmailSettingsContainer::VERYSMALL, i18n("Very Small (320 pixels)"));
    d->imagesResize->insertItem(EmailSettingsContainer::SMALL,     i18n("Small (640 pixels)"));
    d->imagesResize->insertItem(EmailSettingsContainer::MEDIUM,    i18n("Medium (800 pixels)"));
    d->imagesResize->insertItem(EmailSettingsContainer::BIG,       i18n("Big (1024 pixels)"));
    d->imagesResize->insertItem(EmailSettingsContainer::VERYBIG,   i18n("Very Big (1280 pixels)"));
    d->imagesResize->insertItem(EmailSettingsContainer::HUGE,      i18n("Huge - for printing (1600 pixels)"));
    d->imagesResize->setCurrentIndex(EmailSettingsContainer::MEDIUM);
    QString whatsThis = i18n("<p>Select here the images size to send:<p>"
                             "<b>%1</b>: use this if you have a very slow internet "
                             "connection or if the target mailbox size is very limited.<p>"
                             "<b>%2</b>: use this if you have a slow internet connection "
                             "and if the target mailbox size is limited.<p>"
                             "<b>%3</b>: this is the default value for a medium internet connection "
                             "and a target mailbox size.<p>"
                             "<b>%4</b>: use this if you have a high-speed internet connection "
                             "and if the target mailbox size is not limited.<p>"
                             "<b>%5</b>: use this if you have no size or speed restrictions.<p>"
                             "<b>%6</b>: use this only for printing purpose.<p>",
                             i18n("very small (320 pixels)"),
                             i18n("small (640 pixels)"),
                             i18n("medium (800 pixels)"),
                             i18n("big (1024 pixels)"),
                             i18n("very big (1280 pixels)"),
                             i18n("huge - for printing (1600 pixels)"));
    d->imagesResize->setWhatsThis(whatsThis);


    d->labelImagesResize = new QLabel( i18n("Image size:"), groupBox);
    d->labelImagesResize->setBuddy(d->imagesResize);

    //---------------------------------------------

    d->imagesFormat = new QComboBox(groupBox);
    d->imagesFormat->insertItem(EmailSettingsContainer::JPEG, "JPEG");
    d->imagesFormat->insertItem(EmailSettingsContainer::PNG,  "PNG");
    d->imagesFormat->setCurrentIndex(EmailSettingsContainer::JPEG);
    whatsThis = i18n("<p>Select here the images files format to send.<p>");
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts Group's file format "
                "is a good Web file format but it uses lossy compression.<p>"
                "<b>PNG</b>: the Portable Network Graphics format is an extensible file format for "
                "the lossless, portable, well-compressed storage of raster images. PNG provides a "
                "patent-free replacement for GIF and can also replace many common uses of TIFF. "
                "PNG is designed to work well in online viewing applications, such as the World Wide Web, "
                "so it is fully streamable with a progressive display option. Also, PNG can store gamma "
                "and chromaticity data for improved color matching on heterogeneous platforms.");
    d->imagesFormat->setWhatsThis(whatsThis);

    d->labelImagesFormat = new QLabel(i18n("Image file format:"), groupBox);
    d->labelImagesFormat->setBuddy(d->imagesFormat);
    
    // --------------------------------------------------------

    d->imageCompression = new KIntNumInput(groupBox);
    d->imageCompression->setRange(1, 100, 1);
    d->imageCompression->setValue(75);
    d->imageCompression->setLabel(i18n("Image quality:"), Qt::AlignLeft | Qt::AlignVCenter);
    whatsThis = i18n("<p>The new compression value of JPEG images to send:<p>");
    whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                 "<b>25</b>: high compression<p>"
                                 "<b>50</b>: medium compression<p>"
                                 "<b>75</b>: low compression (default value)<p>"
                                 "<b>100</b>: no compression");

    d->imageCompression->setWhatsThis(whatsThis);

    //---------------------------------------------
    grid2->addWidget(d->labelImagesResize, 0, 0, 1, 1);
    grid2->addWidget(d->imagesResize, 0, 1, 1, 2);
    grid2->addWidget(d->labelImagesFormat, 1, 0, 1, 1);
    grid2->addWidget(d->imagesFormat, 1, 1, 1, 2);
    grid2->addWidget(d->imageCompression, 2, 0, 1, 3);
    grid2->setRowStretch(4, 10);    
    grid2->setColumnStretch(2, 10);                     
    grid2->setMargin(KDialog::spacingHint());
    grid2->setSpacing(KDialog::spacingHint());        
    grid2->setAlignment(Qt::AlignTop);
            
    // --------------------------------------------------------

    grid->addWidget(d->labelMailAgent, 0, 0, 1, 1);
    grid->addWidget(d->mailAgentName, 0, 1, 1, 2);
    grid->addWidget(d->attachmentlimit, 1, 0, 1, 4);
    grid->addWidget(d->addComments, 2, 0, 1, 4);
    grid->addWidget(d->changeImagesProp, 3, 0, 1, 4);
    grid->addWidget(groupBox, 4, 0, 1, 4);
    grid->setRowStretch(5, 10);    
    grid->setColumnStretch(3, 10);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());        

    //---------------------------------------------

    connect(d->imagesFormat, SIGNAL(activated(int)),
            this, SLOT(slotImagesFormatChanged(int)));

    connect(d->changeImagesProp, SIGNAL(toggled(bool)),
            groupBox, SLOT(setEnabled(bool)));
}

EmailPage::~EmailPage()
{
    delete d;
}

void EmailPage::slotImagesFormatChanged(int i)
{
    if ( i == EmailSettingsContainer::JPEG )
        d->imageCompression->setEnabled(true);
    else
        d->imageCompression->setEnabled(false);
}

void EmailPage::setEmailSettings(const EmailSettingsContainer& settings)
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
                                    
EmailSettingsContainer EmailPage::emailSettings()
{
    EmailSettingsContainer settings;
    settings.emailProgram            = EmailSettingsContainer::EmailClient(d->mailAgentName->currentIndex());
    settings.imageSize               = EmailSettingsContainer::ImageSize(d->imagesResize->currentIndex());
    settings.imageFormat             = EmailSettingsContainer::ImageFormat(d->imagesFormat->currentIndex());

    settings.imagesChangeProp        = d->changeImagesProp->isChecked();
    settings.addCommentsAndTags      = d->addComments->isChecked();

    settings.imageCompression        = d->imageCompression->value();
    settings.attachmentLimitInMbytes = d->attachmentlimit->value();

    return settings;
}

}  // namespace KIPISendimagesPlugin
