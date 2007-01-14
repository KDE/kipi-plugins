/* ============================================================
 * File  : gallerylist.h
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
 * ============================================================ */

#ifndef GALLERYLIST_H
#define GALLERYLIST_H

#include <kdialogbase.h>

#include <qlistview.h>

namespace KIPIGalleryExportPlugin
{

class Gallery;
class Galleries;

class GalleryList : public KDialogBase
{
  Q_OBJECT

public:

  GalleryList(QWidget *parent, Galleries* pGalleries, bool blnShowOpen = true);
  ~GalleryList();

  Gallery* GetGallery(void);

private:

  Galleries* mpGalleries;
  Gallery* mpCurrentGallery;
  QListView* mpGalleryList;

private slots:

  void selectionChanged();
  void doubleClicked(QListViewItem*, const QPoint&, int);

  void slotUser1();
  void slotUser2();
  void slotUser3();
};

}

#endif /* GALLERYLIST_H */
