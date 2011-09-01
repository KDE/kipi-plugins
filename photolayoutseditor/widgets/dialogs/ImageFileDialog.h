#ifndef IMAGEFILEDIALOG_H
#define IMAGEFILEDIALOG_H

#include <kfiledialog.h>

namespace KIPIPhotoLayoutsEditor
{
    class ImageFileDialog : public KFileDialog
    {
            Q_OBJECT

            static QString FILTER;
            const char * m_format;

        public:

            ImageFileDialog(const KUrl & startDir, QWidget * parent = 0, QWidget * widget = 0);
            virtual int exec();
            const char * format();

        private:

            Q_DISABLE_COPY(ImageFileDialog)

    };
}

#endif // IMAGEFILEDIALOG_H
