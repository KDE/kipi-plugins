#ifndef PATTERNSCOMBOBOX_H
#define PATTERNSCOMBOBOX_H

#include <QComboBox>

namespace KIPIPhotoLayoutsEditor
{
    class PatternsComboBox : public QComboBox
    {
            Q_OBJECT

        public:

            explicit PatternsComboBox(QWidget * parent = 0);
            Qt::BrushStyle pattern() const;
            void setPattern(Qt::BrushStyle pattern);

        signals:

            void currentPatternChanged(Qt::BrushStyle pattern);

        protected:

            virtual void paintEvent(QPaintEvent *e);

        protected slots:

            void emitPatternChanged(int index);

    };
}

#endif // PATTERNSCOMBOBOX_H
