//////////////////////////////////////////////////////////////////////////////
//
//    FINDDUPPLICATEDIALOG.H
//
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
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

#ifndef FINDDUPPLICATEDIALOG_H
#define FINDDUPPLICATEDIALOG_H

// Include files for Qt

#include <qstring.h>
#include <qguardedptr.h>

// Include files for KDE

#include <kdialogbase.h>

// Include files for KIPI

#include <libkipi/interface.h>

class QProgressDialog;
class QComboBox;
class QFrame;
class QPushButton;

class KFileItem;
class KIntNumInput;
class KSqueezedTextLabel;
class KListView;

namespace KIPIFindDupplicateImagesPlugin
{

class FindDuplicateDialog : public KDialogBase
{
 Q_OBJECT

 public:
  
  FindDuplicateDialog( KIPI::Interface* interface, QWidget *parent=0);
  ~FindDuplicateDialog();

  const QString getFindMethod() const;
  void setFindMethod(QString Value);

  const int getApproximateThreeshold() const;
  void setApproximateThreeshold(int Value);

  bool setAlbumsList(void);

  QValueList<KIPI::ImageCollection> getAlbumsSelection(void);

 signals:
  
  void updateCache(QStringList fromDir);
  void clearCache(QStringList fromDir);
  void clearAllCache(void);

 protected slots:
  
  void albumSelected( QListViewItem * item );
  void slotOk();
  void slotbuttonSelectAll(void);
  void slotbuttonInvertSelection(void);
  void slotbuttonSelectNone(void);
  void slotUpdateCache(void);
  void slotPurgeCache(void);
  void slotPurgeAllCache(void);
  void slotfindMethodChanged(const QString &string);
  void slotGotPreview(const KFileItem* url, const QPixmap &pixmap);
  void slotStopParsingAlbums(void);
  void slotHelp();

 private:
  
  QComboBox          *m_findMethod;

  QProgressDialog    *m_progressDlg;

  KSqueezedTextLabel *m_AlbumComments;
  KSqueezedTextLabel *m_AlbumCollection;
  KSqueezedTextLabel *m_AlbumDate;
  KSqueezedTextLabel *m_AlbumItems;

  KListView          *m_AlbumsList;

  QLabel             *m_albumPreview;

  QFrame             *page_setupSelection;
  QFrame             *page_setupMethod;
  QFrame             *page_about;

  QPushButton        *m_helpButton;
  
  KIntNumInput       *m_approximateThreshold;

  KIPI::Interface    *m_interface;

  bool                m_stopParsingAlbum;

  void setupSelection(void);
  void setupPageMethod(void);
  
};

}  // NameSpace KIPIFindDupplicateImagesPlugin

#endif // FINDDUPPLICATEDIALOG_H
