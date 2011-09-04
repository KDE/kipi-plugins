#ifndef PLEABOUTDATA_H
#define PLEABOUTDATA_H

#include <kaboutdata.h>

namespace KIPIPhotoLayoutsEditor
{
    class PLEAboutData : public KAboutData
    {
        public:

            PLEAboutData() :
                KAboutData("photolayoutseditor",
                    0,
                    ki18n("Photo Layouts Editor"),
                    "0.1",
                    ki18n("Collage creator and editor"),
                    KAboutData::License_GPL,
                    ki18n("Copyright (C) 2011 Łukasz Spas"),
                    ki18n(""),
                    QString("http://www.digikam.org").toUtf8())
            {
                addAuthor(ki18n("Your Name"),
                          ki18n("Author & Developer"),
                          "lukasz dot spas at gmail dot com");
            }
    };
}

#endif // PLEABOUTDATA_H
