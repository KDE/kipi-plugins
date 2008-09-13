
// QT includes
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <QtSvg>
#include <QSize>

// KDE includes
#include <kstandarddirs.h>

// local includes
#include "imageviewer.h"

namespace KIPISlideShowPlugin
{

ImageViewer::ImageViewer(QWidget* parent)
            :QWidget(parent)
{
  m_svgRenderer = new QSvgRenderer( KStandardDirs::locate("data", "kipi/data/KIPIicon.svg") );
  m_clear = true;
  clear();
}
  
ImageViewer::~ImageViewer() 
{
  delete m_svgRenderer;
}

QImage ImageViewer::image() const 
{ 
  return m_image; 
}

void ImageViewer::setImage(const QImage &image) 
{
  m_clear = false;
  m_image = image; 
  updateViewer(); 
  update();
}

void ImageViewer::clear() 
{
  m_clear = true;
  m_image = QImage(0,0);
  updateViewer();
  update();
}

void ImageViewer::updateViewer() 
{ 
  if ( m_image.isNull() ) 
  {
    m_clear = true;
    update();
  }
  else
    m_pixmap = QPixmap::fromImage(m_image.scaled(size(), Qt::KeepAspectRatio)); 
}

void ImageViewer::resizeEvent(QResizeEvent *) 
{ 
  updateViewer(); 
}

void ImageViewer::paintEvent(QPaintEvent *) 
{
  QPainter painter(this);

  if (m_clear) 
  {
    QSize finalSize = m_svgRenderer->defaultSize();
    finalSize.scale(width(),height(), Qt::KeepAspectRatio);
    m_svgRenderer->render(&painter,
                          QRectF(qreal((width()-m_svgRenderer->defaultSize().width())/2), 
                                  qreal((height()-m_svgRenderer->defaultSize().height())/2), 
                                  qreal(finalSize.width()),
                                  qreal(finalSize.height())
                          ));
  }
  else
    painter.drawPixmap( QPoint( (int) ((width()-m_pixmap.width())/2),
                                (int) ((height()-m_pixmap.height())/2)), m_pixmap );
}

} // NameSpace KIPISlideShowPlugin

