//////////////////////////////////////////////////////////////////////////////
//
//    DISPLAYCOMPARE.H
//
//    Copyright (C) 2001 Richard Groult <rgroult at jalix.org> (from ShowImg project)
//    Copyright (C) 2004 Gilles CAULIER <caulier dot gilles at free.fr>
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

#ifndef __DISPLAYCOMPARE_H__
#define __DISPLAYCOMPARE_H__

// Include files for Qt

#include <qptrvector.h>
#include <qdict.h>
#include <qguardedptr.h>

// Include files for KDE

#include <kdialogbase.h>

// Include files for Digikam

#include <digikam/thumbnailjob.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QPrtList;
class QFile;

class KSqueezedTextLabel;

class  DisplayCompare : public KDialogBase
{
Q_OBJECT

public:
   DisplayCompare(QWidget* parent, QDict < QPtrVector < QFile > >* cmp);
   ~DisplayCompare();

private slots :
   void slotDisplayRight(QListViewItem *);
   void slotDisplayLeft(QListViewItem *);
   void slotAbout( void );
   void slotDelete( void );
   void slotGotPreview1(const KURL &url, const QPixmap &pixmap);
   void slotGotPreview2(const KURL &url, const QPixmap &pixmap);

private:
   QDict < QPtrVector < QFile > >* cmp;

   QGroupBox*          GroupBox1;
   QGroupBox*          GroupBox2;

   KSqueezedTextLabel* OriginalNameLabel;
   KSqueezedTextLabel* originalInfoLabel1;
   KSqueezedTextLabel* originalInfoLabel2;
   KSqueezedTextLabel* originalInfoLabel3;
   KSqueezedTextLabel* originalInfoLabel4;
   KSqueezedTextLabel* originalInfoLabel5;

   KSqueezedTextLabel* similarNameLabel;
   KSqueezedTextLabel* similarInfoLabel1;
   KSqueezedTextLabel* similarInfoLabel2;
   KSqueezedTextLabel* similarInfoLabel3;
   KSqueezedTextLabel* similarInfoLabel4;
   KSqueezedTextLabel* similarInfoLabel5;

   QLabel*             preview1;
   QLabel*             preview2;

   QListView*          listName;
   QListView*          listEq;

   QGuardedPtr<Digikam::ThumbnailJob> m_thumbJob1;
   QGuardedPtr<Digikam::ThumbnailJob> m_thumbJob2;   
};

#endif   // __DISPLAYCOMPARE_H__

