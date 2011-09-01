#ifndef PLUGIN_PHOTOLAYOUTSEDITOR_H
#define PLUGIN_PHOTOLAYOUTSEDITOR_H

#include <QWidget>
#include <QVariant>

#include <libkipi/plugin.h>

namespace KIPI
{
    class Interface;
}

namespace KIPIPhotoLayoutsEditor
{
    class PhotoLayoutsEditor;
}

class Plugin_PhotoLayoutsEditor : public KIPI::Plugin
{
        Q_OBJECT

    public:

        Plugin_PhotoLayoutsEditor(QObject* parent, const QVariantList& args);
        virtual ~Plugin_PhotoLayoutsEditor();

        KIPI::Category category( KAction* action ) const;
        void setup( QWidget* );

        static Plugin_PhotoLayoutsEditor * self();

    public Q_SLOTS:

        void slotActivate();

    private:

        QWidget*                                    m_parentWidget;

        KAction*                                    m_action;

        KIPIPhotoLayoutsEditor::PhotoLayoutsEditor*   m_manager;

        KIPI::Interface*                            m_interface;
};

#endif // PLUGIN_PHOTOLAYOUTSEDITOR_H
