/* ============================================================
 * File  : gallerylist.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2006-09-04
 * Copyright 2006 by Colin Guthrie <kde@colin.guthr.ie>
 *
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
 *
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Include files for Qt

#include <QTreeWidget>
#include <QProgressDialog>      // FIXME sure we need it?

#include <QPushButton>
#include <QCheckBox>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <QFrame>

// Include files for KDE

#include <KLocale>
#include <KMessageBox>
#include <KApplication>
#include <KStandardDirs>

// Local includes.

#include "galleries.h"
#include "gallerylist.h"
#include "galleryconfig.h"

namespace KIPIGalleryExportPlugin
{

GalleryList::GalleryList(QWidget *pParent, Galleries* pGalleries, bool blnShowOpen)
        : KDialog(pParent),
        mpGalleries(pGalleries),
        mpCurrentGallery(0)
{
    setCaption("Remote Gallery Settings");
    setButtons(KDialog::Ok | KDialog::Close |  KDialog::User1 |  KDialog::User2 |  KDialog::User3) ;
    showButton(KDialog::Ok , blnShowOpen);

    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotUser1()));
    connect(this, SIGNAL(user2Clicked()), this, SLOT(slotUser2()));
    connect(this, SIGNAL(user3Clicked()), this, SLOT(slotUser3()));

    setButtonText(KDialog::User1 , i18n("Remove"));
    setButtonText(KDialog::User2 , i18n("Edit"));
    setButtonText(KDialog::User3 , i18n("Add"));

    enableButton(Ok,    false);
    enableButton(User1, false);
    enableButton(User2, false);

    QFrame *page = new QFrame(this);
    QHBoxLayout *hl = new QHBoxLayout(page);
    page->setMinimumSize(400, 200);
    setMainWidget(page);


    QLabel *label = new QLabel(page);
    hl->addWidget(label);
//FIXME cannot see image gallery.png!!
    QString galleryImagePath = KStandardDirs::locate("data" , "plugin_galleryexport/pics/gallery.png");
    label->setPixmap(galleryImagePath);
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    label->resize(100, 250);

    mpGalleryList = mpGalleries->asQTreeWidget(page);
    hl->addWidget(mpGalleryList);

    hl->setSpacing(KDialog::spacingHint());

    connect(mpGalleryList, SIGNAL(currentItemChanged(QTreeWidgetItem* , QTreeWidgetItem*)),
            this, SLOT(selectionChanged()));
    connect(mpGalleryList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(doubleClicked(QTreeWidgetItem*, int)));
}

GalleryList::~GalleryList()
{

}

Gallery* GalleryList::GetGallery()
{
    return mpCurrentGallery;
}

void GalleryList::selectionChanged()
{
    QTreeWidgetItem* p_lvi = mpGalleryList->currentItem();
    bool bln_selected = (p_lvi ? true : false);
    enableButton(User1, bln_selected);
    enableButton(User2, bln_selected);
    enableButton(Ok,    bln_selected);

    if (bln_selected) {
        GalleryQTreeWidgetItem* p_glvi = dynamic_cast<GalleryQTreeWidgetItem*>(p_lvi);
        mpCurrentGallery = p_glvi->GetGallery();
    } else {
        mpCurrentGallery = 0;
    }
}

void GalleryList::doubleClicked(QTreeWidgetItem* pCurrent, int column)
{
    if (!pCurrent)
        return;

//   if (actionButton(Ok)->isVisible())
//   {
//     accept();
//   }
//   else
//   {
//     slotUser2();
//   }
}

//==================   Add   =====
void GalleryList::slotUser3(void)
{
    Gallery* p_gallery = new Gallery();
    GalleryEdit dlg(this, p_gallery, i18n("New Remote Gallery"));
    dlg.show();
    if (QDialog::Accepted == dlg.exec()) {
        mpGalleries->Add(*p_gallery);
        mpGalleries->Save();
        p_gallery->asQTreeWidgetItem(mpGalleryList);
    } else {
        delete p_gallery;
    }
};


//==================   Edit  ======
void GalleryList::slotUser2(void)
{
    QTreeWidgetItem* p_lvi = mpGalleryList->currentItem();
    if (!p_lvi) {
        KMessageBox::error(kapp->activeWindow(), i18n("No gallery selected!"));
    } else {
        GalleryQTreeWidgetItem* p_glvi = dynamic_cast<GalleryQTreeWidgetItem*>(p_lvi);
        GalleryEdit dlg(this, p_glvi->GetGallery(), i18n("Edit Remote Gallery"));

        if (QDialog::Accepted == dlg.exec()) {
            p_glvi->Refresh();
            mpGalleries->Save();
        }
    }
};


//==================  Remove ======
void GalleryList::slotUser1(void)
{

    QTreeWidgetItem* p_lvi = mpGalleryList->currentItem();
    if (!p_lvi) {
        KMessageBox::error(kapp->activeWindow(), i18n("No gallery selected!"));
    } else {
        if (KMessageBox::Yes ==
                KMessageBox::warningYesNo(kapp->activeWindow(),
                                          i18n("Are you sure you want to remove this gallery? "
                                               "All synchronisaton settings will be lost. "
                                               "You cannot undo this action."),
                                          i18n("Remove Remote Gallery"),
                                          KStandardGuiItem::yes(), KStandardGuiItem::no(),
                                          QString::null, KMessageBox::Dangerous)) {
            GalleryQTreeWidgetItem* p_glvi = dynamic_cast<GalleryQTreeWidgetItem*>(p_lvi);
            Gallery* p_gallery = p_glvi->GetGallery();
            delete p_glvi;
            mpGalleries->Remove(*p_gallery);
            mpGalleries->Save();
        }
    }
};

}

#include "gallerylist.moc"

