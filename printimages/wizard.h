/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-01-11
 * Description : a kipi plugin to print images
 *
 * Copyright 2008-2009 by Angelo Naselli <anaselli at linux dot it>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef WIZARD_H
#define WIZARD_H

// QT incudes

#include <QPainter>
#include <QIcon>

// KDE includes

#include <kassistantdialog.h>


namespace KIPI
{
class Interface;
}

namespace KIPIPrintImagesPlugin
{

class TPhoto;

typedef struct _TPhotoSize
{
  QString label;
  int dpi;
  bool autoRotate;
  QList<QRect*> layouts;  // first element is page size
  QIcon         icon;
} TPhotoSize;


/**
 * The wizard used by the user to select the various settings.
 */
  class Wizard : public KAssistantDialog 
  {
    Q_OBJECT
    public:
      Wizard(QWidget* parent, KIPI::Interface*);
      ~Wizard();
      void print( KUrl::List fileList, QString tempPath);

    protected Q_SLOTS:
      virtual void accept();
      virtual void slotHelp();
      virtual void pageChanged(KPageWidgetItem *, KPageWidgetItem *);
      virtual void captionChanged(const QString & text);
      virtual void outputChanged ( const QString & );
      virtual void BtnPrintOrderUp_clicked();
      virtual void BtnPrintOrderDown_clicked();

      virtual void BtnPreviewPageDown_clicked();
      virtual void BtnPreviewPageUp_clicked();
      virtual void BtnCropRotate_clicked();
      virtual void BtnCropNext_clicked();
      virtual void BtnCropPrev_clicked();
      virtual void ListPrintOrder_selected();
      virtual void ListPhotoOrder_highlighted (int );
      virtual void ListPhotoSizes_selected();

      virtual void reject();
      virtual void crop_selection(int);
      virtual void PageRemoved(KPageWidgetItem *page);

      virtual void pagesetupclicked();
      virtual void pagesetupdialogexit();
      virtual void infopage_imageSelected();
      virtual void infopage_selectNext();
      virtual void infopage_selectPrev();
      virtual void infopage_decreaseCopies();
      virtual void infopage_increaseCopies();
      //private slots:
      //	void updateFinishButton();

    private:
      enum AvailableCaptions {
        NoCaptions = 0,
        FileNames,
        ExifDateTime,
        Comment,
        Free
      };

      void initPhotoSizes(QSizeF pageSize);  // pageSize in mm
      void previewPhotos();
      void infopage_enableButtons();
      void infopage_imagePreview();

      // fn filename, pageSize in mm 
      void parseTemplateFile( QString fn, QSizeF pageSize );

      void updateCropFrame(TPhoto *, int);
      void setBtnCropEnabled();
      void removeGimpFiles();
      void printPhotos(QList<TPhoto*> photos, QList<QRect*> layouts, QPrinter &printer);
      QStringList printPhotosToFile(QList<TPhoto*> photos, QString &baseFilename, TPhotoSize *layouts);

      int getPageCount();
      QRect *getLayout(int photoIndex);
      QString captionFormatter(TPhoto *photo, const QString& format);
      void printCaption(QPainter &p, TPhoto* photo, int captionW, int captionH, QString caption);

      bool paintOnePage(QPainter &p, QList<TPhoto*> photos, QList<QRect*> layouts,
                        int captionType, int &current, bool useThumbnails = false);

      void manageBtnPrintOrder();
      void manageBtnPreviewPage();


      void saveSettings(QString pageName);
      void readSettings(QString pageName);

      struct Private;
      Private* d;
  };

} // namespace KIPIPrintImagesPlugin

#endif /* WIZARD_H */
