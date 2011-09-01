#ifndef SEPIAPHOTOEFFECT_P_H
#define SEPIAPHOTOEFFECT_P_H

#include "SepiaPhotoEffect_global.h"
#include "AbstractPhotoEffectInterface.h"

using namespace KIPIPhotoLayoutsEditor;

class SepiaPhotoEffectFactory;
class SepiaPhotoEffect : public AbstractPhotoEffectInterface
{
        Q_INTERFACES(AbstractPhotoEffectInterface)

    public:

        explicit SepiaPhotoEffect(SepiaPhotoEffectFactory * factory, QObject * parent = 0);
        virtual QImage apply(const QImage & image) const;
        virtual QString effectName() const;
        virtual QString toString() const;
        virtual operator QString() const;

    private:

        static inline QImage sepia_converted(const QImage & image)
        {
            QImage result = image;
            unsigned int pixels = result.width() * result.height();
            unsigned int * data = (unsigned int *) result.bits();
            for (unsigned int i = 0; i < pixels; ++i)
            {
                int gr = qGray(data[i]);
                int r = gr+40.0;
                int g = gr+20.0;
                int b = gr-20.0;
                data[i] = qRgb((r>255?255:r),(g>255?255:g),(b<0?0:b));
            }
            return result;
        }
};

#endif // SEPIAPHOTOEFFECT_P_H
