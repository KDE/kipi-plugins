/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "intropage.moc"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QButtonGroup>
#include <QVBoxLayout>

// KDE includes

#include <kstandarddirs.h>
#include <kvbox.h>
#include <klocale.h>

namespace KIPIPanoramaPlugin
{

struct IntroPage::IntroPagePriv
{
    IntroPagePriv(Manager* m) : mngr(m), hdrCheckBox(0), formatGroupBox(0), jpegRadioButton(0), tiffRadioButton(0) {}

    Manager*      mngr;

    QCheckBox*    hdrCheckBox;
    QGroupBox*    formatGroupBox;
    QRadioButton* jpegRadioButton;
    QRadioButton* tiffRadioButton;
};

IntroPage::IntroPage(Manager* mngr, KAssistantDialog* dlg)
    : KIPIPlugins::WizardPage(dlg, i18n("<b>Welcome to Panorama Tool</b>")), d(new IntroPagePriv(mngr))
{
    KVBox* vbox   = new KVBox(this);
    QLabel* title = new QLabel(vbox);
    title->setWordWrap(true);
    title->setOpenExternalLinks(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Welcome to Panorama tool</b></h1></p>"
                        "<p>This tool stitch several images together to create a panorama, making the "
                        "seam between images not visible.</p>"
                        "<p>This assistant will help you to configure how to import images before "
                        "stitching them into a panorama.</p>"
                        "<p>Images must be taken from the same point of view.</p>"
                        "<p>For more information, please take a look at "
                        "<a href='http://hugin.sourceforge.net/tutorials/overview/en.shtml'>this page</a></p>"
                        "</qt>"));

    QLabel* space   = new QLabel(vbox);

    QLabel* options = new QLabel(vbox);
    options->setWordWrap(true);
    options->setText(i18n("<qt><p>Panorama Settings: </p></qt>"));

    d->hdrCheckBox  = new QCheckBox(i18n("HDR Output"), vbox);
    d->hdrCheckBox->setToolTip(i18n("When checked, the panorama will be stitched into an High Dynamic Range (HDR) "
                                    "image, that can be processed further with a dedicated software."));
    d->hdrCheckBox->setWhatsThis(i18n("<b>HDR Output</b>: Output in High Dynamic Range, meaning that every piece of "
                                      "information contained in the original photos are preserved. Note that you "
                                      "need another software to process the resulting panorama, like "
                                      "<a href=\"http://qtpfsgui.sourceforge.net/\">Luminance HDR</a>"));

    QVBoxLayout* formatVBox = new QVBoxLayout();
    d->formatGroupBox       = new QGroupBox(i18n("File Format"), vbox);
    d->formatGroupBox->setLayout(formatVBox);
    QButtonGroup* group     = new QButtonGroup();
    formatVBox->addStretch(1);

    d->jpegRadioButton      = new QRadioButton(i18n("JPEG Output"), d->formatGroupBox);
    d->jpegRadioButton->setToolTip(i18n("Selects a JPEG output with 90% compression rate "
                                        "(lossy compression, smaller size)."));
    d->jpegRadioButton->setWhatsThis(i18n("<b>JPEG Output</b>: Using JPEG output, the panorama file will be smaller "
                                          "at the cost of information loss during compression. This is the easiest "
                                          "way to share the result, or print it online or in a shop."));
    formatVBox->addWidget(d->jpegRadioButton);
    group->addButton(d->jpegRadioButton);
    d->tiffRadioButton      = new QRadioButton(i18n("TIFF Output"), d->formatGroupBox);
    d->tiffRadioButton->setToolTip(i18n("Selects a TIFF output compressed using the LZW algorithm "
                                        "(lossless compression, bigger size)."));
    d->jpegRadioButton->setWhatsThis(i18n("<b>TIFF Output</b>: Using TIFF output, you get the same color depth than "
                                          "your original photos using RAW images at the cost of a bigger panorama file."));
    formatVBox->addWidget(d->tiffRadioButton);
    group->addButton(d->tiffRadioButton);

    switch (d->mngr->format())
    {
        case ActionThread::JPEG:
            d->jpegRadioButton->setChecked(true);
            break;
        case ActionThread::TIFF:
            d->tiffRadioButton->setChecked(true);
            break;
    }

    vbox->setStretchFactor(space, 2);

    setPageWidget(vbox);

    QPixmap leftPix = KStandardDirs::locate("data", "kipiplugin_panorama/pics/assistant-tripod.png");
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));

    connect(d->hdrCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(slotShowFileFormat(int)));

    connect(group, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(slotChangeFileFormat(QAbstractButton*)));

    d->hdrCheckBox->setChecked(d->mngr->hdr());
}

IntroPage::~IntroPage()
{
}

void IntroPage::slotShowFileFormat(int state)
{
    d->mngr->setHDR(state);
    if (state)
    {
        d->formatGroupBox->setEnabled(false);
    }
    else
    {
        d->formatGroupBox->setEnabled(true);
    }
}

void IntroPage::slotChangeFileFormat(QAbstractButton* button)
{
    if (button == d->jpegRadioButton)
        d->mngr->setFileFormatJPEG();
    else if (button == d->tiffRadioButton)
        d->mngr->setFileFormatTIFF();
}

}   // namespace KIPIPanoramaPlugin
