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
        mailAgentLabel          = 0;
        mailAgentName           = 0;
        labelThunderbirdBinPath = 0;
        thunderbirdBinPath      = 0;
        addComments             = 0;
    }
    
    QLabel          *mailAgentLabel;
    QLabel          *labelThunderbirdBinPath;

    QComboBox       *mailAgentName;

    QCheckBox       *addComments;

    KUrlRequester   *thunderbirdBinPath;
};

EmailPage::EmailPage(QWidget* parent)
         : QWidget(parent)
{
    d = new EmailPagePriv;

    QGridLayout* grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->mailAgentLabel = new QLabel(i18n("Mail program:"), this);

    d->mailAgentName = new QComboBox(this);
    d->mailAgentName->insertItem(DEFAULT,       i18n("Default"));
    d->mailAgentName->insertItem(BALSA,         "Balsa");
    d->mailAgentName->insertItem(CLAWSMAIL,     "Claws Mail");
    d->mailAgentName->insertItem(EVOLUTION,     "Evolution");
    d->mailAgentName->insertItem(GMAILAGENT,    "GmailAgent");
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
                                        "<b>GmailAgent</b>: >= 0.2<p>"
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

    //---------------------------------------------
/*
    QGroupBox * groupBox2 = new QGroupBox( i18n("Image Properties"), page_setupEmailOptions );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox2, i18n("<p>The properties of images to send.") );

    QVBoxLayout * groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    m_changeImagesProp = new QCheckBox(i18n("Adjust image properties"), groupBox2);
    QWhatsThis::add( m_changeImagesProp, i18n("<p>If you enable this option, "
                     "all images to send can be resized and recompressed.") );
    m_changeImagesProp->setChecked( true );
    groupBox2Layout->addWidget( m_changeImagesProp );

    QHBoxLayout *hlay12  = new QHBoxLayout();
    groupBox2Layout->addLayout( hlay12 );

    m_imagesResize = new QComboBox(false, groupBox2);
    m_imagesResize->insertItem(i18n("Very Small (320 pixels)"));
    m_imagesResize->insertItem(i18n("Small (640 pixels)"));
    m_imagesResize->insertItem(i18n("Medium (800 pixels)"));
    m_imagesResize->insertItem(i18n("Big (1024 pixels)"));
    m_imagesResize->insertItem(i18n("Very Big (1280 pixels)"));
    m_imagesResize->insertItem(i18n("Huge - for printing (1600 pixels)"));
    m_imagesResize->setCurrentText (i18n("Medium (800 pixels)"));
    whatsThis = i18n("<p>Select here the images size to send:<p>"
                     "<b>%1</b>: use this if you have a very slow internet "
                     "connection or if the target mailbox size is very limited.<p>"
                     "<b>%2</b>: use this if you have a slow internet connection "
                     "and if the target mailbox size is limited.<p>"
                     "<b>%3</b>: this is the default value for a medium internet connection "
                     "and a target mailbox size.<p>"
                     "<b>%4</b>: use this if you have a high-speed internet connection "
                     "and if the target mailbox size is not limited.<p>"
                     "<b>%5</b>: use this if you have no size or speed restrictions.<p>"
                     "<b>%6</b>: use this only for printing purpose.<p>")
                     ,i18n("very small (320 pixels)")
                     ,i18n("small (640 pixels)")
                     ,i18n("medium (800 pixels)")
                     ,i18n("big (1024 pixels)")
                     ,i18n("very big (1280 pixels)")
             ,i18n("huge - for printing (1600 pixels)");
    QWhatsThis::add( m_imagesResize, whatsThis );

    m_labelImageSize = new QLabel( i18n("Sent image size:"), groupBox2);
    hlay12->addWidget( m_labelImageSize );
    m_labelImageSize->setBuddy( m_imagesResize );
    hlay12->addStretch( 1 );
    hlay12->addWidget(m_imagesResize);

    //---------------------------------------------

    m_imageCompression = new KIntNumInput(75, groupBox2);
    m_imageCompression->setRange(1, 100, 1, true );
    m_imageCompression->setLabel( i18n("Sent image quality level:") );
    groupBox2Layout->addWidget( m_imageCompression );
    whatsThis = i18n("<p>The new compression value of images to send:<p>");
    whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                 "<b>25</b>: high compression<p>"
                                 "<b>50</b>: medium compression<p>"
                                 "<b>75</b>: low compression (default value)<p>"
                                 "<b>100</b>: no compression");

    QWhatsThis::add( m_imageCompression, whatsThis);

    //---------------------------------------------

    QHBoxLayout *hlay13  = new QHBoxLayout();
    groupBox2Layout->addLayout( hlay13 );

    m_imagesFormat = new QComboBox(false, groupBox2);
    m_imagesFormat->insertItem("JPEG");
    m_imagesFormat->insertItem("PNG");
    m_imagesFormat->setCurrentText ("JPEG");
    whatsThis = i18n("<p>Select here the images files format to send.<p>");
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts Group's file format "
                "is a good Web file format but it uses lossy compression.<p>"
                "<b>PNG</b>: the Portable Network Graphics format is an extensible file format for "
                "the lossless, portable, well-compressed storage of raster images. PNG provides a "
                "patent-free replacement for GIF and can also replace many common uses of TIFF. "
                "PNG is designed to work well in online viewing applications, such as the World Wide Web, "
                "so it is fully streamable with a progressive display option. Also, PNG can store gamma "
                "and chromaticity data for improved color matching on heterogeneous platforms.");
    QWhatsThis::add( m_imagesFormat, whatsThis );

    m_labelImageFormat = new QLabel(i18n("Image file format:"), groupBox2);
    hlay13->addWidget(m_labelImageFormat);
    m_labelImageFormat->setBuddy(m_imagesFormat);
    hlay13->addStretch(1);
    hlay13->addWidget(m_imagesFormat);

    vlay->addWidget(groupBox2);
    vlay->addStretch(1);

    m_attachmentlimit = new KIntNumInput(17, page_setupEmailOptions);
    m_attachmentlimit->setRange(1, 50, 1, true );
    m_attachmentlimit->setLabel( i18n("Maximum Email size limit:"));
    m_attachmentlimit->setSuffix(i18n("MB"));
    vlay->addWidget( m_attachmentlimit );
*/            
    // --------------------------------------------------------

    grid->addWidget(d->mailAgentLabel, 0, 0, 1, 1);
    grid->addWidget(d->mailAgentName, 0, 1, 1, 2);
    grid->addWidget(d->labelThunderbirdBinPath, 1, 0, 1, 1);
    grid->addWidget(d->thunderbirdBinPath, 1, 1, 1, 3);
    grid->addWidget(d->addComments, 2, 0, 1, 4);
    grid->setColumnStretch(3, 10);                     
    grid->setRowStretch(3, 10);    
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());        

    //---------------------------------------------

    connect(d->mailAgentName, SIGNAL(activated(int)),
            this, SLOT(slotMailAgentChanged(int)));

    connect(d->thunderbirdBinPath, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotThunderbirdBinPathChanged(const QString&)));

/*
    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_labelImageSize, SLOT(setEnabled(bool)));

    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_imagesResize, SLOT(setEnabled(bool)));

    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_imageCompression, SLOT(setEnabled(bool)));

    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_labelImageFormat, SLOT(setEnabled(bool)));

    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_imagesFormat, SLOT(setEnabled(bool)));
*/
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
