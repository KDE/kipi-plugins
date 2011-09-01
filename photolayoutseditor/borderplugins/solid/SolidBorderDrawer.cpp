#include "SolidBorderDrawer.h"
#include "SolidBorderDrawer_p.h"

#include <kpluginfactory.h>
#include <klocalizedstring.h>

K_PLUGIN_FACTORY( SolidBorderDrawerFactoryLoader, registerPlugin<SolidBorderDrawerFactory>(); )
K_EXPORT_PLUGIN ( SolidBorderDrawerFactoryLoader("photolayoutseditorborderplugin_solid") )

SolidBorderDrawerFactory::SolidBorderDrawerFactory(QObject * parent, const QVariantList&) :
    BorderDrawerFactoryInterface(parent)
{
}

QString SolidBorderDrawerFactory::drawerName() const
{
    return i18n("Solid");
}

BorderDrawerInterface * SolidBorderDrawerFactory::getDrawerInstance(QObject * parent)
{
    return new SolidBorderDrawer(this, parent);
}
