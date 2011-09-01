#ifndef EFFECTSEDITORTOOL_H
#define EFFECTSEDITORTOOL_H

#include "AbstractItemsTool.h"

#include <QListView>
#include <QModelIndex>
#include <QStyledItemDelegate>

#include <kpushbutton.h>

namespace KIPIPhotoLayoutsEditor
{
    class AbstractPhotoEffectInterface;
    class EffectsEditorToolPrivate;

    class EffectListViewDelegate : public QWidget
    {
            KPushButton * m_acceptButton;
            Q_OBJECT
        public:
            EffectListViewDelegate(QWidget * parent = 0);
        signals:
            void editorClosed();
            void editorAccepted();
            void effectSelected(const QString & effectName);
        protected slots:
            void emitEditorClosed()
            {
                emit editorClosed();
            }
            void emitEditorAccepted()
            {
                emit editorAccepted();
            }
            void emitEffectSelected(const QString & effectName)
            {
                m_acceptButton->setEnabled(!effectName.isEmpty());
                emit effectSelected(effectName);
            }
    };

    class EffectsEditorTool : public AbstractItemsTool
    {
            Q_OBJECT

        public:
            explicit EffectsEditorTool(Scene * scene, QWidget * parent = 0);
            ~EffectsEditorTool();
            virtual void currentItemAboutToBeChanged();
            virtual void currentItemChanged();
            virtual void positionAboutToBeChanged(){} // Unused
            virtual void positionChanged(){} // Unused

        protected slots:
            void viewCurrentEffectEditor(const QModelIndex & index);
            void viewEffectEditor(AbstractPhotoEffectInterface * effect);
            void addEffect();
            void editorEfectSelected(const QString & effectName);
            void addEffectCommand();
            void closeEffectChooser();
            void removeSelected();
            void moveSelectedDown();
            void moveSelectedUp();
        private:
            void closeEffectPropertyBrowser();
            EffectsEditorToolPrivate * d;
    };

    class EffectsListView : public QListView
    {
            Q_OBJECT
        public:
            EffectsListView(QWidget * parent = 0) : QListView(parent)
            {
                this->setSelectionMode(QAbstractItemView::SingleSelection);
            }
            QModelIndex selectedIndex() const
            {
                QModelIndexList indexes = selectedIndexes();
                if (indexes.count() == 1)
                    return indexes.at(0);
                return QModelIndex();
            }
        signals:
            void selectedIndex(const QModelIndex & index);
        protected:
            virtual void selectionChanged(const QItemSelection & selected, const QItemSelection & /*deselected*/)
            {
                QModelIndexList indexes = selected.indexes();
                if (indexes.count())
                {
                    QModelIndex index = indexes.at(0);
                    if (index.isValid())
                    {
                        emit selectedIndex(index);
                        return;
                    }
                }
                emit selectedIndex(QModelIndex());
            }

        friend class EffectsEditorTool;
    };
}

#endif // EFFECTSEDITORTOOL_H
