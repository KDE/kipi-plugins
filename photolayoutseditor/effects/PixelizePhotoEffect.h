#ifndef PIXELIZEPHOTOEFFECT_H
#define PIXELIZEPHOTOEFFECT_H

#include "PhotoEffectsLoader.h"

namespace KIPIPhotoFramesEditor
{
    class PixelizePhotoEffect : public PhotoEffectsLoader
    {
            int m_pixelSize;
            class PixelizeUndoCommand;

        public:

            PixelizePhotoEffect(int pixelSize, QObject * parent = 0);
            virtual QtAbstractPropertyBrowser * propertyBrowser() const;
            virtual QString toString() const;

          /**
            * Pixel size property
            */
            Q_PROPERTY(int m_pixelSize READ pixelSize WRITE setPixelSize)
            void setPixelSize(int pixelSize)
            {
                m_pixelSize = pixelSize;
                emit effectChanged(this);
            }
            int pixelSize() const
            {
                return m_pixelSize;
            }

          /**
            * Pixelize effect identifier (name).
            */
            virtual QString effectName() const;

        protected:

            static const QString PIXEL_SIZE_STRING;
            virtual QImage apply(const QImage &image);

        protected slots:

            virtual void propertyChanged(QtProperty * property);

        private:

            static inline QImage pixelize(const QImage & image, int pixelSize)
            {
                Q_ASSERT(pixelSize > 0);
                if (pixelSize <= 1)
                    return image;
                int width = image.width();
                int height = image.height();
                QImage result = image.copy(image.rect());
                for (int y = 0; y < height; y += pixelSize)
                {
                    int ys = qMin(height - 1, y + pixelSize / 2);
                    QRgb *sbuf = reinterpret_cast<QRgb*>(result.scanLine(ys));
                    for (int x = 0; x < width; x += pixelSize) {
                        int xs = qMin(width - 1, x + pixelSize / 2);
                        QRgb color = sbuf[xs];
                        for (int yi = 0; yi < qMin(pixelSize, height - y); ++yi)
                        {
                            QRgb *buf = reinterpret_cast<QRgb*>(result.scanLine(y + yi));
                            for (int xi = 0; xi < qMin(pixelSize, width - x); ++xi)
                                buf[x + xi] = color;
                        }
                    }
                }
                return result;
            }
    };
}

#endif // PIXELIZEPHOTOEFFECT_H
