/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-08-04
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
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

#include "piwindow.h"
#include "piwindow.moc"

// Local includes

#include "kpaboutdata.h"
#include "piimglist.h"
#include "piwidget.h"

namespace KIPIPhotivoIntegrationPlugin
{

// pImpl idiom for stable ABI /////////////////////////////////////////////////

class PIWindow::Private
{
public:

    Private(PIWindow* const parent)
        : widget(new PIWidget(parent))
    {
    }

    PIWidget* const widget;
};

// public /////////////////////////////////////////////////////////////////////

PIWindow::PIWindow(QWidget* const /*parent*/ /* = 0 */)
    : KPToolDialog(0),
      d(new Private(this))
{
    // window & widget settings
    setMainWidget(d->widget);
    setWindowIcon(KIcon("photivo"));
    setWindowTitle(i18n("Photivo Integration"));
    setModal(false);
    setMinimumSize(600, 450);

    // buttons
    setButtons(Help | Close /*| User1*/);//TODO: Button for Photivo
    setDefaultButton(Close);
//     setButtonGuiItem(...);
//     enableButton(User1, ...);

    // About data and help button.
    KPAboutData* about = new KPAboutData(ki18n("Photivo Integration"),
                             0,
                             KAboutData::License_GPL,
                             ki18n("A tool to integrate Photivo, a RAW processor"),
                             ki18n("(c) 2012, Dominic Lyons"));

    about->addAuthor(ki18n("Dominic Lyons"),
                     ki18n("Author and Maintainer"),
                     "domlyons at googlemail dot com");

    about->setHandbookEntry("photivointegration");
    setAboutData(about);
}

// ----------------------------------------------------------------------------

PIWindow::~PIWindow()
{
    // clean up pImpl; Qt takes care of the windows and child widgets
    delete d;
}

// ----------------------------------------------------------------------------

void PIWindow::reactivate()
{
    d->widget->imagesList()->loadImagesFromCurrentSelection();
    show();
}

// ----------------------------------------------------------------------------

} // namespace KIPIPhotivoIntegrationPlugin
