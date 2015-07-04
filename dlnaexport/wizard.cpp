/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#include "wizard.h"

// Qt includes

#include <QFileInfo>
#include <QPalette>
#include <QtGlobal>
#include <QMap>
#include <QProcess>
#include <QUrl>

// KDE includes

#include <kassistantdialog.h>
#include <kdialog.h>

// libkipi includes

#include <KIPI/ImageCollection>
#include <KIPI/ImageCollectionSelector>
#include <KIPI/Interface>

// Local includes

#include "kipiplugins_debug.h"
#include "kpimageslist.h"
#include "kpaboutdata.h"
#include "finalpage.h"
#include "welcomepage.h"

using namespace KIPI;

namespace KIPIDLNAExportPlugin
{

class Wizard::Private
{
public:

    Private()
    {
        finalPage                    = 0;
        collectionSelector           = 0;
        welcomePage                  = 0;
        finalPageItem                = 0;
        welcomePageItem              = 0;
        collectionSelectorPageItem   = 0;
    }

    FinalPage*                           finalPage;
    ImageCollectionSelector*             collectionSelector;
    WelcomePage*                         welcomePage;
    KPageWidgetItem*                     finalPageItem;
    KPageWidgetItem*                     collectionSelectorPageItem;
    KPageWidgetItem*                     welcomePageItem;
    QList<QUrl>                          imageList;
    QMap<QString, QList<QUrl>>           collectionMap;
    QStringList                          directories;
};

Wizard::Wizard(QWidget* const parent)
    : KPWizardDialog(parent),
      d(new Private)
{
    setWindowTitle(i18n("DLNA Export"));
    setMinimumSize(300, 500);

    //--------------------------------------------------------------------

    KPAboutData* const about = new KPAboutData(ki18n("DLNA Export"),
                                   0,
                                   KAboutLicense::GPL,
                                   ki18n("A Kipi plugin to export image collections via DLNA"),
                                   ki18n("(c) 2012-2013, Smit Mehta"));

    about->addAuthor(ki18n("Smit Mehta").toString(),
                     ki18n("Author").toString(),
                     "smit dot meh at gmail dot com");

    about->addAuthor(ki18n("Marcel Wiesweg").toString(),
                     ki18n("Developer").toString(),
                     "marcel dot wiesweg at gmx dot de");

    about->addAuthor(ki18n("Gilles Caulier").toString(),
                     ki18n("Developer").toString(),
                     "caulier dot gilles at gmail dot com");

    about->setHandbookEntry("dlnaexport");
    setAboutData(about);

    //-----------------------------------------------------------------------

    d->welcomePage                = new WelcomePage(this);
    d->welcomePageItem            = addPage(d->welcomePage, "");
    setValid(d->welcomePageItem, false);

    d->collectionSelector         = iface()->imageCollectionSelector(this);
    d->collectionSelector->enableVirtualCollections(false);
    d->collectionSelectorPageItem = addPage(d->collectionSelector, i18n("Select the required collections"));

    setValid(d->collectionSelectorPageItem, false);

    connect(d->collectionSelector, SIGNAL(selectionChanged()),
            this, SLOT(updateCollectionSelectorPageValidity()));

    connect(d->collectionSelector, SIGNAL(selectionChanged()),
            this, SLOT(getImagesFromCollection()));

    connect(d->collectionSelector, SIGNAL(selectionChanged()),
            this, SLOT(getDirectoriesFromCollection()));

    d->finalPage     = new FinalPage(this);
    d->finalPageItem = addPage(d->finalPage, i18n("Images to be exported"));

    // To activate / deactivate back button once the sharing stops / starts.

    connect(d->finalPage, SIGNAL(sharing(bool)),
            this, SLOT(changeBackButtonState(bool)));

    connect(button(QDialogButtonBox::Cancel), SIGNAL(released()),
            this, SLOT(deleteDlnaObjects()));
}

Wizard::~Wizard()
{
    delete d;
}

void Wizard::deleteDlnaObjects()
{
    d->finalPage->turnOff();
}

void Wizard::slotBinariesFound(bool flag)
{
    setValid(d->welcomePageItem, flag);
    qCDebug(KIPIPLUGINS_LOG) << flag;
}

void Wizard::changeBackButtonState(bool state)
{
    backButton()->setEnabled(!state);
    nextButton()->setEnabled(!state);
}

void Wizard::next()
{
    if (currentPage() == d->welcomePageItem)
    {
        d->finalPage->setMinidlnaBinaryPath(d->welcomePage->getMinidlnaBinaryPath());

        d->finalPage->clearImages();
        KAssistantDialog::next();
    }
    else if (currentPage() == d->collectionSelectorPageItem)
    {
        d->finalPage->clearImages();
        d->finalPage->setImages(d->imageList);
        KAssistantDialog::next();
    }
    else
    {
        KAssistantDialog::next();
    }
}

void Wizard::updateCollectionSelectorPageValidity()
{
    setValid(d->collectionSelectorPageItem, !d->collectionSelector->selectedImageCollections().empty());
}

void Wizard::getImagesFromCollection()
{
    d->imageList.clear();
    d->collectionMap.clear();

    foreach(ImageCollection images, d->collectionSelector->selectedImageCollections())
    {
        d->imageList.append(images.images());
        d->collectionMap.insert(images.name(), images.images());
    }

    d->finalPage->setCollectionMap(d->collectionMap);
}

void Wizard::getDirectoriesFromCollection()
{
    d->directories.clear();

    foreach(ImageCollection images, d->collectionSelector->selectedImageCollections())
    {
        qCDebug(KIPIPLUGINS_LOG) << images.url().toLocalFile();
        d->directories << images.url().toLocalFile();
    }

    d->finalPage->setDirectories(d->directories);
}

} // namespace KIPIDLNAExportPlugin
