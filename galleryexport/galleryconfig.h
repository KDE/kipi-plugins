/* ============================================================
 * File  : galleryconfig.h
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

#ifndef GALLERYCONFIG_H
#define GALLERYCONFIG_H

#include <kdialogbase.h>
#include <qvaluelist.h>
#include <qpair.h>
#include <qintdict.h>

class QListView;
class QListViewItem;
class QLabel;
class QLineEdit;
class QCheckBox;

namespace KIPI
{
class Interface;
}

namespace KIPIGalleryExportPlugin
{

class Gallery;
class Galleries;

class GalleriesList : public KDialogBase
{
  Q_OBJECT

public:

  GalleriesList(KIPI::Interface *interface, QWidget *parent, Galleries* pGalleries);
  ~GalleriesList();

private:

  KIPI::Interface          *m_interface;
  Galleries* mpGalleries;
  QListView* mpGalleryList;

private slots:

  /**
    * selection has changed
    */
  void selectionChanged();

  void slotUser1();
  void slotUser2();
  void slotUser3();
};


class GalleryEdit : public KDialogBase
{
  Q_OBJECT

public:

  GalleryEdit(KIPI::Interface *pInterface, QWidget* pParent,
              Gallery* pGallery,
              QString title);
  ~GalleryEdit();

private:

  KIPI::Interface* mpInterface;
  Gallery*         mpGallery;
  QLabel*          mpHeaderLabel;
  QLineEdit*       mpNameEdit;
  QLineEdit*       mpUrlEdit;
  QLineEdit*       mpUsernameEdit;
  QLineEdit*       mpPasswordEdit;
  QCheckBox*       mpGalleryVersion;

private slots:
  void slotOk(void);
};

}

#endif /* GALLERYCONFIG_H */
