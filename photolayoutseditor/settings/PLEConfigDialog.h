#ifndef PLECONFIGDIALOG_H
#define PLECONFIGDIALOG_H

#include <kconfigdialog.h>
#include <QDebug>
namespace KIPIPhotoLayoutsEditor
{
    class PLEConfigDialogPrivate;

    class PLEConfigDialog : public KConfigDialog
    {
            Q_OBJECT

            PLEConfigDialogPrivate * d;

        public:

            explicit PLEConfigDialog(QWidget * parent = 0);

            virtual void updateSettings();
            virtual void updateWidgets();

        friend class PLEConfigDialogPrivate;
    };
}

#endif // PLECONFIGDIALOG_H
