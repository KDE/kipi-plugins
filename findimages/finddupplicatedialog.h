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

class QComboBox;
class QFrame;
class QPushButton;

class KFileItem;
class KIntNumInput;
class KSqueezedTextLabel;
class KListView;

namespace KIPI
{
  class ImageCollectionSelector;
}

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
  
  QValueList<KIPI::ImageCollection> getSelectedAlbums() const { return m_selectedAlbums; }

 signals:
  
  void updateCache(QStringList fromDir);
  void clearCache(QStringList fromDir);
  void clearAllCache(void);

 protected slots:
  
  void slotOk();
  void slotUpdateCache(void);
  void slotPurgeCache(void);
  void slotPurgeAllCache(void);
  void slotfindMethodChanged(const QString &string);
  void slotHelp();

 private:
  
  QComboBox                     *m_findMethod;

  QFrame                        *page_setupSelection;
  QFrame                        *page_setupMethod;

  QPushButton                   *m_helpButton;
  
  KIntNumInput                  *m_approximateThreshold;

  KIPI::Interface               *m_interface;
  
  QValueList<KIPI::ImageCollection>  m_selectedAlbums;
  KIPI::ImageCollectionSelector     *m_imageCollectionSelector;

 private :
 
  void setupSelection(void);
  void setupPageMethod(void);
  
};

}  // NameSpace KIPIFindDupplicateImagesPlugin

#endif // FINDDUPPLICATEDIALOG_H
