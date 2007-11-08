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

// QT includes.

#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>
#include <kiconloader.h>
#include <kurlrequester.h>

// LibKDcraw includes.

#include <libkdcraw/rawfiles.h>
#include <libkdcraw/dcrawbinary.h>

// Local includes.

#include "emailpage.h"
#include "emailpage.moc"

namespace KIPISendimagesPlugin
{

class EmailPagePriv
{
public:

    EmailPagePriv()
    {
        labelMailAgent          = 0;
        mailAgentName           = 0;
        labelThunderbirdBinPath = 0;
        thunderbirdBinPath      = 0;
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
    QLabel        *labelThunderbirdBinPath;
    QLabel        *labelImagesResize;
    QLabel        *labelImagesFormat;

    QComboBox     *mailAgentName;
    QComboBox     *imagesResize;
    QComboBox     *imagesFormat;

    QCheckBox     *changeImagesProp;
    QCheckBox     *addComments;

    KIntNumInput  *imageCompression;
    KIntNumInput  *attachmentlimit;

    KUrlRequester *thunderbirdBinPath;
};

EmailPage::EmailPage(QWidget* parent)
         : QWidget(parent)
{
    d = new EmailPagePriv;

    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->labelMailAgent = new QLabel(i18n("Mail program:"), this);

    d->mailAgentName = new QComboBox(this);
    d->mailAgentName->insertItem(DEFAULT,       i18n("Default"));
    d->mailAgentName->insertItem(BALSA,         "Balsa");
    d->mailAgentName->insertItem(CLAWSMAIL,     "Claws Mail");
    d->mailAgentName->insertItem(EVOLUTION,     "Evolution");
    d->mailAgentName->insertItem(GMAILAGENT,    "Gmail-Agent");
    d->mailAgentName->insertItem(KMAIL,         "KMail");
    d->mailAgentName->insertItem(MOZILLA,       "Mozilla");
    d->mailAgentName->insertItem(NETSCAPE,      "Netscape");
    d->mailAgentName->insertItem(SYLPHEED,      "Sylpheed");
    d->mailAgentName->insertItem(SYLPHEEDCLAWS, "Sylpheed-Claws");
    d->mailAgentName->insertItem(THUNDERBIRD,   "Thunderbird");
    d->mailAgentName->setCurrentIndex(DEFAULT);
    d->mailAgentName->setWhatsThis(i18n("<p>Select here your preferred external mail program."
                                        "These mail program versions are supported:<p>"
                                        "<b>Balsa</b>: >= 2.x<p>"
                                        "<b>Claws Mail</b>: >= 2.6.1<p>"
                                        "<b>Evolution</b>: >= 1.4<p>"
                                        "<b>Gmail-Agent</b>: >= 0.2<p>"
                                        "<b>KMail</b>: >= 1.3<p>"
                                        "<b>Mozilla</b>: >= 1.4<p>"
                                        "<b>Netscape</b>: >= 7.x<p>"
                                        "<b>Sylpheed</b>: >= 0.9<p>"
                                        "<b>Sylpheed-Claws</b>: >= 0.9<p>"
                                        "<b>Thunderbird</b>: >= 0.4<p>"));

    //---------------------------------------------

    d->labelThunderbirdBinPath = new QLabel(i18n("&Thunderbird path:"), this);

    d->thunderbirdBinPath      = new KUrlRequester(this);
    d->thunderbirdBinPath->setUrl(KUrl("/usr/bin/thunderbird"));
    d->labelThunderbirdBinPath->setBuddy( d->thunderbirdBinPath );
    d->thunderbirdBinPath->setWhatsThis(i18n("<p>The path name to the Thunderbird program."));

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

    QGroupBox * groupBox = new QGroupBox(i18n("Image Properties"), this);
    QGridLayout* grid2   = new QGridLayout(groupBox);

    d->imagesResize = new QComboBox(groupBox);
    d->imagesResize->insertItem(VERYSMALL, i18n("Very Small (320 pixels)"));
    d->imagesResize->insertItem(SMALL,     i18n("Small (640 pixels)"));
    d->imagesResize->insertItem(MEDIUM,    i18n("Medium (800 pixels)"));
    d->imagesResize->insertItem(BIG,       i18n("Big (1024 pixels)"));
    d->imagesResize->insertItem(VERYBIG,   i18n("Very Big (1280 pixels)"));
    d->imagesResize->insertItem(HUGE,      i18n("Huge - for printing (1600 pixels)"));
    d->imagesResize->setCurrentIndex(MEDIUM);
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

    d->imageCompression = new KIntNumInput(groupBox);
    d->imageCompression->setRange(1, 100, 1);
    d->imageCompression->setValue(75);
    d->imageCompression->setLabel(i18n("Image quality:"), Qt::AlignLeft | Qt::AlignVCenter);
    whatsThis = i18n("<p>The new compression value of images to send:<p>");
    whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                 "<b>25</b>: high compression<p>"
                                 "<b>50</b>: medium compression<p>"
                                 "<b>75</b>: low compression (default value)<p>"
                                 "<b>100</b>: no compression");

