#ifndef GRAYSCALEPHOTOEFFECT_P_H
#define GRAYSCALEPHOTOEFFECT_P_H

#include "GrayscalePhotoEffect_global.h"
#include "AbstractPhotoEffectInterface.h"

using namespace KIPIPhotoLayoutsEditor;

class GrayscalePhotoEffectFactory;
class GrayscalePhotoEffect : public AbstractPhotoEffectInterface
{
        Q_INTERFACES(AbstractPhotoEffectInterface)

    public:

        explicit GrayscalePhotoEffect(GrayscalePhotoEffectFactory * factory, QObject * parent = 0);
        virtual QImage apply(const QImage & image) const;
        virtual QString effectName() const;
        virtual QString toString() const;
        virtual operator QString() const;

    private:
        static inline QImage greyscaled(const QImage & image)
        {
            QImage result = image;
            unsigned int pixels = result.width() * result.height();
            unsigned int * data = (unsigned int *) result.bits();
            for (unsigned int i = 0; i < pixels; ++i)
            {
                int val = qGray(data[i]);
                data[i] = qRgb(val,val,val);
            }
            return result;
        }
};

#endif // GRAYSCALEPHOTOEFFECT_P_H
