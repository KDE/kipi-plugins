/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

// Include files for Qt

#include <q3scrollview.h>
#include <qimage.h>
#include <qstring.h>
//Added by qt3to4:
#include <QPixmap>
#include <QWheelEvent>
#include <QMouseEvent>

// Include files for KDE

#include <kdialog.h>

// Local includes

#include "kpaboutdata.h"

class QPixmap;
class QCursor;
class QLCDNumber;
class QSlider;

class K3Process;

namespace KIPIBatchProcessImagesPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////////////

class PixmapView : public Q3ScrollView
{
Q_OBJECT

public:

    PixmapView(bool cropAction, QWidget *parent=0, const char *name=0);
    ~PixmapView();

    void setImage(const QString &ImagePath, const QString &tmpPath);
    void resizeImage(int ZoomFactor);

private slots:
    
    void slotPreviewReadStd(K3Process* proc, char *buffer, int buflen);
    void PreviewProcessDone(K3Process* proc);
    void contentsWheelEvent( QWheelEvent * e );
    void contentsMousePressEvent ( QMouseEvent * e );
    void contentsMouseReleaseEvent ( QMouseEvent * e );
    void contentsMouseMoveEvent( QMouseEvent * e );

signals:

    void wheelEvent( int delta );

protected:

    QPixmap     *m_pix;
    
    QImage       m_img;

    int          m_w;
    int          m_h;
    int          m_xpos;
    int          m_ypos;

    K3Process    *m_PreviewProc;

    QString      m_previewOutput;
    QString      m_previewFileName;

    bool         m_validPreview;
    bool         m_cropAction;

    QCursor     *m_handCursor;

    void drawContents(QPainter *p, int x, int y, int w, int h);
    void PreviewCal(const QString &ImagePath, const QString &tmpPath);

};

/////////////////////////////////////////////////////////////////////////////////////////////////////

class ImagePreview : public KDialog
{
Q_OBJECT

public:
    
    ImagePreview(const QString &fileOrig, const QString &fileDest, const QString &tmpPath,
                 bool cropActionOrig, bool cropActionDest, const QString &EffectName,
                 const QString &FileName, QWidget *parent=0);
    ~ImagePreview();

private slots:
    
    void slotZoomFactorValueChanged( int ZoomFactorValue );
    void slotWheelChanged( int delta );
    void slotHelp(void);
    
protected:
    
    PixmapView  *m_previewOrig;
    PixmapView  *m_previewDest;
    
    QLCDNumber  *LCDZoomFactorValue;
    
    QSlider     *ZoomFactorSlider;

private:

    KIPIPlugins::KPAboutData    *m_about;
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // IMAGEPREVIEW_H