    d->imageCompression->setWhatsThis(whatsThis);

    //---------------------------------------------

    d->imagesFormat = new QComboBox(groupBox);
    d->imagesFormat->insertItem(JPEG, "JPEG");
    d->imagesFormat->insertItem(PNG,  "PNG");
    d->imagesFormat->setCurrentIndex(JPEG);
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

    grid2->addWidget(d->labelImagesResize, 0, 0, 1, 1);
    grid2->addWidget(d->imagesResize, 0, 1, 1, 2);
    grid2->addWidget(d->imageCompression, 1, 0, 1, 3);
    grid2->addWidget(d->labelImagesFormat, 2, 0, 1, 1);
    grid2->addWidget(d->imagesFormat, 2, 1, 1, 2);
    grid2->setRowStretch(4, 10);    
    grid2->setColumnStretch(3, 10);                     
    grid2->setMargin(KDialog::spacingHint());
    grid2->setSpacing(KDialog::spacingHint());        
    grid2->setAlignment(Qt::AlignTop);
            
    // --------------------------------------------------------

    grid->addWidget(d->labelMailAgent, 0, 0, 1, 1);
    grid->addWidget(d->mailAgentName, 0, 1, 1, 2);
    grid->addWidget(d->labelThunderbirdBinPath, 1, 0, 1, 1);
    grid->addWidget(d->thunderbirdBinPath, 1, 1, 1, 3);
    grid->addWidget(d->attachmentlimit, 2, 0, 1, 4);
    grid->addWidget(d->addComments, 3, 0, 1, 4);
    grid->addWidget(d->changeImagesProp, 4, 0, 1, 4);
    grid->addWidget(groupBox, 5, 0, 1, 4);
    grid->setRowStretch(6, 10);    
    grid->setColumnStretch(3, 10);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());        

    //---------------------------------------------

    connect(d->mailAgentName, SIGNAL(activated(int)),
            this, SLOT(slotMailAgentChanged(int)));

    connect(d->thunderbirdBinPath, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotThunderbirdBinPathChanged(const QString&)));

    connect(d->changeImagesProp, SIGNAL(toggled(bool)),
            groupBox, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    slotMailAgentChanged(d->mailAgentName->currentIndex());
}

EmailPage::~EmailPage()
{
    delete d;
}

void EmailPage::slotMailAgentChanged(int i)
{
    if ( i == THUNDERBIRD )
    {
        d->labelThunderbirdBinPath->setEnabled(true);
        d->thunderbirdBinPath->setEnabled(true);
    }
    else
    {
       d->labelThunderbirdBinPath->setEnabled(false);
       d->thunderbirdBinPath->setEnabled(false);
    }
}

void EmailPage::slotThunderbirdBinPathChanged(const QString& url )
{
    if ( d->mailAgentName->currentIndex() == THUNDERBIRD )
       emit signalEnableButtonOK( !url.isEmpty());
}

}  // namespace KIPISendimagesPlugin
