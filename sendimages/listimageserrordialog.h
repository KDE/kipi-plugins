//////////////////////////////////////////////////////////////////////////////
//
//    LISTIMAGESERRORDIALOG.H
//
//    Copyright (C) 2003 Gilles CAULIER <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef LISTIMAGESERRORDIALOG_included
#define LISTIMAGESERRORDIALOG_included

// KDElib includes

#include <kdialogbase.h>

class KListView;

class listImagesErrorDialog : public KDialogBase
{
Q_OBJECT

public:
  listImagesErrorDialog(QWidget* parent, QString Caption,
                        const QString &Mess1, const QString &Mess2,
                        QStringList ListOfiles);
  ~listImagesErrorDialog();

private:
  KListView*     m_listFiles;
};

#endif  // LISTIMAGESERRORDIALOG_included

