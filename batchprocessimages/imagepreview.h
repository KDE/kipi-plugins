//////////////////////////////////////////////////////////////////////////////
//
//    IMAGEPREVIEW.H
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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

// Include files for Qt

#include <qscrollview.h>
#include <qimage.h>
#include <qstring.h>

// Include files for KDE

#include <kdialogbase.h>

class QPixmap;
class QCursor;
class QLCDNumber;
class QSlider;
class QPushButton;

class KProcess;

namespace KIPIBatchProcessImagesPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////////////

class PixmapView : public QScrollView
{
Q_OBJECT

public:

    PixmapView(bool cropAction, QWidget *parent=0, const char *name=0);
    ~PixmapView();

    void setImage(const QString &ImagePath, const QString &tmpPath);
    void resizeImage(int ZoomFactor);

private slots:
    
    void slotPreviewReadStd(KProcess* proc, char *buffer, int buflen);
    void PreviewProcessDone(KProcess* proc);
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

    KProcess    *m_PreviewProc;

    QString      m_previewOutput;
    QString      m_previewFileName;

    bool         m_validPreview;
    bool         m_cropAction;

    QCursor     *m_handCursor;

    void drawContents(QPainter *p, int x, int y, int w, int h);
    void PreviewCal(const QString &ImagePath, const QString &tmpPath);
    
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

class ImagePreview : public KDialogBase
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
    
    QPushButton *m_helpButton;
    
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // IMAGEPREVIEW_H

