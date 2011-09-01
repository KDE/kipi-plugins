#include "plugin_photolayoutseditor.h"

#include <kdebug.h>
#include <kgenericfactory.h>
#include <kicon.h>
#include <kaction.h>
#include <kactioncollection.h>

#include <libkipi/interface.h>

#include "photolayoutseditor.h"

using namespace KIPIPhotoLayoutsEditor;

K_PLUGIN_FACTORY ( PhotoFrmesEditorFactory, registerPlugin<Plugin_PhotoLayoutsEditor>(); )
K_EXPORT_PLUGIN  ( PhotoFrmesEditorFactory("kipiplugin_photolayoutseditor") )

Plugin_PhotoLayoutsEditor::Plugin_PhotoLayoutsEditor(QObject * parent, const QVariantList & args) :
    KIPI::Plugin( PhotoFrmesEditorFactory::componentData(), parent, "Photo Frames Editor" )
{
    m_interface    = 0;
    m_action       = 0;
    m_parentWidget = 0;
    m_manager      = 0;

    kDebug() << "Plugin_PhotoLayoutsEditor plugin loaded";
}

Plugin_PhotoLayoutsEditor::~Plugin_PhotoLayoutsEditor()
{}

KIPI::Category Plugin_PhotoLayoutsEditor::category( KAction * action ) const
{
    if ( action == m_action )
       return KIPI::ToolsPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ToolsPlugin; // no warning from compiler, please
}

void Plugin_PhotoLayoutsEditor::setup( QWidget * widget )
{
    m_parentWidget = widget;
    KIPI::Plugin::setup(m_parentWidget);

    m_action = actionCollection()->addAction("photolayoutseditor");
    m_action->setText(i18n("Create photo layouts..."));
    m_action->setIcon(KIcon("photolayoutseditor"));

    connect(m_action, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction(m_action);

    m_interface = dynamic_cast< KIPI::Interface* >(parent());
    if (!m_interface)
    {
       kError() << "Kipi interface is null!";
       return;
    }
}

void Plugin_PhotoLayoutsEditor::slotActivate()
{
    if (!m_interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    m_manager = PhotoLayoutsEditor::instance(m_parentWidget);
    m_manager->setInterface(m_interface);
    m_manager->show();
}
