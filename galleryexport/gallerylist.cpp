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

#include <QListWidget>
#include <q3progressdialog.h>

#include <QPushButton>
#include <QCheckBox>

//Added by qt3to4:
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <Q3Frame>

// Include files for KDE

#include <KLocale>
#include <KMessageBox>
#include <KApplication>

// Local includes.

#include "galleries.h"
#include "gallerylist.h"
#include "galleryconfig.h"

namespace KIPIGalleryExportPlugin
{

GalleryList::GalleryList(QWidget *pParent, Galleries* pGalleries, bool blnShowOpen)
    : KDialog(pParent, Qt::Dialog),//Ok|Close|User1|User2|User3),
      mpGalleries(pGalleries),
      mpCurrentGallery(0)
{
// TODO: system this
//  if (!blnShowOpen)
//    showButtonOK(false);

//   setButtonGuiItem(User3, KStandardGuiItem::add());
//   setButtonGuiItem(User2, KStandardGuiItem::configure());
//   setButtonGuiItem(User1, KStandardGuiItem::remove());
//   setButtonGuiItem(Close, KStandardGuiItem::close());
//   setButtonGuiItem(Ok,    KStandardGuiItem::open());

  enableButton(Ok,    false);
  enableButton(User1, false);
  enableButton(User2, false);

  Q3Frame *page = new Q3Frame(this);
  QHBoxLayout *tll = new QHBoxLayout(page);
  page->setMinimumSize(400, 200);
  setMainWidget(page);

  QHBoxLayout *hb = new QHBoxLayout();
  hb->setSpacing(KDialog::spacingHint());
  tll->addItem(hb);

  QLabel *label = new QLabel(page);
  hb->addWidget(label);
  label->setPixmap(UserIcon("gallery"));
  label->setFrameStyle (Q3Frame::Panel | Q3Frame::Sunken);
  label->setAlignment(Qt::AlignTop);
  QVBoxLayout *vb = new QVBoxLayout();
  vb->setSpacing (KDialog::spacingHint());
  tll->addItem(vb);

  mpGalleryList = mpGalleries->asQListWidget(page);
  vb->addWidget(mpGalleryList);
  connect(mpGalleryList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  connect(mpGalleryList, SIGNAL(doubleClicked(QListWidgetItem*, const QPoint&, int)),
          this, SLOT(doubleClicked(QListWidgetItem*, const QPoint&, int)));
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
  QListWidgetItem* p_lvi = mpGalleryList->currentItem();
  bool bln_selected = (p_lvi ? true : false);
  enableButton(User1, bln_selected);
  enableButton(User2, bln_selected);
  enableButton(Ok,    bln_selected);

  if (bln_selected)
  {
    GalleryQListWidgetItem* p_glvi = dynamic_cast<GalleryQListWidgetItem*>(p_lvi);
    mpCurrentGallery = p_glvi->GetGallery();
  }
  else
  {
    mpCurrentGallery = 0;
  }
}

void GalleryList::doubleClicked(QListWidgetItem* pCurrent, const QPoint&, int)
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
    if (QDialog::Accepted == dlg.exec())
    {
        mpGalleries->Add(*p_gallery);
        mpGalleries->Save();
        p_gallery->asQListWidgetItem(mpGalleryList);
    }
    else
    {
        delete p_gallery;
    }
};


//==================   Edit  ======
void GalleryList::slotUser2(void)
{
    QListWidgetItem* p_lvi = mpGalleryList->currentItem();
    if (!p_lvi)
    {
        KMessageBox::error(kapp->activeWindow(), i18n("No gallery selected!"));
    }
    else
    {
        GalleryQListWidgetItem* p_glvi = dynamic_cast<GalleryQListWidgetItem*>(p_lvi);
        GalleryEdit dlg(this, p_glvi->GetGallery(), i18n("Edit Remote Gallery"));

        if (QDialog::Accepted == dlg.exec())
        {
            p_glvi->Refresh();
            mpGalleries->Save();
        }
    }
};


//==================  Remove ======
void GalleryList::slotUser1(void)
{

//   Q3ListViewItem* p_lvi = mpGalleryList->selectedItem();
//   if (!p_lvi)
//   {
//     KMessageBox::error(kapp->activeWindow(), i18n("No gallery selected!"));
//   }
//   else
//   {
//     if (KMessageBox::Yes == 
//           KMessageBox::warningYesNo(kapp->activeWindow(),
//             i18n("Are you sure you want to remove this gallery? "
//                  "All synchronisaton settings will be lost. "
//                  "You cannot undo this action."), 
//             i18n("Remove Remote Gallery"), 
//             KStandardGuiItem::yes(), KStandardGuiItem::no(),
//             QString::null, KMessageBox::Dangerous))
//     {
//       GalleryQListViewItem* p_glvi = dynamic_cast<GalleryQListViewItem*>(p_lvi);
//       Gallery* p_gallery = p_glvi->GetGallery();
//       delete p_glvi;
//       mpGalleries->Remove(p_gallery);
//       mpGalleries->Save();
//     }
//   }
// }
    return;
};

}

#include "gallerylist.moc"

