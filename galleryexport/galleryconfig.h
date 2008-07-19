/* ============================================================
 * File  : galleryconfig.h
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
 * ============================================================ */

#ifndef GALLERYCONFIG_H
#define GALLERYCONFIG_H

#include <KDialog>

//Added by qt3to4:
#include <QLabel>

#include <q3valuelist.h>
#include <q3intdict.h>


namespace KIPIGalleryExportPlugin
{

class Gallery;

class GalleryEdit : public KDialog
{
  Q_OBJECT

public:

  GalleryEdit(QWidget* pParent,
              Gallery* pGallery,
              QString title);
  ~GalleryEdit();

private:

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
