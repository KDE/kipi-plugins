#ifndef BORDEREDITTOOL_H
#define BORDEREDITTOOL_H

#include "AbstractItemsListViewTool.h"

#include <QDockWidget>
#include <QDoubleSpinBox>

#include <kcombobox.h>
#include <kcolorbutton.h>
#include <kpushbutton.h>

namespace KIPIPhotoLayoutsEditor
{
    class BorderEditTool : public AbstractItemsListViewTool
    {
            Q_OBJECT

        public:

            explicit BorderEditTool(Scene * scene, QWidget * parent = 0);
            virtual QStringList options() const;
            virtual AbstractMovableModel * model();
            virtual QObject * createItem(const QString & name);
            virtual QWidget * createEditor(QObject * item, bool createCommands = true);

    };
}

#endif // BORDEREDITTOOL_H
