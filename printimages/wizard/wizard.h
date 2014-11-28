/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-01-11
 * Description : a kipi plugin to print images
 *
 * Copyright 2008-2012 by Angelo Naselli <anaselli at linux dot it>
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

// Local includes

#include "kpimageslist.h"
#include "kptooldialog.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIPrintImagesPlugin
{

class TPhoto;

typedef struct _TPhotoSize
{
    QString       label;
    int           dpi;
    bool          autoRotate;
    QList<QRect*> layouts;  // first element is page size
    QIcon         icon;
} TPhotoSize;

/**
 * The wizard used by the user to select the various settings.
 */
class Wizard : public KPWizardDialog
{
    Q_OBJECT

public:

    Wizard(QWidget* const);
    ~Wizard();

    void print(const KUrl::List& fileList, const QString& tempPath);

protected Q_SLOTS:

    virtual void accept();
    virtual void pageChanged(KPageWidgetItem*, KPageWidgetItem*);
    virtual void captionChanged(const QString& text);
    virtual void saveCaptionSettings();
    virtual void outputChanged(const QString&);
    virtual void BtnPrintOrderUp_clicked();
    virtual void BtnPrintOrderDown_clicked();

    virtual void BtnPreviewPageDown_clicked();
    virtual void BtnPreviewPageUp_clicked();
    virtual void BtnCropRotateLeft_clicked();
    virtual void BtnCropRotateRight_clicked();
    virtual void BtnCropNext_clicked();
    virtual void BtnCropPrev_clicked();
    virtual void BtnSaveAs_clicked();
    virtual void ListPhotoSizes_selected();

    virtual void reject();
    virtual void crop_selection(int);
    virtual void slotPageRemoved(KPageWidgetItem* page);

    virtual void pagesetupclicked();
    virtual void pagesetupdialogexit();
    virtual void imageSelected(QTreeWidgetItem*);
    virtual void decreaseCopies();
    virtual void increaseCopies();
    virtual void infopage_updateCaptions();

    virtual void slotAddItems(const KUrl::List&);
    virtual void slotRemovingItem(KIPIPlugins::KPImagesListViewItem*);
    virtual void slotContextMenuRequested();
    virtual void slotXMLSaveItem(QXmlStreamWriter&, KIPIPlugins::KPImagesListViewItem*);
    virtual void slotXMLLoadElement(QXmlStreamReader&);
    virtual void slotXMLCustomElement(QXmlStreamWriter&);
    virtual void slotXMLCustomElement(QXmlStreamReader&);

private:

    // Initialize page layout to the given pageSize in mm
    void initPhotoSizes(const QSizeF& pageSize);
    void previewPhotos();

    void infopage_blockCaptionButtons(bool block=true);
    void infopage_setCaptionButtons();
    void infopage_readCaptionSettings();

    // fn filename, pageSize in mm 
    void parseTemplateFile( const QString& fn, const QSizeF& pageSize );

    void updateCaption(TPhoto* const);
    void updateCropFrame(TPhoto* const, int);
    void setBtnCropEnabled();
    void removeGimpFiles();
    void printPhotos(const QList<TPhoto*>& photos, const QList<QRect*>& layouts, QPrinter& printer);
    QStringList printPhotosToFile(const QList<TPhoto*>& photos, const QString& baseFilename, TPhotoSize* const layouts);

    int     getPageCount()                        const;
    QRect*  getLayout(int photoIndex)             const;
    QString captionFormatter(TPhoto* const photo) const;
    void    printCaption(QPainter& p, TPhoto* photo, int captionW, int captionH, const QString& caption);

    bool paintOnePage(QPainter& p, const QList<TPhoto*>& photos, const QList<QRect*>& layouts,
                      int& current, bool cropDisabled, bool useThumbnails = false);

    void manageBtnPreviewPage();

    // fix caption group layout according to captions combobox text
    void enableCaptionGroup(const QString& text);

    void saveSettings(const QString& pageName);
    void readSettings(const QString& pageName);

private:

    struct Private;
    Private* const d;
};

} // namespace KIPIPrintImagesPlugin

#endif /* WIZARD_H */
