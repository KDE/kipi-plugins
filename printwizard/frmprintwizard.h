/***************************************************************************
                          frmprintwizard.h  -  description
                             -------------------
    begin                : Mon Sep 30 2002
    copyright            : (C) 2002 by Todd Shoemaker
    email                : jtshoe11@yahoo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FRMPRINTWIZARD_H
#define FRMPRINTWIZARD_H

#include <qwidget.h>

#include <kprinter.h>

#include "frmprintwizardbase.h"
#include "tphoto.h"

/**
  *@author Todd Shoemaker
  */

typedef struct _TPhotoSize {
  QString label;
  QPtrList<QRect> layouts;  // first element is page size
} TPhotoSize;

class FrmPrintWizard : public FrmPrintWizardBase  {
   Q_OBJECT

private:
  QPtrList<TPhoto> m_photos;
  QPtrList<TPhotoSize> m_photoSizes;

  KPrinter::PageSize m_pageSize;

  QString m_tempPath;

  QStringList m_gimpFiles;

  bool m_cancelPrinting;

  void updateCropFrame(TPhoto *, int);
  void setBtnCropEnabled();
  void removeGimpFiles();
  void initPhotoSizes(KPrinter::PageSize);
  void previewPhotos();
  void printPhotos(QPtrList<TPhoto> photos, QPtrList<QRect> layouts, KPrinter &printer);
  QStringList printPhotosToFile(QPtrList<TPhoto> photos, QString &baseFilename, QPtrList<QRect> layouts);
  void loadSettings();
  void saveSettings();

public:
  FrmPrintWizard(QWidget *parent=0, const char *name=0);
  ~FrmPrintWizard();
  void print( KURL::List fileList, QString tempPath);
  QRect * getLayout(int photoIndex);

public slots:
  void BtnCropRotate_clicked();
  void BtnCropNext_clicked();
  void BtnCropPrev_clicked();
  void FrmPrintWizardBaseSelected(const QString &);
  void ListPhotoSizes_selected( QListBoxItem * );
  void ListPhotoSizes_highlighted ( int );
  void EditCopies_valueChanged( int );
  void GrpOutputSettings_clicked(int id);
  void EditOutputPath_textChanged(const QString &);
  void BtnBrowseOutputPath_clicked();
  void CmbPaperSize_activated( int );

protected slots:
  void accept();
  void reject();
  void slotHelp();
};

#endif

