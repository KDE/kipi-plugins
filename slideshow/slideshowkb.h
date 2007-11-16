/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-14
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2007 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 *
 * Parts of this code are based on smoothslidesaver by Carsten Weinhold 
 * <carsten dot weinhold at gmx dot de> and slideshowgl.{cpp|h} by Renchi Raju     
 * <renchi@pooh.tam.uiuc.edu>                                           
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

#ifndef SLIDESHOWKB_H
#define SLIDESHOWKB_H

// C++ includes.

#include <cstdlib> 

// Qt includes.

#include <qgl.h>
#include <qtimer.h>
#include <q3valuelist.h>
#include <qpair.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qmap.h>
//Added by qt3to4:
#include <QMouseEvent>

// KDE includes.

#include <kconfig.h>

// Local includes.

#include "kbeffect.h"
#include "screenproperties.h"

namespace KIPISlideShowPlugin
{
  class SmoothSlideSaver;
  class ImageLoadThread;

  // -------------------------------------------------------------------------

  class ViewTrans
  {

  public:

      ViewTrans(bool m_zoomIn, float relAspect);

      float transX(float pos) const { return m_baseX + m_deltaX * pos; };
      float transY(float pos) const { return m_baseY + m_deltaY * pos; };    
      float scale (float pos) const { return m_baseScale * (1.0 + m_deltaScale * pos); };
      float m_xScaleCorrect() { return m_xScale; };
      float m_yScaleCorrect() { return m_yScale; };

  private:

      double rnd() const { return (double)rand() / (double)RAND_MAX; };
      double rndSign() const { return (rand() < RAND_MAX / 2) ? 1.0 : -1.0; };

  private:

      // delta and scale values (begin to end) and the needed offsets
      double m_deltaX, m_deltaY, m_deltaScale;
      double m_baseScale, m_baseX, m_baseY;
      float  m_xScale, m_yScale;
  };

  // -------------------------------------------------------------------------

  class Image 
  {

  public:

      Image(ViewTrans *viewTrans, float aspect = 1.0);
      ~Image();

      ViewTrans* m_viewTrans;
      float      m_aspect;
      float      m_pos;
      float      m_opacity;
      bool       m_paint;
      GLuint     m_texture;
  };

  // -------------------------------------------------------------------------

  class SlideShowKB : public QGLWidget
  {

    Q_OBJECT

    public:

      SlideShowKB(const Q3ValueList<QPair<QString, int> >& fileList,
                  const QStringList& commentsList, bool ImagesHasComments);

      ~SlideShowKB();

      static QStringList effectNames();
      static QMap<QString,QString> effectNamesI18N();

    private:

      float aspect() { return (float)width() / (float)height(); };
      bool  setupNewImage(int imageIndex);
      void  startSlideShowOnce();
      void  swapImages();
      void  setNewKBEffect();
      void  endOfShow();

      void     initializeGL();
      void     paintGL();
      void     resizeGL(int w, int h);
      void     applyTexture(Image *img, const QImage &image);
      void     paintTexture(Image *img);
      unsigned suggestFrameRate(unsigned forceRate);

      void readSettings();

    protected:

      void mousePressEvent(QMouseEvent *event);
      void mouseMoveEvent(QMouseEvent *);

    private slots:

      void moveSlot();
      void slotEndOfShow();
      void slotMouseMoveTimeOut();
      void slotClose();

    private:

      int m_deskX;
      int m_deskY;
      int m_deskWidth;
      int m_deskHeight;

      KConfig* m_config;
      
      bool m_imagesHasComments;
      QStringList  m_commentsList;

      ScreenProperties* m_screen;
      QTimer           *m_timer;
      QTimer           *m_mouseMoveTimer;
      ImageLoadThread  *m_imageLoadThread;
      bool              m_haveImages;

      Image  *m_image[2];
      KBEffect *m_effect;
      int     m_numKBEffectRepeated;
      bool    m_zoomIn, m_initialized;
      float   m_step;

      bool    m_endOfShow;
      bool    m_showingEnd;

      // settings from config file
      int      m_delay;
      bool     m_disableFadeInOut;
      bool     m_disableCrossFade;
      unsigned m_forceFrameRate;

      friend class KBEffect;
  };

}  // NameSpace KIPISlideShowPlugin

#endif // SLIDESHOWKB_H
