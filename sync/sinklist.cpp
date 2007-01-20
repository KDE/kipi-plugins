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


// Include files for KDE

#include <klocale.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kpushbutton.h>


// Local includes.

#include "sinks.h"
#include "sinklist.h"
//#include "sinkconfig.h"

namespace KIPISyncPlugin
{

SinkList::SinkList(QWidget *pParent, Sinks* pSinks, bool blnShowOpen)
    : SinkListBase(pParent),
      mpSinks(pSinks),
      mpCurrentSink(0)
{
  buttonOpen->setEnabled(blnShowOpen);
  buttonRemove->setEnabled(false);
  buttonConfigure->setEnabled(false);
  
  mpSinks->asQListView(listSinks);
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
  QListViewItem* p_lvi = listSinks->selectedItem();
  bool bln_selected = (p_lvi ? true : false);
  buttonRemove->setEnabled(bln_selected);
  buttonConfigure->setEnabled(bln_selected);
  buttonOpen->setEnabled(bln_selected);

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

  if (buttonOpen->isEnabled())
  {
    accept();
  }
  else
  {
    buttonConfigure_clicked();
  }
}

//==================   Add   =====
void SinkList::buttonAdd_clicked(void)
{
/*
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
*/
}


//==================   Edit  ======
void SinkList::buttonConfigure_clicked(void)
{
  QListViewItem* p_lvi = listSinks->selectedItem();
  if (!p_lvi)
  {
    KMessageBox::error(kapp->activeWindow(), i18n("No sink selected!"));
  }
  else
  {
    /*
    SinkQListViewItem* p_glvi = dynamic_cast<SinkQListViewItem*>(p_lvi);
    SinkEdit dlg(this, p_glvi->GetSink(), i18n("Edit Sink"));
    if (QDialog::Accepted == dlg.exec())
    {
      p_glvi->Refresh();
      mpSinks->Save();
    }
    */
  }
}


//==================  Remove ======
void SinkList::buttonRemove_clicked(void)
{
  QListViewItem* p_lvi = listSinks->selectedItem();
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

