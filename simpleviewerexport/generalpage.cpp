/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-02-21
 * Description : general settings page.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QLayout>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>
#include <klineedit.h>
#include <kurlrequester.h>

// Local includes.

#include "generalpage.h"
#include "generalpage.moc"

namespace KIPISimpleViewerExportPlugin
{

class GeneralPagePriv
{
public:

    GeneralPagePriv()
    {
        title              = 0;
        resizeExportImages = 0;
        showComments       = 0;
        imagesExportSize   = 0;
        maxImageDimension  = 0;
        exportUrl          = 0;
        rightClick         = 0;
    }

    KLineEdit     *title;

    QCheckBox     *resizeExportImages;
    QCheckBox     *showComments;
    QCheckBox     *rightClick;

    KIntNumInput  *imagesExportSize;
    KIntNumInput  *maxImageDimension;

    KUrlRequester *exportUrl;
};

GeneralPage::GeneralPage(QWidget* parent)
           : QWidget(parent)
{
    d = new GeneralPagePriv;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ------------------------------------------------------------------------

    QGroupBox *box    = new QGroupBox(i18n("Gallery &Title"), this);
    QVBoxLayout *vlay = new QVBoxLayout(box);
    d->title          = new KLineEdit(this);
    d->title->setWhatsThis(i18n("<p>Enter here the gallery title"));

    vlay->setMargin(KDialog::spacingHint());
    vlay->setSpacing(KDialog::spacingHint());
    vlay->addWidget(d->title);

    // ------------------------------------------------------------------------

    QGroupBox *box2    = new QGroupBox(i18n("Save Gallery To"), this);
    QVBoxLayout *vlay2 = new QVBoxLayout(box2);
    d->exportUrl       = new KUrlRequester(KGlobalSettings::documentPath() + "simpleviewer", this);
    d->exportUrl->setMode(KFile::Directory | KFile::LocalOnly);

    vlay2->setMargin(KDialog::spacingHint());
    vlay2->setSpacing(KDialog::spacingHint());
    vlay2->addWidget(d->exportUrl);

    // ------------------------------------------------------------------------

    QGroupBox *box3       = new QGroupBox(i18n("Image Size"), this);
    QVBoxLayout *vlay3    = new QVBoxLayout(box3);
    d->resizeExportImages = new QCheckBox(i18n("Resize Target Images"), this);
    d->resizeExportImages->setChecked(true);
    d->resizeExportImages->setWhatsThis(i18n("<p>If you enable this option, "
                                             "all target images can be resized.") );

    d->imagesExportSize = new KIntNumInput(this);
    d->imagesExportSize->setRange(200, 2000, 1);
    d->imagesExportSize->setValue(640);
    d->imagesExportSize->setLabel(i18n("&Target Images Size:"), Qt::AlignVCenter);
    d->imagesExportSize->setWhatsThis(i18n("<p>The new size of the exported images in pixels "
                                           "SimpleViewer resizes the images as well, but this "
                                           "resizes your images before they are uploaded to your server"));

    connect(d->resizeExportImages, SIGNAL(toggled(bool)),
            d->imagesExportSize, SLOT(setEnabled(bool)));

    d->maxImageDimension = new KIntNumInput(this);
    d->maxImageDimension->setRange(200, 2000, 1);
    d->maxImageDimension->setValue(640);
    d->maxImageDimension->setLabel(i18n("&Displayed Images Size:"), Qt::AlignVCenter);
    d->maxImageDimension->setWhatsThis(i18n("<p>scales the displayed images to this size. "
                                            "Largest height or width of your largest image (in pixels). "
                                            "Images will not be scaled up above this size, to ensure "
                                            "best image quality."));

    vlay3->setMargin(KDialog::spacingHint());
    vlay3->setSpacing(KDialog::spacingHint());
    vlay3->addWidget(d->resizeExportImages);
    vlay3->addWidget(d->imagesExportSize);
    vlay3->addWidget(d->maxImageDimension);

    // ------------------------------------------------------------------------

    QGroupBox *box4    = new QGroupBox(i18n("Misc"), this);
    QVBoxLayout *vlay4 = new QVBoxLayout(box4);

    d->showComments = new QCheckBox(i18n("Display Captions"), this);
    d->showComments->setChecked(true);
    d->showComments->setWhatsThis(i18n("<p>If you enable this option, "
                                       "the images caption will be shown"));

    d->rightClick = new QCheckBox(i18n("Open Image with Right Click"), this);
    d->rightClick->setChecked(true);
    d->rightClick->setWhatsThis(i18n("<p>If you enable this option, "
                                     "user will be able to open the target image in a separate window "
                                     "using right mouse button"));

    vlay4->setMargin(KDialog::spacingHint());
    vlay4->setSpacing(KDialog::spacingHint());
    vlay4->addWidget(d->showComments);
    vlay4->addWidget(d->rightClick);

    // ------------------------------------------------------------------------

    mainLayout->addWidget(box);
    mainLayout->addWidget(box2);
    mainLayout->addWidget(box3);
    mainLayout->addWidget(box4);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->addStretch(10);
}

GeneralPage::~GeneralPage()
{
    delete d;
}

void GeneralPage::setSettings(const SimpleViewerSettingsContainer& settings)
{
    d->title->setText(settings.title);
    d->exportUrl->setUrl(settings.exportUrl);
    d->resizeExportImages->setChecked(settings.resizeExportImages);
    d->imagesExportSize->setValue(settings.imagesExportSize);
    d->maxImageDimension->setValue(settings.maxImageDimension);
    d->showComments->setChecked(settings.showComments);
    d->rightClick->setChecked(settings.enableRightClickOpen);
}

void GeneralPage::settings(SimpleViewerSettingsContainer& settings)
{
    settings.title                = d->title->text();
    settings.exportUrl            = d->exportUrl->url();
    settings.resizeExportImages   = d->resizeExportImages->isChecked();
    settings.imagesExportSize     = d->imagesExportSize->value();
    settings.maxImageDimension    = d->maxImageDimension->value();
    settings.showComments         = d->showComments->isChecked();
    settings.enableRightClickOpen = d->rightClick->isChecked();
}

}  // namespace KIPISimpleViewerExportPlugin
