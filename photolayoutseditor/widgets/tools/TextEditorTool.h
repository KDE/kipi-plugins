#ifndef TEXTEDITORTOOL_H
#define TEXTEDITORTOOL_H

#include "AbstractItemsTool.h"

class QtAbstractPropertyBrowser;

namespace KIPIPhotoLayoutsEditor
{
    class TextItem;
    class TextEditorToolPrivate;

    class TextEditorTool : public AbstractItemsTool
    {
            Q_OBJECT

            TextEditorToolPrivate * d;

            TextItem * m_text_item;
            TextItem * m_created_text_item;
            QtAbstractPropertyBrowser * m_browser;
            bool m_create_new_item;

        public:

            explicit TextEditorTool(Scene * scene, QWidget * parent = 0);
            ~TextEditorTool();

        signals:

        public slots:

            virtual void currentItemAboutToBeChanged();
            virtual void currentItemChanged();
            virtual void positionAboutToBeChanged();
            virtual void positionChanged();

        protected slots:

            void createNewItem();

        friend class TextEditorToolPrivate;
    };
}

#endif // TEXTEDITORTOOL_H
