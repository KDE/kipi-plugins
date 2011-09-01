#ifndef PLECONFIGVIEWWIDGET_H
#define PLECONFIGVIEWWIDGET_H

#include <QWidget>

namespace KIPIPhotoLayoutsEditor
{
    class PLEConfigViewWidgetPrivate;

    class PLEConfigViewWidget : public QWidget
    {
            Q_OBJECT

            PLEConfigViewWidgetPrivate * d;

        public:

            explicit PLEConfigViewWidget(QWidget * parent = 0, const QString & caption = QString());
            ~PLEConfigViewWidget();
            void updateSettings();
            void updateWidgets();

        private:

            void setupGUI();


        friend class PLEConfigViewWidgetPrivate;
    };
}

#endif // PLECONFIGVIEWWIDGET_H
