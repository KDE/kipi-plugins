#ifndef NEWCANVASDIALOG_H
#define NEWCANVASDIALOG_H

#include <kdialog.h>

#include <QListWidgetItem>

namespace KIPIPhotoLayoutsEditor
{
    class CanvasSize;
    class TemplatesModel;

    class NewCanvasDialog : public KDialog
    {
            Q_OBJECT

        public:

            explicit NewCanvasDialog(QWidget * parent = 0);
            ~NewCanvasDialog();

            bool hasTemplateSelected() const;
            QString templateSelected() const;
            CanvasSize canvasSize() const;

        private slots:

            void paperSizeSelected(QListWidgetItem * current, QListWidgetItem * previous);
            void orientationChanged();
            void setHorizontal(bool);
            void setVertical(bool);

        private:

            void setupUI();
            void loadTemplatesList(const QString & path, TemplatesModel * model);

            class Private;
            Private * d;
            friend class Private;

    };
}

#endif // NEWCANVASDIALOG_H
