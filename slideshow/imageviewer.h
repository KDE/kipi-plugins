
#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

// QT includes
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QtSvg>


namespace KIPISlideShowPlugin
{

class ImageViewer : public QWidget 
{
    Q_OBJECT

  public:
    ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

    QImage image() const; 
    void setImage(const QImage &image);
    void clear() ;

  protected:
    void updateViewer();

    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);

  private:
    QImage          m_image;
    QPixmap         m_pixmap;
    QSvgRenderer*   m_svgRenderer;
    bool            m_clear;
};

} // NameSpace KIPISlideShowPlugin
#endif
