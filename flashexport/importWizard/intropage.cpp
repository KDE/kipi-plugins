/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-09-13
 * Description : a plugin to export images to flash
 *
 * Copyright (C) 2011      by Veaceslav Munteanu <slavuttici at gmail dot com>
 * Copyright (C) 2009-2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "intropage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QComboBox>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "simpleviewer.h"
#include "kipiplugins_debug.h"
#include "kputil.h"

namespace KIPIFlashExportPlugin
{

class IntroPage::Private
{
public:

    Private()
    {
        plugin_select  = 0;
        imageGetOption = 0;
    }

    QComboBox* plugin_select;
    QComboBox* imageGetOption;
};

IntroPage::IntroPage(KPWizardDialog* const dlg)
    : KPWizardPage(dlg, i18n("Welcome to FlashExport Tool")),
      d(new Private)
{
    KPVBox* const vbox   = new KPVBox(this);
    QLabel* const title = new QLabel(vbox);

    title->setWordWrap(true);
    title->setOpenExternalLinks(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Welcome to Flash Export tool</b></h1></p>"
                        "<p>This tool uses <a href='http://www.simpleviewer.net/'>SimpleViewer, AutoViewer, TiltViewer"
                        " or Postcardviewer</a> to export images to flash galleries</p>"
                        "<p>This assistant will help you to configure and import images before "
                        "exporting them to flash</p>"
                        "<p>Note that:</p>"
                        "<p>1. Free version of SimpleViewer "
                        "allow only a maximum of 50 pictures to be exported.</p>"
                        "<p>2. TiltViewer is quite CPU intensive</p>"
                        "<p>3. Postcardviewer is designed only for small amount of photos</p>"
                        "</qt>"));
    KPHBox* const hbox   = new KPHBox(vbox);
    QLabel* const label = new QLabel(i18n("Select &Plugin:"), hbox);

    d->plugin_select    = new QComboBox(hbox);
    QString simplew     = i18nc("SimpleViewer",   "SimpleViewer");
    QString tilt        = i18nc("TiltViewer",     "TiltViewer");
    QString autov       = i18nc("AutoViewer",     "AutoViewer");
    QString postcard    = i18nc("PostcardViewer", "PostcardViewer");
    d->plugin_select->insertItem(SimpleViewerSettingsContainer::SIMPLE,   simplew);
    d->plugin_select->insertItem(SimpleViewerSettingsContainer::TILT,     tilt);
    d->plugin_select->insertItem(SimpleViewerSettingsContainer::AUTO,     autov);
    d->plugin_select->insertItem(SimpleViewerSettingsContainer::POSTCARD, postcard);
    d->plugin_select->setCurrentIndex(SimpleViewerSettingsContainer::SIMPLE);

    label->setBuddy(d->plugin_select);

    // ComboBox for image selection method

    KPHBox* const hbox2         = new KPHBox(vbox);
    QLabel* const getImageLabel = new QLabel(i18n("&Choose image selection method:"),hbox2);
    d->imageGetOption           = new QComboBox(hbox2);
    QString collection          = i18nc("Collections",     "Collections");
    QString dialog              = i18nc("Images",          "Images");
    d->imageGetOption->insertItem(SimpleViewerSettingsContainer::COLLECTION,  collection);
    d->imageGetOption->insertItem(SimpleViewerSettingsContainer::IMAGEDIALOG, dialog);

    getImageLabel->setBuddy(d->imageGetOption);

    setPageWidget(vbox);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("kipi-flash")).pixmap(128));
}

IntroPage::~IntroPage()
{
}

void IntroPage::settings(SimpleViewerSettingsContainer* const settings)
{
    settings->plugType     = (SimpleViewerSettingsContainer::PluginType)d->plugin_select->currentIndex();
    settings->imgGetOption = (SimpleViewerSettingsContainer::ImageGetOption)d->imageGetOption->currentIndex();
    qCDebug(KIPIPLUGINS_LOG) << "Plugin type obtained";
}

}   // namespace KIPIFlashExportPlugin
