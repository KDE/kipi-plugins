/* ============================================================
 * File  : sinklist.h
 * Author: Colin Guthrie <kde@colin.guthr.ie>
 * Date  : 2006-09-04
 * Copyright 2006 by Colin Guthrie <kde@colin.guthr.ie>
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
 * ============================================================ */

#ifndef SINKLIST_H
#define SINKLIST_H

#include "sinklistbase.h"

class QWidget;

namespace KIPISyncPlugin
{

class Sink;
class Sinks;

class SinkList : public SinkListBase
{
  Q_OBJECT

public:

  SinkList(QWidget* pParent, Sinks* pSinks, bool blnShowOpen = true);
  ~SinkList();

  Sink* GetSink(void);

private:

  Sinks* mpSinks;
  Sink* mpCurrentSink;
  QListView* mpSinkList;

private slots:

  void buttonRemove_clicked();
  void buttonConfigure_clicked();
  void buttonAdd_clicked();
  
  void selectionChanged();
  void doubleClicked(QListViewItem*, const QPoint&, int);
};

}

#endif /* SINKLIST_H */
