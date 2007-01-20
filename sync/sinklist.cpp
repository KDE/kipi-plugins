/* ============================================================
 * File  : sinklist.cpp
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2006-09-04
 * Copyright 2006 by Colin Guthrie
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

#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kiconloader.h>

// Local includes.

#include "sinks.h"
#include "sinklist.h"
#include "sinkconfig.h"

namespace KIPISyncPlugin
{

SinkList::SinkList(QWidget *pParent, Sinks* pSinks, bool blnShowOpen)
    : KDialogBase(pParent, 0, true, i18n("Sinks"),
                  Ok|Close|User1|User2|User3,
                  Close, false),
      mpSinks(pSinks),
      mpCurrentSink(0)
{
  if (!blnShowOpen)
    showButtonOK(false);

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
  label->setPixmap(UserIcon("sink"));
  label->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  label->setAlignment(Qt::AlignTop);
  QVBoxLayout *vb = new QVBoxLayout();
  vb->setSpacing (KDialog::spacingHint());
  tll->addItem(vb);

  mpSinkList = mpSinks->asQListView(page);
  vb->addWidget(mpSinkList);
  connect(mpSinkList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  connect(mpSinkList, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)),
          this, SLOT(doubleClicked(QListViewItem*, const QPoint&, int)));
}

SinkList::~SinkList()
{

}

Sink* SinkList::GetSink()
{
  return mpCurrentSink;
}

void SinkList::selectionChanged()
{
  QListViewItem* p_lvi = mpSinkList->selectedItem();
  bool bln_selected = (p_lvi ? true : false);
  enableButton(User1, bln_selected);
  enableButton(User2, bln_selected);
  enableButton(Ok,    bln_selected);

  if (bln_selected)
  {
    SinkQListViewItem* p_glvi = dynamic_cast<SinkQListViewItem*>(p_lvi);
    mpCurrentSink = p_glvi->GetSink();
  }
  else
  {
    mpCurrentSink = 0;
  }
}

void SinkList::doubleClicked(QListViewItem* pCurrent, const QPoint&, int)
{
  if (!pCurrent)
    return;

  if (actionButton(Ok)->isVisible())
  {
    accept();
  }
  else
  {
    slotUser2();
  }
}

//==================   Add   =====
void SinkList::slotUser3(void)
{
  Sink* p_sink = new Sink();
  SinkEdit dlg(this, p_sink, i18n("New Sink"));
  if (QDialog::Accepted == dlg.exec())
  {
    mpSinks->Add(p_sink);
    mpSinks->Save();
    p_sink->asQListViewItem(mpSinkList);
  }
  else
  {
    delete p_sink;
  }
}


//==================   Edit  ======
void SinkList::slotUser2(void)
{
  QListViewItem* p_lvi = mpSinkList->selectedItem();
  if (!p_lvi)
  {
    KMessageBox::error(kapp->activeWindow(), i18n("No sink selected!"));
  }
  else
  {
    SinkQListViewItem* p_glvi = dynamic_cast<SinkQListViewItem*>(p_lvi);
    SinkEdit dlg(this, p_glvi->GetSink(), i18n("Edit Sink"));
    if (QDialog::Accepted == dlg.exec())
    {
      p_glvi->Refresh();
      mpSinks->Save();
    }
  }
}


//==================  Remove ======
void SinkList::slotUser1(void)
{
  QListViewItem* p_lvi = mpSinkList->selectedItem();
  if (!p_lvi)
  {
    KMessageBox::error(kapp->activeWindow(), i18n("No sink selected!"));
  }
  else
  {
    if (KMessageBox::Yes == 
          KMessageBox::warningYesNo(kapp->activeWindow(),
            i18n("Are you sure you want to remove this sink? "
                 "All synchronisaton settings will be lost. "
                 "You cannot undo this action."), 
            i18n("Remove Sink?"), 
            KStdGuiItem::yes(), KStdGuiItem::no(),
            QString::null, KMessageBox::Dangerous))
    {
      SinkQListViewItem* p_glvi = dynamic_cast<SinkQListViewItem*>(p_lvi);
      Sink* p_sink = p_glvi->GetSink();
      delete p_glvi;
      mpSinks->Remove(p_sink);
      mpSinks->Save();
    }
  }
}

}

#include "sinklist.moc"

