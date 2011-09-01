#include "KEditFactory.h"

#include <QHBoxLayout>
#include <QSpinBox>
#include <QSlider>
#include <QDebug>

KSpinBoxFactory::KSpinBoxFactory(QObject * parent) :
    QtSpinBoxFactory(parent)
{
}

QWidget * KSpinBoxFactory::createEditor(QtIntPropertyManager * manager, QtProperty * property, QWidget * parent)
{
    QWidget * widget = QtSpinBoxFactory::createEditor(manager,property,parent);
    connect(widget,SIGNAL(destroyed()),this,SLOT(emitEditingFinished()));
    return widget;
}

KSliderFactory::KSliderFactory(QObject * parent) :
    QtSliderFactory(parent)
{
}

KScrollBarFactory::KScrollBarFactory(QObject * parent) :
    QtScrollBarFactory(parent)
{
}

KCheckBoxFactory::KCheckBoxFactory(QObject * parent) :
    QtCheckBoxFactory(parent)
{
}

KDoubleSpinBoxFactory::KDoubleSpinBoxFactory(QObject * parent) :
    QtDoubleSpinBoxFactory(parent)
{
}

KLineEditFactory::KLineEditFactory(QObject * parent) :
    QtLineEditFactory(parent)
{
}

KDateEditFactory::KDateEditFactory(QObject * parent) :
    QtDateEditFactory(parent)
{
}

KTimeEditFactory::KTimeEditFactory(QObject * parent) :
    QtTimeEditFactory(parent)
{
}

KDateTimeEditFactory::KDateTimeEditFactory(QObject * parent) :
    QtDateTimeEditFactory(parent)
{
}

KKeySequenceEditorFactory::KKeySequenceEditorFactory(QObject * parent) :
    QtKeySequenceEditorFactory(parent)
{
}

KCharEditorFactory::KCharEditorFactory(QObject * parent) :
    QtCharEditorFactory(parent)
{
}

KEnumEditorFactory::KEnumEditorFactory(QObject * parent) :
    QtEnumEditorFactory(parent)
{
}

KCursorEditorFactory::KCursorEditorFactory(QObject * parent) :
    QtCursorEditorFactory(parent)
{
}

KColorEditorFactory::KColorEditorFactory(QObject * parent) :
    QtColorEditorFactory(parent)
{
}

QWidget * KColorEditorFactory::createEditor(QtColorPropertyManager * manager, QtProperty * property, QWidget * parent)
{
    QWidget * widget = QtColorEditorFactory::createEditor(manager,property,parent);
    connect(widget,SIGNAL(destroyed()),this,SLOT(emitEditingFinished()));
    return widget;
}

KFontEditorFactory::KFontEditorFactory(QObject * parent) :
    QtFontEditorFactory(parent)
{
}

KVariantEditorFactory::KVariantEditorFactory(QObject *parent) :
    QtVariantEditorFactory(parent)
{}

QWidget * KVariantEditorFactory::createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent)
{
    QWidget * widget = QtVariantEditorFactory::createEditor(manager,property,parent);
    connect(widget,SIGNAL(destroyed()),this,SLOT(emitEditingFinished()));
    return widget;
}

KSliderEditFactory::KSliderEditFactory(QObject *parent) :
    QtAbstractEditorFactory<QtIntPropertyManager>(parent)
{
    originalFactory = new QtSliderFactory(this);
}

void KSliderEditFactory::connectPropertyManager(QtIntPropertyManager * manager)
{
    this->addPropertyManager(manager);
    originalFactory->addPropertyManager(manager);
}

QWidget * KSliderEditFactory::createEditor(QtIntPropertyManager * manager, QtProperty * property, QWidget * parent)
{
    QtAbstractEditorFactoryBase * base = originalFactory;
    QWidget * w = base->createPropertyEditor(property,parent);
    if (!w)
        return 0;
    QSlider * slider = qobject_cast<QSlider*>(w);
    if (!w)
        return 0;

    w = new QWidget(parent);
    slider->setParent(w);
    QSpinBox * spinbox = new QSpinBox(w);
    spinbox->setMaximum(manager->maximum(property));
    spinbox->setMinimum(manager->minimum(property));
    spinbox->setValue(manager->value(property));
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(slider,1);
    layout->addWidget(spinbox,0);
    w->setLayout(layout);

    createdEditors[property].append(w);
    editorToProperty[w] = property;

    connect(slider,SIGNAL(valueChanged(int)),spinbox,SLOT(setValue(int)));
    connect(spinbox,SIGNAL(valueChanged(int)),slider,SLOT(setValue(int)));
    connect(w,SIGNAL(destroyed(QObject*)),this,SLOT(slotEditorDestroyed(QObject*)));

    return w;
}

void KSliderEditFactory::disconnectPropertyManager(QtIntPropertyManager * manager)
{
    this->removePropertyManager(manager);
    originalFactory->removePropertyManager(manager);
}

void KSliderEditFactory::slotEditorDestroyed(QObject *object)
{
    emit editingFinished();
    QMap<QWidget *, QtProperty *>::ConstIterator itEditor = editorToProperty.constBegin();
    while (itEditor != editorToProperty.constEnd())
    {
        if (itEditor.key() == object)
        {
            QWidget *editor = itEditor.key();
            QtProperty *property = itEditor.value();
            editorToProperty.remove(editor);
            createdEditors[property].removeAll(editor);
            if (createdEditors[property].isEmpty())
                createdEditors.remove(property);
            return;
        }
        itEditor++;
    }
}
