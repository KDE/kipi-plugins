#include "StandardBordersFactory.h"
#include "PolaroidBorderDrawer.h"
#include "SolidBorderDrawer.h"

#include <klocalizedstring.h>

using namespace KIPIPhotoLayoutsEditor;

StandardBordersFactory::StandardBordersFactory(QObject * parent) :
    BorderDrawerFactoryInterface(parent)
{
}

QString StandardBordersFactory::drawersNames() const
{
    return i18n("Polaroid border") + QString(";") +
           i18n("Solid border");
}

BorderDrawerInterface * StandardBordersFactory::getDrawerInstance(const QString & name)
{
    if (name == i18n("Solid border"))
       return new SolidBorderDrawer(this);
    if (name == i18n("Polaroid border"))
       return new PolaroidBorderDrawer(this);
    return 0;
}
