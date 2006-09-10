/* ============================================================
 * File  : gallerylist.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2006-09-04
 * Copyright 2006 by Colin Guthrie
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

#include <qlistview.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qlineedit.h>
#include <qprogressdialog.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>

// Include files for KDE

#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>

// Local includes.

#include "galleries.h"
#include "gallerylist.h"
#include "galleryconfig.h"

namespace KIPIGalleryExportPlugin
{

GalleryList::GalleryList(QWidget *pParent, Galleries* pGalleries, bool blnShowOpen)
    : KDialogBase(pParent, 0, true, i18n("Remote Galleries"),
                  Close|User1|User2|User3|(blnShowOpen ? Ok : 0),
                  Close, false),
      mpGalleries(pGalleries),
      mpCurrentGallery(0)
{
  setButtonGuiItem(User3, KStdGuiItem::add());
  setButtonGuiItem(User2, KStdGuiItem::configure());
  setButtonGuiItem(User1, KStdGuiItem::remove());
  setButtonGuiItem(Close, KStdGuiItem::close());
  setButtonGuiItem(Ok,    KStdGuiItem::open());

  enableButton(Ok,    false);
  enableButton(User1, false);
  enableButton(User2, false);

  QFrame *page = new QFrame(this);
  QHBoxLayout *tll = new QHBoxLayout(page);
  page->setMinimumSize(400, 200);
  setMainWidget(page);

  QHBoxLayout *hb = new QHBoxLayout();
  hb->setSpacing(KDialog::spacingHint());
  tll->addItem(hb);

  QLabel *label = new QLabel(page);
  hb->addWidget(label);
  label->setPixmap(UserIcon("gallery"));
  label->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  label->setAlignment(Qt::AlignTop);
  QVBoxLayout *vb = new QVBoxLayout();
  vb->setSpacing (KDialog::spacingHint());
  tll->addItem(vb);

  mpGalleryList = mpGalleries->asQListView(page);
  vb->addWidget(mpGalleryList);
  connect(mpGalleryList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
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
  QListViewItem* p_lvi = mpGalleryList->selectedItem();
  bool bln_selected = (p_lvi ? true : false);
  enableButton(User1, bln_selected);
  enableButton(User2, bln_selected);
  enableButton(Ok,    bln_selected);

  if (bln_selected)
  {
    GalleryQListViewItem* p_glvi = dynamic_cast<GalleryQListViewItem*>(p_lvi);
    mpCurrentGallery = p_glvi->GetGallery();
  }
  else
  {
    mpCurrentGallery = 0;
  }
}

//==================   Add   =====
void GalleryList::slotUser3(void)
{
  Gallery* p_gallery = new Gallery();
  GalleryEdit dlg(this, p_gallery, i18n("New Remote Gallery"));
  if (QDialog::Accepted == dlg.exec())
  {
    mpGalleries->Add(p_gallery);
    mpGalleries->Save();
    p_gallery->asQListViewItem(mpGalleryList);
  }
  else
  {
    delete p_gallery;
  }
}


//==================   Edit  ======
void GalleryList::slotUser2(void)
{
  QListViewItem* p_lvi = mpGalleryList->selectedItem();
  if (!p_lvi)
  {
    KMessageBox::error(kapp->activeWindow(), i18n("No gallery selected!"));
  }
  else
  {
    GalleryQListViewItem* p_glvi = dynamic_cast<GalleryQListViewItem*>(p_lvi);
    GalleryEdit dlg(this, p_glvi->GetGallery(), i18n("Edit Remote Gallery"));
    if (QDialog::Accepted == dlg.exec())
    {
      p_glvi->Refresh();
      mpGalleries->Save();
    }
  }
}


//==================  Remove ======
void GalleryList::slotUser1(void)
{
  QListViewItem* p_lvi = mpGalleryList->selectedItem();
  if (!p_lvi)
  {
    KMessageBox::error(kapp->activeWindow(), i18n("No gallery selected!"));
  }
  else
  {
    if (KMessageBox::Yes == 
          KMessageBox::warningYesNo(kapp->activeWindow(),
            i18n("Are you sure you want to remove this gallery? "
                 "All synchronisaton settings will be lost. "
                 "You cannot undo this action."), 
            i18n("Remove Remote Gallery"), 
            KStdGuiItem::yes(), KStdGuiItem::no(),
            QString::null, KMessageBox::Dangerous))
    {
      GalleryQListViewItem* p_glvi = dynamic_cast<GalleryQListViewItem*>(p_lvi);
      Gallery* p_gallery = p_glvi->GetGallery();
      delete p_glvi;
      mpGalleries->Remove(p_gallery);
      mpGalleries->Save();
    }
  }
}

}

#include "gallerylist.moc"

