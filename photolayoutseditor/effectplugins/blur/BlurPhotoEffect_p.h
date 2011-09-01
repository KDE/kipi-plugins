#ifndef BLURPHOTOEFFECT_P_H
#define BLURPHOTOEFFECT_P_H

#include "AbstractPhotoEffectInterface.h"

#include <QImage>
#include <QRect>

using namespace KIPIPhotoLayoutsEditor;

#define RADIUS_PROPERTY "Radius"

class BlurPhotoEffectFactory;
class BlurPhotoEffect : public AbstractPhotoEffectInterface
{
        Q_INTERFACES(AbstractPhotoEffectInterface)

    public:

        explicit BlurPhotoEffect(BlurPhotoEffectFactory * factory, QObject * parent = 0);
        virtual QImage apply(const QImage & image) const;
        virtual QString effectName() const;
        virtual QString toString() const;
        virtual operator QString() const;

        int radius() const
        {
            foreach (AbstractPhotoEffectProperty * property, m_properties)
                if (property->id == RADIUS_PROPERTY)
                    return property->value.toInt();
            return 0;
        }

    private:

        void setRadius(int radius)
        {
            foreach (AbstractPhotoEffectProperty * property, m_properties)
                if (property->id == RADIUS_PROPERTY)
                    property->value = radius;
        }

        static QImage blurred(const QImage & image, const QRect& rect, unsigned int radius)
        {
            int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
            int alpha = (radius < 1)  ? 16 : (radius > sizeof(tab)) ? 1 : tab[radius-1];

            QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
            int r1 = rect.top();
            int r2 = rect.bottom();
            int c1 = rect.left();
            int c2 = rect.right();

            int bpl = result.bytesPerLine();
            int rgba[4];
            unsigned char * p;

            for (int col = c1; col <= c2; col++)
            {
                p = result.scanLine(r1) + col * 4;
                for (int i = 0; i < 4; i++)
                    rgba[i] = p[i] << 4;

                p += bpl;
                for (int j = r1; j < r2; j++, p += bpl)
                    for (int i = 0; i < 4; i++)
                        p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
            }

            for (int row = r1; row <= r2; row++)
            {
                p = result.scanLine(row) + c1 * 4;
                for (int i = 0; i < 4; i++)
                    rgba[i] = p[i] << 4;

                p += 4;
                for (int j = c1; j < c2; j++, p += 4)
                    for (int i = 0; i < 4; i++)
                        p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
            }

            for (int col = c1; col <= c2; col++)
            {
                p = result.scanLine(r2) + col * 4;
                for (int i = 0; i < 4; i++)
                    rgba[i] = p[i] << 4;

                p -= bpl;
                for (int j = r1; j < r2; j++, p -= bpl)
                    for (int i = 0; i < 4; i++)
                        p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
            }

            for (int row = r1; row <= r2; row++)
            {
                p = result.scanLine(row) + c2 * 4;
                for (int i = 0; i < 4; i++)
                    rgba[i] = p[i] << 4;

                p -= 4;
                for (int j = c1; j < c2; j++, p -= 4)
                    for (int i = 0; i < 4; i++)
                        p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
            }

            return result;
        }

    friend class BlurPhotoEffectFactory;
};

#endif // BLURPHOTOEFFECT_P_H
