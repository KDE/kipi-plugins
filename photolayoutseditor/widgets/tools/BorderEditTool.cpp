#include "BorderEditTool.h"
#include "ToolsDockWidget.h"
#include "AbstractPhoto.h"

#include "BordersGroup.h"
#include "BorderDrawersLoader.h"

#include <QDebug>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QFont>
#include <QColorDialog>
#include <QMetaProperty>
#include <QtTreePropertyBrowser>
#include "KEditFactory.h"

#include <kpushbutton.h>
#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

BorderEditTool::BorderEditTool(Scene * scene, QWidget * parent) :
    AbstractItemsListViewTool(i18n("Borders editor"), scene, Canvas::SingleSelcting, parent)
{
}

QStringList BorderEditTool::options() const
{
    return BorderDrawersLoader::registeredDrawers();
}

AbstractMovableModel * BorderEditTool::model()
{
    if (currentItem() && currentItem()->bordersGroup())
        return currentItem()->bordersGroup();
    return 0;
}

QObject * BorderEditTool::createItem(const QString & name)
{
    return BorderDrawersLoader::getDrawerByName(name);
}

QWidget * BorderEditTool::createEditor(QObject * item, bool createCommands)
{
    BorderDrawerInterface * drawer = qobject_cast<BorderDrawerInterface*>(item);
    if (!drawer)
        return 0;
    return BorderDrawersLoader::createEditor(drawer, createCommands);
}
